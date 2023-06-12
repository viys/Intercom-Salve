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

#include "hwregs.h"
#include "hal_chip.h"
#include "boot_platform.h"
#include "boot_app.h"
#include "boot_fs.h"

int bootFsInit()
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_init();
    }
    return -1;
}

int bootFsOpen(const char *path, int flags, ...)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_open(path,flags);
    }
    return -1;
}
int bootFsClose(int fd)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_close(fd);
    }
    return -1;
}
int bootFsRead(int fd, void *data, size_t size)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_read(fd,data,size);
    }
    return -1;
}
int bootFsWrite(int fd, const void *data, size_t size)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_write(fd,data,size);
    }
    return -1;
}
long bootFsLseek(int fd, long offset, int mode)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_lseek(fd,offset,mode);
    }
    return -1;
}
int bootFsFstat(int fd, struct stat *st)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_fstat(fd,st);
    }
    return -1;
}
int bootFsUnlink(const char *path)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_unlink(path);
    }
    return -1;
}
int bootFsRename(const char *src, const char *dest)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_rename(src,dest);
    }
    return -1;
}
int bootFsMkdir(const char *path, uint32 mode)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_mkdir(path,mode);
    }
    return -1;
}
int bootFsRmdir(const char *path)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_rmdir(path);
    }
    return -1;
}
int bootFsStatfs(const char *path, struct statvfs *buf)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_statfs(path,buf);
    }
    return -1;
}
int bootFsFileSize(const char *path)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_fileSize(path);
    }
    return -1;
}
int bootFsFileRead(const char *path, void *data, size_t size)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_fileRead(path,data,size);
    }
    return -1;
}
int bootFsFileWrite(const char *path, const void *data, size_t size)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_fileWrite(path,data,size);
    }
    return -1;
}
int bootFsSfileInit(const char *path)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_sfile_init(path);
    }
    return -1;
}
int bootFsSfileRead(const char *path, void *data, size_t size)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_sfile_read(path,data,size);
    }
    return -1;
}
int bootFsSfileWrite(const char *path, const void *data, size_t size)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_sfile_write(path,data,size);
    }
    return -1;
}
int bootFsSfileSize(const char *path)
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_sfile_size(path);
    }
    return -1;
}
int bootFsDeinit()
{
    if(s_bootAppParamPtr != NULL && s_bootAppParamPtr->fsApi != NULL)
    {
        return s_bootAppParamPtr->fsApi->fs_deinit();
    }
    return -1;
}
