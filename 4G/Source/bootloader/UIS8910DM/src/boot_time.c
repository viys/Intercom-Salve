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
#include "hwregs.h"
#include <stdlib.h>

int64_t gBootUpTimeOffset = 0; // hwtick + offset => up time in tick

static inline uint64_t bootChipHwTick(void)
{
    for (;;)
    {
        uint32_t hi = hwp_timer4->hwtimer_curval_h;
        uint32_t lo = hwp_timer4->hwtimer_curval_l;
        if (hwp_timer4->hwtimer_curval_h == hi)
            return ((uint64_t)hi << 32) | lo;
    }
}

int64_t bootUpHWTick(void)
{
    uint32_t critical = bootEnterCritical();
    int64_t tick = bootChipHwTick() + gBootUpTimeOffset;
    bootExitCritical(critical);
    return tick;
}

void bootElapsedTimerStart(uint32_t *timer)
{
    if (timer != NULL)
        *timer = bootUpHWTick();
}

uint32_t bootElapsedTimeUS(uint32_t *timer)
{
    if (timer == NULL)
        return 0;

    uint32_t curr = bootUpHWTick();
    uint32_t delta = curr - (*timer);
    return bootChipTickToUs(delta);
}

void bootDelayUS(uint32_t us)
{
    uint32_t start = hwp_timer4->hwtimer_curval_l;
    uint32_t tick = US2TICK(us);
    while ((unsigned)(hwp_timer4->hwtimer_curval_l - start) < tick)
        asm("nop;");
}

uint32_t bootHWTick16K(void)
{
    return hwp_timer2->hwtimer_curval;
}


