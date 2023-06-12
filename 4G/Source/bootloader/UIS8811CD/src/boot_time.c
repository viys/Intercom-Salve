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

#include "boot_time.h"
#include "boot_platform.h"
#include "boot_tick_unit.h"
#include "hal_chip.h"
#include "hwregs.h"
#include <stdlib.h>

#define CONFIG_KERNEL_HWTICK_FREQ 1000000

#if (CONFIG_KERNEL_HWTICK_FREQ == 16384)
#define __OSI_HWTICK_CMN_DIV_MS (8)
#define __OSI_HWTICK_CMN_DIV_US (64)
#define __OSI_HWTICK_CMN_DIV_16K (16384)
#define __OSI_HWTICK_CMN_DIV_32K (16384)
#endif

#if (CONFIG_KERNEL_HWTICK_FREQ == 2000000)
#define __OSI_HWTICK_CMN_DIV_MS (1000)
#define __OSI_HWTICK_CMN_DIV_US (1000000)
#define __OSI_HWTICK_CMN_DIV_16K (128)
#define __OSI_HWTICK_CMN_DIV_32K (128)
#endif

#if (CONFIG_KERNEL_HWTICK_FREQ == 1000000)
#define __OSI_HWTICK_CMN_DIV_MS (1000)
#define __OSI_HWTICK_CMN_DIV_US (1000000)
#define __OSI_HWTICK_CMN_DIV_16K (64)
#define __OSI_HWTICK_CMN_DIV_32K (64)
#endif

#define __OSI_TICK_CONVERT(t, sfreq, dfreq, cdiv) (((t) * ((dfreq) / (cdiv))) / ((sfreq) / (cdiv)))

/**
 * \brief tick unit conversion with 32bits calculation
 *
 * It is implemented as a macro to be able to be used in const initializer.
 * Common divider \p cdiv is helpful to avoid intermediate overflow. It will
 * only be used when performance is really cared.
 *
 * \param t source tick
 * \param sfreq source tick frequency
 * \param dfres destination tick frequency
 * \param cdiv common divider of source and destination frequency
 * \return destination tick
 */
#define OSI_TICK_CONVERT_U32(t, sfreq, dfreq, cdiv) ((uint32_t)__OSI_TICK_CONVERT((uint32_t)(t), (unsigned)(sfreq), (unsigned)(dfreq), (unsigned)(cdiv)))

#define OSI_HWTICK_TO_US_U32(t) OSI_TICK_CONVERT_U32(t, CONFIG_KERNEL_HWTICK_FREQ, 1000000, __OSI_HWTICK_CMN_DIV_US)

int64_t gOsiUpTimeOffset = 0; // hwtick + offset => up time in tick


void bootElapsedTimerStart(uint32_t *timer)
{
    if (timer != NULL)
        *timer = HAL_TIMER_CURVAL_LO + (uint32_t)gOsiUpTimeOffset;
}

uint32_t bootElapsedTimeUS(uint32_t *timer)
{
    if (timer == NULL)
        return 0;

    uint32_t curr = HAL_TIMER_CURVAL_LO + (uint32_t)gOsiUpTimeOffset;
    uint32_t delta = curr - (*timer);
    return OSI_HWTICK_TO_US_U32(delta);
}

BOOT_FORCE_INLINE static uint64_t prvChipHwTick(void)
{
#ifdef HAL_TIMER_CURVAL_HI
    for (;;)
    {
        uint32_t hi = HAL_TIMER_CURVAL_HI;
        uint32_t lo = HAL_TIMER_CURVAL_LO;
        if (HAL_TIMER_CURVAL_HI == hi)
            return ((uint64_t)hi << 32) | lo;
    }
#else
    return HAL_TIMER_CURVAL_LO;
#endif
}

int64_t bootUpHWTick(void)
{
    uint32_t critical = bootEnterCritical();
    int64_t tick = prvChipHwTick() + gOsiUpTimeOffset;
    bootExitCritical(critical);
    return tick;
}


int64_t bootUpTimeUS(void)
{
    return BOOT_HWTICK_TO_US(bootUpHWTick());
}

