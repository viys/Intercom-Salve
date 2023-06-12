/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "boot_adi_bus.h"
#include "boot_trace.h"
#include "boot_compiler.h"
#include "boot_platform.h"
#include "hwregs.h"
#include <stdarg.h>

#define VALUE_MASK (0xffff)
#define ADDRESS_MASK (0xfff)
#define ADDRESS_START (REG_RDA2720M_GLOBAL_BASE & ~0xfff)

#define hwp_adi hwp_adiIf
#define ENTER_CRITICAL() bootEnterCritical()
#define EXIT_CRITICAL(critical) bootExitCritical(critical)

static void prvAdiRegInit(void)
{
    REG_ADI_MST_ADI_CTRL_T adi_ctrl = {
        .b.wr_cmd_en = 0,
    };
    REG_ADI_MST_ADI_CFG0_T adi_cfg0 = {
        .b.rf_gssi_frame_len = 29,
        .b.rf_gssi_cmd_len = 13,
        .b.rf_gssi_data_len = 16,
        .b.rf_gssi_wr_pos = 16,
        .b.rf_gssi_sync_mode = 1,
        .b.rf_gssi_oe_cfg = 1,
        .b.rf_gssi_dummy_clk_en = 1,
        .b.rf_gssi_sck_all_on = 1,
    };
    REG_ADI_MST_ADI_CFG1_T adi_cfg1 = {
        .b.rf_gssi_ng_tx = 1,
        .b.rf_gssi_ng_rx = 1,
        .b.rf_gssi_clk_div = 1,
        .b.rf_gssi_dummy_len = 5,
        .b.rf_gssi_strtbit_mode = 1,
    };

    hwp_adi->adi_ctrl = adi_ctrl.v;
    hwp_adi->adi_cfg0 = adi_cfg0.v;
    hwp_adi->adi_cfg1 = adi_cfg1.v;
}


void bootAdiBusInit(void)
{
    prvAdiRegInit();
}

static BOOT_FORCE_INLINE uint32_t prvReadLocked(volatile uint32_t *reg)
{
    REG_ADI_MST_ARM_CMD_STATUS_T arm_cmd_status;
    REG_WAIT_FIELD_EQZ(arm_cmd_status, hwp_adi->arm_cmd_status, adi_busy);

    hwp_adi->arm_rd_cmd = ADI_MST_ARM_RD_CMD_ARM_RD_CMD((uint32_t)reg);

    REG_ADI_MST_ARM_RD_DATA_T arm_rd_data;
    (void)hwp_adi->arm_rd_data; // avoid status not updated due to async
    REG_WAIT_FIELD_EQZ(arm_rd_data, hwp_adi->arm_rd_data, arm_rd_cmd_busy);

    if (arm_rd_data.b.arm_rd_addr != (((uint32_t)reg & 0xffff) >> 2))
        bootPanic();

    return arm_rd_data.b.arm_rd_cmd;
}

static BOOT_FORCE_INLINE void prvWriteLocked(volatile uint32_t *reg, uint32_t value)
{
    REG_ADI_MST_ARM_CMD_STATUS_T arm_cmd_status;
    REG_WAIT_FIELD_EQZ(arm_cmd_status, hwp_adi->arm_cmd_status, adi_busy);

    *reg = value;
    (void)hwp_adi->arm_cmd_status; // avoid status not updated due to async
    REG_WAIT_FIELD_EQZ(arm_cmd_status, hwp_adi->arm_cmd_status, adi_busy);
}

uint32_t bootAdiBusRead(volatile uint32_t *reg)
{
    uint32_t critical = ENTER_CRITICAL();
    uint32_t value = prvReadLocked(reg);
    EXIT_CRITICAL(critical);
    return value;
}

void bootAdiBusWrite(volatile uint32_t *reg, uint32_t value)
{
    uint32_t critical = ENTER_CRITICAL();
    prvWriteLocked(reg, value);
    EXIT_CRITICAL(critical);
}

void bootAdiBusBatchChange(volatile uint32_t *reg, uint32_t write_mask, uint32_t write_value, ...)
{
    uint32_t critical = ENTER_CRITICAL();

    va_list ap;
    va_start(ap, write_value);

    bool first = true;
    while ((uint32_t)reg != 0)
    {
        if (!first)
        {
            write_mask = va_arg(ap, uint32_t);
            write_value = va_arg(ap, uint32_t);
        }

        bootLog("ADI change reg/%p mask/0x%x value/0x%x",
                 (void *)reg, write_mask, write_value);

        if ((write_mask & VALUE_MASK) != VALUE_MASK)
        {
            uint32_t value = prvReadLocked(reg);
            write_value = (value & ~write_mask) | (write_value & write_mask);
        }

        prvWriteLocked(reg, write_value);

        reg = va_arg(ap, volatile uint32_t *);
        first = false;
    }

    va_end(ap);
    EXIT_CRITICAL(critical);
}

uint32_t bootAdiBusChange(volatile uint32_t *reg, uint32_t mask, uint32_t value)
{
    uint32_t critical = ENTER_CRITICAL();

    bootLog("ADI change reg/%p mask/0x%x value/0x%x",
             (void *)reg, mask, value);

    if ((mask & VALUE_MASK) != VALUE_MASK)
    {
        uint32_t read_value = prvReadLocked(reg);
        value = (read_value & ~mask) | (value & mask);
    }

    prvWriteLocked(reg, value);
    EXIT_CRITICAL(critical);

    return value;
}
