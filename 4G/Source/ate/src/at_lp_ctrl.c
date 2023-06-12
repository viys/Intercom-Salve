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

#include "at_lp_ctrl.h"
#include "at_device.h"
#include "at_utils.h"
#include "at_cfg.h"

#define HAL_GPIO_NONE 0

#define DTR_GPIO (HAL_GPIO_NONE)

#define DTR_HIGH_FOR_LP true
#define DEVICE_MAX 2
#define DTR_STABLE_PERIOD 100

#define RX_TIMEOUT_PERIOD 3500  // 3.5s

struct AT_LP_CTRL_CONTEXT
{
    AT_LP_CTRL_MODE_T mode;
    bool lpcurr;
    bool lpnext;
    AT_DEVICE_T *devices[DEVICE_MAX];
};

typedef struct AT_LP_TIMER_CONTEXT
{
    bool isActive;
    TIMER_HANDLE handle;
    TIMER_ATTR_S attr;
} AT_LP_TIMER_T;

// singleton global variable
static struct AT_LP_CTRL_CONTEXT gLpCtrlCtx;
static AT_LP_TIMER_T gLpTimer;

// =============================================================================
// at_LpCtrlSetDeviceSleep
// =============================================================================
static void at_LpCtrlSetDeviceSleep(bool lp)
{
    if (lp == gLpCtrlCtx.lpcurr)
        return;

    gLpCtrlCtx.lpcurr = lp;
    bool wakeupEnabled = (gLpCtrlCtx.mode == AT_LP_CTRL_BY_RX) ? true : false;
    for (int n = 0; n < DEVICE_MAX; n++)
    {
        if (gLpCtrlCtx.devices[n] != NULL)
        {
            if (lp)
                at_DeviceSleep(gLpCtrlCtx.devices[n], wakeupEnabled);
            else
                at_DeviceWakeup(gLpCtrlCtx.devices[n]);
        }
    }
}

// =============================================================================
// at_LpCtrlTimeout
// =============================================================================
static void at_LpCtrlTimeout(void *param)
{
    at_LpCtrlSetDeviceSleep(gLpCtrlCtx.lpnext);
}

// =============================================================================
// at_LpCtrlDtrUpdateLevel
// =============================================================================
static void at_LpCtrlDtrUpdateLevel(void)
{
    if (DTR_GPIO == HAL_GPIO_NONE)
        return;
}

// =============================================================================
// at_LpCtrlDtrGpioIrq
// =============================================================================
static void POSSIBLY_UNUSED at_LpCtrlDtrGpioIrq(UINT32 param)
{
    at_LpCtrlDtrUpdateLevel();
    //TODO
    //at_StartCallbackTimer(DTR_STABLE_PERIOD, at_LpCtrlTimeout);
}

// =============================================================================
// at_LpCtrlDtrGpioOpen
// =============================================================================
static void at_LpCtrlDtrGpioOpen(void)
{
    if (DTR_GPIO == HAL_GPIO_NONE)
        return;
}

// =============================================================================
// at_LpCtrlInputReceived
// =============================================================================
void at_LpCtrlInputReceived(void)
{
    if (gLpCtrlCtx.mode != AT_LP_CTRL_BY_RX)
        return;

    at_LpCtrlSetDeviceSleep(false);
    //at_StartCallbackTimer(RX_TIMEOUT_PERIOD, at_LpCtrlTimeout);
    if (true == gLpTimer.isActive)
    {
        KING_TimerDeactive(gLpTimer.handle);
        KING_Sleep(10);
    }

    KING_TimerActive(gLpTimer.handle, &gLpTimer.attr);
    gLpTimer.isActive = true;
}

