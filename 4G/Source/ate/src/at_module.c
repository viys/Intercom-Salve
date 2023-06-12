/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "at_sa.h"
#include "at_module.h"
#include "at_device.h"
#include "at_dispatch.h"
#include "at_parse.h"
#include "at_data_engine.h"
#include "at_device_uart.h"
#include "at_device_usb.h"
#include "at_utils.h"
#include "at_errinfo.h"
#include "at_define.h"
#include "at_cfg.h"
#include "at_lp_ctrl.h"
#include "at_cmd_tcpip.h"

//#define CMUX_SEND_CRLF_WITH_DATA //send CRLF with data in one cmux frame

UINT8 gPppState = 0;

static void at_ModuleClearCmdList(AT_CMD_ENGINE_T *engine);
static void at_ModuleClearCurCmd(AT_CMD_ENGINE_T *engine);
static void at_CmdFinalHandle(AT_CMD_ENGINE_T *engine, bool ok);

extern VOID AT_SimCardDetectGpioInit(void);
#ifdef KING_SDK_FTM_SUPPORT
extern int CzPrdGetIPR(int32 *baud);
#endif
struct AT_DISPATCH_T *g_dispatch[AT_DISPATCH_ID_MAX];
static UINT8 g_rspStrEx[AT_RSP_STR_LEN + 4] = {0};

typedef struct AT_GLOBAL_T
{
    AT_MODULE_INIT_STATUS_T uiModuleInitStatus;
    bool smsReady;
    bool pbkReady;
    bool copsFlag[CFW_SIM_COUNT];
    bool setComFlag[CFW_SIM_COUNT];
    bool powerCommFlag[CFW_SIM_COUNT];
    int urcChannel;
    AT_DEVICE_T *devices[AT_MODULE_DEVICE_MAX];
} AT_GLOBAL_T;

static AT_GLOBAL_T gAtGlobal = {
    .uiModuleInitStatus = AT_MODULE_NOT_INIT,
    .smsReady = false,
    .pbkReady = false,
    .copsFlag = {[0 ...(CFW_SIM_COUNT - 1)] = false},
    .setComFlag = {[0 ...(CFW_SIM_COUNT - 1)] = false},
    .powerCommFlag = {[0 ...(CFW_SIM_COUNT - 1)] = false},
    .urcChannel = 0,
    .devices = {[0 ...(AT_MODULE_DEVICE_MAX - 1)] = NULL},
};

#define RDY_STRING "\r\nRDY\r\n"
static void at_ModuleCreateDefaultDevice(void)
{
#ifdef KING_SDK_FTM_SUPPORT
    int ret = FAIL;
    int32 baud = 0;
#endif
    AT_DEVICE_UART_CONFIG_T uartcfg;
    uartcfg.id = 1;
#ifdef KING_SDK_FTM_SUPPORT
    ret = CzPrdGetIPR(&baud);
    KING_SysLog("ret=%d, baud=%d", ret, baud);
    if((SUCCESS == ret) && (baud != 0))
    {
        gATCurrentu32nBaudRate = baud;
    }
#endif
    uartcfg.baud = gATCurrentu32nBaudRate;
    uartcfg.format = AT_DEVICE_FORMAT_8N1;
    uartcfg.parity = AT_DEVICE_PARITY_SPACE;
    uartcfg.ctsEnabled = false;
    uartcfg.rtsEnabled = false;
    uartcfg.autoBaudLC = false;
    AT_DEVICE_T *device = at_UartCreate(&uartcfg, 0);

    AT_DISPATCH_T *dispatch = at_DispatchCreate(device, NULL, 0);
    at_DeviceSetDispatch(device, dispatch);

    g_dispatch[AT_DISPATCH_ID_UART1] = device->recv;

    at_DeviceOpen(device);
    device->write(device, (uint8_t *)RDY_STRING, strlen(RDY_STRING));

    at_LpCtrlSetDevice(device);
}

static void at_ModuleCreateUsbDevice(void)
{
    AT_DEVICE_USB_CONFIG_T usbcfg;
    usbcfg.id = 5;
    usbcfg.baud = gATCurrentu32nBaudRate;
    usbcfg.format = AT_DEVICE_FORMAT_8N1;
    usbcfg.parity = AT_DEVICE_PARITY_SPACE;
    usbcfg.ctsEnabled = false;
    usbcfg.rtsEnabled = false;
    usbcfg.autoBaudLC = false;
    AT_DEVICE_T *device = at_UsbCreate(&usbcfg, 1);

    AT_DISPATCH_T *dispatch = at_DispatchCreate(device, NULL, 0);
    at_DeviceSetDispatch(device, dispatch);

    g_dispatch[AT_DISPATCH_ID_USB] = device->recv;

    at_DeviceOpen(device);

    at_LpCtrlSetDevice(device);
}

void at_SetUrcPort(uint8_t port)
{
    int channel = 0;

    if (port != gAtCurAsSetting.urcport)
    {
        gAtCurAsSetting.urcport = port;
        at_CfgSaveAsSettings();
    }

    if (AT_URC_PORT_UART1 == port)
    {
        channel = at_DispatchGetChannel(g_dispatch[AT_DISPATCH_ID_UART1]);
    }
    else
    {
        channel = at_DispatchGetChannel(g_dispatch[AT_DISPATCH_ID_USB]);
    }
    at_SetUrcChannel(channel);
}

VOID AT_ModuleInit(VOID)
{
    ATLOGI("AT_ModuleInit");

    at_CfgInit();
    BOOL bCfg = at_CfgGetAtSettings(USER_SETTING_1, 0);
    ATLOGI("at_CfgGetAtSettings 0: %d", bCfg);
    ATLOGI("gATCurrentu8nBaudRate: %d", gATCurrentu32nBaudRate);

    at_LpCtrlInit();
    at_ModuleCreateDefaultDevice();
    at_ModuleCreateUsbDevice();
    at_SetUrcPort(gAtCurAsSetting.urcport);
}

