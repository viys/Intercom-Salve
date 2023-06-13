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

uint8 U2_RxBuff[U2_RX_SIZE];
uint8 U2_TxBuff[U2_TX_SIZE];
UCB_CB 	U2CB;


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


/**
 * uart2 初始化
 *
 */
int Uart2_Init(void)
{
    int ret = SUCCESS;
    UART_CONFIG_S cfg;
    cfg.baudrate = UART_BAUD_115200;
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

void u2rx_ptr_init(void)
{
	/* IN指针指向SE指针对结构体数组的0号成员 */
	U2CB.URxDataIN = &U2CB.URxDataPtr[0];
	/* OUT指针指向SE指针对结构体数组的0号成员 */
	U2CB.URxDataOUT = &U2CB.URxDataPtr[0];
	/* END指针指向SE指针对结构体数组的最后一个成员 */
	U2CB.URxDataEND = &U2CB.URxDataPtr[URxNUM-1];
	/* 使用 IN 指针指向的SE指针对中的S指针标记第一次接收的起始位置 */
	U2CB.URxDataIN->start = U2_RxBuff;
    U2CB.URxDataIN->end = U2_RxBuff;

	/* 积累接收数量清零 */
	U2CB.URxCounter = 0;
}

/**
 * 485串口初始化
 *
 */
int rs485_init(void)
{
    Uart2_Init();

    u2rx_ptr_init();

    U2_Send("U2CB.URxDataIN->end - U2CB.URxDataIN->start =%d\r\n",(U2CB.URxDataIN->end - U2CB.URxDataIN->start));
    U2_Send("&U2_RxBuff[0] - &U2_RxBuff[1] = %d\r\n",&U2_RxBuff[1] - &U2_RxBuff[0]);

    return SUCCESS;
}

/**
 * uart2 回调函数
 *
 */
void Uart2EventCb(uint32 eventID, void* pData, uint32 len)
{
    if (eventID != UART_EVT_DATA_TO_READ)return;

    /* 产生485标志事件 */
    KING_EventSet(rs485EH);
}

void rs_485_data_resive(void)
{
    int ret = 0;
    uint32 readLen = 0;
    uint32 availableLen = 0;

    /*获取缓存区数据字节长*/
    // ret =  ret = KING_UartGetAvailableBytes(UART_2,&availableLen);
    // LOG_P(ret,"KING_UartGetAvailableBytes() UART_2 Fail!\r\n");
    /* 将缓存区数据存入环形队列中 */
    KING_UartRead(UART_2, (uint8 *)U2CB.URxDataIN->start, U2_RX_MAX, &readLen);
    U2_Send("Resive %d byte\r\n",readLen);
    
    /* 将本次接收量累加到 URxCounrer */
    U2CB.URxCounter += readLen;
    /* IN指针指向的结构体中的e指针记录本次接收结束位置 */
    U2CB.URxDataIN->end = &U2_RxBuff[U2CB.URxCounter - 1];

    /* IN指针后移 */
    U2CB.URxDataIN++;
    U2_Send("Total %d byte\r\n",U2CB.URxCounter);
    /* 后移至END标记的位置 */
    if(U2CB.URxDataIN == U2CB.URxDataEND){
        /* 回卷,重新指向0号成员 */
        U2CB.URxDataIN = &U2CB.URxDataPtr[0];
    }
    /* 如果剩余空间量大于等于单次接收最大量 */
    if(U2_RX_SIZE - U2CB.URxCounter >= U2_RX_MAX){
        U2CB.URxDataIN->start = &U2_RxBuff[U2CB.URxCounter];
        /* 非必须 */
        U2CB.URxDataIN->end = &U2_RxBuff[U2CB.URxCounter];
    }else{
        /* 回卷,下次的接收位置返回缓冲区的起始位置 */
        U2CB.URxDataIN->start = U2_RxBuff;
        /* 累计值清零 */
        U2CB.URxCounter = 0;
    }

}

/* 串口3输入检测 */
void rs485_input_detection(void)
{
	if(U2CB.URxDataOUT != U2CB.URxDataIN){
		/* 命令处理 */
		rs485_event_handle(U2CB.URxDataOUT->start,U2CB.URxDataOUT->end - U2CB.URxDataOUT->start + 1);
        U2_Send("U2CB.URxDataOUT->end - U2CB.URxDataOUT->start + 1 = %d\r\n",U2CB.URxDataOUT->end - U2CB.URxDataOUT->start + 1);
		U2CB.URxDataOUT ++;
		
        /* 数据回卷 */
		if(U2CB.URxDataOUT == U2CB.URxDataEND){
			U2CB.URxDataOUT = &U2CB.URxDataPtr[0];
		}	
	}
}

/* 485命令行事件处理 */
void rs485_event_handle(uint8 *data,uint16 datalen)
{
    if(datalen!=0){
        KING_UartWrite(UART_2,data,datalen,&writeLen);
        U2_Send("\r\nrs485_event_handle() is run,datalen: %d\r\n",datalen);
    }
}


void U2_Send(char *format,...)
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
    memset(U2_TxBuff,0x00,2048);
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
