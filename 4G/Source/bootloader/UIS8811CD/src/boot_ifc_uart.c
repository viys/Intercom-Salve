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

#include "boot_mem.h"
#include "boot_uart.h"
#include "boot_platform.h"
#include "hal_iomux.h"
#include "hal_chip.h"
#include <stdlib.h>
#include "hwregs.h"

#define INPUT_CLOCK_FREQ (52000000 / 4) // always use DIVISOR_MODE_4
#define BAUD_MIN (9600)
#define BAUD_MAX (INPUT_CLOCK_FREQ)
#define DIVISOR_MODE_16 (0)
#define DIVISOR_MODE_4 (1)

struct bootUart
{
    uint32_t name;
    HWP_UART_T *hwp;
    bootUartCfg_t cfg;
};


static bootUart_t gBootUarts[] = {
    {BOOT_NAME_UART1, hwp_uart1},
    {BOOT_NAME_UART2, hwp_uart2},
    {BOOT_NAME_UART3, hwp_uart3},
    {BOOT_NAME_UART4, hwp_uart4},
    {BOOT_NAME_UART5, hwp_uart5},
};

static bootUart_t *prvFindByName(unsigned name)
{
    for (unsigned n = 0; n < ARRAY_SIZE(gBootUarts); n++)
    {
        if (gBootUarts[n].name == name)
            return &gBootUarts[n];
    }
    return NULL;
}

static bool prvSetIomux(bootUart_t *d)
{
    if (d->name == BOOT_NAME_UART1)
    {
        halIomuxSetFunction(HAL_IOMUX_FUN_UART1_RXD);
        halIomuxSetFunction(HAL_IOMUX_FUN_UART1_TXD);
    }
    else if (d->name == BOOT_NAME_UART2)
    {
        halIomuxSetFunction(HAL_IOMUX_FUN_UART2_RXD);
        halIomuxSetFunction(HAL_IOMUX_FUN_UART2_TXD);
    }
    else if (d->name == BOOT_NAME_UART3)
    {
        halIomuxSetFunction(HAL_IOMUX_FUN_UART3_RXD);
        halIomuxSetFunction(HAL_IOMUX_FUN_UART3_TXD);
    }
    else if (d->name == BOOT_NAME_UART4)
    {
        halIomuxSetFunction(HAL_IOMUX_FUN_UART4_RXD);
        halIomuxSetFunction(HAL_IOMUX_FUN_UART4_TXD);
    }
    else if (d->name == BOOT_NAME_UART5)
    {
        halIomuxSetFunction(HAL_IOMUX_FUN_UART5_RXD);
        halIomuxSetFunction(HAL_IOMUX_FUN_UART5_TXD);
    }
    else
    {
        return false;
    }

    return true;
}

static inline void prvStartConfig(bootUart_t *d)
{
    d->hwp->irq_mask = 0;

    REG_UART_CTRL_T ctrl = {
        .b.enable = UART_ENABLE_V_DISABLE,
        .b.data_bits = (d->cfg.data_bits == BOOT_UART_DATA_BITS_7) ? 0 : 1,
        .b.data_bits_56 = 0,
        .b.tx_stop_bits = (d->cfg.stop_bits == BOOT_UART_STOP_BITS_1) ? 0 : 1,
        .b.parity_enable = (d->cfg.parity == BOOT_UART_NO_PARITY) ? 0 : 1,
        .b.parity_select = 0,
        .b.soft_flow_ctrl_enable = 0,
        .b.auto_enable = 0,
        .b.divisor_mode = DIVISOR_MODE_4,
        .b.irda_enable = 0,
        .b.dma_mode = 0,
        .b.auto_flow_control = 0,
        .b.loop_back_mode = 0,
        .b.rx_lock_err = 0,
        .b.rx_break_length = 15,
    };
    d->hwp->ctrl = ctrl.v;

    REG_UART_CMD_SET_T cmd_set = {
        .b.rx_fifo_reset = 1,
        .b.tx_fifo_reset = 1,
    };
    d->hwp->cmd_set = cmd_set.v;
    d->hwp->cmd_set = cmd_set.v;

    ctrl.b.enable = UART_ENABLE_V_ENABLE;
    d->hwp->ctrl = ctrl.v;
}

bootUart_t *bootUartOpen(uint32_t name, bootUartCfg_t cfg, bool reconfig)
{
    bootUart_t *d = prvFindByName(name);
    if (d == NULL)
        bootPanic();

    if (reconfig)
    {
        d->cfg = cfg;
        prvSetIomux(d);
        if (!bootUartSetBaud(d, cfg.baud))
        {
            free(d);
            return NULL;
        }
        prvStartConfig(d);
    }
    return d;
}

bool bootUartBaudSupported(bootUart_t *d, uint32_t baud)
{
    return (baud >= BAUD_MIN && baud <= BAUD_MAX);
}

bool bootUartSetBaud(bootUart_t *d, uint32_t baud)
{
    if (!bootUartBaudSupported(d, baud))
        return false;

    #include "boot_app.h"
    unsigned divider = s_bootAppParamPtr->calcDivider24(INPUT_CLOCK_FREQ, baud);
    if (divider == 0)
        return false;

    // for UART1 and UART2
    REG_PMUC_CLK_UART1_CFG_T lpuart = {divider};
    lpuart.b.uart1_div_update = 1;
    lpuart.b.sel_clk_uart1 = 2;

    // for UART3, UART4 and UART5
    REG_SYS_CTRL_CFG_CLK_UART3_T normuart = {divider};

    if (d->name == BOOT_NAME_UART1)
        hwp_pwrCtrl->clk_uart1_cfg = lpuart.v;
    else if (d->name == BOOT_NAME_UART2)
        hwp_pwrCtrl->clk_uart2_cfg = lpuart.v;
    else if (d->name == BOOT_NAME_UART3)
        hwp_sysCtrl->cfg_clk_uart3 = normuart.v;
    else if (d->name == BOOT_NAME_UART4)
        hwp_sysCtrl->cfg_clk_uart4 = normuart.v;
    else if (d->name == BOOT_NAME_UART5)
        hwp_sysCtrl->cfg_clk_uart5 = normuart.v;
    else
        return false;

    return true;
}

int bootUartAvail(bootUart_t *d)
{
    REG_UART_STATUS_T status = {d->hwp->status};
    return status.b.rx_fifo_level;
}

int bootUartRead(bootUart_t *d, void *data, size_t size)
{
    REG_UART_STATUS_T status = {d->hwp->status};
    int bytes = status.b.rx_fifo_level;
    if (bytes > size)
        bytes = size;

    uint8_t *data8 = (uint8_t *)data;
    for (int n = 0; n < bytes; n++)
        *data8++ = d->hwp->rxtx_buffer & 0xff;
    return bytes;
}

int bootUartWrite(bootUart_t *d, const void *data, size_t size)
{
    REG_UART_STATUS_T status = {d->hwp->status};
    int bytes = status.b.tx_fifo_space;
    if (bytes > size)
        bytes = size;

    const uint8_t *data8 = (const uint8_t *)data;
    for (int n = 0; n < bytes; n++)
        d->hwp->rxtx_buffer = *data8++;
    return bytes;
}

void bootUartFlush(bootUart_t *d)
{
    REG_UART_STATUS_T status;
    BOOT_POLL_WAIT(({status.v = d->hwp->status;
        status.b.tx_fifo_space >= UART_TX_FIFO_SIZE && !status.b.tx_active; }));
}