// =============================================================================
// at_LpCtrlOutputRequest
// =============================================================================
bool at_LpCtrlOutputRequest(void)
{
    at_LpCtrlSetDeviceSleep(false);
    if (gLpCtrlCtx.mode == AT_LP_CTRL_BY_RX)
    {        
        //at_StopCallbackTimer(at_LpCtrlTimeout);
        //at_StartCallbackTimer(RX_TIMEOUT_PERIOD, at_LpCtrlTimeout);        
        if (true == gLpTimer.isActive)
        {
            KING_TimerDeactive(gLpTimer.handle);
            KING_Sleep(10);
        }
        
        KING_TimerActive(gLpTimer.handle, &gLpTimer.attr);
        gLpTimer.isActive = true;
    }
    else if (gLpCtrlCtx.mode == AT_LP_CTRL_BY_DTR)
    {
    	//TODO
        //at_StopCallbackTimer(at_LpCtrlTimeout);
        //at_StartCallbackTimer(DTR_STABLE_PERIOD, at_LpCtrlTimeout);
    }

    return true;
}

// =============================================================================
// at_LpCtrlSetMode
// =============================================================================
bool at_LpCtrlSetMode(AT_LP_CTRL_MODE_T mode)
{
    if (mode == gLpCtrlCtx.mode)
        return true;

    //at_StopCallbackTimer(at_LpCtrlTimeout);
    if (true == gLpTimer.isActive)
    {
        KING_TimerDeactive(gLpTimer.handle);
        gLpTimer.isActive = false;
    }

    gLpCtrlCtx.mode = mode;

    if (mode == AT_LP_CTRL_BY_DTR)
    {
        if (DTR_GPIO != HAL_GPIO_NONE)
        {
            at_LpCtrlDtrGpioOpen();
            at_LpCtrlDtrUpdateLevel();
            //TODO
            //at_StartCallbackTimer(DTR_STABLE_PERIOD, at_LpCtrlTimeout);
        }
        else
        {
            gLpCtrlCtx.lpnext = false;
            at_LpCtrlSetDeviceSleep(false);
        }
    }
    else if (mode == AT_LP_CTRL_BY_RX)
    {       
        gLpCtrlCtx.lpnext = true;
        //at_StartCallbackTimer(RX_TIMEOUT_PERIOD, at_LpCtrlTimeout);
        KING_TimerActive(gLpTimer.handle, &gLpTimer.attr);
        gLpTimer.isActive = true;
    }
    else
    {
        gLpCtrlCtx.lpnext = false;
        at_LpCtrlSetDeviceSleep(false);
    }
    return true;
}

// =============================================================================
// at_LpCtrlGetMode
// =============================================================================
AT_LP_CTRL_MODE_T at_LpCtrlGetMode(void)
{
    return gLpCtrlCtx.mode;
}

// =============================================================================
// at_LpCtrlSetDevice
// =============================================================================
bool at_LpCtrlSetDevice(AT_DEVICE_T *device)
{
    for (int n = 0; n < DEVICE_MAX; n++)
    {
        if (gLpCtrlCtx.devices[n] == device)
            return true;
        if (gLpCtrlCtx.devices[n] == NULL)
        {
            gLpCtrlCtx.devices[n] = device;
            return true;
        }
    }
    return false;
}

// =============================================================================
// at_LpCtrlUnsetDevice
// =============================================================================
void at_LpCtrlUnsetDevice(AT_DEVICE_T *device)
{
    for (int n = 0; n < DEVICE_MAX; n++)
    {
        if (gLpCtrlCtx.devices[n] == device)
        {
            gLpCtrlCtx.devices[n] = NULL;
            break;
        }
    }
}

// =============================================================================
// at_LpCtrlInit
// =============================================================================
void at_LpCtrlInit(void)
{
    gLpCtrlCtx.mode = AT_LP_CTRL_DISABLE;
    gLpCtrlCtx.lpcurr = false;
    gLpCtrlCtx.lpnext = false;

    gLpTimer.isActive = false;
    if (FAIL == KING_TimerCreate(&gLpTimer.handle))
    {
        ATLOGE("%s: create timer fail 0x%x\r\n", __func__, KING_GetLastErrCode());
    }
    gLpTimer.attr.timer_fun = (TIMER_FUN)at_LpCtrlTimeout;
    gLpTimer.attr.timeout = RX_TIMEOUT_PERIOD;
    gLpTimer.attr.isPeriod = false;

    if (gATCurrentSleepMode != (uint8_t)gLpCtrlCtx.mode)
    {
        at_LpCtrlSetMode((AT_LP_CTRL_MODE_T)gATCurrentSleepMode);
    }
}
