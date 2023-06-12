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

#ifndef _BOOT_APP_H_
#define _BOOT_APP_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include "boot_uart.h"
#include "boot_rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 ** Variables
 ******************************************************************************/
typedef enum _BootAppWorkResult
{
    WORK_RESULT_NONE,
    WORK_RESULT_ENTER_AP,
    WORK_RESULT_ENTER_WFI,
    WORK_RESULT_ENTER_PSM,
    WORK_RESULT_MAX = 0x7fffffff,
}BootAppWorkResult_e;

typedef struct _BootAppMemApi
{
    void* (*malloc)(unsigned size);
    void* (*calloc)(unsigned nmemb, unsigned size);
    void* (*realloc)(void *ptr, unsigned size);
    void (*free)(void *ptr);
    void (*showMemInfo)(void);
}BootAppMemApi_t;

typedef struct _BootAppFsApi
{
    int (*fs_init)();
    int (*fs_open)(const char *path, int flags, ...);
    int (*fs_close)(int fd);
    int (*fs_read)(int fd, void *data, size_t size);
    int (*fs_write)(int fd, const void *data, size_t size);
    long (*fs_lseek)(int fd, long offset, int mode);
    int (*fs_fstat)(int fd, struct stat *st);
    int (*fs_unlink)(const char *path);
    int (*fs_rename)(const char *src, const char *dest);
    int (*fs_mkdir)(const char *path, uint32 mode);
    int (*fs_rmdir)(const char *path);
    int (*fs_statfs)(const char *path, struct statvfs *buf);
    int (*fs_fileSize)(const char *path);
    int (*fs_fileRead)(const char *path, void *data, size_t size);
    int (*fs_fileWrite)(const char *path, const void *data, size_t size);
    int (*fs_sfile_init)(const char *path);
    int (*fs_sfile_read)(const char *path, void *data, size_t size);
    int (*fs_sfile_write)(const char *path, const void *data, size_t size);
    int (*fs_sfile_size)(const char *path);
    int (*fs_deinit)();
}BootAppFsApi_t;

typedef struct _BootAppParam
{
    unsigned (*calcDivider24)(unsigned input, unsigned output);
    void (*bootLog)(const char* logs);
    void (*bootReadRtcTime)(mytime_t* mytime,bool daylightSaving);
    BootAppMemApi_t* memApi;
    BootAppFsApi_t* fsApi;
} BootAppParam_t;

extern BootAppParam_t* s_bootAppParamPtr;

int _boot_app_init(const void* param);
BootAppWorkResult_e _boot_app_work(const void* param);
int _boot_app_deinit(const void* param);


#ifdef __cplusplus
}
#endif
#endif