VOID at_ModuleModeSwitch(AT_MODE_SWITCH_CAUSE_T cause, UINT8 nDLCI)
{
    AT_DISPATCH_T *dispatch = at_DispatchGetByChannel(nDLCI);
    AT_CMD_ENGINE_T *engine = at_DispatchGetCmdEngine(dispatch);
    switch (cause)
    {
    case AT_CMD_MODE_SWITCH_CC_RELEASE_CALL:
    case AT_CMD_MODE_SWITCH_CC_SPEECH_IND:
    case AT_CMD_MODE_SWITCH_CC_ACCEPT_CALL:
    case AT_CMD_MODE_SWITCH_CC_INITIATE_SPEECH_CALL:
    case AT_CMD_MODE_SWITCH_ATH:
        break;
    case AT_MODE_SWITCH_DATA_END:
        at_DispatchEndDataMode(dispatch);
        if (tcpip_ttConnected() == TRUE)
        {
            ATLOGI("tcpip_ttConnected");
            at_CmdFinalHandle(engine, true);
            at_CmdCommandFinished(engine);
            at_CmdSetLineMode(engine);
        }
        else
        {
            at_CmdRespOK(engine);
        }
        gPppState = AT_PPP_STATE_END;
        break;
    case AT_MODE_SWITCH_DATA_START:
        at_CmdClearRemainCmds(engine);
        at_CmdRespIntermCode(engine, CMD_RC_CONNECT);
        at_DispatchSetDataMode(dispatch);
        gPppState = AT_PPP_STATE_START;
        break;
    case AT_MODE_SWITCH_DATA_ESCAPE:
        at_DispatchSetCmdMode(dispatch);
        at_CmdSetLineMode(engine);
        if (tcpip_ttConnected() == TRUE)
        {
            tcpip_ttEscapeDataMode(nDLCI);
        }
        at_CmdRespOK(engine);
        gPppState = AT_PPP_STATE_ESCAPE;
        break;
    case AT_MODE_SWITCH_DATA_RESUME:
        at_CmdRespIntermCode(engine, CMD_RC_CONNECT);
        at_DispatchSetDataMode(dispatch);
        gPppState = AT_PPP_STATE_RESUME;
        break;
    }
}
UINT8 at_GetPppState()
{
    return gPppState;
}

int at_ModuleNotifyResult(PAT_CMD_RESULT pCmdResult)
{
    if(pCmdResult == NULL)
    {
        return -1;
    }

    int channel = at_CmdGetChannel(pCmdResult->engine);
    if (pCmdResult->nDataSize == 0)
        ATCMDLOG(I, "AT CMD%d return=%d result=%d size=%d", channel,
                 pCmdResult->uReturnValue, pCmdResult->uResultCode, pCmdResult->nDataSize);
    else
        ATCMDLOGS(I, TSM(4), "AT CMD%d return=%d result=%d size=%d: %s", channel,
                  pCmdResult->uReturnValue, pCmdResult->uResultCode, pCmdResult->nDataSize, pCmdResult->pData);

    int r = 0;

    // why to output URC here?
    //at_ModuleWriteUrc(pCmdResult->engine);

    switch (pCmdResult->uReturnValue)
    {
    case CMD_FUNC_FAIL:
        at_CmdRespInfoNText(pCmdResult->engine, pCmdResult->pData, pCmdResult->nDataSize);
        if (pCmdResult->nErrorType == CMD_ERROR_CODE_TYPE_CME)
            at_CmdRespCmeError(pCmdResult->engine, pCmdResult->uErrorCode);
        else if (pCmdResult->nErrorType == CMD_ERROR_CODE_TYPE_CMS)
            at_CmdRespCmsError(pCmdResult->engine, pCmdResult->uErrorCode);
        else
            at_CmdRespErrorCode(pCmdResult->engine, pCmdResult->uResultCode);
        break;

    case CMD_FUNC_SUCC:
        at_CmdRespInfoNText(pCmdResult->engine, pCmdResult->pData, pCmdResult->nDataSize);
        at_CmdRespOK(pCmdResult->engine);
        break;

    case CMD_FUNC_SUCC_ASYN:
        AT_SetAsyncTimerMux(pCmdResult->engine, pCmdResult->nDelayTime);
        at_CmdRespInfoNText(pCmdResult->engine, pCmdResult->pData, pCmdResult->nDataSize);
        break;

    case CMD_FUNC_WAIT_SMS:
        AT_SetAsyncTimerMux(pCmdResult->engine, pCmdResult->nDelayTime);
        at_CmdRespInfoNText(pCmdResult->engine, pCmdResult->pData, pCmdResult->nDataSize);
        at_CmdSetPromptMode(pCmdResult->engine);
        break;

    case CMD_FUNC_WAIT_IP_DATA:
        AT_SetAsyncTimerMux(pCmdResult->engine, pCmdResult->nDelayTime);
        at_CmdRespInfoNText(pCmdResult->engine, pCmdResult->pData, pCmdResult->nDataSize);
        at_CmdSetPromptMode(pCmdResult->engine);
        break;

    case CMD_FUNC_CONTINUE:
        at_CmdRespInfoNText(pCmdResult->engine, pCmdResult->pData, pCmdResult->nDataSize);
        break;

    case CSW_IND_NOTIFICATION:
        at_CmdRespUrcText(pCmdResult->engine, pCmdResult->pData);
        break;

    default:
        break;
    }
    return r;
}

// =============================================================================
// at_CmdClearRemainCmds
// =============================================================================
void at_CmdClearRemainCmds(AT_CMD_ENGINE_T *engine)
{
    AT_ModuleInfo *module = at_CmdGetModule(engine);
    if (module->cmds == NULL)
    {
        ATCMDLOG(E, "AT CMD%d not in cmd to clear remain cmds", at_CmdGetChannel(engine));
        return;
    }

    ATCMDLOG(I, "AT CMD%d clear remain cmds", at_CmdGetChannel(engine));
    AT_COMMAND_T *next = module->cmds->next;
    at_CmdlistFreeAll(&next);
}

