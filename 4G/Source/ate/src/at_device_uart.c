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
#include "at_device_uart.h"
#include "at_dispatch.h"
#include "at_utils.h"
#include "kinguart.h"
#include "kingplat.h"
#include "KingPowerManage.h"
#include "at_lp_ctrl.h"

#define AT_UART UART_1


#define XCPU_CACHE_LINE_SIZE (16)
#define XCPU_CACHE_LINE_MASK (XCPU_CACHE_LINE_SIZE - 1)

#define UART_RX_DMA_SIZE (256)
#define UART_RX_BUF_SIZE (4 * 1024)
#define UART_TX_BUF_SIZE (4 * 1024)
#define UART_RX_POLL_PERIOD (15) // ms

#define UART_MIN_TX_DONE_TIMEOUT (500)

#define UART_POOL_SIZE (UART_RX_DMA_SIZE + UART_RX_BUF_SIZE + UART_TX_BUF_SIZE + XCPU_CACHE_LINE_SIZE)

typedef struct
{
    AT_DEVICE_T ops; // API
    AT_DEVICE_UART_CONFIG_T config; // UART configuration

    uint32_t send_timeout;  // send data timeout
    bool sleep; // whether in sleep
} AT_DEVICE_UART_T;

extern struct AT_DISPATCH_T *g_dispatch[AT_DISPATCH_ID_MAX];


static void at_UartDestroy(AT_DEVICE_T *th);
static bool at_UartOpen(AT_DEVICE_T *th);
static bool at_UartReopen(AT_DEVICE_T *th, UART_CONFIG_S *drvcfg);
static void at_UartClose(AT_DEVICE_T *th);
static int at_UartWrite(AT_DEVICE_T *th, const uint8_t *data, unsigned length);
static int at_UartRead(AT_DEVICE_T *th, uint8_t *data, unsigned size);
static void at_UartSleep(AT_DEVICE_T *th, bool wakeupEnabled);
static void at_UartWakeup(AT_DEVICE_T *th);
static bool at_UartIsBaudSupported(AT_DEVICE_T *th, int baud);
static void at_UartCallback(uint32 eventID, void* pData, uint32 len);
static bool at_UartSetFlowCtrl(AT_DEVICE_T *th, uint8_t rxfc, uint8_t txfc);
static void at_UartSetFormat(AT_DEVICE_T *th, int baud, AT_DEVICE_FORMAT_T format, AT_DEVICE_PARITY_T parity);

static inline  UART_PARITY_E at_UartDrvParity(AT_DEVICE_PARITY_T parity)
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

static void at_UartDrvFormat(UART_CONFIG_S *cfg, AT_DEVICE_FORMAT_T format, AT_DEVICE_PARITY_T parity)
{
    cfg->byte_size = (format == AT_DEVICE_FORMAT_7N2 ||
                     format == AT_DEVICE_FORMAT_711 ||
                     format == AT_DEVICE_FORMAT_7N1)
                        ? UART_BYTE_LEN_7
                        : UART_BYTE_LEN_8;
    cfg->stop_bits = (format == AT_DEVICE_FORMAT_8N2 ||
                     format == AT_DEVICE_FORMAT_7N2)
                        ? UART_TWO_STOP_BITS
                        : UART_ONE_STOP_BIT;
    cfg->parity = (format == AT_DEVICE_FORMAT_811 ||
                   format == AT_DEVICE_FORMAT_711)
                      ? at_UartDrvParity(parity)
                      : UART_NO_PARITY;
}

// =============================================================================
// at_UartCreate
// =============================================================================
AT_DEVICE_T *at_UartCreate(AT_DEVICE_UART_CONFIG_T *cfg, uint8_t devno)
{
    unsigned memsize = sizeof(AT_DEVICE_UART_T) + UART_POOL_SIZE;
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)at_malloc(memsize);
    memset(uart, 0, sizeof(*uart));

    uart->config = *cfg;
    ATUARTLOG(I, "AT UART%d create: baud/%d", uart->config.id, cfg->baud);

    uart->ops.devno = devno;
    uart->ops.destroy = at_UartDestroy;
    uart->ops.open = at_UartOpen;
    uart->ops.close = at_UartClose;
    uart->ops.write = at_UartWrite;
    uart->ops.read = at_UartRead;
    uart->ops.sleep = at_UartSleep;
    uart->ops.wakeup = at_UartWakeup;
    uart->ops.isBaudSupported = at_UartIsBaudSupported;
    uart->ops.setFlowCtrl = at_UartSetFlowCtrl;
    uart->ops.setFormat = at_UartSetFormat;

    uint32_t rate = cfg->baud / 10;
    if (rate == 0)
        rate = 9600;
    uart->send_timeout = 2 * 1000 * UART_RX_BUF_SIZE / rate;
    if (uart->send_timeout < UART_MIN_TX_DONE_TIMEOUT)
        uart->send_timeout = UART_MIN_TX_DONE_TIMEOUT;
    
    return (AT_DEVICE_T *)uart;
}

