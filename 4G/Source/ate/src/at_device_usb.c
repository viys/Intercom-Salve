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

#include "at_cmd_engine.h"
#include "at_device_usb.h"
#include "at_dispatch.h"
#include "at_utils.h"
#include "KingUart.h"
#include "KingUsb.h"
#include "kingplat.h"
#include "KingPowerManage.h"
#include "at_lp_ctrl.h"

#define AT_USB_ID 5

#define XCPU_CACHE_LINE_SIZE (16)
#define XCPU_CACHE_LINE_MASK (XCPU_CACHE_LINE_SIZE - 1)

#define USB_RX_DMA_SIZE (256)
#define USB_RX_BUF_SIZE (4 * 1024)
#define USB_TX_BUF_SIZE (4 * 1024)
#define USB_RX_POLL_PERIOD (15) // ms

#define USB_MIN_TX_DONE_TIMEOUT (500)

#define USB_POOL_SIZE (USB_RX_DMA_SIZE + USB_RX_BUF_SIZE + USB_TX_BUF_SIZE + XCPU_CACHE_LINE_SIZE)

typedef struct
{
    AT_DEVICE_T ops; // API
    AT_DEVICE_USB_CONFIG_T config; // USB configuration

    uint32_t send_timeout;  // send data timeout
    bool sleep; // whether in sleep
} AT_DEVICE_USB_T;

extern struct AT_DISPATCH_T *g_dispatch[AT_DISPATCH_ID_MAX];


static void at_UsbDestroy(AT_DEVICE_T *th);
static bool at_UsbOpen(AT_DEVICE_T *th);
static bool at_UsbReopen(AT_DEVICE_T *th);
static void at_UsbClose(AT_DEVICE_T *th);
static int at_UsbWrite(AT_DEVICE_T *th, const uint8_t *data, unsigned length);
static int at_UsbRead(AT_DEVICE_T *th, uint8_t *data, unsigned size);
static void at_UsbSleep(AT_DEVICE_T *th, bool wakeupEnabled);
static void at_UsbWakeup(AT_DEVICE_T *th);
static bool at_UsbIsBaudSupported(AT_DEVICE_T *th, int baud);
static void at_UsbCallback(uint32 eventID, void* pData, uint32 len);
static bool at_UsbSetFlowCtrl(AT_DEVICE_T *th, uint8_t rxfc, uint8_t txfc);
static void at_UsbSetFormat(AT_DEVICE_T *th, int baud, AT_DEVICE_FORMAT_T format, AT_DEVICE_PARITY_T parity);

static inline  UART_PARITY_E at_UsbDrvParity(AT_DEVICE_PARITY_T parity)
{
    switch (parity)
    {
    case AT_DEVICE_PARITY_ODD:
        return UART_ODD_PARITY;
    case AT_DEVICE_PARITY_EVEN:
        return UART_EVEN_PARITY;
    default:
        return UART_NO_PARITY;
    }
}

// =============================================================================
// at_UsbCreate
// =============================================================================
AT_DEVICE_T *at_UsbCreate(AT_DEVICE_USB_CONFIG_T *cfg, uint8_t devno)
{
    unsigned memsize = sizeof(AT_DEVICE_USB_T) + USB_POOL_SIZE;
    AT_DEVICE_USB_T *usb = (AT_DEVICE_USB_T *)at_malloc(memsize);
    memset(usb, 0, sizeof(*usb));

    usb->config = *cfg;
    ATUARTLOG(I, "AT USB%d create: baud/%d", usb->config.id, cfg->baud);

    usb->ops.devno = devno;
    usb->ops.destroy = at_UsbDestroy;
    usb->ops.open = at_UsbOpen;
    usb->ops.close = at_UsbClose;
    usb->ops.write = at_UsbWrite;
    usb->ops.read = at_UsbRead;
    usb->ops.sleep = at_UsbSleep;
    usb->ops.wakeup = at_UsbWakeup;
    usb->ops.isBaudSupported = at_UsbIsBaudSupported;
    usb->ops.setFlowCtrl = at_UsbSetFlowCtrl;
    usb->ops.setFormat = at_UsbSetFormat;

    uint32_t rate = cfg->baud / 10;
    if (rate == 0)
        rate = 9600;
    usb->send_timeout = 2 * 1000 * USB_RX_BUF_SIZE / rate;
    if (usb->send_timeout < USB_MIN_TX_DONE_TIMEOUT)
        usb->send_timeout = USB_MIN_TX_DONE_TIMEOUT;
    
    return (AT_DEVICE_T *)usb;
}