// =============================================================================
// at_CmdRespInfoNText/at_CmdRespInfoText
// =============================================================================
void at_CmdRespInfoNText(AT_CMD_ENGINE_T *engine, const uint8_t *text, unsigned length)
{
    if (text == NULL || length == 0)
        return;

    ATCMDLOGS(I, TSM(2), "AT CMD%d info text len=%d: %s", at_CmdGetChannel(engine), length, text);

    AT_ModuleInfo *module = at_CmdGetModule(engine);
    uint8_t crlf[2] = {gATCurrentnS3, gATCurrentnS4};
    uint8_t atv = gATCurrenteResultcodeFormat;

#ifndef CMUX_SEND_CRLF_WITH_DATA
    if (module->firstInfoText && atv == 1)
        at_CmdWrite(engine, (const uint8_t *)crlf, 2);
    module->firstInfoText = false;
    at_CmdWrite(engine, (const uint8_t *)text, length);
    at_CmdWrite(engine, (const uint8_t *)crlf, 2);
#else
    unsigned send = 0;

    if (module->firstInfoText && atv == 1)
    {
        memcpy(g_rspStrEx + send, crlf, 2);
        send += 2;
    }
    module->firstInfoText = false;
    memcpy(g_rspStrEx + send, text, length);
    send += length;

    memcpy(g_rspStrEx + send, crlf, 2);
    send += 2;
    at_CmdWrite(engine, g_rspStrEx, send);
#endif
}

void at_CmdRespInfoNTextBegin(AT_CMD_ENGINE_T *engine, const uint8_t *text, unsigned length)
{
    if (text == NULL)
        return;

    ATCMDLOGS(I, TSM(2), "AT CMD%d info text (begin) len=%d: %s", at_CmdGetChannel(engine), length, text);

    AT_ModuleInfo *module = at_CmdGetModule(engine);
    uint8_t crlf[2] = {gATCurrentnS3, gATCurrentnS4};
    uint8_t atv = gATCurrenteResultcodeFormat;

#ifndef CMUX_SEND_CRLF_WITH_DATA
    if (module->firstInfoText && atv == 1)
        at_CmdWrite(engine, (const uint8_t *)crlf, 2);
    module->firstInfoText = false;
    if (length != 0)
        at_CmdWrite(engine, (const uint8_t *)text, length);
#else
    unsigned send = 0;

    if (module->firstInfoText && atv == 1)
    {
        memcpy(g_rspStrEx + send, crlf, 2);
        send += 2;
    }
    module->firstInfoText = false;
    if (length != 0)
    {
        memcpy(g_rspStrEx + send, text, length);
        send += length;
    }    
    at_CmdWrite(engine, g_rspStrEx, send);
#endif
}

void at_CmdRespInfoNTextEnd(AT_CMD_ENGINE_T *engine, const uint8_t *text, unsigned length)
{
    if (text == NULL)
        return;

    ATCMDLOGS(I, TSM(2), "AT CMD%d info text (end) len=%d: %s", at_CmdGetChannel(engine), length, text);

    uint8_t crlf[2] = {gATCurrentnS3, gATCurrentnS4};
#ifndef CMUX_SEND_CRLF_WITH_DATA
    if (length != 0)
        at_CmdWrite(engine, (const uint8_t *)text, length);
    at_CmdWrite(engine, (const uint8_t *)crlf, 2);
#else
    unsigned send = 0;
    if (length != 0)
    {
        memcpy(g_rspStrEx + send, text, length);
        send += length;
    }  
    memcpy(g_rspStrEx + send, crlf, 2);
    send += 2;
    at_CmdWrite(engine, g_rspStrEx, send);
#endif
}

void at_CmdRespInfoText(AT_CMD_ENGINE_T *engine, const uint8_t *text) { at_CmdRespInfoNText(engine, text, strlen((const char*)text)); }
void at_CmdRespInfoTextBegin(AT_CMD_ENGINE_T *engine, const uint8_t *text) { at_CmdRespInfoNTextBegin(engine, text, strlen((const char*)text)); }
void at_CmdRespInfoTextEnd(AT_CMD_ENGINE_T *engine, const uint8_t *text) { at_CmdRespInfoNTextEnd(engine, text, strlen((const char*)text)); }

// =============================================================================
// at_CmdRespOutputResultCode
// =============================================================================
static void at_CmdRespOutputResultCode(AT_CMD_ENGINE_T *engine, int code)
{
    uint8_t atq = gATCurrenteResultcodePresentation;
    if (atq != 0)
        return;

    uint8_t resp[32];
    uint8_t *p = resp;
    uint8_t atv = gATCurrenteResultcodeFormat;
    if (atv == 0)
    {
        p += AT_Sprintf((char*)p, "%u", code);
        *p++ = gATCurrentnS3;
    }
    else
    {
        *p++ = gATCurrentnS3;
        *p++ = gATCurrentnS4;

        const uint8_t *s = at_GetResultCodeInfo(code);
        unsigned len = strlen((const char*)s);
        memcpy(p, s, len);
        p += len;

        *p++ = gATCurrentnS3;
        *p++ = gATCurrentnS4;
    }
    at_CmdWrite(engine, (const uint8_t *)resp, p - resp);
}

// =============================================================================
// at_CmdFinalHandle
// =============================================================================
static void at_CmdFinalHandle(AT_CMD_ENGINE_T *engine, bool ok)
{
    AT_ModuleInfo *module = at_CmdGetModule(engine);

    AT_KillAsyncTimerMux(engine);
    if (ok)
        at_CmdlistFree(at_CmdlistPopFront(&module->cmds));
    else
        at_CmdlistFreeAll(&module->cmds);

    at_CmdSetLineMode(engine);
    if (module->cmds != NULL)
        at_ModuleScheduleNextCommand(engine);
}

