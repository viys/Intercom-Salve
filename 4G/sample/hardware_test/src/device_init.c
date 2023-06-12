/*******************************************************************************
** File Name:   device_init.c
** Copyright:   Copyright 2020-2020 by Xiamen CHEERZING IOT Technology Co., Ltd.
**              All Rights Reserved.
**              This software is supplied under the terms of a license
**              agreement or non-disclosure agreement with Cheerzing.
**              Passing on and copying of this document,and communication
**              of its contents is not permitted without prior written
**              authorization.
** Description: device init
*******************************************************************************

*******************************************************************************
**                        Edit History                                        *
** ---------------------------------------------------------------------------*
** DATE           NAME             Description:
**----------------------------------------------------------------------------*
** 2020-05-20     suzhiliang         Create.
******************************************************************************/

/*******************************************************************************
** Header Files
******************************************************************************/

#include "app_main.h"
#include "device_init.h"
#include "uart_init.h"
/*******************************************************************************
** MACROS
******************************************************************************/

/*******************************************************************************
** Type Definitions
******************************************************************************/


/*******************************************************************************
** Variables
******************************************************************************/

/*******************************************************************************
** External Function Declerations
******************************************************************************/

/*******************************************************************************
** Local Function Declerations
******************************************************************************/
extern const unsigned char gImage_logo[38400];

/*******************************************************************************
** Functions
******************************************************************************/

/**
 *    keypad 回调
 *
 */
static void Keypad_CallBack(KEYPAD_EVENT_E eventCode,uint32 keyCode)
{
    uint16 keyCodeH = 0; //row value,row index start with 0
    uint16 keyCodeL = 0; //colum value,colum index start with 0

    keyCodeL = keyCode & 0x0000ffff;
    keyCodeH = (keyCode >> 16) & 0x0000ffff;
    LogPrintf("edge trigger : %s, row colum value : %d,%d\r\n", eventCode?"Up":"Down", keyCodeH,keyCodeL);
}

/**
 *    keypad 初始化
 *    @return 0 SUCCESS  -1 FAIL
 */
int Keypad_Init(void)
{
    int ret = SUCCESS;
    ret = KING_KpInit();
    if (FAIL == ret)
    {
        LogPrintf("KING_KpInit Fail！ errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    ret = KING_KpRegCB(Keypad_CallBack);
    if (FAIL == ret)
    {
        LogPrintf("KING_KpRegCB Fail！ errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    return SUCCESS;
}

/**
 *    keypad 去初始化
 *    @return 0 SUCCESS  -1 FAIL
 */
int Keypad_DeInit(void)
{
    int ret = SUCCESS;
    ret = KING_KpUnregCB();
    if (FAIL == ret)
    {
        LogPrintf("KING_KpUnregCB Fail！ errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    
    ret = KING_KpDeinit();
    if (FAIL == ret)
    {
        LogPrintf("KING_KpDeinit Fail！ errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    return SUCCESS;
    
}

/**
 *    lcd 初始化
 *
 */
static void Lcd_Init(void)
{
    int ret = SUCCESS;
    LCDDISPLAY_T rec = {0, 120, 240, 80};
    LCDCONFIG_T lcd_cfg;
    memset(&lcd_cfg, 0x00, sizeof(LCDCONFIG_T));
    ret = KING_LcdInit(0, &lcd_cfg);
    if(FAIL == ret)
    {
         LogPrintf("KING_LcdInit() errcode=0x%x\r\n", KING_GetLastErrCode());
    }

    KING_LcdDisplay(rec, (uint8 *)gImage_logo, TRUE, 0);
    KING_LcdShow(0);
    ret = KING_LcdScreenOn(0);
    if(FAIL == ret)
    {
         LogPrintf("KING_LcdScreenOn() errcode=0x%x\r\n", KING_GetLastErrCode());
    }
}

/**
 *    Gpio 初始化
 *
 */
static void Gpio_Init(void)
{
    int ret = SUCCESS;
    ret = KING_GpioInit();
    if (FAIL == ret)
    {
        LogPrintf("KING_GpioInit() errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    
    
}

/**
 *    iic 初始化
 *
 */
static void I2c_Init(void)
{
    int ret = SUCCESS;
    ret = KING_IicInit();
    if (FAIL == ret)
    {
       LogPrintf("KING_IicInit() errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    
}

/**
 *    fs 初始化
 *
 */
static void Fs_Init(void)
{
    int ret = SUCCESS;

    ret = KING_FsInit();
    if (FAIL == ret)
    {
        LogPrintf("KING_FsInit() errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_FSMount(STORAGE_TYPE_SD,0,0,0,0);
    if (FAIL == ret)
    {
        LogPrintf("KING_FSMount(SD) errcode=0x%x\r\n", KING_GetLastErrCode());
    }
}

/**
 *    spi 初始化
 *
 */
static void Spi_Init(void)
{
    int ret = SUCCESS;

    ret = KING_SpiInit();
    if (-1 == ret)
    {
        LogPrintf("KING_SpiInit() errcode=0x%x\r\n", KING_GetLastErrCode());
    }
}

/**
 *    Audio 初始化
 *
 */
static void Audio_Init(void)
{
    int ret = SUCCESS;
    ret = KING_AudioPlayInit();
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioPlayInit fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    
    ret = KING_AudioRecordInit();
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioRecordInit fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
        return;
    }
}

/**
 *    Sink 初始化
 *
 */
static void Sink_Init(void)
{
    int ret = SUCCESS;

    ret = KING_SinkInit();
    if (FAIL == ret)
    {
        LogPrintf("KING_SinkInit fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
        return;
    }
}

/**
 *   全部设备初始化
 *
 */
void Device_Init(void)
{
    I2c_Init();
    Gpio_Init();
    Lcd_Init();
    Fs_Init();
    Spi_Init();
    Keypad_Init();
    Audio_Init();
    Sink_Init();
}


