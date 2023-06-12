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

#ifndef _BOOT_TIME_H_
#define _BOOT_TIME_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BOOT_TICK2M_TO_US(tick) ((tick) / 2)
#define US2TICK(us) (us * 2)
#define BOOT_HWTICK_TO_US(tick) BOOT_TICK2M_TO_US(tick)
static inline int64_t bootChipTickToUs(int64_t tick) { return BOOT_HWTICK_TO_US(tick); }

int64_t bootUpHWTick(void);
void bootElapsedTimerStart(uint32_t *timer);
uint32_t bootElapsedTimeUS(uint32_t *timer);
void bootDelayUS(uint32_t us);
uint32_t bootHWTick16K(void);


#ifdef __cplusplus
}
#endif
#endif
