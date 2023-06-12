/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

// #define LOCAL_LOG_LEVEL LOG_LEVEL_VERBOSE

#include "at_device.h"
#include "at_utils.h"
#include "at_cmd_engine.h"
#include "at_dispatch.h"
#include "at_data_engine.h"
#include "at_cmux_engine.h"
#include "at_module.h"
#include "at_device_uart.h"
#include "kingrtc.h"

#define FETCH_BUF_SIZE (32)       // Size for each read
#define TPLUS_LEADING_TICK (1000) // 1s
#define TPLUS_DURATION_TICK (500) // 0.5s
#define TPLUS_TRAILING_TICK (500) // 0.5s

#define BUF_FRAME_COUNT (2)
#define TPLUS_BYTE_COUNT (3)

#define CMUX_PARENT(th) th->cmuxParent

enum DISPATCH_MODE
{
    DISPATCH_MODE_CMD,
    DISPATCH_MODE_DATA,
    DISPATCH_MODE_CMUX
};

struct AT_DISPATCH_T
{
    int channelId;
    enum DISPATCH_MODE mode;
    uint8_t *buff;
    int buffLen;
    int buffSize;
    AT_DEVICE_T *device;
    AT_CMD_ENGINE_T *cmdEngine;
    AT_DATA_ENGINE_T *dataEngine;
    AT_CMUX_ENGINE_T *cmuxEngine;

    unsigned prevRecvTick;
    unsigned tplusStartTick;
    int tplusCount;

    AT_CMUX_ENGINE_T *cmuxParent;
    int cmuxDlci;
    struct AT_DISPATCH_T *next;
};

static AT_DISPATCH_T *gAtDispatchHead = NULL;
static uint16 uPlusLeadingTick = TPLUS_LEADING_TICK;
static uint16 uPlusDurationTick = TPLUS_DURATION_TICK;
static uint16 uPlusTrailingTick = TPLUS_TRAILING_TICK;

    
extern struct AT_DISPATCH_T *g_dispatch[AT_DISPATCH_ID_MAX];


// +++ (tplus) detection
//
// tplus is used to escape from data mode to command line mode. A valid tplus:
// * distance from previous input > 1.0s
// * distance among the three plus < 0.5s
// * distance from next input > 0.5s
//
// tplus detection should be located in dipatch rather than data engine,
// because dispatch can calculate time better. Due to the cache in dispatch,
// downstream is much harder to know the arrive time.

// channel ID
//
// Multiple dispatch support in ATS uses DLCI to distinguish channels. To
// integrate with existed ATS, each dispatch channel will be associated with
// an ID. And this ID is only used in ATS, and ATE shall use object pointer.

// =============================================================================
// Register AT dispatch, and bind to a channel ID.
// =============================================================================
static void at_DispatchRegisterChannel(AT_DISPATCH_T *dispatch)
{
    if (gAtDispatchHead == NULL)
    {
        gAtDispatchHead = dispatch;
        return;
    }

    AT_DISPATCH_T *node = gAtDispatchHead;
    while (node->next != NULL)
        node = node->next;

    node->next = dispatch;
}

// =============================================================================
// Unregister AT dispatch, and unbind to the channel ID.
// =============================================================================
static void at_DispatchUnregisterChannel(AT_DISPATCH_T *dispatch)
{
    if (dispatch == gAtDispatchHead)
    {
        gAtDispatchHead = gAtDispatchHead->next;
        return;
    }

    AT_DISPATCH_T *node = gAtDispatchHead;
    for (;;)
    {
        if (node == NULL)
        {
            ATDISPLOG(E, "AT dispatch unregister found fail");
            return;
        }
        if (node->next == dispatch)
        {
            node->next = dispatch->next;
            return;
        }
        node = node->next;
    }
}

// =============================================================================
// at_DispatchGetByChannel
// =============================================================================
AT_DISPATCH_T *at_DispatchGetByChannel(int channel)
{
    AT_DISPATCH_T *node = gAtDispatchHead;
    while (node != NULL)
    {
        if (node->channelId == channel)
            return node;
        node = node->next;
    }

    ATDISPLOG(E, "AT dispatch invalid channel id=%d", channel);
    return gAtDispatchHead;
}

// =============================================================================
// at_DispatchFindByChannel
// =============================================================================
AT_DISPATCH_T *at_DispatchFindByChannel(int channel)
{
    AT_DISPATCH_T *node = gAtDispatchHead;
    while (node != NULL)
    {
        if (node->channelId == channel)
            return node;
        node = node->next;
    }
    return NULL;
}

// =============================================================================
// at_DispatchGetChannel
// =============================================================================
int at_DispatchGetChannel(AT_DISPATCH_T *th)
{
    return th->channelId;
}

