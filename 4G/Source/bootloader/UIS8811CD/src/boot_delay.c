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

#include "hwregs.h"
#include "hal_chip.h"
#include "boot_tick_unit.h"

#define HWTICK_FROM(start) ((unsigned)(HAL_TIMER_CURVAL_LO - (start)))

void BOOT_SECTION_SRAM_BOOT_TEXT bootDelayLoops(uint32_t count)
{
    while (count-- > 0)
        BOOT_NOP;
}

void BOOT_SECTION_SRAM_BOOT_TEXT bootDelayUS(uint32_t us)
{
    uint32_t start = HAL_TIMER_CURVAL_LO;
    uint32_t ticks = BOOT_US_TO_HWTICK_U32(us);
    BOOT_POLL_WAIT(HWTICK_FROM(start) > ticks);
}
