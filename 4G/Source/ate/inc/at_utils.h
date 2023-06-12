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

#ifndef _AT_UTILS_H_
#define _AT_UTILS_H_

#include "kingplat.h"
#include "kingrtos.h"
#include "kingcstd.h"

// Macro can trace caller address
#define at_malloc  malloc
#define at_free    free
#define at_realloc realloc
#define at_memcpy  memcpy

typedef void (*AT_CALLBACK_FUNC_T)(void *param);


typedef struct {
    AT_CALLBACK_FUNC_T func;
    void *param;
}AT_TASK_MSG_PARAM_T;

bool at_CmuxCheckFcs(const uint8_t *data, unsigned length, uint8_t crc);
uint8_t at_CmuxCalcFcs(const uint8_t *data, unsigned length);


// Callback and timer to be executed in AT task
void at_TaskCallback(AT_CALLBACK_FUNC_T func, void *param);
void at_TaskCallbackNotif(AT_CALLBACK_FUNC_T func, void *param);
int at_StartCallbackTimer(unsigned ms, AT_CALLBACK_FUNC_T callback, void *ctx); // CWWUSB
void at_StopCallbackTimer(AT_CALLBACK_FUNC_T callback, void *ctx);
void *at_GetTimerCtx(TIMER_HANDLE handle);

#define AT_TRACE_ID (TID(__AT) | TLEVEL(1))
#define ATLOGV(...) KING_SysLog(__VA_ARGS__)
#define ATLOGD(...) KING_SysLog(__VA_ARGS__)
#define ATLOGI(...) KING_SysLog(__VA_ARGS__)
#define ATLOGW(...) KING_SysLog(__VA_ARGS__)
#define ATLOGE(...) KING_SysLog(__VA_ARGS__)
#define ATLOGSV(tsmap, ...) KING_SysLog(__VA_ARGS__)
#define ATLOGSD(tsmap, ...) KING_SysLog(__VA_ARGS__)
#define ATLOGSI(tsmap, ...) KING_SysLog(__VA_ARGS__)
#define ATLOGSW(tsmap, ...) KING_SysLog(__VA_ARGS__)
#define ATLOGSE(tsmap, ...) KING_SysLog(__VA_ARGS__)

#define ATUARTLOG(v, ...) KING_SysLog(__VA_ARGS__)
#define ATUARTLOGS(v, tsmap, ...) KING_SysLog(__VA_ARGS__)

#define ATUATLOG(v, ...) KING_SysLog(__VA_ARGS__)
#define ATUATLOGS(v, tsmap, ...) KING_SysLog(__VA_ARGS__)

#define ATDISPLOG(v, ...) KING_SysLog(__VA_ARGS__)
#define ATDISPLOGS(v, tsmap, ...) KING_SysLog(__VA_ARGS__)

#define ATCMDLOG(v, ...) KING_SysLog(__VA_ARGS__)
#define ATCMDLOGS(v, tsmap, ...) KING_SysLog(__VA_ARGS__)

#define ATDATALOG(v, ...) KING_SysLog(__VA_ARGS__)
#define ATDATALOGS(v, tsmap, ...) KING_SysLog(__VA_ARGS__)

#define AT_ASSERT(x)

#endif
