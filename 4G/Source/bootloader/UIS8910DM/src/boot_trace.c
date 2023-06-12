/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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
#include "boot_platform.h"
#include "boot_trace.h"
#include "boot_debuguart.h"
#include "cmsis_compiler.h"
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <sys/alloca.h>

extern void *memcpy(void *dstpp, const void *srcpp, size_t len);
extern void *memset(void *dst0, int c0, size_t length);
extern int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);
extern size_t strnlen(const char *str,size_t len);


#define BOOT_TRACE_SIZE (256)

uint32_t gTraceSequence = 0;

extern size_t strlen(const char *str);

static inline uint32_t bootChipTraceTick(void) { return hwp_idle->idl_32k_ref >> 1; }

bool gTraceEnabled = false;

/**
 * trace level, larger value is less important
 */
bool bootTraceBufPutMulti(const bootBuffer_t *bufs, unsigned count, int size)
{
    for (unsigned n = 0; n < count; n++)
        bootDebuguartWriteAll((const void *)bufs[n].ptr, bufs[n].size);
    return true;
}

/**
 * Fill application trace header
 */
static BOOT_FORCE_INLINE void prvFillTraceHeader(bootTraceHeader_t *h, unsigned tag, unsigned tlen)
{
    h->host.sync = 0xad;
    h->host.frame_len_msb = (tlen - 4) >> 8;
    h->host.frame_len_lsb = (tlen - 4) & 0xff;
    h->host.flowid = 0x98;
    h->sn = gTraceSequence;
    h->tag = tag;
    h->tick = bootChipTraceTick();
}

/**
 * Application basic trace
 */
static void prvTraceBasic(unsigned tag, unsigned nargs, const char *fmt, va_list ap)
{
    bootTraceHeader_t header;
    unsigned fmt_len = strlen(fmt);
    unsigned fmt_len_aligned = BOOT_ALIGN_UP(fmt_len + 1, 4);
    unsigned dlen = nargs * 4;
    unsigned tlen = sizeof(header) + fmt_len_aligned + dlen;
    uint32_t *buf = (uint32_t *)alloca(dlen);
    bootBuffer_t bufs[3] = {
        {(intptr_t)&header, sizeof(header)},
        {(intptr_t)fmt, fmt_len_aligned},
        {(intptr_t)buf, dlen},
    };

    for (unsigned n = 0; n < nargs; n++)
        *buf++ = va_arg(ap, uint32_t);

    unsigned critical = bootEnterCritical();
    gTraceSequence++;
    prvFillTraceHeader(&header, tag, tlen);
    bootTraceBufPutMulti(bufs, 3, tlen);
    bootExitCritical(critical);
}

/**
 * Application basic trace
 */
void bootTraceBasic(unsigned tag, unsigned nargs, const char *fmt, ...)
{
    //if (!gTraceEnabled)
    //    return;
    va_list ap;
    va_start(ap, fmt);
    prvTraceBasic(tag, nargs, fmt, ap);
    va_end(ap);
}

void bootLog(const char *fmt, ...)
{
    char msg[BOOT_TRACE_SIZE];
    va_list ap;
    memset(msg,0,BOOT_TRACE_SIZE);
    va_start(ap, fmt);
    vsnprintf(msg, BOOT_TRACE_SIZE - 1, fmt, ap);
    va_end(ap);
    BOOT_LOG(msg);
}


