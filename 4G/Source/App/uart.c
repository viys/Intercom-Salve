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
#include "io.h"

uint8 U2_TxBuff[500];
uint8 RS485_RxBuff[RS485_RX_SIZE];
uint8 RS485_TxBuff[RS485_TX_SIZE];
UCB_CB RS485CB;

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

/* 485串口配置 */
int rs485_config(void)
{
    int ret = SUCCESS;

    UART_CONFIG_S cfg;
    cfg.baudrate = UART_BAUD_115200;
    cfg.byte_size = UART_BYTE_LEN_8;
    cfg.flow_ctrl = UART_NO_FLOW_CONTROL;
    cfg.parity = UART_NO_PARITY;
    cfg.stop_bits = UART_ONE_STOP_BIT; 
    ret = KING_UartInit(UART_485, &cfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_UartInit() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    ret = KING_RegNotifyFun(DEV_CLASS_UART, UART_485, RS485EventCb);
    if (FAIL == ret)
    {
        LogPrintf("KING_RegNotifyFun() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    return SUCCESS;
}

void rs485_ptr_init(void)
{
	/* IN指针指向SE指针对结构体数组的0号成员 */
	RS485CB.URxDataIN = &RS485CB.URxDataPtr[0];
	/* OUT指针指向SE指针对结构体数组的0号成员 */
	RS485CB.URxDataOUT = &RS485CB.URxDataPtr[0];
	/* END指针指向SE指针对结构体数组的最后一个成员 */
	RS485CB.URxDataEND = &RS485CB.URxDataPtr[URxNUM-1];
	/* 使用 IN 指针指向的SE指针对中的S指针标记第一次接收的起始位置 */
	RS485CB.URxDataIN->start = RS485_RxBuff;
    RS485CB.URxDataIN->end = RS485_RxBuff;

	/* 积累接收数量清零 */
	RS485CB.URxCounter = 0;
}

/**
 * 485串口初始化
 *
 */
int rs485_init(void)
{
    rs485_config();

    rs485_ptr_init();

    return SUCCESS;
}

/**
 * 485串口回调函数
 *
 */
void RS485EventCb(uint32 eventID, void* pData, uint32 len)
{
    if (eventID != UART_EVT_DATA_TO_READ)return;

    /* 产生485标志事件 */
    KING_EventSet(rs485EH);
}

void rs_485_data_resive(void)
{
    uint32 readLen = 0;

    /* 将缓存区数据存入环形队列中 */
    KING_UartRead(UART_485, (uint8 *)RS485CB.URxDataIN->start, RS485_RX_MAX, &readLen);
    
    /* 将本次接收量累加到 URxCounrer */
    RS485CB.URxCounter += readLen;
    /* IN指针指向的结构体中的e指针记录本次接收结束位置 */
    RS485CB.URxDataIN->end = &RS485_RxBuff[RS485CB.URxCounter - 1];

    /* IN指针后移 */
    RS485CB.URxDataIN++;

    /* 后移至END标记的位置 */
    if(RS485CB.URxDataIN == RS485CB.URxDataEND){
        /* 回卷,重新指向0号成员 */
        RS485CB.URxDataIN = &RS485CB.URxDataPtr[0];
    }
    /* 如果剩余空间量大于等于单次接收最大量 */
    if(RS485_RX_SIZE - RS485CB.URxCounter >= RS485_RX_MAX){
        RS485CB.URxDataIN->start = &RS485_RxBuff[RS485CB.URxCounter];
        /* 非必须 */
        RS485CB.URxDataIN->end = &RS485_RxBuff[RS485CB.URxCounter];
    }else{
        /* 回卷,下次的接收位置返回缓冲区的起始位置 */
        RS485CB.URxDataIN->start = RS485_RxBuff;
        /* 累计值清零 */
        RS485CB.URxCounter = 0;
    }
}

/* 485输入检测 */
void rs485_input_detection(void)
{
	if(RS485CB.URxDataOUT != RS485CB.URxDataIN){
		/* 命令处理 */
		rs485_event_handle(RS485CB.URxDataOUT->start,RS485CB.URxDataOUT->end - RS485CB.URxDataOUT->start + 1);
        /* 指针后移 */
		RS485CB.URxDataOUT ++;
		
        /* 数据回卷 */
		if(RS485CB.URxDataOUT == RS485CB.URxDataEND){
			RS485CB.URxDataOUT = &RS485CB.URxDataPtr[0];
		}	
	}
}

/* 485命令行事件处理 */
void rs485_event_handle(uint8 *data,uint16 datalen)
{
    if(datalen!=0){
        KING_UartWrite(UART_485,data,datalen,&writeLen);
    }
}

/* 485数据发送函数 */
void RS485_Send(char *format,...)
{
	/* 建立一个va_list变量listdata */
	va_list listdata;
	/* 向listdata加载,...代表不定长的参数 */
	va_start(listdata,format);
	
	/* 格式化输出到缓冲区 U0_TxBuff */
	vsprintf((char *)RS485_TxBuff,format,listdata);
	/* 释放lostdata */
	va_end(listdata);
    /* 发送缓冲区数据 */

#if RS485_AUTO
    KING_GpioSet(GPIO_15,1);
#endif
    KING_UartWrite(UART_485,RS485_TxBuff,strlen((const char*)RS485_TxBuff),&writeLen);
#if RS485_AUTO
    KING_GpioSet(GPIO_15,0);
#endif
    memset(RS485_TxBuff,0x00,RS485_TX_SIZE);
}

void LOG_P(int ret,char *format,...)
{
#if DBG_LOG
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
        memset(U2_TxBuff,0x00,500);
    }
#endif
}
