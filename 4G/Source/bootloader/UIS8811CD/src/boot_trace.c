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
#include "boot_app.h"
#include "boot_trace.h"
#include "hal_chip.h"
#include "boot_platform.h"
#include "boot_compiler.h"
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <sys/alloca.h>

#define BOOT_DEBUG_LOG_MAX_LEN      512
static char msg[BOOT_DEBUG_LOG_MAX_LEN];

extern void *memcpy(void *dstpp, const void *srcpp, size_t len);
extern void *memset(void *dst0, int c0, size_t length);
extern int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);
extern size_t strnlen(const char *str,size_t len);

void bootLog(const char *fmt, ...)
{
    if(s_bootAppParamPtr != NULL)
    {
        va_list ap;
        memset(msg,0,BOOT_DEBUG_LOG_MAX_LEN);
        va_start(ap, fmt);
        vsnprintf(msg, BOOT_DEBUG_LOG_MAX_LEN - 1, fmt, ap);
        va_end(ap);
        s_bootAppParamPtr->bootLog(msg);
    }
}

