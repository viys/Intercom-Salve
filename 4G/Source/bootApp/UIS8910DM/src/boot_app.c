/*******************************************************************************
 ** File Name:   boot_app.c
 ** Copyright:   Copyright 2017-2019 by Xiamen CHEERZING IoT Technology Co., Ltd.
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
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2019-06-16    XUZHE         Create
 ******************************************************************************/
/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "cs_types.h"
#include "boot_app.h"
#include "boot_gpio.h"
#include "boot_lcd.h"
#include "boot_platform.h"
#include "boot_mem.h"
#include "boot_trace.h"
#include "boot_debuguart.h"
#include "boot_adc.h"
#include "boot_charge.h"
#include "boot_pmic_intr.h"
#include "boot_powkey.h"
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/statvfs.h>

#define CONFIG_BOOT_SRAM_HEAP_START 0x82d000
#define CONFIG_BOOT_SRAM_HEAP_SIZE 0x9800

#define CONFIG_RAM_PHY_ADDRESS      0x80C00000
#define CONFIG_RAM_SIZE             0x400000

extern void *memset(void *dst0, int c0, size_t length);
extern int sprintf(char *buf, const char *fmt, ...);
extern size_t strlen(const char *str);

BootAppParam_t* s_bootAppParamPtr;

int _boot_app_init(const void* param);
BootAppWorkResult_e _boot_app_work(const void* param);
int _boot_app_deinit(const void* param);

volatile __attribute__ ((section ("._boot_app_api"))) uint32* g_bootAppApiTable[]=
{
    (uint32*)_boot_app_init,
    (uint32*)_boot_app_work,
    (uint32*)_boot_app_deinit,
};

/*******************************************************************************
 ** External Function Delcarations
 ******************************************************************************/


/*******************************************************************************
 ** Local Function Delcarations
 ******************************************************************************/


/*******************************************************************************
 ** Functions
 ******************************************************************************/
void __attribute__((section("._boot_app_entry"))) _boot_app_entry(uint32_t param)
{
}



static void _boot_app_gpio_test()
{
     bootGpioConfig_t cfg = {
         .mode = BOOT_GPIO_OUTPUT,
         .out_level = false,
     };
    bootGpio_t *p;
    p = bootGpioOpen(7, &cfg);
    bootGpioReconfig(p, &cfg);
    if (NULL != p)
    {
        for(uint8 i=0;i<5;i++)
        {
            bootGpioWrite(p, true);
            bootDelayUS(500000);
            bootGpioWrite(p, false);
            bootDelayUS(500000);
        }
    }
    bootGpioClose(p);
}

static void _boot_app_lcd_test()
{
    lcdSpec_t lcdInfo = {0};
    uint16* pCanvas = NULL;
    //LCDÅäÖÃ
    LCDCONFIG_T *lcd_config = NULL;
    bootLcdInit(lcd_config);
    bootLcdGetLcdInfo(&lcdInfo);
    bootLog("height:%d,width:%d",lcdInfo.height,lcdInfo.width);
    pCanvas = malloc(lcdInfo.height * lcdInfo.width * sizeof(uint16));
    if (pCanvas == NULL)
    {
        return;
    }
    bootLog("pCanvas:/%p",pCanvas);
    memset(pCanvas, 0xff, lcdInfo.height * lcdInfo.width * sizeof(uint16));
    
    bootLcdShow(lcdInfo.height,lcdInfo.width , pCanvas);
    bootLcdBacklightCtrl(1);
    bootDelayUS(5000000);
    free(pCanvas);
    pCanvas = NULL;
    bootLcdBacklightCtrl(0);
    bootLcdClose();
}