// =============================================================================
// at_DispatchPushToEngine
// =============================================================================
static int at_DispatchPushToEngine(AT_DISPATCH_T *th, const uint8_t *data, unsigned length)
{
    enum DISPATCH_MODE mode = th->mode;

    if (mode == DISPATCH_MODE_CMD)
        return at_CmdPushData(th->cmdEngine, data, length);
    if (mode == DISPATCH_MODE_DATA)
        return at_DataPushData(th->dataEngine, data, length);
    if (mode == DISPATCH_MODE_CMUX)
        return at_CmuxPushData(th->cmuxEngine, data, length);
    return -1; // never reach
}

// =============================================================================
// Timer callback for tplus timer. When called, tplus trailing period is
// satisfied, and dispatch will be switched to cmd mode forcedly.
// =============================================================================
static void at_DispatchTplusTimeout(void* param)
{
    AT_DISPATCH_T *th = (AT_DISPATCH_T *)param;
    
    ATDISPLOG(I, "AT dispatch%d +++ timeout", th->channelId);

    if (th->tplusCount == TPLUS_BYTE_COUNT && th->mode == DISPATCH_MODE_DATA)
    {
        th->tplusCount = 0;
        th->buffLen = 0;
        at_ModuleModeSwitch(AT_MODE_SWITCH_DATA_ESCAPE, th->channelId);
    }
    else
    {
        th->tplusCount = 0;
        at_DispatchScheduleDrain(th);
    }
}

static void at_DispatchTplusTimeoutFun(void *param)
{
    void *ctx;
    TIMER_HANDLE handle = (TIMER_HANDLE)param;
    
    ctx = at_GetTimerCtx(handle);
    at_TaskCallback(at_DispatchTplusTimeout, ctx);
}
// =============================================================================
// tplus detection. If tplusCount != 0, data should be cached.
// =============================================================================
static void at_DispatchTplusDetect(AT_DISPATCH_T *th, const uint8_t *data, unsigned length)
{
    unsigned tick;
    KING_CurrentTickGet(&tick);
    unsigned prevRecvTick = th->prevRecvTick;
    th->prevRecvTick = tick;

    if (th->tplusCount + length > TPLUS_BYTE_COUNT)
        goto tplus_reset;

    for (unsigned n = 0; n < length; n++)
    {
        if (data[n] != '+')
            goto tplus_reset;
    }

    if (th->tplusCount == 0)
    {
        th->tplusStartTick = tick;
        if ((unsigned)(tick - prevRecvTick) < uPlusLeadingTick)
            goto tplus_reset;
    }
    else
    {
        if ((unsigned)(tick - th->tplusStartTick) > uPlusDurationTick)
            goto tplus_reset;
    }

    th->tplusCount += length;
    if (th->tplusCount == TPLUS_BYTE_COUNT)
        at_StartCallbackTimer(uPlusTrailingTick, at_DispatchTplusTimeoutFun, th);

    return;

tplus_reset:
    for (unsigned n = 0; n < th->tplusCount; n++)
        th->buff[th->buffLen++] = '+';
    if (th->tplusCount == TPLUS_BYTE_COUNT)
        at_StopCallbackTimer(at_DispatchTplusTimeoutFun, th);
    th->tplusCount = 0;
}

// =============================================================================
// at_DispatchPushData
// =============================================================================
void at_DispatchPushData(AT_DISPATCH_T *th, const uint8_t *data, unsigned length)
{
    if (th->buffLen + length + TPLUS_BYTE_COUNT > th->buffSize)
    {
        ATDISPLOG(E, "AT dispatch%d overflow len=%d drop=%d",
                  th->channelId, th->buffLen, length);
        return;
    }

    if (length > 0)
    {
        if (th->mode == DISPATCH_MODE_DATA)
        {
            at_DispatchTplusDetect(th, data, length);
            ATDISPLOG(D, "AT dispatch%d push buff=%d len=%d tplus=%d",
                      th->channelId, th->buffLen, length, th->tplusCount);
        }

        if (th->tplusCount == 0)
        {
            memcpy(&th->buff[th->buffLen], data, length);
            th->buffLen += length;
        }
    }

    // Push existed buffer to engine, if any.
    int len = at_DispatchPushToEngine(th, th->buff, th->buffLen);
    if (len >= th->buffLen)
    {
        // Clear buffer, and continue to push new data
        th->buffLen = 0;
    }
    else
    {
        if (len > 0)
        {
            // Remove used data and exit.
            th->buffLen -= len;
            memmove(th->buff, th->buff + len, th->buffLen);
        }
        at_DispatchScheduleDrain(th);
    }
}

