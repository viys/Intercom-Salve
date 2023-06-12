#ifndef __UART_INIT_H__
#define __UART_INIT_H__


void Uart_All_Init(void);
int Uart_Init(uint32 id, UART_CONFIG_S cfg,NOTIFY_FUN cb);
int Uart_Deinit(uint32 id);

void ATPrintf(const char *fmt, ...);
void LogPrintf(const char *fmt, ...);
int Uart_ReadEcho(uint32 id);


void AtEventCb(uint32 eventID, void* pData, uint32 len);
void Uart2EventCb(uint32 eventID, void* pData, uint32 len);
void Uart3EventCb(uint32 eventID, void* pData, uint32 len);



#endif

