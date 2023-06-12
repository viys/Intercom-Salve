/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "KingUart.h"
#include "KingRtos.h"
#include "KingCSTD.h"
#include "KingPlat.h"

#include "app_main.h"

/*******************************************************************************
 ** MACROS
 ******************************************************************************/
#define LogPrintf(fmt, args...)    do { KING_SysLog("at: "fmt, ##args); } while(0)

#define AT_CMD_BUF_MAX_LEN (2048-8)
#define AT_CMD_RSP_MAX_LEN 200
#define UART_LOG_MAX_LEN   2048

#define EX_AT_CMD_TAB_SIZE sizeof(s_externAtCmdTab)/sizeof(s_externAtCmdTab[0])

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/

typedef int(*EX_AT_PRO_CALLBACK)(char* data, uint32 dataLen);

typedef struct
{
    char* cmdStr;
    EX_AT_PRO_CALLBACK callBack;
} EX_AT_PRO_TAB_T;

enum Msg_Evt
{

    MSG_EVT_BASE = 0,
    MSG_EVT_UART_READ,
    MSG_EVT_END,
};

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static MSG_HANDLE msgHandle = NULL;

static char s_atCmdStr[AT_CMD_BUF_MAX_LEN+1] = {0};


/*******************************************************************************
 ** External Function Declerations
 ******************************************************************************/
static int AtSysImei(char* data, uint32 dataLen);

static EX_AT_PRO_TAB_T s_externAtCmdTab[] =
{
    {"+IMEI",AtSysImei},
};


/*******************************************************************************
 ** Local Function Declerations
 ******************************************************************************/

/*******************************************************************************
 ** Functions
 ******************************************************************************/
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
 /**
 *  AT system imei获取
 *
 **/
static int AtSysImei(char* data, uint32 dataLen)
{
    uint8 imei[17] = {0};
    if (0 != dataLen)
    {
        return FAIL;
    }

    //get system imei
    memset(imei, 0x00, sizeof(imei));
    if (FAIL == KING_GetSysImei(imei))
    {
       return FAIL;
    }
    ATPrintf("\r\nIMEI:%s\r\n",imei);
    return SUCCESS;
}
/**
*  AT 串口事件回调
*
**/

static void AtEventCb(uint32 eventID, void* pData, uint32 len)
{
    MSG_S msg;
    if(eventID != UART_EVT_DATA_TO_READ)return;
    KING_SysLog("uart read success\r\n");
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = MSG_EVT_UART_READ;
    KING_MsgSend(&msg, msgHandle);
    
}

/**
 * uart 初始化
 *
 */
int Uart_Init()
{
    int ret;
    UART_CONFIG_S cfg;
    cfg.baudrate = UART_BAUD_115200;
    cfg.byte_size = UART_BYTE_LEN_8;
    cfg.flow_ctrl = UART_NO_FLOW_CONTROL;
    cfg.parity = UART_NO_PARITY;
    cfg.stop_bits = UART_ONE_STOP_BIT; 
    ret = KING_UartInit(UART_1, &cfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_UartInit() Failed! errcode=0x%x\r\n",KING_GetLastErrCode());
        return FAIL;
    }
    ret = KING_RegNotifyFun(DEV_CLASS_UART, UART_1, AtEventCb);
    if (FAIL == ret)
    {
        LogPrintf("KING_RegNotifyFun() Failed! errcode=0x%x\r\n",KING_GetLastErrCode());
        return FAIL;
    }
    return SUCCESS;
}

/**
 *    AT数据读取
 *    
 *    @return 0 SUCCESS  -1 FAIL
 */

static int At_CmdDataGet(void)
{
    uint32 availableLen = 0;
    uint32 readLen = 0;
    int ret = SUCCESS;
    
    //获取缓存区数据字节长
    ret = KING_UartGetAvailableBytes(UART_1,&availableLen);
    if((0 == availableLen) || (FAIL == ret))
    {
        LogPrintf("KING_UartGetAvailableBytes() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    if (availableLen > AT_CMD_BUF_MAX_LEN)
    {
        availableLen = AT_CMD_BUF_MAX_LEN;
    }
    LogPrintf("uart available len: %d \r\n",availableLen);

    //读缓存内容
    memset(s_atCmdStr, 0x00, AT_CMD_BUF_MAX_LEN+1);
    ret = KING_UartRead(UART_1, (uint8 *)s_atCmdStr, availableLen,&readLen);
    if (FAIL == ret)
    {
        LogPrintf("KING_UartRead() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    KING_UartPurge(UART_1);
    LogPrintf("uart read len: %d read data: %s \r\n",readLen ,s_atCmdStr);
    return SUCCESS;
}

/**
 *    AT命令处理判断
 *    
 *
 */
void At_CmdHandle(void)
{

    uint32 len;
    int ret = -1;
    int i = 0;
    char *p_cmd;

    ret = At_CmdDataGet();
    if (FAIL == ret)
    {
        LogPrintf("at read error\r\n");
        return;
    }

    ATPrintf("%s",s_atCmdStr);
    //处理\r\n问题
    p_cmd = strstr(s_atCmdStr, "\r\n");
    if (NULL != p_cmd)
    {
        memset(p_cmd,0x00,2);
    }
    
    //判断是不是AT指令
    if (strncasecmp(s_atCmdStr, "AT", 2))
    {
        ATPrintf("ERROR\r\n");
        return;
    }
    
    //如果只是AT
    if (!strncasecmp(s_atCmdStr, "AT", strlen(s_atCmdStr)))
    {
        ATPrintf("OK\r\n");
        return;
    }

    //查找AT指令
    for ( i = 0; i < EX_AT_CMD_TAB_SIZE; i++)
    {
        len = strlen((s_externAtCmdTab[i].cmdStr));
        if (strncasecmp(s_atCmdStr+2, s_externAtCmdTab[i].cmdStr, len) == 0)
        {
            if (s_externAtCmdTab[i].callBack != NULL)
            { 
                ret = s_externAtCmdTab[i].callBack(s_atCmdStr+2+len, strlen(s_atCmdStr)-2-len);
                if (FAIL == ret)
                {

                    ATPrintf("ERROR\r\n");
                }
                else
                {
                    ATPrintf("OK\r\n");
                }
                return;
              }
        }
    }
    //未找到
    ATPrintf("ERROR\r\n");
    return;
}

/**
*  AT 测试
*
**/

void At_Test(void)
{
    int ret;
    MSG_S msg;
    
    LogPrintf("\r\n-----At Test Start------\r\n");
    if(FAIL == Uart_Init())
    {
        return;
    }
  
    //msg
    ret = KING_MsgCreate(&msgHandle);
    if (FAIL == ret)
    {
        LogPrintf("KING_MsgCreate() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    
    while (1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        KING_MsgRcv(&msg, msgHandle, WAIT_OPT_INFINITE);
        switch (msg.messageID)
        {
            case MSG_EVT_UART_READ:
            {
                At_CmdHandle();
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



