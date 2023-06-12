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
 ** 2020-10-27    XUZHE         Create
 ******************************************************************************/
/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "string.h"
#include "stdio.h"
#include <sys/fcntl.h>
#include "cs_types.h"
#include "hal/hal_iomux.h"
#include "boot_delay.h"
#include "boot_uart.h"
#include "boot_i2c.h"
#include "boot_gpio.h"
#include "boot_app.h"
#include "boot_trace.h"
#include "boot_platform.h"
#include "boot_mem.h"
#include "boot_fs.h"
#include "boot_rtc.h"

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


volatile __attribute__ ((section ("._boot_app_api"))) uint32* g_bootAppApiTable[]=
{
    (uint32*)_boot_app_init,
    (uint32*)_boot_app_work,
    (uint32*)_boot_app_deinit,
};

/*******************************************************************************
 ** External Function Delcarations
 ******************************************************************************/
extern void* memset(void *dst, int value, size_t count);


/*******************************************************************************
 ** Local Function Delcarations
 ******************************************************************************/

/*******************************************************************************
 ** Local global parameters
 ******************************************************************************/
BootAppParam_t* s_bootAppParamPtr = NULL;

/*******************************************************************************
 ** Functions
 ******************************************************************************/
void __attribute__((section("._boot_app_entry"))) _boot_app_entry(uint32_t param)
{
}

#define KXTJ2_DEVICE_ADDR       0x0E
#define KXTJ2_CHIP_ID           0x0F // default WHO_AM_I value is 0x09
#define KXTJ2_CTRL_REG1         0x1B

void i2cTest(void)
{
    uint8 buffer[3];

    bootI2cMaster_t* i2c_master = bootI2cMasterAcquire(BOOT_NAME_I2C1,BOOT_I2C_BPS_100K);
    if(i2c_master == NULL)
    {
        bootLog("bootI2cMasterAcquire() Failed!");
        return;
    }

    bootI2cChangeFreq(i2c_master,BOOT_I2C_BPS_400K);

    bootI2cSlave_t slave;
    slave.addr_device = KXTJ2_DEVICE_ADDR;
    slave.reg_16bit = false;
    slave.addr_data = KXTJ2_CHIP_ID;
    slave.addr_data_low = 0;
    memset(buffer, 0x00, 3);
    if (false == bootI2cRead(i2c_master, &slave, buffer, 1))
    {
        bootLog("bootI2cRead() KXTJ2_CHIP_ID Failed!");
        return;
    }
    bootLog("Read KXTJ2_CHIP_ID value is 0x%X.\r\n", buffer[0]);

    /*
     * Read/Write KXTJ2-1009 control regisger.
     */
    slave.addr_data = KXTJ2_CTRL_REG1;
    memset(buffer, 0x00, 3);
    if (false == bootI2cRead(i2c_master, &slave,buffer, 1))
    {
        bootLog("bootI2cRead() KXTJ2_CTRL_REG1 Failed!");
        return;
    }
    bootLog("Read KXTJ2_CTRL_REG1 value is 0x%X\r\n", buffer[0]);

    buffer[0] = 0xC2;
    bootLog("Write 0x%X value to KXTJ2_CTRL_REG1...\r\n", buffer[0]);
    if (false == bootI2cWrite(i2c_master, &slave, buffer, 1))
    {
        bootLog("KING_IicWrite() KXTJ2_CTRL_REG1 Failed!");
        return;
    }

    memset(buffer, 0x00, 3);
    if (false == bootI2cRead(i2c_master, &slave,buffer, 1))
    {
        bootLog("bootI2cRead() KXTJ2_CTRL_REG1 Failed!");
        return;
    }
    bootLog("Read KXTJ2_CTRL_REG1 value is 0x%X\r\n", buffer[0]);

    bootI2cMasterRelease(i2c_master);
}


void uartTest(void)
{
    char buf[32] = {0};
    uint8 cnt = 0;
    bootUartCfg_t cfg = 
    {
        .baud = 115200,
        .data_bits = BOOT_UART_DATA_BITS_8,
        .stop_bits = BOOT_UART_STOP_BITS_1,
        .parity = BOOT_UART_NO_PARITY,
    };
        
    bootUart_t* uartDev = bootUartOpen(BOOT_NAME_UART1, cfg, true);
    if(uartDev == NULL)
    {
        bootLog("bootUartOpen() Failed!");
        return;
    }

    while(cnt<3)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "\r\nuartTest\r\n");
        bootUartWrite(uartDev, buf, strlen(buf));
        bootDelayUS(1000000);
        cnt ++;
    }
}

static void gpio_test()
{
     bootGpioConfig_t cfg = {
         .mode = BOOT_GPIO_OUTPUT,
         .out_level = false,
     };
    bootGpio_t *p;
    p = bootGpioOpen(5, &cfg);
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

static void fs_test()
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
    
    bootFsStatfs("/",&statbuff);
    bootLog("fs_statfs %d %d",statbuff.f_bfree , statbuff.f_bsize);

    for(i=0; i<10; i++)
    {
        memset(path,0,sizeof(path));
        memset(buff_write,0,sizeof(buff_write));
        memset(buff_read,0,sizeof(buff_read));
        
        sprintf(path,"/file_%d.txt",i);
        sprintf(buff_write,"file_test_%d",i);

        fd = bootFsOpen(path, O_RDWR | O_CREAT, 0);
        if(fd < 0)
        {
            bootLog("%s fs_open fail", path);
            continue;
        }
        bootLog("%s open success fd:%d",path,fd);

        wirtten_len = bootFsWrite(fd, buff_write, strlen(buff_write));
        if (wirtten_len < 0)
        {
            bootLog("%s write fail,errno:%d",path,wirtten_len);
            bootFsClose(fd);
            bootDelayUS(500000);
            continue;
        }
        bootLog("%s write %d bytes success",path,wirtten_len);
        
        if(bootFsFstat(fd, &st) < 0)
        {
            bootLog("%s fstat fail",path);
            bootFsClose(fd);
            bootDelayUS(500000);
            continue;
        }
        bootLog("%s,fstat success %d bytes",path,st.st_size);

        bootFsLseek(fd,0,SEEK_SET);
        
        read_len = bootFsRead(fd, buff_read, st.st_size);
        if(read_len < 0)
        {
            bootLog("%s,read fail",path);
            bootFsClose(fd);
            bootDelayUS(500000);
            continue;
        }
        bootLog("%s read %d bytes [%s]",path,read_len,buff_read);
        bootFsClose(fd);
        bootDelayUS(500000);
    }
}

//TTODO: do not modify this function
int _boot_app_init(const void* param)
{
    LOAD_DATA(sram_data);
    CLEAR_DATA(sram_bss);
    s_bootAppParamPtr = (BootAppParam_t*)param;
    bootShowMemInfo();
    return 0;
}

BootAppWorkResult_e _boot_app_work(const void* param)
{
    bootLog("_boot_app_work");
    i2cTest();
    uartTest();
    gpio_test();
    fs_test();
    mytime_t my_time;
    bootReadRtcTime(&my_time,false);
    bootLog("%d-%d-%d %d:%d:%d %d\n", my_time.nYear, my_time.nMonth, my_time.nDay,
    my_time.nHour, my_time.nMin, my_time.nSec, my_time.DayIndex);
    return WORK_RESULT_ENTER_AP;
}

int _boot_app_deinit(const void* param)
{
    return 0;
}
