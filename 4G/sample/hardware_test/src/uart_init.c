/*******************************************************************************
** File Name:   uart_test.c
** Copyright:   Copyright 2020-2020 by Xiamen CHEERZING IOT Technology Co., Ltd.
**              All Rights Reserved.
**              This software is supplied under the terms of a license
**              agreement or non-disclosure agreement with Cheerzing.
**              Passing on and copying of this document,and communication
**              of its contents is not permitted without prior written
**              authorization.
** Description: uart2 api test code
*******************************************************************************

*******************************************************************************
**                        Edit History                                        *
** ---------------------------------------------------------------------------*
** DATE           NAME             Description:
**----------------------------------------------------------------------------*
** 2020-04-27     suzhiliang         Create.
******************************************************************************/

/*******************************************************************************
** Header Files
******************************************************************************/
#include "app_main.h"
#include "uart_init.h"
/*******************************************************************************
** MACROS
******************************************************************************/
#define UART_LOG_MAX_LEN        2048

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

/*******************************************************************************
** Functions
******************************************************************************/

/**
 * uart1 回调函数
 *
 */
void AtEventCb(uint32 eventID, void* pData, uint32 len)
{
    MSG_S msg;
    if (eventID != UART_EVT_DATA_TO_READ)return;
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = MSG_EVT_UART1_READ;
    KING_MsgSend(&msg, msgHandle);

}

/**
 * uart2 回调函数
 *
 */
void Uart2EventCb(uint32 eventID, void* pData, uint32 len)
{
    MSG_S msg;
    if (eventID != UART_EVT_DATA_TO_READ)return;
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = MSG_EVT_UART2_READ;
    KING_MsgSend(&msg, msgHandle);
    
}

/**
 * uart3 回调函数
 *
 */
void Uart3EventCb(uint32 eventID, void* pData, uint32 len)
{
    MSG_S msg;
    if (eventID != UART_EVT_DATA_TO_READ)return;
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = MSG_EVT_UART3_READ;
    KING_MsgSend(&msg, msgHandle);
}


/**
 * uart 初始化
 *
 */
int Uart_Init(uint32 id, UART_CONFIG_S cfg, NOTIFY_FUN cb)
{
    int ret;
    ret = KING_UartInit(id, &cfg);
    if (FAIL == ret)
    {
        LogPrintf("uart id: %d KING_UartInit() Failed! errcode=0x%x\r\n", id, KING_GetLastErrCode());
        return FAIL;
    }
    ret = KING_RegNotifyFun(DEV_CLASS_UART, id, cb);
    if (FAIL == ret)
    {
        LogPrintf("uart id: %d KING_RegNotifyFun() Failed! errcode=0x%x\r\n", id, KING_GetLastErrCode());
        return FAIL;
    }
    return SUCCESS;
}

/**
 * uart 去初始化
 *
 */
int Uart_Deinit(uint32 id)
{
    int ret;
    ret = KING_UartDeinit(id);
    if (FAIL == ret)
    {
        LogPrintf("uart id: %d KING_UartInit() Failed! errcode=0x%x\r\n", id, KING_GetLastErrCode());
        return FAIL;
    }
    return SUCCESS;
}

/**
 * uart1 uart2 uart3初始化
 *
 */
void Uart_All_Init(void)
{
    UART_CONFIG_S cfg;
    cfg.baudrate = UART_BAUD_115200;
    cfg.byte_size = UART_BYTE_LEN_8;
    cfg.flow_ctrl = UART_NO_FLOW_CONTROL;
    cfg.parity = UART_NO_PARITY;
    cfg.stop_bits = UART_ONE_STOP_BIT; 
    Uart_Init(UART_1,cfg,AtEventCb);
    Uart_Init(UART_2,cfg,Uart2EventCb);
    Uart_Init(UART_3,cfg,Uart3EventCb);
}

int Uart_ReadEcho(uint32 id)
{
    int ret = SUCCESS;
    uint8 *pdata = NULL;
    uint32 availableLen = 0;
    uint32 readLen = 0;
    uint32 writeLen = 0;
    
    //获取缓存区数据字节长
    ret = KING_UartGetAvailableBytes(id,&availableLen);
    if (FAIL == ret)
    {
        LogPrintf("KING_UartGetAvailableBytes() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }

    ret = KING_MemAlloc((void **)&pdata, availableLen+1);
    if (FAIL == ret)
    {
        LogPrintf("KING_MemAlloc() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    
    //读缓存内容
    memset(pdata, 0x00, availableLen+1);
    ret = KING_UartRead(id, pdata, availableLen, &readLen);
    if (FAIL == ret)
    {
        LogPrintf("KING_UartRead() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto EXIT;
        
    }

    //写
    ret = KING_UartWrite(id, pdata, readLen, &writeLen);
    if (FAIL == ret)
    {
        LogPrintf("KING_UartWrite() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto EXIT;
    }
    ret = SUCCESS;
EXIT:
    memset(pdata, 0x00, availableLen+1);
    KING_MemFree(pdata);
    pdata = NULL;
    return ret;
}


void ATPrintf(const char *fmt, ...)
{
    va_list ap; 
    char buffer[UART_LOG_MAX_LEN];
    uint32 writeLen = 0;

    if (fmt == NULL)
        return;
    
    memset(buffer, 0, UART_LOG_MAX_LEN);
    va_start(ap, fmt);
    vsnprintf(buffer, UART_LOG_MAX_LEN, fmt, ap);
    va_end(ap);
    KING_UartWrite(UART_1,(uint8 *)buffer, strlen(buffer), &writeLen);
}

void LogPrintf(const char *fmt, ...)
{
    va_list ap; 
    char buffer[UART_LOG_MAX_LEN];
    uint32 writeLen = 0;

    if (fmt == NULL)
        return;
    
    memset(buffer, 0, UART_LOG_MAX_LEN);
    va_start(ap, fmt);
    vsnprintf(buffer, UART_LOG_MAX_LEN, fmt, ap);
    va_end(ap);
    KING_UartWrite(UART_2,(uint8 *)buffer, strlen(buffer), &writeLen);
}


