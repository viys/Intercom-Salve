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

#ifndef _BOOT_DELAY_H_
#define _BOOT_DELAY_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * busy loop delay
 *
 * The precision of delay depends on platform. And it will ensure to delay
 * at least the specified time.
 *
 * \param us    delay time in microseconds
 */
void bootDelayUS(uint32_t us);

/**
 * delay by CPU loop
 *
 * The absolute delay time depends on CPU frequency. \p count is the loop
 * count, not the delayed instruction count.
 *
 * \param count     delay loop count
 */
void bootDelayLoops(uint32_t count);

#ifdef __cplusplus
}
#endif
#endif