// =============================================================================
// at_UsbDestroy
// =============================================================================
static void at_UsbDestroy(AT_DEVICE_T *th)
{
    AT_DEVICE_USB_T *usb = (AT_DEVICE_USB_T *)th;

    if (usb == NULL)
        return;

    ATUARTLOG(I, "AT USB%d destroy", usb->config.id);
    KING_UsbDeinit(usb->config.id);
    KING_UnRegNotifyFun(DEV_CLASS_USB, usb->config.id, at_UsbCallback);
    at_free(usb);
}

// =============================================================================
// Sleep
// =============================================================================
static void at_UsbSleep(AT_DEVICE_T *th, bool wakeupEnabled)
{
    AT_DEVICE_USB_T *usb = (AT_DEVICE_USB_T *)th;
    if (usb->sleep)
        return;

    ATUARTLOG(I, "AT USB%d sleep", usb->config.id);
    //DRV_UsbSleep(usb->drv, false, wakeupEnabled);
    usb->sleep = true;    
    KING_WakeUnlock(LOCK_SUSPEND, (uint8 *)"Modem");
}

// =============================================================================
// Wakeup
// =============================================================================
static void at_UsbWakeup(AT_DEVICE_T *th)
{
    AT_DEVICE_USB_T *usb = (AT_DEVICE_USB_T *)th;
    if (!usb->sleep)
        return;

    ATUARTLOG(I, "AT USB%d wakeup", usb->config.id);
    //DRV_UsbWakeup(usb->drv);   
    KING_WakeLock(LOCK_SUSPEND, (uint8 *)"Modem");
    usb->sleep = false;
}

// =============================================================================
// Write
// =============================================================================
static int at_UsbWrite(AT_DEVICE_T *th, const uint8_t *data, unsigned length)
{
    AT_DEVICE_USB_T *usb = (AT_DEVICE_USB_T *)th;
    uint32 trans = 0;

    if (data == NULL || length == 0)
        return 0;

    ATUARTLOG(I, "AT USB%d write: len=%d", usb->config.id, length);

    at_LpCtrlOutputRequest();

    KING_UsbWrite(usb->config.id, (uint8_t*)data, length, &trans);
    if (trans == 0)
        ATUARTLOG(E, "AT USB%d output error %d", usb->config.id, trans);
    else if (trans < length)
        ATUARTLOG(E, "AT USB%d output overflow, drop %d bytes", usb->config.id, length - trans);
    return trans;
}

static void at_UsbCallback(uint32 eventID, void* pData, uint32 len)
{
    AT_DEVICE_T* device = NULL;
    switch(eventID)
    {
        case USB_EVT_DATA_TO_READ:
            if(g_dispatch[AT_DISPATCH_ID_USB] != NULL)
            {
                device = at_DispatchGetDevice(g_dispatch[AT_DISPATCH_ID_USB]);
                if(device != NULL)
                {
                    device->wakeup(device);
                    at_DeviceScheduleRead(device);
                    at_LpCtrlInputReceived();
                }
            }
            break;

        case USB_EVT_WRITE_COMPLETE:
            break;

        default:
            ATUARTLOG(E,"%s: eventid = %d !!!", __FUNCTION__, eventID);
            return;
    }
}

// =============================================================================
// Read
// =============================================================================
static int at_UsbRead(AT_DEVICE_T *th, uint8_t *data, unsigned size)
{
    AT_DEVICE_USB_T *usb = (AT_DEVICE_USB_T *)th;
    uint32 trans = 0;
    if (usb->sleep)
        return 0;
    if (data == NULL || size == 0)
        return 0;
    KING_UsbRead(usb->config.id, data, size, &trans);
    if (trans == 0)
        ATUARTLOG(E, "AT USB%d read error %d", usb->config.id, trans);
    else
        ATUARTLOG(E, "AT USB%d read %d, got %d", usb->config.id, size, trans);
    return trans;
}

