/********************************************************************************* File Name:   app_main.c** Copyright:   Copyright 2020-2020 by Xiamen CHEERZING IOT Technology Co., Ltd.**              All Rights Reserved.**              This software is supplied under the terms of a license**              agreement or non-disclosure agreement with Cheerzing.**              Passing on and copying of this document,and communication**              of its contents is not permitted without prior written**              authorization.** Description: ****************************************************************************************************************************************************************                        Edit History                                        *** ---------------------------------------------------------------------------*** DATE           NAME             Description:**----------------------------------------------------------------------------*** 2020-04-23     suzhiliang         Create.******************************************************************************//********************************************************************************* Header Files******************************************************************************/#include "app_main.h"#include "at.h"#include "uart_init.h"#include "device_init.h"/********************************************************************************* MACROS******************************************************************************//********************************************************************************* Type Definitions******************************************************************************/MSG_HANDLE msgHandle = NULL;/********************************************************************************* Variables******************************************************************************//********************************************************************************* External Function Declerations******************************************************************************//********************************************************************************* Local Function Declerations******************************************************************************//********************************************************************************* Functions******************************************************************************//** *  king APP 入口函数 * **/void APP_Main(uint32 bootMode){    int ret = -1;    MSG_S msg;    KING_WakeLock(LOCK_SUSPEND,(uint8 *)"mainlock");    Uart_All_Init();    Device_Init();        LogPrintf("------APP Main is running--------\r\n");    ATPrintf("RDY\r\n");        //msg    ret = KING_MsgCreate(&msgHandle);    if (-1 == ret)    {        LogPrintf("KING_MsgCreate Fail\r\n");    }      while (1)    {        memset(&msg, 0x00, sizeof(MSG_S));        KING_MsgRcv(&msg, msgHandle, WAIT_OPT_INFINITE);        switch (msg.messageID)        {            case MSG_EVT_UART1_READ:            {                At_CmdHandle();                break;            }            case MSG_EVT_UART2_READ:            {                ret = Uart_ReadEcho(UART_2);                if (FAIL == ret)                {                    LogPrintf("Uart 2 read echo fail!\r\n");                }                break;            }            case MSG_EVT_UART3_READ:            {                ret = Uart_ReadEcho(UART_3);                if (-1 == ret)                {                    LogPrintf("Uart 3 read echo fail!\r\n");                }                break;            }            case MSG_EVT_RECORD:            {                uint32 bytesWrite = 0;                                KING_FsFileWrite(AudioFileHandle, msg.pData, msg.DataLen, &bytesWrite);                                break;            }            case MSG_EVT_RECORD_STOP:            {                uint32 size;                //文件大小                KING_FsFileSizeGet(AudioFileHandle, &size);                LogPrintf("get file size %d \r\n", size);                KING_FsCloseHandle(AudioFileHandle);                AudioFileHandle = 0;                LogPrintf("stop\r\n");                break;            }            case MSG_EVT_GPIO_INT:            {                LogPrintf("gpio int\r\n");                break;            }            default:            {                LogPrintf("msg no know\r\n");                break;            }        }    }}