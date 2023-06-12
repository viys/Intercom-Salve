#include "app_main.h"
#include "uart_init.h"
#include "at.h"
int AtUartCfg(char* data, uint32 dataLen)
{

    int ret = SUCCESS;
    UART_CONFIG_S cfg;
    uint32 uart_id;
    
    char *puart_id;
    char *puart_baudrate;
    char *puart_flow_ctrl;
    char *pend;
    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    data++;

    if (*data == '?')
    {
        ATPrintf("+UARTCFG: (1-3),(1200,2400,3600,4800,9600,14400,19200,28800,38400,57600,115200,230400,260800,921600),(0-2)\r\n");
        return SUCCESS;
    }

    puart_id = strtok(data,",");
    puart_baudrate = strtok(NULL, ",");
    puart_flow_ctrl = strtok(NULL, ",");
    pend = strtok(NULL, ",");
    //²ÎÊýÅÐ¶Ï
    if((NULL == puart_id) || (NULL == puart_baudrate) ||(NULL == puart_flow_ctrl) || (NULL != pend))
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }
    memset(&cfg, 0x00, sizeof(UART_CONFIG_S));
    uart_id = atoi(puart_id);
    cfg.baudrate = atoi(puart_baudrate);
    cfg.flow_ctrl = atoi(puart_flow_ctrl);
    if((UART_1 != uart_id) && (UART_2 != uart_id) && (UART_3 != uart_id))
    {
        LogPrintf("uart id param error\r\n");
        return FAIL;
    }
    if( (UART_BAUD_1200 != cfg.baudrate) && (UART_BAUD_2400 != cfg.baudrate) && (UART_BAUD_3600 != cfg.baudrate) &&
        (UART_BAUD_4800 != cfg.baudrate) && (UART_BAUD_9600 != cfg.baudrate) && (UART_BAUD_14400 != cfg.baudrate) && 
        (UART_BAUD_19200 != cfg.baudrate) && (UART_BAUD_28800 != cfg.baudrate) && (UART_BAUD_38400 != cfg.baudrate) &&
        (UART_BAUD_57600 != cfg.baudrate) && (UART_BAUD_115200 != cfg.baudrate) && (UART_BAUD_230400 != cfg.baudrate) &&
        (UART_BAUD_460800 != cfg.baudrate) && (UART_BAUD_921600 != cfg.baudrate))
    {
        LogPrintf("uart baudrate param error\r\n");
        return FAIL;
    }
    if((UART_NO_FLOW_CONTROL != cfg.flow_ctrl) && (UART_HW_FLOW_CONTROL != cfg.flow_ctrl) && (UART_SW_FLOW_CONTROL != cfg.flow_ctrl))
    {
        LogPrintf("uart flow control param error\r\n");
        return FAIL;
    }
    cfg.byte_size = UART_BYTE_LEN_8;
    cfg.parity = UART_NO_PARITY;
    cfg.stop_bits = UART_ONE_STOP_BIT; 
    LogPrintf("baudrate: %d,flow control: %d\r\n", cfg.baudrate, cfg.flow_ctrl);
    KING_Sleep(100);
    ret = Uart_Deinit(uart_id);
    if(FAIL == ret)
    {

        LogPrintf("uart deinit fail\n");
        return FAIL;
    }
    switch(uart_id)
    {
        case UART_1:
        {

            ret = Uart_Init(uart_id,cfg,AtEventCb);
            break;
        }

        case UART_2:
        {
            ret = Uart_Init(uart_id,cfg,Uart2EventCb);
            break;
        }
        case UART_3:
        {
            ret = Uart_Init(uart_id,cfg,Uart3EventCb);
            break;
        }
    }
    if (FAIL == ret)
    {
        
        LogPrintf("Uart%d Init Fail\r\n",uart_id);
        return FAIL;
    }
    
    return SUCCESS;
}