// =============================================================================
// at_CmdRespOK
// =============================================================================
void at_CmdRespOK(AT_CMD_ENGINE_T *engine)
{
    ATCMDLOG(I, "AT CMD%d OK", at_CmdGetChannel(engine));

    at_CmdFinalHandle(engine, true);

    AT_ModuleInfo *module = at_CmdGetModule(engine);
    if (module->cmds == NULL)
        at_CmdRespOutputResultCode(engine, CMD_RC_OK);

    at_CmdCommandFinished(engine);
}

// =============================================================================
// at_CmdRespOKText
// =============================================================================
void at_CmdRespOKText(AT_CMD_ENGINE_T *engine, const uint8_t *text)
{
    ATCMDLOGS(I, TSM(1), "AT CMD%d OK: %s", at_CmdGetChannel(engine), text);

    at_CmdFinalHandle(engine, true);

    // output it even it is not the last one, and not affected by ATV, ATQ
    uint8_t crlf[2] = {gATCurrentnS3, gATCurrentnS4};
#ifndef CMUX_SEND_CRLF_WITH_DATA
    at_CmdWrite(engine, (const uint8_t *)crlf, 2);
    at_CmdWrite(engine, (const uint8_t *)text, strlen((const char*)text));
    at_CmdWrite(engine, (const uint8_t *)crlf, 2);
#else
    unsigned send = 0;    

    memcpy(g_rspStrEx + send, crlf, 2);
    send += 2;
    memcpy(g_rspStrEx + send, text, strlen((const char*)text));
    send += strlen((const char*)text);
    memcpy(g_rspStrEx + send, crlf, 2);
    send += 2;
    at_CmdWrite(engine, g_rspStrEx, send);
#endif

    at_CmdCommandFinished(engine);
}

// =============================================================================
// at_CmdRespError
// =============================================================================
void at_CmdRespError(AT_CMD_ENGINE_T *engine)
{
    ATCMDLOG(I, "AT CMD%d ERROR", at_CmdGetChannel(engine));

    at_CmdRespErrorCode(engine, CMD_RC_ERROR);

    at_CmdCommandFinished(engine);
}

// =============================================================================
// at_CmdRespErrorText
// =============================================================================
void at_CmdRespErrorText(AT_CMD_ENGINE_T *engine, const uint8_t *text)
{
    ATCMDLOGS(I, TSM(1), "AT CMD%d ERROR: %s", at_CmdGetChannel(engine), text);

    at_CmdFinalHandle(engine, false);

    // not affected by ATV, ATQ
    uint8_t crlf[2] = {gATCurrentnS3, gATCurrentnS4};
#ifndef CMUX_SEND_CRLF_WITH_DATA
    at_CmdWrite(engine, (const uint8_t *)crlf, 2);
    at_CmdWrite(engine, (const uint8_t *)text, strlen((const char*)text));
    at_CmdWrite(engine, (const uint8_t *)crlf, 2);
#else
    unsigned send = 0;    

    memcpy(g_rspStrEx + send, crlf, 2);
    send += 2;
    memcpy(g_rspStrEx + send, text, strlen((const char*)text));
    send += strlen((const char*)text);
    memcpy(g_rspStrEx + send, crlf, 2);
    send += 2;
    at_CmdWrite(engine, g_rspStrEx, send);
#endif

    at_CmdCommandFinished(engine);
}

// =============================================================================
// at_CmdRespErrorCode
// =============================================================================
void at_CmdRespErrorCode(AT_CMD_ENGINE_T *engine, int code)
{
    ATCMDLOG(I, "AT CMD%d ERROR code=%d", at_CmdGetChannel(engine), code);

    at_CmdFinalHandle(engine, false);
    at_CmdRespOutputResultCode(engine, code);

    at_CmdCommandFinished(engine);
}

// =============================================================================
// at_CmdRespIntermCode
// =============================================================================
void at_CmdRespIntermCode(AT_CMD_ENGINE_T *engine, int code)
{
    ATCMDLOG(I, "AT CMD%d interm code=%d", at_CmdGetChannel(engine), code);

    at_CmdRespOutputResultCode(engine, code);
}

// =============================================================================
// at_CmdRespUrcCode
// =============================================================================
void at_CmdRespUrcCode(AT_CMD_ENGINE_T *engine, int code)
{
    ATCMDLOG(I, "AT CMD%d urc code=%d", at_CmdGetChannel(engine), code);

    uint8_t atq = gATCurrenteResultcodePresentation;
    if (atq != 0)
        return;
    
#if defined(AT_CMUX_SUPPORT) && defined(AT_URC_2_ALL_MUX_CHANNEL)
    AT_CMD_ENGINE_T *engines[MAX_DLC_NUM] = {NULL};
    unsigned count = at_DispatchGetAllCmdEngine(engines, MAX_DLC_NUM);
    for (unsigned i = 0; i < count; i++)
    {
        if (engines[i])
            at_CmdRespOutputResultCode(engines[i], code);
    }
#else
    // TODO: CMER is not considered
    at_CmdRespOutputResultCode(engine, code);
#endif
}