// =============================================================================
// at_UartDestroy
// =============================================================================
static void at_UartDestroy(AT_DEVICE_T *th)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;

    if (uart == NULL)
        return;

    ATUARTLOG(I, "AT UART%d destroy", uart->config.id);
    KING_UartDeinit(uart->config.id);
    at_free(uart);
}

// =============================================================================
// Sleep
// =============================================================================
static void at_UartSleep(AT_DEVICE_T *th, bool wakeupEnabled)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;
    if (uart->sleep)
        return;

    ATUARTLOG(I, "AT UART%d sleep", uart->config.id);
    //DRV_UartSleep(uart->drv, false, wakeupEnabled);
    uart->sleep = true;    
    KING_WakeUnlock(LOCK_SUSPEND, (uint8 *)"Modem");
}

// =============================================================================
// Wakeup
// =============================================================================
static void at_UartWakeup(AT_DEVICE_T *th)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;
    if (!uart->sleep)
        return;

    ATUARTLOG(I, "AT UART%d wakeup", uart->config.id);
    //DRV_UartWakeup(uart->drv);   
    KING_WakeLock(LOCK_SUSPEND, (uint8 *)"Modem");
    uart->sleep = false;
}

// =============================================================================
// Write
// =============================================================================
static int at_UartWrite(AT_DEVICE_T *th, const uint8_t *data, unsigned length)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;
    uint32 trans = 0;

    if (data == NULL || length == 0)
        return 0;

    ATUARTLOG(I, "AT UART%d write: len=%d", uart->config.id, length);

    at_LpCtrlOutputRequest();

    KING_UartWrite(uart->config.id, (uint8_t*)data, length, &trans);
    if (trans == 0)
        ATUARTLOG(E, "AT UART%d output error %d", uart->config.id, trans);
    else if (trans < length)
        ATUARTLOG(E, "AT UART%d output overflow, drop %d bytes", uart->config.id, length - trans);
    return trans;
}

static void at_UartCallback(uint32 eventID, void* pData, uint32 len)
{
    AT_DEVICE_T* device = NULL;
    switch(eventID)
    {
        case UART_EVT_DATA_TO_READ:
            if(g_dispatch[AT_DISPATCH_ID_UART1] != NULL)
            {
                device = at_DispatchGetDevice(g_dispatch[AT_DISPATCH_ID_UART1]);
                if(device != NULL)
                {
                    device->wakeup(device);
                    at_DeviceScheduleRead(device);
                    at_LpCtrlInputReceived();
                }
            }
            break;

        case UART_EVT_WRITE_COMPLETE:
            break;

        default:
            ATUARTLOG(E,"%s: eventid = %d fail !!!", __FUNCTION__, eventID);
            return;
    }
}

// =============================================================================
// Read
// =============================================================================
static int at_UartRead(AT_DEVICE_T *th, uint8_t *data, unsigned size)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;
    uint32 trans = 0;
    if (uart->sleep)
        return 0;
    if (data == NULL || size == 0)
        return 0;
    KING_UartRead(uart->config.id, data, size, &trans);
    if (trans == 0)
        ATUARTLOG(E, "AT UART%d read error %d", uart->config.id, trans);
    else
        ATUARTLOG(E, "AT UART%d read %d, got %d", uart->config.id, size, trans);
    return trans;
}

// =============================================================================
// Open
// =============================================================================
static bool at_UartOpen(AT_DEVICE_T *th)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;
    int ret = -1;

    ATUARTLOG(I, "AT UART%d open", uart->config.id);
    UART_CONFIG_S drvcfg;
    drvcfg.baudrate = uart->config.baud;
    at_UartDrvFormat(&drvcfg, uart->config.format, uart->config.parity);
    drvcfg.flow_ctrl = UART_NO_FLOW_CONTROL;

    uint32_t rate = uart->config.baud / 10;
    if (rate == 0)
        rate = 9600;
    uart->send_timeout = 2 * 1000 * UART_RX_BUF_SIZE / rate;
    if (uart->send_timeout < UART_MIN_TX_DONE_TIMEOUT)
        uart->send_timeout = UART_MIN_TX_DONE_TIMEOUT;

    if (SUCCESS != KING_UartInit(uart->config.id, &drvcfg))
    {
        ret = KING_GetLastErrCode();
        ATUARTLOG(I, "AT UART%d Init fail %d", uart->config.id,ret);
        return false;
    }
    
    if(SUCCESS != KING_RegNotifyFun(DEV_CLASS_UART, AT_UART, at_UartCallback))
    {
        ret = KING_GetLastErrCode();
        ATUARTLOG(I, "AT UART%d RegNotifyFun fail %d", uart->config.id,ret);
        return false;
    }
    
    uart->sleep = false;
    KING_WakeLock(LOCK_SUSPEND, (uint8 *)"Modem");
    return true;
}

