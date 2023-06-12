#ifndef __UART_H
#define __UART_H

/*******************************************************************************
 ** MACROS
 ******************************************************************************/
#define LogPrintf(fmt, args...)    do { KING_SysLog("uart: "fmt, ##args); } while(0)

#define UART_BUF_MAX_LEN 128

#define U2_RX_SIZE	2048
#define U2_TX_SIZE	2048
#define U2_RX_MAX	256

#define URxNUM		10

/* uart control blok */
typedef struct{
	/* s用于标记起始位置 */
	uint8 *start;
	/* e用于标记结束位置 */
	uint8 *end;
}UCB_URxBuffPtr;

/* 串口控制区指针结构体 */
typedef struct{
	/* 积累接受数据量 */
	uint16 URxCounter;
	/* se指针对于结构体数组 */
	UCB_URxBuffPtr URxDataPtr[URxNUM];
	/* IN指针用于标记接收数据 */
	UCB_URxBuffPtr *URxDataIN;
	/* OUT指针用于提取接受的数据 */
	UCB_URxBuffPtr *URxDataOUT;
	/* IN 和 OUT 指针的结尾标志 */
	UCB_URxBuffPtr *URxDataEND;
}UCB_CB;

extern UCB_CB U2CB;
extern uint8 U2_RxBuff[U2_RX_SIZE];

//#define LOG_P(ret,format)   {if(ret!=0){DBG_U2_Send(#format)}}
//#define LOG_E(format)       DBG_U2_Send(#format)

/* 串口2调试函数 */
void U2_Send(char *format,...);
/* 串口2初始化 */
int Uart2_Init(void);
/* 串口2日志打印函数 */
void LOG_P(int ret,char *format,...);
/* 485串口初始化函数(串口三) */
int rs485_init(void);
/* 485串口数据转存函数 */
void rs_485_data_resive(void);
/* 485串口接收检测函数 */
void rs485_input_detection(void);
/* 485数据处理函数 */
void rs485_event_handle(uint8 *data,uint16 datalen);
/* 串口2数据回调函数 */
void Uart2EventCb(uint32 eventID, void* pData, uint32 len);


#endif /* __UART_H */