// =============================================================================
// at_CmdRespUrcNText/at_CmdRespUrcText
// =============================================================================
void at_CmdRespUrcNText(AT_CMD_ENGINE_T *engine, const uint8_t *text, unsigned length)
{
    ATCMDLOGS(I, TSM(2), "AT CMD%d urc len=%d: %s", at_CmdGetChannel(engine), length, text);

    uint8_t atq = gATCurrenteResultcodePresentation;
    if (atq != 0)
        return;

    // TODO: CMER is not considered
    uint8_t crlf[2] = {gATCurrentnS3, gATCurrentnS4};
#if defined(AT_CMUX_SUPPORT) && defined(AT_URC_2_ALL_MUX_CHANNEL)
    AT_CMD_ENGINE_T *engines[MAX_DLC_NUM] = {NULL};
    unsigned count = at_DispatchGetAllCmdEngine(engines, MAX_DLC_NUM);
    for (unsigned i = 0; i < count; i++)
    {
        if (engines[i])
        {
            at_CmdWrite(engines[i], (const uint8_t *)crlf, 2);
            at_CmdWrite(engines[i], (const uint8_t *)text, length);
            at_CmdWrite(engines[i], (const uint8_t *)crlf, 2);
        }
    }
#else
    memset(g_rspStrEx, 0x00, sizeof(g_rspStrEx));
    memcpy(g_rspStrEx, crlf, 2);
    memcpy(g_rspStrEx + 2, text, length);
    memcpy(g_rspStrEx + 2 + length, crlf, 2);
    at_CmdWrite(engine, g_rspStrEx, length + 4);
#endif
}

void at_CmdRespUrcText(AT_CMD_ENGINE_T *engine, const uint8_t *text) { at_CmdRespUrcNText(engine, text, strlen((const char*)text)); }

// =============================================================================
// at_CmdRespCmeError
// =============================================================================
void at_CmdRespCmeError(AT_CMD_ENGINE_T *engine, int code)
{
    ATCMDLOG(I, "AT CMD%d CME error code=%d", at_CmdGetChannel(engine), code);

    at_CmdFinalHandle(engine, false);

    uint8_t atq = gATCurrenteResultcodePresentation;
    if (atq == 0)
    {
        uint8_t cmee = gATCurrentuCmee;
        if (cmee == 0)
        {
            at_CmdRespOutputResultCode(engine, CMD_RC_ERROR);
        }
        else
        {
            uint8_t crlf[2] = {gATCurrentnS3, gATCurrentnS4};
#ifndef CMUX_SEND_CRLF_WITH_DATA
            at_CmdWrite(engine, (const uint8_t *)crlf, 2);
            if (cmee == 1)
            {
                uint8_t str[256];
                AT_Sprintf((char*)str, "+CME ERROR: %d", code);
                at_CmdWrite(engine, (const uint8_t *)str, strlen((const char*)str));
            }
            else
            {
                const uint8_t *str = at_GetCmeCodeInfo(code);
                at_CmdWrite(engine, (const uint8_t *)"+CME ERROR: ", strlen((const char*)"+CME ERROR: "));
                at_CmdWrite(engine, (const uint8_t *)str, strlen((const char*)str));
            }
            at_CmdWrite(engine, (const uint8_t *)crlf, 2);
#else
            unsigned send = 0;    
            //at_CmdWrite(engine, (const uint8_t *)crlf, 2);
            memcpy(g_rspStrEx + send, crlf, 2);
            send += 2;
            if (cmee == 1)
            {
                uint8_t str[256];
                AT_Sprintf((char*)str, "+CME ERROR: %d", code);
                //at_CmdWrite(engine, (const uint8_t *)str, strlen((const char*)str));
                memcpy(g_rspStrEx + send, str, strlen((const char*)str));
                send += strlen((const char*)str);
            }
            else
            {
                const uint8_t *str = at_GetCmeCodeInfo(code);
                //at_CmdWrite(engine, (const uint8_t *)"+CME ERROR: ", strlen((const char*)"+CME ERROR: "));
                memcpy(g_rspStrEx + send, (const uint8_t *)"+CME ERROR: ", strlen((const char*)"+CME ERROR: "));
                send += strlen((const char*)"+CME ERROR: ");
                //at_CmdWrite(engine, (const uint8_t *)str, strlen((const char*)str));
                memcpy(g_rspStrEx + send, str, strlen((const char*)str));
                send += strlen((const char*)str);
            }
            //at_CmdWrite(engine, (const uint8_t *)crlf, 2);
            memcpy(g_rspStrEx + send, crlf, 2);
            send += 2;
            at_CmdWrite(engine, g_rspStrEx, send);
#endif
        }
    }

    at_CmdCommandFinished(engine);
}

// =============================================================================
// at_CmdRespCmsError
// =============================================================================
void at_CmdRespCmsError(AT_CMD_ENGINE_T *engine, int code)
{
    ATCMDLOG(I, "AT CMD%d CMS error code=%d", at_CmdGetChannel(engine), code);

    at_CmdFinalHandle(engine, false);

    uint8_t atq = gATCurrenteResultcodePresentation;
    if (atq == 0)
    {
        uint8_t cmee = gATCurrentuCmee;
        if (cmee == 0)
        {
            at_CmdRespErrorCode(engine, CMD_RC_ERROR);
        }
        else
        {
            // not affected by ATV
            uint8_t crlf[2] = {gATCurrentnS3, gATCurrentnS4};
#ifndef CMUX_SEND_CRLF_WITH_DATA
            at_CmdWrite(engine, (const uint8_t *)crlf, 2);
            if (cmee == 1)
            {
                uint8_t str[256];
                AT_Sprintf((char*)str, "+CMS ERROR: %d", code);
                at_CmdWrite(engine, (const uint8_t *)str, strlen((const char*)str));
            }
            else
            {
                const uint8_t *str = at_GetCmsCodeInfo(code);
                at_CmdWrite(engine, (const uint8_t *)"+CMS ERROR: ", strlen((const char*)"+CMS ERROR: "));
                at_CmdWrite(engine, (const uint8_t *)str, strlen((const char*)str));
            }
            at_CmdWrite(engine, (const uint8_t *)crlf, 2);
#else
            unsigned send = 0;    
            //at_CmdWrite(engine, (const uint8_t *)crlf, 2);
            memcpy(g_rspStrEx + send, crlf, 2);
            send += 2;
            if (cmee == 1)
            {
                uint8_t str[256];
                AT_Sprintf((char*)str, "+CMS ERROR: %d", code);
                //at_CmdWrite(engine, (const uint8_t *)str, strlen((const char*)str));
                memcpy(g_rspStrEx + send, str, strlen((const char*)str));
                send += strlen((const char*)str);
            }
            else
            {
                const uint8_t *str = at_GetCmsCodeInfo(code);
                //at_CmdWrite(engine, (const uint8_t *)"+CMS ERROR: ", strlen((const char*)"+CMS ERROR: "));
                memcpy(g_rspStrEx + send, (const uint8_t *)"+CMS ERROR: ", strlen((const char*)"+CMS ERROR: "));
                send += strlen((const char*)"+CMS ERROR: ");
                //at_CmdWrite(engine, (const uint8_t *)str, strlen((const char*)str));
                memcpy(g_rspStrEx + send, str, strlen((const char*)str));
                send += strlen((const char*)str);
            }
            //at_CmdWrite(engine, (const uint8_t *)crlf, 2);
            memcpy(g_rspStrEx + send, crlf, 2);
            send += 2;
#endif
        }
    }

    at_CmdCommandFinished(engine);
}

