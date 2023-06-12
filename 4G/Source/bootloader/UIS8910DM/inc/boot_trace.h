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

#ifndef _BOOT_TRACE_H_
#define _BOOT_TRACE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hwregs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOOT_ALIGN_UP(v, n) (((unsigned long)(v) + (n)-1) & ~((n)-1))

void bootTraceBasic(unsigned tag, unsigned nargs, const char *fmt, ...);

#define BOOT_LOCAL_LOG_TAG BOOT_MAKE_LOG_TAG('B', 'A', 'P', 'P')
#define BOOT_MAKE_LOG_TAG(a, b, c, d) ((unsigned)(a) | ((unsigned)(b) << 7) | ((unsigned)(c) << 14) | ((unsigned)(d) << 21))
#define BOOT_VA_NARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, N, ...) N
#define BOOT_VA_NARGS(...) BOOT_VA_NARGS_IMPL(0, ##__VA_ARGS__, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define BOOT_LOG(fmt, ...) __BOOT_LOGB(fmt, ##__VA_ARGS__)
#define __BOOT_LOGB(fmt, ...)                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        __BOOT_LOGB_IMP((BOOT_LOCAL_LOG_TAG), BOOT_VA_NARGS(__VA_ARGS__), fmt, ##__VA_ARGS__);    \
    } while (0)
    
#define __BOOT_LOGB_IMP(tag, nargs, fmt, ...) bootTraceBasic(tag, nargs, fmt, ##__VA_ARGS__)


typedef struct
{
    uint8_t sync;          ///< sync byte, 0xAD
    uint8_t frame_len_msb; ///< MSB byte of frame length
    uint8_t frame_len_lsb; ///< LSB byte of frame length
    uint8_t flowid;        ///< flow ID
} bootHostPacketHeader_t;

typedef struct
{
    bootHostPacketHeader_t host;
    uint16_t sn;   ///< trace sequence
    uint16_t tick; ///< tick of trace
    uint32_t tag;  ///< application tag
} bootTraceHeader_t;

typedef struct
{
    intptr_t ptr; ///< buffer pointer
    unsigned size; ///< buffer size
} bootBuffer_t;

void bootLog(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
#endif