static void _boot_app_fs_test()
{
    char buff_read[32] = {0};
    char buff_write[32] = {0};
    char path[32] = {0};
    int fd = -1;
    uint16 i=0;

    struct statvfs statbuff;
    struct stat st;
    ssize_t wirtten_len;
    ssize_t read_len;
    
    s_bootAppParamPtr->fsApi->fs_statfs("/",&statbuff);
    bootLog("fs_statfs %d %d",statbuff.f_bfree , statbuff.f_bsize);

    for(i=0; i<10; i++)
    {
        memset(path,0,sizeof(path));
        memset(buff_write,0,sizeof(buff_write));
        memset(buff_read,0,sizeof(buff_read));
        
        sprintf(path,"/file_%d.txt",i);
        sprintf(buff_write,"file_test_%d",i);

        fd = s_bootAppParamPtr->fsApi->fs_open(path, O_RDWR | O_CREAT, 0);
        if(fd < 0)
        {
            bootLog("%s fs_open fail", path);
            continue;
        }
        bootLog("%s open success fd:%d",path,fd);

        wirtten_len = s_bootAppParamPtr->fsApi->fs_write(fd, buff_write, strlen(buff_write));
        if (wirtten_len < 0)
        {
            bootLog("%s write fail,errno:%d",path,wirtten_len);
            s_bootAppParamPtr->fsApi->fs_close(fd);
            bootDelayUS(500000);
            continue;
        }
        bootLog("%s write %d bytes success",path,wirtten_len);
        
        if(s_bootAppParamPtr->fsApi->fs_fstat(fd, &st) < 0)
        {
            bootLog("%s fstat fail",path);
            s_bootAppParamPtr->fsApi->fs_close(fd);
            bootDelayUS(500000);
            continue;
        }
        bootLog("%s,fstat success %d bytes",path,st.st_size);

        s_bootAppParamPtr->fsApi->fs_lseek(fd,0,SEEK_SET);
        
        read_len = s_bootAppParamPtr->fsApi->fs_read(fd, buff_read, st.st_size);
        if(read_len < 0)
        {
            bootLog("%s,read fail",path);
            s_bootAppParamPtr->fsApi->fs_close(fd);
            bootDelayUS(500000);
            continue;
        }
        bootLog("%s read %d bytes [%s]",path,read_len,buff_read);
        s_bootAppParamPtr->fsApi->fs_close(fd);
        bootDelayUS(500000);
    }
}


//TTODO: do not modity this function
int _boot_app_init(const void* param)
{
    LOAD_DATA(sram_data);
    CLEAR_DATA(sram_bss);
    
    bootHeapInit((void*)CONFIG_BOOT_SRAM_HEAP_START,CONFIG_BOOT_SRAM_HEAP_SIZE,
                 (void*)CONFIG_RAM_PHY_ADDRESS,CONFIG_RAM_SIZE);
    bootHeapDefaultExtRam();
    
    s_bootAppParamPtr = (BootAppParam_t*)param;

    if(s_bootAppParamPtr->mode == BOOT_APP_WORK_NORMAL_MODE)
    {
        bootDebugEvent(0xffff0001);
        bootPmicInit();
        bootGpioInit();
        bootAdcInit();
        bootChargerInit();
    }
    else if(s_bootAppParamPtr->mode == BOOT_APP_WORK_FOTA_MODE)
    {
        bootDebugEvent(0xffff0002);
    }
    else
    {
        bootLog("_boot_app_init unknown mode");
    }
    return 0;
}


BootAppWorkResult_e _boot_app_work(const void* param)
{
    bootLog("_boot_app_work");
    if(BOOT_APP_WORK_NORMAL_MODE == s_bootAppParamPtr->mode)
    {
        bootDebugEvent(0xffff0010);
        _boot_app_gpio_test();
        _boot_app_lcd_test();
        _boot_app_fs_test();
    }
    else if(BOOT_APP_WORK_FOTA_MODE == s_bootAppParamPtr->mode)
    {
        bootDebugEvent(0xffff0020);
        uint8 fota_progress = s_bootAppParamPtr->bootAppFotaParam.fotaProgress;
        bootLog("fota_progress :%d",fota_progress);
    }
    else
    {
        bootDebugEvent(0xffff0030);
        bootLog("work unknown mode");
    }
    return WORK_RESULT_ENTER_AP;
}

int _boot_app_deinit(const void* param)
{
    bootLog("_boot_app_deinit");
    s_bootAppParamPtr->fsApi->fs_deinit();
    return 0;
}