// =============================================================================
// at_CmdRespIntermText
// =============================================================================
void at_CmdRespIntermText(AT_CMD_ENGINE_T *engine, const uint8_t *text)
{
    ATCMDLOGS(I, TSM(1), "AT CMD%d interm: %s", at_CmdGetChannel(engine), text);
    at_CmdWrite(engine, (const uint8_t *)text, strlen((const char*)text));
}

// =============================================================================
// at_CmdReportUrcCode
// =============================================================================
void at_CmdReportUrcCode(uint8_t sim, int code)
{
    ATCMDLOG(I, "AT CMD sim%d urc code=%d", sim, code);

    AT_DISPATCH_T *ch = at_DispatchFindByChannel(at_GetUrcChannel());
    if (ch == NULL)
        return;
    
    if (at_DispatchIsCmuxMode(ch))
    {
        ch = at_DispatchFindByChannel(at_GetUrcChannel() + 1);
        if (ch == NULL)
        {
            return;
        }
    }
    
    at_CmdRespUrcCode(at_DispatchGetCmdEngine(ch), code);
}

// =============================================================================
// at_CmdReportUrcText
// =============================================================================
void at_CmdReportUrcText(uint8_t sim, const uint8_t *text)
{
    ATCMDLOGS(I, TSM(1), "AT CMD sim%d urc: %s", sim, text);

    AT_DISPATCH_T *ch = at_DispatchFindByChannel(at_GetUrcChannel());
    if (ch == NULL)
        return;
    
    if (at_DispatchIsCmuxMode(ch))
    {
        ch = at_DispatchFindByChannel(at_GetUrcChannel() + 1);
        if (ch == NULL)
        {
            return;
        }
    }
    
    at_CmdRespUrcText(at_DispatchGetCmdEngine(ch), text);
}

// =============================================================================
// at_CmdReportOutputText
// =============================================================================
void at_CmdReportOutputText(uint8_t sim, const uint8_t *text)
{
    ATCMDLOGS(I, TSM(1), "AT CMD sim%d urc: %s", sim, text);

    AT_DISPATCH_T *ch = at_DispatchFindByChannel(at_GetUrcChannel());
    if (ch == NULL)
        return;
    
    if (at_DispatchIsCmuxMode(ch))
    {
        ch = at_DispatchFindByChannel(at_GetUrcChannel() + 1);
        if (ch == NULL)
        {
            return;
        }
    }
    at_CmdRespOutputText(at_DispatchGetCmdEngine(ch), text);
}


// =============================================================================
// at_CmdRespOutputText
// =============================================================================
void at_CmdRespOutputText(AT_CMD_ENGINE_T *engine, const uint8_t *text)
{
    if (text == NULL)
        return;

    ATCMDLOGS(I, TSM(1), "AT CMD%d output: %s", at_CmdGetChannel(engine), text);
    at_CmdWrite(engine, (const uint8_t *)text, strlen((const char*)text));
}

// =============================================================================
// at_CmdRespOutputNText
// =============================================================================
void at_CmdRespOutputNText(AT_CMD_ENGINE_T *engine, const uint8_t *text, unsigned length)
{
    if (text == NULL || length == 0)
        return;

    ATCMDLOG(I, "AT CMD%d output length %d", at_CmdGetChannel(engine), length);
    at_CmdWrite(engine, (const uint8_t *)text, length);
}

// =============================================================================
// at_CmdRespOutputPrompt
// =============================================================================
void at_CmdRespOutputPrompt(AT_CMD_ENGINE_T *engine)
{
    uint8_t rsp[4] = {gATCurrentnS3, gATCurrentnS4, '>', ' '};
    at_CmdWrite(engine, (const uint8_t *)rsp, 4);
}

// =============================================================================
// at_CmdRingInd
// =============================================================================
void at_CmdRingInd(uint8_t sim)
{
    static const uint8_t cring[] = "+CRING: <voice>";
    if (gATCurrentCrc)
        at_CmdReportUrcText(sim, cring);
    else
        at_CmdReportUrcCode(sim, CMD_RC_RING);

    // TODO: other V.24 signals
}

void at_ModuleScheduleNextCommand(AT_CMD_ENGINE_T *engine)
{
    at_TaskCallback((AT_CALLBACK_FUNC_T)at_ModuleRunCommand, engine);
}