static bool at_UartReopen(AT_DEVICE_T *th, UART_CONFIG_S *drvcfg)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;
    bool ret = true;

    KING_Sleep(100);
    if (FAIL == KING_UartDeinit(uart->config.id))
    {
        ATLOGE("%s: UartDeinit fail", __FUNCTION__);
        ret = false;
    }
    else if (FAIL == KING_UartInit(uart->config.id, drvcfg))
    {
        ATLOGE("%s: UartInit fail", __FUNCTION__);
        ret = false;
    }
    else if (FAIL == KING_RegNotifyFun(DEV_CLASS_UART, AT_UART, at_UartCallback))
    {
        ATLOGE("%s: RegNotifyFun fail", __FUNCTION__);
        ret = false;
    }

    return ret;
}

// =============================================================================
// Close
// =============================================================================
static void at_UartClose(AT_DEVICE_T *th)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;

    ATUARTLOG(I, "AT UART%d close", uart->config.id);
    KING_UartDeinit(uart->config.id);
    uart->sleep = true;    
    KING_WakeUnlock(LOCK_SUSPEND, (uint8 *)"Modem");
}

// =============================================================================
// Check whether baud rate is supported. Ideally, it shoule be integrated with
// AT+IPR
// =============================================================================
static bool at_UartIsBaudSupported(AT_DEVICE_T *th, int baud)
{
    static const int list[] = {/*0, */1200, 2400, 4800, 9600, 14400, 19200,
                               28800, 33600, 38400, 57600, 115200,
                               230400, 460800, 921600};
    for (int n = 0; n < ARRAY_SIZE(list); n++)
        if (baud == list[n])
            return true;
    return false;
}

// =============================================================================
// Set flow control
// =============================================================================
static bool at_UartSetFlowCtrl(AT_DEVICE_T *th, uint8_t rxfc, uint8_t txfc)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;
    bool ret = false;

    ATUARTLOG(I, "AT UART%d set flow ctrl %d/%d", uart->config.id, rxfc, txfc);

    UART_CONFIG_S drvcfg;
    drvcfg.baudrate = uart->config.baud;
    at_UartDrvFormat(&drvcfg, uart->config.format, uart->config.parity);
    drvcfg.flow_ctrl = UART_NO_FLOW_CONTROL;

    uint32_t rate = uart->config.baud / 10;
    if (rate == 0)
        rate = 9600;
    uart->send_timeout = 2 * 1000 * UART_RX_BUF_SIZE / rate;
    if (uart->send_timeout < UART_MIN_TX_DONE_TIMEOUT)
        uart->send_timeout = UART_MIN_TX_DONE_TIMEOUT;

    ret = at_UartReopen(th, &drvcfg);
    return ret;
}

// =============================================================================
// Set baud, format and parity
// =============================================================================
static void at_UartSetFormat(AT_DEVICE_T *th, int baud, AT_DEVICE_FORMAT_T format, AT_DEVICE_PARITY_T parity)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;
    ATUARTLOG(I, "AT UART%d change format baud=%d, format=%d, parity=%d",
              uart->config.id, baud, format, parity);

    uart->config.baud = baud;
    uart->config.format = format;
    uart->config.parity = parity;

    UART_CONFIG_S drvcfg;
    drvcfg.baudrate = uart->config.baud;
    at_UartDrvFormat(&drvcfg, uart->config.format, uart->config.parity);
    drvcfg.flow_ctrl = UART_NO_FLOW_CONTROL;

    uint32_t rate = uart->config.baud / 10;
    if (rate == 0)
      rate = 9600;
    uart->send_timeout = 2 * 1000 * UART_RX_BUF_SIZE / rate;
    if (uart->send_timeout < UART_MIN_TX_DONE_TIMEOUT)
      uart->send_timeout = UART_MIN_TX_DONE_TIMEOUT;

    at_UartReopen(th, &drvcfg);
}
