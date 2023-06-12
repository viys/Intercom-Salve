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

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 ** MACROS
 ******************************************************************************/
#define LOAD_DATA(name)                   \
    do                                         \
    {                                          \
        extern uint32_t __##name##_start;      \
        extern uint32_t __##name##_end;        \
        extern uint32_t __##name##_load_start; \
        uint32_t *p;                           \
        uint32_t *l;                           \
        for (p = &__##name##_start,            \
            l = &__##name##_load_start;        \
             p < &__##name##_end;)             \
            *p++ = *l++;                       \
    } while (0)

#define CLEAR_DATA(name)                  \
    do                                    \
    {                                     \
        extern uint32_t __##name##_start; \
        extern uint32_t __##name##_end;   \
        uint32_t *p;                      \
        for (p = &__##name##_start;       \
             p < &__##name##_end;)        \
            *p++ = 0;                     \
    } while (0)

/*******************************************************************************
 ** Variables
 ******************************************************************************/
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

typedef enum _BootAppWorkResult
{
    WORK_RESULT_NONE,
    WORK_RESULT_ENTER_AP,
    WORK_RESULT_ENTER_WFI,
    WORK_RESULT_MAX = 0x7fffffff,
}BootAppWorkResult_e;

typedef enum _BootAppWorkMode
{
    BOOT_APP_WORK_NORMAL_MODE,
    BOOT_APP_WORK_FOTA_MODE,
    BOOT_APP_WORK_KINGLOADER_MODE,
    BOOT_APP_WORK_MOD_MAX = 0x7fffffff,
}BootAppWorkMode_e;

typedef struct _BootAppNormalModeParam
{
    //reserve
}BootAppNormalModeParam_t;

typedef struct _BootAppFotaModeParam
{
    uint8_t fotaProgress;
}BootAppFotaModeParam_t;

typedef struct _BootAppParam
{
    BootAppWorkMode_e mode;
    BootAppFsApi_t* fsApi;
    union {
        BootAppNormalModeParam_t bootAppNormalParam;
        BootAppFotaModeParam_t   bootAppFotaParam;
    };
} BootAppParam_t;



#ifdef __cplusplus
}
#endif
#endif