void at_ModuleRunCommand(AT_CMD_ENGINE_T *engine)
{
    AT_ModuleInfo *module = at_CmdGetModule(engine);
    AT_COMMAND_T *cmd = module->cmds;
    if (cmd == NULL)
        return;

    unsigned paramlen = 0;
    for (int n = 0; n < cmd->paramCount; n++)
    {
        if (n != 0)
            module->param[paramlen++] = ',';
        memcpy(&module->param[paramlen], cmd->params[n]->value, cmd->params[n]->length);
        paramlen += cmd->params[n]->length;
    }
    module->param[paramlen] = 0;

    ATCMDLOGS(I, TSM(1, 3), "AT CMD%d run %s type=%d param=%s", at_CmdGetChannel(engine),
              cmd->desc->pName, cmd->type, module->param);
    AT_CMD_PARA para = {};
    para.iType = cmd->type;
    para.pPara = module->param;
    para.uCmdStamp = 0; //pCmd->uCmdStamp;
    para.engine = engine;
    para.nDLCI = at_CmdGetChannel(engine);
    para.pExData = NULL;
    para.iExDataLen = 0;
    para.paramCount = cmd->paramCount;
    memcpy(para.params, cmd->params, sizeof(cmd->params));
    module->firstInfoText = true;
    cmd->desc->pFunc(&para);
}

void at_ModuleRunCommandExtra(AT_CMD_ENGINE_T *engine, const char *data, unsigned length)
{
    AT_ModuleInfo *module = at_CmdGetModule(engine);
    AT_COMMAND_T *cmd = module->cmds;

    if (cmd == NULL)
    {
        ATCMDLOG(E, "AT CMD%d no command for prompt", at_CmdGetChannel(engine));
        at_CmdSetLineMode(engine);
        return;
    }

    unsigned paramlen = 0;
    for (int n = 0; n < cmd->paramCount; n++)
    {
        if (n != 0)
            module->param[paramlen++] = ',';
        memcpy(&module->param[paramlen], cmd->params[n]->value, cmd->params[n]->length);
        paramlen += cmd->params[n]->length;
    }
    module->param[paramlen] = 0;

    ATCMDLOGS(I, TSM(1, 3), "AT CMD%d run %s type=%d param=%s extralen=%d",
              at_CmdGetChannel(engine), cmd->desc->pName, cmd->type, module->param, length);
    AT_CMD_PARA para = {};
    para.iType = cmd->type;
    para.pPara = module->param;
    para.uCmdStamp = 0; //pCmd->uCmdStamp;
    para.engine = engine;
    para.nDLCI = at_CmdGetChannel(engine);
    para.pExData = (UINT8 *)data;
    para.iExDataLen = length;
    para.paramCount = cmd->paramCount;
    memcpy(para.params, cmd->params, sizeof(cmd->params));
    cmd->desc->pFunc(&para);
}

static void at_ModuleClearCmdList(AT_CMD_ENGINE_T *engine)
{
    AT_ModuleInfo *module = at_CmdGetModule(engine);
    at_CmdlistFreeAll(&module->cmds);
    at_CmdSetLineMode(engine);
    return;
}

static void at_ModuleClearCurCmd(AT_CMD_ENGINE_T *engine)
{
    AT_ModuleInfo *module = at_CmdGetModule(engine);

    // if async timer exist, kill it.
    AT_KillAsyncTimerMux(engine);
    at_CmdlistFree(at_CmdlistPopFront(&module->cmds));
    at_CmdSetLineMode(engine);
    return;
}

int at_ModuleProcessLine(UINT8 *pBuf, UINT32 len, AT_CMD_ENGINE_T *engine)
{
    AT_ModuleInfo *module = at_CmdGetModule(engine);

    if (pBuf == NULL && len == 0)
        return -1;

    int result = at_ParseLine(pBuf, len, &module->cmds);
    if (result != 0)
    {
        ATCMDLOG(W, "AT CMD%d parser failed, error code %d", at_CmdGetChannel(engine), result);
        at_CmdlistFreeAll(&module->cmds);
        return result;
    }

    if (module->cmds != NULL)
    {
        //if the first AT command is +CMMS ,then record the number of +CMSS.
        if (AT_StrCaselessCmp((const char *)"+CMMS", (const char *)module->cmds->desc->pName) == 0)
            module->cmssNum = at_CmdlistCount(module->cmds) - 1;
    }

    return 0;
}


#define ERR_STR "COMMAND NO RESPONSE!"
VOID at_ModuleHandleTimeout(AT_CMD_ENGINE_T *engine)
{
    UINT8 crlf[2] = {gATCurrentnS3, gATCurrentnS4};
#ifndef CMUX_SEND_CRLF_WITH_DATA

    ATLOGW("AT CMD async timeout");

    AT_KillAsyncTimerMux(engine);

    // It's a ATE bug, delete this global flag after fixing bug by ATE.
    at_ModuleSetCopsFlag(at_ModuleGetChannelSimID(at_CmdGetChannel(engine)), FALSE);

    at_CmdWrite(engine, (const uint8_t *)crlf, 2);
    at_CmdWrite(engine, (const uint8_t *)ERR_STR, strlen((const char*)ERR_STR));
    at_CmdWrite(engine, (const uint8_t *)crlf, 2);
#else
    unsigned send = 0;    

    ATLOGW("AT CMD async timeout");

    AT_KillAsyncTimerMux(engine);

    // It's a ATE bug, delete this global flag after fixing bug by ATE.
    at_ModuleSetCopsFlag(at_ModuleGetChannelSimID(at_CmdGetChannel(engine)), FALSE);

    //at_CmdWrite(engine, (const uint8_t *)crlf, 2);
    memcpy(g_rspStrEx + send, crlf, 2);
    send += 2;
    //at_CmdWrite(engine, (const uint8_t *)ERR_STR, strlen((const char*)ERR_STR));
    memcpy(g_rspStrEx + send, ERR_STR, strlen((const char*)ERR_STR));
    send += strlen((const char*)ERR_STR);
    //at_CmdWrite(engine, (const uint8_t *)crlf, 2);
    memcpy(g_rspStrEx + send, crlf, 2);
    send += 2;
    at_CmdWrite(engine, g_rspStrEx, send);
#endif

    // Need more work to Verify Ind List
    UINT8 nDLCI = at_CmdGetChannel(engine);

    AT_DelWaitingEventOnDLCI(nDLCI);
    at_ModuleClearCmdList(engine);
    at_CmdSetLineMode(engine);
}