// =============================================================================
// at_DispatchRead
// =============================================================================
void at_DispatchRead(AT_DISPATCH_T *th)
{
    if (th->device == NULL)
        return;

    for (;;)
    {
        int readLen = 0;
        // Read if there are room
        if (th->buffLen + TPLUS_BYTE_COUNT < th->buffSize)
        {
            uint8_t *fetch = &th->buff[th->buffLen];
            unsigned length = th->buffSize - (th->buffLen + TPLUS_BYTE_COUNT);
            readLen = at_DeviceRead(th->device, fetch, length);
            if (readLen > 0)
            {
                if (th->mode == DISPATCH_MODE_DATA)
                {
                    at_DispatchTplusDetect(th, fetch, readLen);
                    ATDISPLOG(D, "AT dispatch%d push buff=%d len=%d tplus=%d",
                              th->channelId, th->buffLen, readLen, th->tplusCount);
                }
                if (th->tplusCount == 0)
                    th->buffLen += readLen;

                if (th->tplusCount > 0)
                    continue;
            }
        }

        if (th->buffLen == 0)
            break;

        // Push existed buffer to engine, if any.
        int len = at_DispatchPushToEngine(th, th->buff, th->buffLen);
        if (len == th->buffLen)
        {
            // Clear buffer, and continue to push new data
            th->buffLen = 0;
        }
        else
        {
            // Remove used data and exit.
            if (len > 0)
            {
                th->buffLen -= len;
                memmove(th->buff, th->buff + len, th->buffLen);
            }
            break;
        }
    }
}

// =============================================================================
// at_DispatchDrain
// -----------------------------------------------------------------------------
/// Callback to run dispatch. In active mode, it will read from device again.
/// In passive mode, it will drain itself buffer.
// =============================================================================
static void at_DispatchDrain(void *par)
{
    AT_DISPATCH_T *th = (AT_DISPATCH_T *)par;
    if (th->device == NULL)
        at_DispatchPushData(th, NULL, 0);
    else
        at_DispatchRead(th);
}

// =============================================================================
// at_DispatchScheduleDrain
// -----------------------------------------------------------------------------
/// Schedule a callback to run dispatch. It shall only be called by downstream.
// =============================================================================
void at_DispatchScheduleDrain(AT_DISPATCH_T *th)
{
    at_TaskCallbackNotif(at_DispatchDrain, th);
}

// =============================================================================
// at_DispatchWrite
// -----------------------------------------------------------------------------
/// Write data to underlay device. There are no cache in dispatch, all data
/// will be written or dropped. Downstream modules should check whether the
/// data should be written based on dispatch mode.
// =============================================================================
void at_DispatchWrite(AT_DISPATCH_T *th, const uint8_t *data, unsigned length)
{
    if (CMUX_PARENT(th) != NULL)
    {
        at_CmuxDlcWrite(CMUX_PARENT(th), data, length, th->cmuxDlci);
        return;
    }
    if (th->device != NULL)
        at_DeviceWrite(th->device, data, length);
}

// =============================================================================
// at_DispatchSetCmdMode
// =============================================================================
void at_DispatchSetCmdMode(AT_DISPATCH_T *th)
{
    ATDISPLOG(I, "AT dispatch%d set CMD mode", th->channelId);
    th->mode = DISPATCH_MODE_CMD;
}

// =============================================================================
// at_DispatchSetDataMode
// =============================================================================
void at_DispatchSetDataMode(AT_DISPATCH_T *th)
{
    ATDISPLOG(I, "AT dispatch%d set DATA mode", th->channelId);
    if (th->dataEngine == NULL)
    {
        th->dataEngine = at_DataEngineCreate(th);
    }
    th->mode = DISPATCH_MODE_DATA;
    th->tplusCount = 0;
}

// =============================================================================
// at_DispatchSetCmuxMode
// =============================================================================
void at_DispatchSetCmuxMode(AT_DISPATCH_T *th, AT_CMUX_CONFIG_T *cfg)
{
    ATDISPLOG(I, "AT dispatch%d set CMUX mode", th->channelId);
    if (th->cmuxEngine == NULL)
    {
        th->cmuxEngine = at_CmuxEngineCreate(th, cfg);
    }
    th->mode = DISPATCH_MODE_CMUX;
}

// =============================================================================
// at_DispatchEndCmuxMode
// =============================================================================
void at_DispatchEndCmuxMode(AT_DISPATCH_T *th)
{
    ATDISPLOG(I, "AT dispatch%d end CMUX mode", th->channelId);
    // assert(th->mode != DISPATCH_MODE_CMUX)
    at_CmuxEngineDestroy(th->cmuxEngine);
    th->cmuxEngine = NULL;
    th->mode = DISPATCH_MODE_CMD;
}

