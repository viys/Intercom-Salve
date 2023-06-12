/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "KingCSTD.h"
#include "KingPlat.h"
#include "KingRtos.h"
#include "KingUart.h"
#include "app.h"
#include "uart.h"




/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
enum Msg_Evt
{

    MSG_EVT_BASE = 0,
    MSG_EVT_UART2_READ,
    MSG_EVT_UART3_READ,
    MSG_EVT_END,
};

/*******************************************************************************
 ** Variables
 ******************************************************************************/

static MSG_HANDLE msgHandle = NULL;

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
 * uart2 回调函数
 *
 */
static void Uart2EventCb(uint32 eventID, void* pData, uint32 len)
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
static void Uart3EventCb(uint32 eventID, void* pData, uint32 len)
{
    MSG_S msg;
    if (eventID != UART_EVT_DATA_TO_READ)return;
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = MSG_EVT_UART3_READ;
    KING_MsgSend(&msg, msgHandle);
}

/**
 * uart2 初始化
 *
 */
int Uart2_Init(void)
{
    int ret = SUCCESS;
    UART_CONFIG_S cfg;
    cfg.baudrate = UART_BAUD_921600;
    cfg.byte_size = UART_BYTE_LEN_8;
    cfg.flow_ctrl = UART_NO_FLOW_CONTROL;
    cfg.parity = UART_NO_PARITY;
    cfg.stop_bits = UART_ONE_STOP_BIT; 
    ret = KING_UartInit(UART_2, &cfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_UartInit() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    ret = KING_RegNotifyFun(DEV_CLASS_UART, UART_2, Uart2EventCb);
    if (FAIL == ret)
    {
        LogPrintf("KING_RegNotifyFun() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    return SUCCESS;
}

/**
 * uart3 初始化
 *
 */
static int Uart3_Init(void)
{
    int ret = SUCCESS;
    UART_CONFIG_S cfg;
    cfg.baudrate = UART_BAUD_115200;
    cfg.byte_size = UART_BYTE_LEN_8;
    cfg.flow_ctrl = UART_NO_FLOW_CONTROL;
    cfg.parity = UART_NO_PARITY;
    cfg.stop_bits = UART_ONE_STOP_BIT; 
    ret = KING_UartInit(UART_3, &cfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_UartInit() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    ret = KING_RegNotifyFun(DEV_CLASS_UART, UART_3, Uart3EventCb);
    if (FAIL == ret)
    {
        LogPrintf("KING_RegNotifyFun() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    return SUCCESS;
}

/**
 * uart2 uart3初始化AT
 *
 */
void Uart_Init(void)
{
    int ret = SUCCESS;
    ret = Uart2_Init();
    if (FAIL == ret)
    {
        LogPrintf("Uart2 Init Fail\r\n");
    }
    ret = Uart3_Init();
    if (FAIL == ret)
    {
        LogPrintf("Uart3 Init Fail\r\n");
    }
    
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
    LogPrintf("uart%d buff len: %d \r\n",id,availableLen);

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
    LogPrintf("uart%d read len: %d read data: %s \r\n",id,readLen ,pdata);

    

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

void Uart_Test(void)
{
    int ret;
    MSG_S msg;

    
    LogPrintf("\r\n-----Uart test start------\r\n");
    Uart_Init();
    //msg
    ret = KING_MsgCreate(&msgHandle);
    if(FAIL == ret)
    {
        LogPrintf("KING_MsgCreate() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    
    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        KING_MsgRcv(&msg, msgHandle, WAIT_OPT_INFINITE);
        switch (msg.messageID)
        {
            case MSG_EVT_UART2_READ:
            {
                Uart_ReadEcho(UART_2);
                break;
            }
            case MSG_EVT_UART3_READ:
            {
                Uart_ReadEcho(UART_3);
                break;
            }
            default:
            {
                LogPrintf("msg no know\r\n");
                break;
            }
        }
    }
}

uint8 U2_TxBuff[1000];

void DBG_U2_Send(char *format,...)
{
	/* 建立一个va_list变量listdata */
	va_list listdata;
	/* 向listdata加载,...代表不定长的参数 */
	va_start(listdata,format);
	
	/* 格式化输出到缓冲区 U0_TxBuff */
	vsprintf((char *)U2_TxBuff,format,listdata);
	/* 释放lostdata */
	va_end(listdata);
    /* 发送缓冲区数据 */
    KING_UartWrite(UART_2,U2_TxBuff,strlen((const char*)U2_TxBuff),&writeLen);
    memset(U2_TxBuff,0x00,1000);
}


void LOG_P(int ret,char *format,...)
{
    if(ret != 0){
        /* 建立一个va_list变量listdata */
        va_list listdata;
        /* 向listdata加载,...代表不定长的参数 */
        va_start(listdata,format);
        
        /* 格式化输出到缓冲区 U0_TxBuff */
        vsprintf((char *)U2_TxBuff,format,listdata);
        /* 释放lostdata */
        va_end(listdata);
        /* 发送缓冲区数据 */
        KING_UartWrite(UART_2,U2_TxBuff,strlen((const char*)U2_TxBuff),&writeLen);
        memset(U2_TxBuff,0x00,1000);
    }
}