static void at_TimeoutAsync(void* param)
{
    // Async Command Time Over Process
    AT_CMD_ENGINE_T *engine = at_DispatchGetCmdEngine((AT_DISPATCH_T*)param);
    at_ModuleHandleTimeout(engine);
}

static void at_TimeoutAsyncFun(void *param)
{
    void *ctx;
    TIMER_HANDLE handle = (TIMER_HANDLE)param;
    
    ctx = at_GetTimerCtx(handle);
    at_TaskCallback(at_TimeoutAsync, ctx);
}

BOOL AT_SetAsyncTimerMux(AT_CMD_ENGINE_T *engine, UINT32 nElapse)
{
    AT_ModuleInfo *module = at_CmdGetModule(engine);

    if (module->cmds == NULL)
    {
        ATCMDLOG(W, "AT CMD%d set async timer period=%d no cmd", at_CmdGetChannel(engine), nElapse);
        return TRUE;
    }

    ATCMDLOGS(I, TSM(2), "AT CMD%d set async timer period=%d cmd=%s", at_CmdGetChannel(engine),
              nElapse, module->cmds->desc->pName);

    if (nElapse == 0)
        nElapse = AT_DEFAULT_ELAPSE;
    at_StartCallbackTimer(nElapse * 1000, (AT_CALLBACK_FUNC_T)at_TimeoutAsyncFun, at_CmdGetDispatch(engine));
    return TRUE;
}

BOOL AT_KillAsyncTimerMux(AT_CMD_ENGINE_T *engine)
{
    at_StopCallbackTimer((AT_CALLBACK_FUNC_T)at_TimeoutAsyncFun, at_CmdGetDispatch(engine));
    return TRUE;
}

int at_ModuleGetChannelSimID(int channel)
{
    AT_DISPATCH_T *dispatch = at_DispatchGetByChannel(channel);
    int dlci = at_DispatchGetDlci(dispatch);
    if (dlci < 0) // not CMUX channel
        return 0;
    return (dlci < MAX_DLC_NUM / 2) ? 0 : 1;
}

int at_ModuleGetSimDlci(int sim)
{
    return (sim == 0) ? 0 : (MAX_DLC_NUM / 2);
}


VOID AT_Clear_CurCMD(UINT8 nDLCI)
{
    at_ModuleClearCurCmd(at_CmdGetByChannel(nDLCI));
}

UINT8 AT_GetCMSSNum(UINT8 nDLCI)
{
    AT_ModuleInfo *module = at_CmdGetModule(at_CmdGetByChannel(nDLCI));
    return module->cmssNum;
}

VOID AT_SetCMSSNum(UINT8 nDLCI, UINT8 num)
{
    AT_ModuleInfo *module = at_CmdGetModule(at_CmdGetByChannel(nDLCI));
    module->cmssNum = num;
}

BOOL AT_IsAsynCmdAvailable(UINT8 *pName, UINT32 uCmdStamp, UINT8 nDLCI)
{
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(nDLCI);
    AT_ModuleInfo *module = at_CmdGetModule(engine);
    if (module->cmds != NULL &&
        AT_StrCmp(pName, module->cmds->desc->pName) == 0 /* &&
        module->cmds->uCmdStamp == uCmdStamp*/)
        return TRUE;
    return FALSE;
}

BOOL PM_SetBatteryChargeMode(UINT8 nMode)
{
    return FALSE;
}

typedef struct
{
    AT_CMD_ENGINE_T *engine;
    uint8_t *data;
    uint32_t data_len;
} ASYNC_WRITE_T;

static void write_uart_at_AT(void *param)
{
    ASYNC_WRITE_T *write = (ASYNC_WRITE_T *)param;
    at_CmdWrite(write->engine, (const uint8_t *)write->data, write->data_len);
    AT_FREE(write->data);
    AT_FREE(write);
}

void AT_AsyncWriteUart(AT_CMD_ENGINE_T *engine, const uint8_t *data, uint32_t data_len)
{
    ASYNC_WRITE_T *write = (ASYNC_WRITE_T *)AT_MALLOC(sizeof(ASYNC_WRITE_T));
    write->engine = engine;
    write->data = (uint8_t *)AT_MALLOC(data_len);
    write->data_len = data_len;
    memcpy(write->data, data, data_len);
    at_TaskCallback(write_uart_at_AT, write);
}

void AT_WriteUart(const uint8_t *data, unsigned length)
{
    at_CmdDirectWrite(data, length);
}

void AT_KillAsyncTimer(void)
{
    // TODO:
}

bool at_SetUrcChannel(int channel)
{
    gAtGlobal.urcChannel = channel;
    return true;
}

int at_GetUrcChannel() { return gAtGlobal.urcChannel; }
AT_MODULE_INIT_STATUS_T at_ModuleGetInitResult(void) { return gAtGlobal.uiModuleInitStatus; }
void at_ModuleSetInitResult(AT_MODULE_INIT_STATUS_T uiStatus) { gAtGlobal.uiModuleInitStatus = uiStatus; }
void at_ModuleSetSmsReady(bool ready) { gAtGlobal.smsReady = ready; }
bool at_ModuleGetSmsReady(void) { return gAtGlobal.smsReady; }
void at_ModuleSetPbkReady(bool ready) { gAtGlobal.pbkReady = ready; }
bool at_ModuleGetPbkReady(void) { return gAtGlobal.pbkReady; }
void at_ModuleSetCopsFlag(UINT8 sim, bool flag) { gAtGlobal.copsFlag[sim] = flag; }
bool at_ModuleGetCopsFlag(UINT8 sim) { return gAtGlobal.copsFlag[sim]; }


