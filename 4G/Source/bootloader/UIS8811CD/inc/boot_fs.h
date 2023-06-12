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

#ifndef _BOOT_FS_H_
#define _BOOT_FS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Flag values for open(2) and fcntl(2)
 * The kernel adds 1 to the open modes to turn it into some
 * combination of FREAD and FWRITE.
 */
#define	O_RDONLY	0		/* +1 == FREAD */
#define	O_WRONLY	1		/* +1 == FWRITE */
#define	O_RDWR		2		/* +1 == FREAD|FWRITE */
#define	O_APPEND	_FAPPEND
#define	O_CREAT		_FCREAT
#define	O_TRUNC		_FTRUNC
#define	O_EXCL		_FEXCL
#define O_SYNC		_FSYNC
/*	O_NDELAY	_FNDELAY 	set in include/fcntl.h */
/*	O_NDELAY	_FNBIO 		set in include/fcntl.h */
#define	O_NONBLOCK	_FNONBLOCK
#define	O_NOCTTY	_FNOCTTY


int bootFsInit();
int bootFsOpen(const char *path, int flags, ...);
int bootFsClose(int fd);
int bootFsRead(int fd, void *data, size_t size);
int bootFsWrite(int fd, const void *data, size_t size);
long bootFsLseek(int fd, long offset, int mode);
int bootFsFstat(int fd, struct stat *st);
int bootFsUnlink(const char *path);
int bootFsRename(const char *src, const char *dest);
int bootFsMkdir(const char *path, uint32 mode);
int bootFsRmdir(const char *path);
int bootFsStatfs(const char *path, struct statvfs *buf);
int bootFsFileSize(const char *path);
int bootFsFileRead(const char *path, void *data, size_t size);
int bootFsFileWrite(const char *path, const void *data, size_t size);
int bootFsSfileInit(const char *path);
int bootFsSfileRead(const char *path, void *data, size_t size);
int bootFsSfileWrite(const char *path, const void *data, size_t size);
int bootFsSfileSize(const char *path);
int bootFsDeinit();

#ifdef __cplusplus
}
#endif
#endif