// =============================================================================
// Open
// =============================================================================
static bool at_UsbOpen(AT_DEVICE_T *th)
{
    AT_DEVICE_USB_T *usb = (AT_DEVICE_USB_T *)th;
    int ret = -1;

    ATUARTLOG(I, "AT USB%d open", usb->config.id);

    uint32_t rate = usb->config.baud / 10;
    if (rate == 0)
        rate = 9600;
    usb->send_timeout = 2 * 1000 * USB_RX_BUF_SIZE / rate;
    if (usb->send_timeout < USB_MIN_TX_DONE_TIMEOUT)
        usb->send_timeout = USB_MIN_TX_DONE_TIMEOUT;

    if (SUCCESS != KING_UsbInit(usb->config.id))
    {
        ret = KING_GetLastErrCode();
        ATUARTLOG(I, "AT USB%d Init fail %d", usb->config.id,ret);
        return false;
    }
    
    if(SUCCESS != KING_RegNotifyFun(DEV_CLASS_USB, usb->config.id, at_UsbCallback))
    {
        ret = KING_GetLastErrCode();
        ATUARTLOG(I, "AT USB%d RegNotifyFun fail %d", usb->config.id,ret);
        return false;
    }
    
    usb->sleep = false;
    KING_WakeLock(LOCK_SUSPEND, (uint8 *)"Modem");
    return true;
}

static bool at_UsbReopen(AT_DEVICE_T *th)
{
    return true;
}

// =============================================================================
// Close
// =============================================================================
static void at_UsbClose(AT_DEVICE_T *th)
{
    AT_DEVICE_USB_T *usb = (AT_DEVICE_USB_T *)th;

    ATUARTLOG(I, "AT USB%d close", usb->config.id);
    KING_UsbDeinit(usb->config.id);
    usb->sleep = true;    
    KING_WakeUnlock(LOCK_SUSPEND, (uint8 *)"Modem");
}

// =============================================================================
// Check whether baud rate is supported. Ideally, it shoule be integrated with
// AT+IPR
// =============================================================================
static bool at_UsbIsBaudSupported(AT_DEVICE_T *th, int baud)
{
    return true;
}

// =============================================================================
// Set flow control
// =============================================================================
static bool at_UsbSetFlowCtrl(AT_DEVICE_T *th, uint8_t rxfc, uint8_t txfc)
{
    AT_DEVICE_USB_T *usb = (AT_DEVICE_USB_T *)th;
    bool ret = false;

    ATUARTLOG(I, "AT USB%d set flow ctrl %d/%d", usb->config.id, rxfc, txfc);

    uint32_t rate = usb->config.baud / 10;
    if (rate == 0)
        rate = 9600;
    usb->send_timeout = 2 * 1000 * USB_RX_BUF_SIZE / rate;
    if (usb->send_timeout < USB_MIN_TX_DONE_TIMEOUT)
        usb->send_timeout = USB_MIN_TX_DONE_TIMEOUT;

    ret = at_UsbReopen(th);
    return ret;
}

// =============================================================================
// Set baud, format and parity
// =============================================================================
static void at_UsbSetFormat(AT_DEVICE_T *th, int baud, AT_DEVICE_FORMAT_T format, AT_DEVICE_PARITY_T parity)
{
    AT_DEVICE_USB_T *usb = (AT_DEVICE_USB_T *)th;
    ATUARTLOG(I, "AT USB%d change format baud=%d, format=%d, parity=%d",
              usb->config.id, baud, format, parity);

    usb->config.baud = baud;
    usb->config.format = format;
    usb->config.parity = parity;

    uint32_t rate = usb->config.baud / 10;
    if (rate == 0)
      rate = 9600;
    usb->send_timeout = 2 * 1000 * USB_RX_BUF_SIZE / rate;
    if (usb->send_timeout < USB_MIN_TX_DONE_TIMEOUT)
      usb->send_timeout = USB_MIN_TX_DONE_TIMEOUT;

    at_UsbReopen(th);
}
