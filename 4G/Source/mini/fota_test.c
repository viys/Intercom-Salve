/*******************************************************************************
 ** File Name:   fota_test.c
 ** Copyright:   Copyright 2019-2019 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: 
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE              NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2019-06-19    Sean.Pan         Create.
 ******************************************************************************/
/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "KingRtos.h"
#include "KingFota.h"
#include "KingCSTD.h"
#include "KingPlat.h"
#include "KingFileSystem.h"
#include "KingPowerManage.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 ** MACROS
 ******************************************************************************/

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/

/*******************************************************************************
 ** Local Function Declarations
 ******************************************************************************/
#if 1
#define BUF_SIZE     512
uint32 g_data[4] = {0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA};
#elif 1
#define BUF_SIZE     512
uint32 g_data[2048] = {0xBBBBBBBB, 0xBBBBBBBB, 0xBBBBBBBB, 0xBBBBBBBB};
#else
#define BUF_SIZE     512
uint32 g_data[512] = {0xCCCCCCCC, 0xCCCCCCCC, 0xCCCCCCCC, 0xCCCCCCCC};
#endif


/*******************************************************************************
 ** Functions
 ******************************************************************************/
void AppLog(const char *fmt, ...)
{
    va_list ap;
    char msg[DEBUG_LOG_MAX_LEN];

    va_start(ap, fmt);
    vsnprintf(msg, DEBUG_LOG_MAX_LEN, fmt, ap);
    va_end(ap);

    KING_SysLog(msg);
}

void Fota_test(void)
{
    int ret = 0;
    uint32 value = 0;
//    GPIO_CFG_S cfg;
    FS_HANDLE fd;
    uint32 flsz;
    uint32 offset = 0;
    uint8 *pBuf = NULL;
    uint32 toRead = 0;
    uint32 bytesRead = 0;
    uint32 bytesWr = 0;
    FOTA_FLAG_E fotaFlag = FOTA_CLEAR;
    int cnt = 0;

    AppLog("g_data: 0x%X, 0x%X, 0x%X, 0x%X", g_data[0], g_data[1],g_data[2],g_data[3]);

#if 1
    if(0 == KING_FotaFlagGet(&fotaFlag))
    {
        if (fotaFlag == FOTA_UPDATE_SUCCESS)
        {
            AppLog("Fota SUCC\r\n");
            KING_FotaFlagSet(FOTA_CLEAR);
            ret = KING_FsFileDelete("fota.img");
            AppLog("file delete fota.img :%d\r\n",ret);
        }
    }

    while (0)
    {
        AppLog("Fota wait %d seconds", cnt << 2);
        if (KING_FsFileCreate("flag", FS_MODE_OPEN_EXISTING, 0, 0, &fd) < 0)
        {
            KING_Sleep(4000);
        }
        else
        {
            break;
        }
        cnt++;
    }

    // KING_FsCloseHandle(fd);

    // KING_FsFileDelete("flag");

#else
    ret = KING_GpioInit();
    if(-1 == ret)
    {
        AppLog("KING_GpioInit error\r\n");
        return;
    }

    KING_memset(&cfg, 0x00, sizeof(GPIO_CFG_S));
    cfg.default_val = 0;
    cfg.int_type = GPIO_INT_DISABLE;
    cfg.pull_mode = GPIO_PULL_DOWN;
    cfg.dir = GPIO_DIR_INPUT;
    ret = KING_GpioModeSet(14, &cfg);
    if(-1 == ret)
    {
        AppLog("KING_GpioModeSet error\r\n");
        return;
    }

    if(0 == KING_FotaFlagGet(&fotaFlag))
    {
        if (fotaFlag == FOTA_UPDATE_SUCCESS)
        {
            AppLog("Fota SUCC\r\n");
            KING_FotaFlagSet(FOTA_CLEAR);
            KING_FsFileDelete("fota.img");
        }
    }
    

    while (1)
    {
        ret = KING_GpioGet(14, &value);
        if ((ret != 0) || (value == 0))
        {
            KING_Sleep(2000);
        }
        else
        {
            break;
        }
    }
#endif


    if (KING_FsFileCreate("fota.img", FS_MODE_OPEN_EXISTING, 0, 0, &fd) < 0)
    {
        AppLog("open fota.img fail\r\n");
        return;
    }

    if (KING_FsFileSizeGet(fd, &flsz) < 0)
    {
        AppLog("get fota.img size fail\r\n");
        KING_FsCloseHandle(fd);
        return;
    }
    else
    {
        AppLog("fota.img size is %d\r\n", flsz);
        if (flsz == 0)
        {
            return;
        }
    }

    if(KING_MemAlloc((void**)(&pBuf), BUF_SIZE) < 0)
    {
        AppLog("malloc fail\r\n");
        KING_FsCloseHandle(fd);
        return;
    }

    while (offset < flsz)
    {
        if (offset + BUF_SIZE < flsz)
        {
            toRead = BUF_SIZE;
        }
        else
        {
            toRead = flsz - offset;
        }

        if (KING_FsFileRead(fd, pBuf, toRead, &bytesRead) < 0)
        {
            KING_FsCloseHandle(fd);
            KING_MemFree(pBuf);
            AppLog("Fota read file fail: start=%d, toRead=%d\r\n", offset, toRead);
            return;
        }
        else
        {
            uint32 idx = 0;
            while (1)
            {
                ret = KING_FotaFlashWrite(offset, pBuf + idx, bytesRead, &bytesWr);
                if (ret < 0)
                {
                    KING_FsCloseHandle(fd);
                    //KING_MemFree(pBuf);
                    AppLog("FotaWriteFlash fail: offset=%d, toWrite=%d\r\n", offset, toRead);
                    return;
                }
                else
                {
                    AppLog("FotaWriteFlash: offset=%d, Writed=%d\r\n", offset, bytesWr);
                    if (bytesWr == bytesRead)
                    {
                        offset += bytesWr;
                        break;
                    }
                    else
                    {
                        offset += bytesWr;
                        bytesRead -= bytesWr;
                        idx += bytesWr;
                    }
                }
            }
        }
    }
    KING_FsCloseHandle(fd);
    AppLog("FotaWriteFlash End!!!\r\n");

    if(0 != KING_FotaImageCheck(NULL))
    {
        AppLog("KING_FotaCheckImage fail\r\n");
        return;
    }

    if (0 == KING_FotaFlagSet(FOTA_IMAGE_IS_READY))
    {
        AppLog("Reboot for upgrade!!!\r\n");
        KING_Sleep(3000);
        
        KING_PowerReboot();
    }
    else
    {
        AppLog("Fota Set FOTA_IMAGE_IS_READY fail\r\n");
    }
}


#ifdef __cplusplus
}
#endif /* __cplusplus */

