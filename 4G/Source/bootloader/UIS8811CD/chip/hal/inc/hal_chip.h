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

#ifndef _HAL_CHIP_H_
#define _HAL_CHIP_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "boot_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_TIMER_CURVAL_LO (hwp_timer2->hwtimer_curval_l)
#define HAL_TIMER_CURVAL_HI (hwp_timer2->hwtimer_curval_h)
#define HAL_TIMER_16K_CURVAL (hwp_timer1->hwtimer_curval_l)

#ifdef __cplusplus
}
#endif
#endif
