#ifndef __UART_H
#define __UART_H

/*******************************************************************************
 ** MACROS
 ******************************************************************************/
#define LogPrintf(fmt, args...)    do { KING_SysLog("uart: "fmt, ##args); } while(0)

#define UART_BUF_MAX_LEN 128

//#define LOG_P(ret,format)   {if(ret!=0){DBG_U2_Send(#format)}}
//#define LOG_E(format)       DBG_U2_Send(#format)

extern uint8 U2_TxBuff[1000];

void DBG_U2_Send(char *format,...);
int Uart2_Init(void);
void LOG_P(int ret,char *format,...);


#endif /* __UART_H */