// =============================================================================
// at_DispatchEndDataMode
// =============================================================================
void at_DispatchEndDataMode(AT_DISPATCH_T *th)
{
    ATDISPLOG(I, "AT dispatch%d end DATA mode", th->channelId);
    // assert(th->mode != DISPATCH_MODE_DATA)
    at_StopCallbackTimer(at_DispatchTplusTimeoutFun, th);
    at_DataEngineDestroy(th->dataEngine);
    th->dataEngine = NULL;
    th->mode = DISPATCH_MODE_CMD;
}

// =============================================================================
// at_DispatchIsCmdMode/at_DispatchIsDataMode/at_DispatchIsCmuxMode
// =============================================================================
bool at_DispatchIsCmdMode(AT_DISPATCH_T *th) { return th->mode == DISPATCH_MODE_CMD; }
bool at_DispatchIsDataMode(AT_DISPATCH_T *th) { return th->mode == DISPATCH_MODE_DATA; }
bool at_DispatchIsCmuxMode(AT_DISPATCH_T *th) { return th->mode == DISPATCH_MODE_CMUX; }
AT_DEVICE_T *at_DispatchGetDevice(AT_DISPATCH_T *th) { return CMUX_PARENT(th) != NULL ? at_CmuxGetDevice(CMUX_PARENT(th)) : th->device; }
AT_CMD_ENGINE_T *at_DispatchGetCmdEngine(AT_DISPATCH_T *th) { return th->cmdEngine; }
AT_DATA_ENGINE_T *at_DispatchGetDataEngine(AT_DISPATCH_T *th) { return th->dataEngine; }
AT_CMUX_ENGINE_T *at_DispatchGetParentCmux(AT_DISPATCH_T *th) { return CMUX_PARENT(th); }
int at_DispatchGetDlci(AT_DISPATCH_T *th) { return CMUX_PARENT(th) == NULL ? -1 : th->cmuxDlci; }

void at_DispatchSetPlusLeadingTick(uint16 tick)
{
    uPlusLeadingTick = tick;
}

void at_DispatchSetPlusDurationTick(uint16 tick)
{
    uPlusDurationTick = tick;
}

void at_DispatchSetPlusTrailingTick(uint16 tick)
{
    uPlusTrailingTick = tick;
}

// =============================================================================
// at_DispatchCreate
// =============================================================================
AT_DISPATCH_T *at_DispatchCreate(AT_DEVICE_T *device, AT_CMUX_ENGINE_T *cmuxParent, int dlci)
{
    AT_DISPATCH_T *th = (AT_DISPATCH_T *)at_malloc(sizeof(*th));

    th->mode = DISPATCH_MODE_CMD;
    th->cmuxParent = cmuxParent;
    th->cmuxDlci = dlci;
    th->buffSize = (CMUX_PARENT(th) == NULL) ? FETCH_BUF_SIZE : (at_CmuxMaxFrameSize(cmuxParent) * BUF_FRAME_COUNT + TPLUS_BYTE_COUNT);
    th->buff = (uint8_t *)at_malloc(th->buffSize);
    th->buffLen = 0;
    th->device = device;
    th->cmdEngine = NULL;
    th->dataEngine = NULL;
    th->cmuxEngine = NULL;
    th->prevRecvTick = 0;
    th->tplusStartTick = 0;
    th->tplusCount = 0;
    if (CMUX_PARENT(th) == NULL)
        th->channelId = at_ChannelFromDevice(at_DeviceGetNumber(device));
    else
        th->channelId = at_ChannelFromDeviceDlci(at_DeviceGetNumber(at_CmuxGetDevice(cmuxParent)), dlci);
    th->next = NULL;
    at_DispatchRegisterChannel(th);

    th->cmdEngine = at_CmdEngineCreate(th);
    return th;
}

// =============================================================================
// at_DispatchDestroy
// =============================================================================
void at_DispatchDestroy(AT_DISPATCH_T *th)
{
    if (th == NULL)
        return;

    at_DispatchUnregisterChannel(th);
    at_CmdEngineDestroy(th->cmdEngine);
    at_DataEngineDestroy(th->dataEngine);
    at_CmuxEngineDestroy(th->cmuxEngine);

    at_free(th->buff);
    at_free(th);
}
// =============================================================================
// at_DispatchGetAllCmdEngine
// =============================================================================
unsigned at_DispatchGetAllCmdEngine(AT_CMD_ENGINE_T *engines[], unsigned count)
{
    AT_DISPATCH_T *node = gAtDispatchHead;
    unsigned found = 0;

    while (node != NULL)
    {
        if (node->mode == DISPATCH_MODE_CMD)
            engines[found++] = at_DispatchGetCmdEngine(node);

        if (found >= count)
            break;
        node = node->next;
    }
    return found;
}
