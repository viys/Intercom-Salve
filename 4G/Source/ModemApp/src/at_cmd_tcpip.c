#include "At_common.h"
#include "At_errcode.h"
#include "At_module.h"
#include "AT_define.h"
#include "at_dispatch.h"
#include "at_data_engine.h"

#include "at_cmd_http.h"
#include "at_cmd_tcpip.h"

#include "at_cmd_utils.h"
#include "at_ringbuf.h"
#include "kingPlat.h"
#include "kingNet.h"
#include "kingRtos.h"
#include "kingCStd.h"
#include "kingCbData.h"
#include "kingAtApi.h"
#include "kingsocket.h"
#include "kingerrcode.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define ACCESS_MODE_BUFFER                  0
#define ACCESS_MODE_DIRECT                  1
#define ACCESS_MODE_TRANSPARENT             2

#define TCPIP_ALLOWED_ACTIVATED_MAX         3

#define TCPIP_OPERATE_SUCC                  0
#define TCPIP_SOCKET_ERROR                  -1

#define TCPIP_PDP_ACTIVE_MAX_TIMEOUT        150
#define TCPIP_PDP_DEACTIVE_MAX_TIMEOUT      40
#define TCPIP_CONNECT_MAX_TIMEOUT           150

#define TCPIP_NETWORK_ADDR_MAX_LEN          256  
#define TCPIP_TX_BUF_MAX_LEN                1460
#define TCPIP_RX_BUF_MAX_LEN                1500
#define TCPIP_RSP_MAX_LEN                   1024
#define TCPIP_TX_HEX_BUF_MAX_LEN            512

#define TCPIP_MAX_STRING_LENGTH             100
#define TCPIP_MAX_STRING_RSP                200
#define TCPIP_CONTEXT_ID_MAX_NUM            4
#define TCPIP_SOCKET_ID_MAX_NUM             5
#define TCPIP_IP_ADDR_STRINT_LEN            64

#define TCPIP_RX_BUF_MAX_SIZE               ((TCPIP_RX_BUF_MAX_LEN << 1) + 1)
#define TCPIP_TX_BUF_MAX_SIZE               ((TCPIP_TX_BUF_MAX_LEN << 1) + 1)

#define TCPIP_HAL_TICK1S                    1000
#define TCPIP_HAL_TICK900MS                 900

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef enum tcpipPdpType {
    TCPIP_PDP_TYPE_NONE,
    TCPIP_PDP_TYPE_IP,
    TCPIP_PDP_TYPE_IPV6,
    TCPIP_PDP_TYPE_IPV4V6,
    TCPIP_PDP_TYPE_MAX
} TCPIP_PDP_TYPE_E;

typedef enum TcpipStateType {
    TCPIP_STATE_INITIAL = 0,
    TCPIP_STATE_GPRSACT = 1,
    TCPIP_STATE_PDP_DEACT = 2,

    TCPIP_STATE_MAX
} TCPIP_STATE_TYPE_E;

typedef enum TcpipPdpActStatus {
    TCPIP_PDP_DEACTIVATE  = 0,
    TCPIP_PDP_ACTIVATE    = 1,

    TCPIP_PDP_MAX
} TCPIP_PDP_ACT_STATUS_E;

typedef enum TcpipSocketType {
    TCPIP_SOCK_TYPE_TCP = 0,
    TCPIP_SOCK_TYPE_UDP = 1,
    TCPIP_SOCK_TYPE_TCP_SERVER = 2,
    TCPIP_SOCK_TYPE_UDP_SERVER = 3,
    TCPIP_SOCK_TYPE_TCP_INCOMING = 4,
    TCPIP_SOCK_TYPE_NUM
} TCPIP_SOCKET_TYPE_E;

typedef enum TcpipSocketStateType{
    SOCKET_MODEM_INITIAL,
    SOCKET_MODEM_OPENING,
    SOCKET_MODEM_CONNECTED,
    SOCKET_MODEM_LISTENING,
    SOCKET_MODEM_CLOSING,
    SOCKET_MODEM_SEND,
    SOCKET_MODEM_CLOSED,
    SOCKET_MODEM_LISTEN,
    SOCKET_MODEM_OPEN_FAIL,

    SOCKET_MODEM_STATE_NUM
} TCPIP_SOCKET_STATE_TYPE_E;

typedef enum TcpipMsgIdType {
    TCPIP_MSG_ID_MIN,

    TCPIP_DNS_PARSE_REQ,
    TCPIP_RECV_DATA,
    
    TCPIP_MSG_ID_MAX
} TCPIP_MSG_ID_TYPE_E;

typedef enum TcpipCmd {
    CMD_QI_NONE,
    CMD_QICSGP,
    CMD_QIACT,
    CMD_QIDEACT,
    CMD_QIOPEN,
    CMD_QICLOSE,
    CMD_QISTATE,
    CMD_QISEND,
    CMD_QIRD,
    CMD_QISENDEX,
    CMD_QISWTMD,
    CMD_QPING,
    CMD_QNTP,
    CMD_QIDNSCFG,
    CMD_QIDNSGIP,
    CMD_QICFG,
    CMD_QISDE,
    CMD_QIGETERROR,
    CMD_ATO,
    CMD_RECVURC,
    CMD_CZPING,
    CMD_MAX
} TCPIP_CMD_E;

typedef struct TcpipSendMsg {
    uint8 socketID;
    uint8 ctxID;
    TCPIP_CMD_E atCmd;
    uint16 len;
} TCPIP_SEND_MSG_S;

typedef struct TcpipSendInfo {
    uint8 socketID;
    uint16 length;
    uint8 argCount;
    uint32 remotePort;
    char remoteAddr[TCPIP_NETWORK_ADDR_MAX_LEN];
} TCPIP_SEND_INFO_S;

typedef struct TcpipConfig {
    uint16 transPktSize;
    uint16 transWaitTm;
    uint16 sendDataFormat;
    uint16 recvDataFormat;  
    uint16 viewMode;
    uint16 backoffs;
    uint32 rto;
} TCPIP_CONFIG_S;

typedef struct tcpipQicsgpPdpCtx {
    uint8 cid; /**< PDP context identifier */
    uint8 type; /**< PDP type  */
    char apn[TCPIP_MAX_STRING_LENGTH + 1]; /**< APN*/
    char userName[TCPIP_MAX_STRING_LENGTH + 1]; /**< UserName length is limited to 50 octets. */
    char password[TCPIP_MAX_STRING_LENGTH + 1]; /**< Password length is limited to 50 octets.  */
    uint8 authType; /**< Authentication type.  */ 
} TCPIP_QICSGP_PDP_CTX_S;

typedef struct TcpipPdpInfo {
    softwareHandle hContext;
    uint8 ctxID;
    TCPIP_STATE_TYPE_E netState;
    TCPIP_PDP_ACT_STATUS_E actStatus;
    TCPIP_PDP_TYPE_E pdpType;
    char ipAddr[TCPIP_IP_ADDR_STRINT_LEN];
    AT_CMD_ENGINE_T *atEngine;
} TCPIP_PDP_INFO_S;

typedef struct TcpipSocketInfo{
    uint8 ctxID;
    uint8 socketID; 
    int socketfd;
    TCPIP_SOCKET_TYPE_E socketType;
    uint8 accessMode;
    uint16 localPort;
    uint8 state;

    uint16 serverID;

    bool inputData;

    char addrStr[TCPIP_NETWORK_ADDR_MAX_LEN];
    uint16 remotePort;
    char ipAddrStr[TCPIP_NETWORK_ADDR_MAX_LEN];

    uint32 totalSendNum;
    uint32 ackNum;

    int32 unsendLen;
    int32 sendDataLen;
    char sendBuf[TCPIP_TX_BUF_MAX_LEN];

    int32 totalRecvSize;
    int32 readLen;
    bool ttDataMode;
    bool isLastTT;

    bool running;
    uint8 nDLCI;
    AT_CMD_ENGINE_T *atEngine;
}TCPIP_SOCKET_INFO_S;

typedef struct tcpipBypassBuffer {
    uint8_t *buff;     // buffer, shared by all mode
    uint32_t buffLen;  // existed buffer data byte count
    uint32_t buffSize; // buffer size
    MUTEX_HANDLE mutex;
} TCPIP_BYPASS_BUFFER_T;

/*******************************************************************************
 ** External Variables 
 ******************************************************************************/
extern UINT8 g_rspStr[];
extern struct AT_DISPATCH_T *g_dispatch[AT_DISPATCH_ID_MAX];

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static TCPIP_QICSGP_PDP_CTX_S qicsgpInfo[TCPIP_CONTEXT_ID_MAX_NUM] = {0};
static TCPIP_PDP_INFO_S tcpipPdpInfo[TCPIP_CONTEXT_ID_MAX_NUM] = {0};
static TCPIP_SOCKET_INFO_S socketInfo[TCPIP_SOCKET_ID_MAX_NUM] = {0};
static AT_RING_BUF_S tcpipRxBuf[TCPIP_SOCKET_ID_MAX_NUM];
static AT_RING_BUF_S tcpipTtTxBuf[TCPIP_SOCKET_ID_MAX_NUM];
static TCPIP_SEND_INFO_S tcpipSendInfo;

static TCPIP_CONFIG_S tcpipConfig;
static hardwareHandle hNIC = -1;
static THREAD_HANDLE tcpipThreadID = NULL;
static MSG_HANDLE    tcpipMsgHandle = NULL;
static uint16 gTcpipCurErr = TCPIP_OPERATE_SUCC;
static uint8 gCurSocketID = 0xff;
static bool isRunning = FALSE;
static NET_ACTION tcpipAction = NET_ACTION_MAX;
static bool isClose = FALSE;
static uint16 activeType = 13;
static char qidnsgipHostname[TCPIP_NETWORK_ADDR_MAX_LEN];
static uint8 curCid = 0;
static TIMER_HANDLE connectTimer = NULL;
static AT_CMD_ENGINE_T * pTcpipAtEngine = NULL;
static MUTEX_HANDLE pSocketMutex[TCPIP_SOCKET_ID_MAX_NUM] = {NULL};

static TCPIP_BYPASS_BUFFER_T *gTTBypassBuf = NULL;
static TIMER_HANDLE ttTimer = NULL;

static const char* sockTypeString[TCPIP_SOCKET_ID_MAX_NUM] = {
  "TCP",          //TCPIP_SOCK_TYPE_TCP = 0,
  "UDP",          //TCPIP_SOCK_TYPE_UDP = 1,
  "TCP LISTENER", //TCPIP_SOCK_TYPE_TCP_SERVER = 2,
  "UDP SERVICE",  //TCPIP_SOCK_TYPE_UDP_SERVER = 3,
  "TCP INCOMING"  //TCPIP_SOCK_TYPE_TCP_INCOMING = 4,
};

/*******************************************************************************
 ** External Function Delcarations
 ******************************************************************************/
extern AT_DATA_ENGINE_T *at_DispatchGetDataEngine(AT_DISPATCH_T *th);


/*******************************************************************************
 ** Local Function Delcarations
 ******************************************************************************/
static int tcpip_qiactActSet(uint8 cid);
static void tcpip_connectTimer(bool isSet);
static void tcpip_ttTimer(bool isSet);
static void tcpip_ttExitDataMode(int isNoCarrier);

/*******************************************************************************
 ** Local Functions
 ******************************************************************************/
static void tcpip_atRspErr(AT_CMD_ENGINE_T *atEngine, uint16 errCode)
{
    gTcpipCurErr = errCode;
    if (atEngine != NULL)
    {
        at_CmdRespError(atEngine);
    }
}

static void tcpip_atRspOk(AT_CMD_ENGINE_T *atEngine)
{
    gTcpipCurErr = TCPIP_OPERATE_SUCC;
    if (atEngine != NULL)
    {
        at_CmdRespOK(atEngine);
    }
}

static void tcpip_SetDefaultConfig(void)
{
    tcpipConfig.transPktSize = 1024;
    tcpipConfig.transWaitTm = 2;
    tcpipConfig.sendDataFormat = 0;
    tcpipConfig.recvDataFormat = 0;  
    tcpipConfig.viewMode = 0;     
    tcpipConfig.backoffs = 8;
    tcpipConfig.rto = 600;
}

static void tcpip_SocketClearOne(uint8 socketID)
{
    KING_UnRegNotifyFun(DEV_CLASS_SOCKET, socketInfo[socketID].socketfd, NULL);
    memset(&(socketInfo[socketID]), 0, sizeof(TCPIP_SOCKET_INFO_S));

    socketInfo[socketID].socketID = 0xff;
    socketInfo[socketID].socketfd = TCPIP_SOCKET_ERROR;
    socketInfo[socketID].socketType = TCPIP_SOCK_TYPE_NUM;
    socketInfo[socketID].unsendLen = 0;
    socketInfo[socketID].sendDataLen = 0;  
    socketInfo[socketID].totalRecvSize = 0;
    socketInfo[socketID].readLen = 0;
    socketInfo[socketID].totalSendNum = 0;
    socketInfo[socketID].ackNum = 0;
    socketInfo[socketID].inputData = FALSE;
    socketInfo[socketID].ctxID = 0;
    socketInfo[socketID].running = FALSE;
    socketInfo[socketID].isLastTT = FALSE;
    socketInfo[socketID].serverID = 0xff;
    socketInfo[socketID].state = SOCKET_MODEM_INITIAL;
    socketInfo[socketID].nDLCI = 0xff;
    if(TRUE == AtRingBuf_IsInited(&(tcpipRxBuf[socketID])))
    {
        AtRingBuf_Uninit(&(tcpipRxBuf[socketID]));
    }
}

static void tcpip_QicsgpInit(void)
{
    uint8 i = 0;
    
    for (i = 0; i < TCPIP_CONTEXT_ID_MAX_NUM; i++)
    {
        memset(&qicsgpInfo[i], 0, sizeof(TCPIP_QICSGP_PDP_CTX_S));
        qicsgpInfo[i].cid = (i + 1);
        qicsgpInfo[i].type = 1;
        memset(qicsgpInfo[i].apn, 0, TCPIP_MAX_STRING_LENGTH);
        memset(qicsgpInfo[i].userName, 0, TCPIP_MAX_STRING_LENGTH);
        memset(qicsgpInfo[i].password, 0, TCPIP_MAX_STRING_LENGTH);
        qicsgpInfo[i].authType = 0;
        KING_TcpipNetApnGet((i + 1), qicsgpInfo[i].apn, qicsgpInfo[i].userName,
            qicsgpInfo[i].password, &(qicsgpInfo[i].authType), &(qicsgpInfo[i].type));
        
    }
}

static void tcpip_PdpInfoInit(void)
{
    uint8 i = 0;

    for (i = 0; i < TCPIP_CONTEXT_ID_MAX_NUM; i++)
    {
        memset(&tcpipPdpInfo[i], 0x00, sizeof(TCPIP_PDP_INFO_S));
        tcpipPdpInfo[i].ctxID = 0;
        tcpipPdpInfo[i].netState = TCPIP_STATE_INITIAL;
        tcpipPdpInfo[i].actStatus = TCPIP_PDP_DEACTIVATE;
        tcpipPdpInfo[i].pdpType = TCPIP_PDP_TYPE_NONE;
        memset(tcpipPdpInfo[i].ipAddr, 0x00, TCPIP_IP_ADDR_STRINT_LEN);
    }
}

static void tcpip_SocketInfoInit(void)
{
    uint8 i = 0;

    for(i = 0; i < TCPIP_SOCKET_ID_MAX_NUM; i++ )
    {
        tcpip_SocketClearOne(i);
    }
}

static void tcpip_IsLastTTInit(void)
{
    uint8 i = 0;

    for(i = 0; i < TCPIP_SOCKET_ID_MAX_NUM; i++ )
    {
        socketInfo[i].isLastTT = FALSE;
    }
}

static void tcpip_TransparentInit(uint8 socketID)
{
    char *buf = NULL;
    int ret = 0;
    
    if(FALSE == AtRingBuf_IsInited(&(tcpipTtTxBuf[socketID])))
    {
        buf = malloc(TCPIP_TX_BUF_MAX_SIZE);
        if(ret == 0 && buf != NULL)
        {
            AtRingBuf_Init(&(tcpipTtTxBuf[socketID]), buf, TCPIP_TX_BUF_MAX_SIZE);
        }
    }
}

static void tcpip_SocketMutexInit(void)
{
    uint8_t i = 0;
    int ret = -1;

    for(i = 0; i < TCPIP_SOCKET_ID_MAX_NUM; i++ )
    {
        ret = KING_MutexCreate("socket mutex", 0, &(pSocketMutex[i]));
        if (ret == -1 || pSocketMutex[i] == NULL)
        {
            KING_SysLog("pSocketMutex[%d] create fail!", i);
        }
    }
}

static void tcpip_SocketUnlock(uint8_t socketID)
{
    if (pSocketMutex[socketID] != NULL)
        KING_MutexUnLock(pSocketMutex[socketID]);
}

static void tcpip_SocketLock(uint8_t socketID)
{
    if (pSocketMutex[socketID] != NULL)
        KING_MutexLock(pSocketMutex[socketID], WAIT_OPT_INFINITE);
}

static uint8 tcpip_ActivedMax(void)
{
    uint8 i = 0, count = 0;

    for (i = 0; i < TCPIP_CONTEXT_ID_MAX_NUM; i++)
    {
        if(tcpipPdpInfo[i].netState == TCPIP_STATE_GPRSACT)
        {
            count++;
        }
    }
    return count;
}

static uint8 tcpip_GetSocketIDBySocketFd(uint8 socketFd)
{
    uint8 socketID = 0xff;
    int i = 0;

    for(i = 0; i < TCPIP_SOCKET_ID_MAX_NUM; i++ )
    {
        if (socketInfo[i].socketfd == socketFd)
        {
            socketID = i;
            break;
        }
    }
    
    return socketID;
}

static TCPIP_SOCKET_TYPE_E tcpip_QiopenServiceType(char *argval)
{
    TCPIP_SOCKET_TYPE_E type = TCPIP_SOCK_TYPE_NUM;

    if(strcasecmp(argval, "TCP") == 0)
    {
        type = TCPIP_SOCK_TYPE_TCP;
    }
    else if(strcasecmp(argval, "UDP") == 0)
    {
        type = TCPIP_SOCK_TYPE_UDP;
    }
    else if(strcasecmp(argval, "TCP LISTENER") == 0)
    {
        type = TCPIP_SOCK_TYPE_TCP_SERVER;
    } 
    else if(strcasecmp(argval, "UDP SERVICE") == 0)
    {
        type = TCPIP_SOCK_TYPE_UDP_SERVER;
    }
    else
    {
        type = TCPIP_SOCK_TYPE_NUM;
    }

    return type;
}

static bool tcpip_IsEnterDataMode(void)
{
    uint8 i = 0;

    for(i = 0; i < TCPIP_SOCKET_ID_MAX_NUM; i++ )
    {
        if (socketInfo[i].state == SOCKET_MODEM_SEND) 
        {
            return TRUE;
        }
    }

    return FALSE;
}

static uint8 tcpip_GetTcpipSocketID(void)
{
    uint8 socketID = 0xff;
    int i = 0;

    for(i = 0; i < TCPIP_SOCKET_ID_MAX_NUM; i++ )
    {
        if ((socketInfo[i].inputData == TRUE)
            && (socketInfo[i].state == SOCKET_MODEM_SEND))
        {
            socketID = i;
            break;
        }
    }

    return socketID;
}

static uint8 tcpip_getEnterDataModeSocketID(uint8 ctxID)
{
    uint8 i = 0;

    if(tcpipPdpInfo[ctxID - 1].netState == TCPIP_STATE_GPRSACT)
    {
        for(i = 0; i < TCPIP_SOCKET_ID_MAX_NUM; i++ )
        {
            if (socketInfo[i].state == SOCKET_MODEM_SEND
                && socketInfo[i].ctxID == ctxID) 
            {
                return i;
            }
        }
    }

    return 0xff;
}

static void tcpip_SendCancel(uint8 socketID)
{
    socketInfo[socketID].inputData = FALSE;
    socketInfo[socketID].sendDataLen = 0;
    socketInfo[socketID].unsendLen = 0;
    if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP_SERVER)
    {
        socketInfo[socketID].state = SOCKET_MODEM_LISTENING;
    }
    else
    {
        socketInfo[socketID].state = SOCKET_MODEM_CONNECTED;
    }
}

static void tcpip_socketExitInputMode(void)
{
    uint8 socketID = tcpip_GetTcpipSocketID();

    if (socketID == 0xff)
    {
        return;
    }
    tcpip_SendCancel(socketID);

    gTcpipCurErr = CME_TCPIP_SOCKET_WRITE_FAILED;
    if (socketInfo[socketID].atEngine == NULL)
    {
        return;
    }
    AT_CMD_RETURN(at_CmdRespOKText(socketInfo[socketID].atEngine, (uint8 *)"SEND FAIL"));
}

static void tcpip_exitDataMode(uint8 socketID)
{
    KING_SysLog("tcpip_exitDataMode socketID:%d", socketID);
    if (socketID == 0xff)
    {
        return;
    }
    if (socketInfo[socketID].accessMode == ACCESS_MODE_TRANSPARENT)
    {
        if (socketInfo[socketID].ttDataMode)
        {
            tcpip_ttExitDataMode(1);
        }
    }
    else
    {
        if (socketInfo[socketID].inputData)
        {
            tcpip_socketExitInputMode();
        }
    }
  
    return;
}

static void tcpip_ErrRsp(AT_CMD_ENGINE_T *atEngine, char *pCmdName, uint16 errCode)
{
    gTcpipCurErr = errCode;
    if (atEngine == NULL)
    {
        return;
    }
    memset(g_rspStr, 0x00, 1024);
    sprintf((char *)g_rspStr, "\r\n%s%d\r\n", pCmdName, errCode);
    at_CmdRespOutputText( atEngine, g_rspStr);
}

static void tcpip_OpenRsp(AT_CMD_ENGINE_T *atEngine, uint8 socketID, uint16 errCode)
{
    gTcpipCurErr = errCode;
    socketInfo[socketID].running = FALSE;
    if (atEngine == NULL)
    {
        return;
    }
    memset(g_rspStr, 0x00, 1024);
    sprintf((char *)g_rspStr, "+QIOPEN: %d,%d", socketID, errCode);
    at_CmdRespUrcText(atEngine, g_rspStr);
}

static void tcpip_OpenErrRsp(AT_CMD_ENGINE_T *atEngine, uint8 socketID, uint8 accessMode, uint16 errCode, int isReturnOk)
{
    isRunning = FALSE;
    gTcpipCurErr = errCode;
    socketInfo[socketID].running = FALSE;
    socketInfo[socketID].state = SOCKET_MODEM_INITIAL;
    if (atEngine == NULL)
    {
        return;
    }
    if (accessMode == ACCESS_MODE_TRANSPARENT)
    {
        at_CmdRespError(atEngine);
    }
    else
    {
        if (isReturnOk)
        {
            at_CmdRespOK(atEngine);
        }
        tcpip_OpenRsp(atEngine, socketID, errCode);
    }
}

static int tcpip_CloseSocketByContext(uint8 ctxID)
{
    int i = 0, ret = 0;

    for (i = 0; i < TCPIP_SOCKET_ID_MAX_NUM; i++)
    {
        if ((socketInfo[i].ctxID == ctxID) && 
            (socketInfo[i].socketfd != TCPIP_SOCKET_ERROR))
        {
            if (socketInfo[i].state == SOCKET_MODEM_OPENING)
            {
                return -1;
            }
            ret = KING_SocketClose(socketInfo[i].socketfd);
            if(ret < 0)
            {
                ret = -1;
                break;
            }
            else
            {
                tcpip_SocketClearOne(i);
            }
        }
    }

    return ret;
}

static int tcpip_getNicHandle(void)
{
    int ret = 0;
    NIC_INFO info;

    memset(&info, 0x00, sizeof(NIC_INFO));
    info.NIC_Type = NIC_TYPE_WWAN;
    ret = KING_HardwareNICRequest(info, &hNIC);
    if (ret != 0)
    {
        KING_SysLog("tcpip_getNicHandle ret=%d, hNIC=%d", ret, hNIC);
        hNIC = -1;
    }
    
    return ret;
}

static int tcpip_getContextHandle(uint8 cid)
{
    int ret = 0;
    NIC_CONTEXT ApnInfo;
    softwareHandle hContext = -1;

    if (hNIC == -1)
    {
        return -1;
    }
    memset(&ApnInfo, 0x00, sizeof(NIC_CONTEXT));
    strcpy((char *)ApnInfo.APN_Name, qicsgpInfo[cid - 1].apn);
    if (qicsgpInfo[cid - 1].type == TCPIP_PDP_TYPE_IPV6)
    {
        ApnInfo.IpType = IP_ADDR_TYPE_V6;
    }
    else
    {
        ApnInfo.IpType = IP_ADDR_TYPE_V4;
    }
    ret = KING_ConnectionAcquire(hNIC, ApnInfo, &hContext);
    KING_SysLog("tcpip_getContextHandle ret=%d, hContext=%d", ret, hContext);
    if (ret == -1 || hContext == -1)
    {
        return -1;
    }
    tcpipPdpInfo[cid - 1].hContext = hContext;
    return ret;
}

static int tcpip_getLocalIp(uint8 cid)
{
    int ret = 0;
    SOCK_IN_ADDR_T ipAddr;
    SOCK_IN_ADDR_T destAddr;
    char *ipAddrStr = NULL;

    if (tcpipPdpInfo[cid - 1].hContext == -1)
    {
        return -1;
    }
    
    memset(&ipAddr, 0x00, sizeof(SOCK_IN_ADDR_T));
    memset(&destAddr, 0x00, sizeof(SOCK_IN_ADDR_T));
    memset(tcpipPdpInfo[cid - 1].ipAddr, 0x00, TCPIP_IP_ADDR_STRINT_LEN);
    if (qicsgpInfo[cid - 1].type == TCPIP_PDP_TYPE_IPV6)
    {
        ret = KING_NetIpv6AddrInfoGet(tcpipPdpInfo[cid - 1].hContext, &ipAddr.u_addr.ip6, NULL, NULL);
        destAddr.type = IP_ADDR_TYPE_V6;
        destAddr.u_addr.ip6.addr[0] = ipAddr.u_addr.ip6.addr[0];
        destAddr.u_addr.ip6.addr[1] = ipAddr.u_addr.ip6.addr[1];
        destAddr.u_addr.ip6.addr[2] = ipAddr.u_addr.ip6.addr[2];
        destAddr.u_addr.ip6.addr[3] = ipAddr.u_addr.ip6.addr[3];
    }
    else
    {
        ret = KING_NetIpv4AddrInfoGet(tcpipPdpInfo[cid - 1].hContext, &ipAddr.u_addr.ip4, NULL, NULL);
        destAddr.type = IP_ADDR_TYPE_V4;
        destAddr.u_addr.ip4.addr = ipAddr.u_addr.ip4.addr;
    }
    KING_ipntoa(destAddr, &ipAddrStr);
    memset(tcpipPdpInfo[cid - 1].ipAddr, 0x00, TCPIP_IP_ADDR_STRINT_LEN);
    memcpy(tcpipPdpInfo[cid - 1].ipAddr, ipAddrStr, strlen(ipAddrStr));
    tcpipPdpInfo[cid - 1].pdpType = qicsgpInfo[cid - 1].type;
    return ret;
}

static int tcpip_SendData(uint8 socketID)
{
    int ret = 0;
    int send_len = 0;
    char *send_ptr;
    union sockaddr_aligned sock_addr;
    uint32 sockaddr_size = 0;
    SOCK_IN_ADDR_T ipAddr;

    KING_SysLog("tcpip_SendData socketID=%d\n", socketID);
    KING_SysLog("tcpip_SendData socketType=%d\n", socketInfo[socketID].socketType);
    if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_UDP_SERVER)
    {
        memset(&ipAddr, 0x00, sizeof(SOCK_IN_ADDR_T));
        ret = KING_ipaton(tcpipSendInfo.remoteAddr, &ipAddr);
        if (ret == -1)
        {
            return -1;        
        }

        if(IP_ADDR_TYPE_V6 == ipAddr.type)
        {
            struct sockaddr_in6 *sock_addr6 = (struct sockaddr_in6 *)&(sock_addr);

            sock_addr6->sin6_len = sizeof(struct sockaddr_in6);
            sock_addr6->sin6_family = AF_INET6;
            sock_addr6->sin6_port = KING_htons(tcpipSendInfo.remotePort);
            sock_addr6->sin6_addr.addr[0] = ipAddr.u_addr.ip6.addr[0];
            sock_addr6->sin6_addr.addr[1] = ipAddr.u_addr.ip6.addr[1];
            sock_addr6->sin6_addr.addr[2] = ipAddr.u_addr.ip6.addr[2];
            sock_addr6->sin6_addr.addr[3] = ipAddr.u_addr.ip6.addr[3];

            sockaddr_size = sizeof(struct sockaddr_in6);
        }
        else 
        {
            struct sockaddr_in *sock_addr4 = (struct sockaddr_in *)&(sock_addr);

            sock_addr4->sin_len = sizeof(struct sockaddr_in);
            sock_addr4->sin_family = AF_INET;
            sock_addr4->sin_port = KING_htons(tcpipSendInfo.remotePort);
            sock_addr4->sin_addr.addr = ipAddr.u_addr.ip4.addr;
            KING_SysLog("[KING]KH_SocketCreate ip_addr=%u\r\n", sock_addr4->sin_addr.addr);

            sockaddr_size = sizeof(SOCK_ADDR_IN_T);
        }
    }
    else if(socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_UDP)
    {
        memset(&ipAddr, 0x00, sizeof(SOCK_IN_ADDR_T));
        ret = KING_ipaton(socketInfo[socketID].ipAddrStr, &ipAddr);
        if (ret == -1)
        {
            return -1;        
        }
        if(IP_ADDR_TYPE_V6 == ipAddr.type)
        {
            struct sockaddr_in6 *sock_addr6 = (struct sockaddr_in6 *)&(sock_addr);

            sock_addr6->sin6_len = sizeof(struct sockaddr_in6);
            sock_addr6->sin6_family = AF_INET6;
            sock_addr6->sin6_port = KING_htons(socketInfo[socketID].remotePort);
            sock_addr6->sin6_addr.addr[0] = ipAddr.u_addr.ip6.addr[0];
            sock_addr6->sin6_addr.addr[1] = ipAddr.u_addr.ip6.addr[1];
            sock_addr6->sin6_addr.addr[2] = ipAddr.u_addr.ip6.addr[2];
            sock_addr6->sin6_addr.addr[3] = ipAddr.u_addr.ip6.addr[3];

            sockaddr_size = sizeof(struct sockaddr_in6);
        }
        else 
        {
            struct sockaddr_in *sock_addr4 = (struct sockaddr_in *)&(sock_addr);

            sock_addr4->sin_len = sizeof(struct sockaddr_in);
            sock_addr4->sin_family = AF_INET;
            sock_addr4->sin_port = KING_htons(socketInfo[socketID].remotePort);
            sock_addr4->sin_addr.addr = ipAddr.u_addr.ip4.addr;
            KING_SysLog("[KING]KH_SocketCreate ip_addr=%u\r\n", sock_addr4->sin_addr.addr);

            sockaddr_size = sizeof(SOCK_ADDR_IN_T);
        }
    }

    KING_SysLog("tcpip_SendData sendDataLen=%d\n", socketInfo[socketID].sendDataLen);
    KING_SysLog("tcpip_SendData unsendLen=%d\n", socketInfo[socketID].unsendLen);
    while (socketInfo[socketID].unsendLen > 0)
    {
        send_ptr = socketInfo[socketID].sendBuf + 
                    (socketInfo[socketID].sendDataLen - 
                    socketInfo[socketID].unsendLen);
        if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_UDP_SERVER
            || socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_UDP)
        {
            send_len = KING_SocketSendto(socketInfo[socketID].socketfd, 
                                        (void *)send_ptr, 
                                        (UINT16)socketInfo[socketID].unsendLen,
                                        0, 
                                        &sock_addr,
                                        sockaddr_size);
        }
        else
        {
            send_len = KING_SocketSend(socketInfo[socketID].socketfd,
                                        (void *)send_ptr,
                                        (UINT16)socketInfo[socketID].unsendLen,
                                        0); 
        }
        KING_SysLog("tcpip_SendData: socketID = %d, send_len = %d, unsendLen = %d",
                    socketID, send_len, socketInfo[socketID].unsendLen);
        if (send_len > 0)
        {
            if ((socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP) ||
                (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP_SERVER))
            {
                socketInfo[socketID].totalSendNum += send_len;
            }
            else
            {
                socketInfo[socketID].totalSendNum += send_len;
                socketInfo[socketID].ackNum = socketInfo[socketID].totalSendNum;
            }

            socketInfo[socketID].unsendLen -= send_len;
            if (socketInfo[socketID].unsendLen == 0)
            {
                ret = 0;
                break;
            }
        }
        else if (send_len <= 0)
        {
            ret = -1;
            break;
        }
    }    

    return ret;
}

static TCPIP_BYPASS_BUFFER_T* tcpip_ttBufCreate(void)
{
    TCPIP_BYPASS_BUFFER_T *bypass_buff;
    int ret = -1;

    bypass_buff = (TCPIP_BYPASS_BUFFER_T *)malloc(sizeof(TCPIP_BYPASS_BUFFER_T));
    if (NULL == bypass_buff)
    {
        return NULL;
    }
    memset(bypass_buff, 0x00, sizeof(TCPIP_BYPASS_BUFFER_T));

    bypass_buff->buff = (uint8_t *)malloc(AT_CMD_LINE_BUFF_LEN);
    if (NULL == bypass_buff->buff)
    {
        free(bypass_buff);
        bypass_buff = NULL;
        return NULL;
    }
    bypass_buff->buffLen = 0;
    bypass_buff->buffSize = AT_CMD_LINE_BUFF_LEN;
    ret = KING_MutexCreate("TT_MUTEX", 0, &(bypass_buff->mutex));
    if (ret == -1 || bypass_buff->mutex == NULL)
    {
        KING_SysLog("tcpip_TTBufCreate create mutex fail!");
    }
    
    return bypass_buff;
}

static void tcpip_ttBufDestroy(TCPIP_BYPASS_BUFFER_T *bypass_buff)
{
    if (bypass_buff == NULL)
    {
        return;
    }

    KING_MutexDelete(bypass_buff->mutex);
    bypass_buff->mutex = NULL;
    if (bypass_buff->buff != NULL)
    {
        free(bypass_buff->buff);
        bypass_buff->buff = NULL;
    }
    if (bypass_buff != NULL)
    {
        free(bypass_buff);
        bypass_buff = NULL;
    }
}

static uint8 tcpip_ttGetLastIdx(void)
{
    uint8 socketID = 0xff;
    int i = 0;

    for(i = 0; i < TCPIP_SOCKET_ID_MAX_NUM; i++ )
    {
        if (socketInfo[i].isLastTT)
        {
            socketID = i;
            break;
        }
    }

    return socketID;
}

static uint8 tcpip_ttGetModeSocketID(void)
{
    uint8 socketID = 0xff;
    int i = 0;

    for(i = 0; i < TCPIP_SOCKET_ID_MAX_NUM; i++ )
    {
        if ((socketInfo[i].accessMode == ACCESS_MODE_TRANSPARENT)
            && (socketInfo[i].ttDataMode == TRUE)
            && (socketInfo[i].state == SOCKET_MODEM_SEND))
        {
            socketID = i;
            break;
        }
    }

    return socketID;
}

static void tcpip_ttStop(int isNoCarrier, uint8 socketID)
{
    AT_DISPATCH_T *dispatch = NULL;
    
    KING_SysLog("%s: socketID=%d", __FUNCTION__, socketID);
    if (socketID == 0xff)
    {
        return;
    }
    
    dispatch = at_DispatchGetByChannel(socketInfo[socketID].nDLCI);
    if (NULL != at_DispatchGetDataEngine(dispatch))
    {
        at_ModuleModeSwitch(AT_MODE_SWITCH_DATA_END, socketInfo[socketID].nDLCI);
    }
    
    tcpip_ttTimer(FALSE);
    socketInfo[socketID].state = SOCKET_MODEM_CONNECTED;
    socketInfo[socketID].ttDataMode = FALSE;
    tcpip_ttBufDestroy(gTTBypassBuf);
    gTTBypassBuf = NULL;
    if (socketInfo[socketID].atEngine == NULL)
    {
        return;
    }
    if (isNoCarrier == 1)
    {
        gTcpipCurErr = CME_TCPIP_UNKNOWN_ERROR;
        AT_CMD_RETURN(at_CmdRespOKText(socketInfo[socketID].atEngine, (uint8 *)"NO CARRIER"));
    }
    else if (isNoCarrier == 2)
    {
        tcpip_atRspErr(socketInfo[socketID].atEngine, CME_TCPIP_SOCKET_WRITE_FAILED);
    }
    else
    {
        tcpip_atRspOk(socketInfo[socketID].atEngine);
    }
}

static void tcpip_ttExitDataMode(int isNoCarrier)
{
    uint8 socketID = tcpip_ttGetModeSocketID();
    
    KING_SysLog("%s: socketID=%d", __FUNCTION__, socketID);

    tcpip_ttStop(isNoCarrier, socketID);
}

static void tcpip_ttBypassDataSend(uint32 tmrId)
{
    uint8 socketID = tcpip_ttGetModeSocketID();
    AT_DISPATCH_T *dispatch = NULL;
    uint8 *pData = NULL;
    uint16 sendLen = 0;
    union sockaddr_aligned sock_addr;
    uint32 sockaddr_size = 0;
    SOCK_IN_ADDR_T ipAddr;
    int ret = -1;
    
    if (socketID == 0xff)
    {
        return;
    }
    
    if (gTTBypassBuf == NULL)
    {
        KING_SysLog("tcpip_ttBypassDataSend error,gTTBypassBuf == null");
        goto RESTART;
    }

    if(socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_UDP)
    {
        memset(&ipAddr, 0x00, sizeof(SOCK_IN_ADDR_T));
        ret = KING_ipaton(socketInfo[socketID].ipAddrStr, &ipAddr);
        if (ret == -1)
        {
            goto RESTART;        
        }
        if(IP_ADDR_TYPE_V6 == ipAddr.type)
        {
            struct sockaddr_in6 *sock_addr6 = (struct sockaddr_in6 *)&(sock_addr);

            sock_addr6->sin6_len = sizeof(struct sockaddr_in6);
            sock_addr6->sin6_family = AF_INET6;
            sock_addr6->sin6_port = KING_htons(socketInfo[socketID].remotePort);
            sock_addr6->sin6_addr.addr[0] = ipAddr.u_addr.ip6.addr[0];
            sock_addr6->sin6_addr.addr[1] = ipAddr.u_addr.ip6.addr[1];
            sock_addr6->sin6_addr.addr[2] = ipAddr.u_addr.ip6.addr[2];
            sock_addr6->sin6_addr.addr[3] = ipAddr.u_addr.ip6.addr[3];

            sockaddr_size = sizeof(struct sockaddr_in6);
        }
        else 
        {
            struct sockaddr_in *sock_addr4 = (struct sockaddr_in *)&(sock_addr);

            sock_addr4->sin_len = sizeof(struct sockaddr_in);
            sock_addr4->sin_family = AF_INET;
            sock_addr4->sin_port = KING_htons(socketInfo[socketID].remotePort);
            sock_addr4->sin_addr.addr = ipAddr.u_addr.ip4.addr;

            sockaddr_size = sizeof(SOCK_ADDR_IN_T);
        }
    }
    
    pData = malloc(gTTBypassBuf->buffLen);
    if (pData == NULL)
    {
        KING_SysLog("tcpip_ttBypassDataSend MALLOC FAILS");
        goto RESTART;
    }
    
    memset(pData, 0, gTTBypassBuf->buffLen);
    memcpy(pData, gTTBypassBuf->buff, gTTBypassBuf->buffLen);
    if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_UDP)
    {
        sendLen = KING_SocketSendto(socketInfo[socketID].socketfd, 
                                    (void *)pData, 
                                    (uint16)gTTBypassBuf->buffLen,
                                    0, 
                                    &sock_addr,
                                    sockaddr_size);
    }
    else
    {
        sendLen = KING_SocketSend(socketInfo[socketID].socketfd, (void *)pData, (uint16)gTTBypassBuf->buffLen, 0);
    }
    if (sendLen == -1)
    {
        KING_SysLog("TCPIP send socket data error");
    }
    else
    {
        if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP)
        {
            socketInfo[socketID].totalSendNum += sendLen;
        }
        else
        {
            socketInfo[socketID].totalSendNum += sendLen;
            socketInfo[socketID].ackNum = socketInfo[socketID].totalSendNum;
        }
        KING_MutexLock(gTTBypassBuf->mutex, WAIT_OPT_INFINITE);
        gTTBypassBuf->buffLen = 0;
        KING_MutexUnLock(gTTBypassBuf->mutex);
    }
    free(pData);
    pData = NULL;

RESTART:
    dispatch = at_DispatchGetByChannel(socketInfo[socketID].nDLCI);
    if (at_DispatchIsDataMode(dispatch))
    {
        tcpip_ttTimer(TRUE);
    }
}

static void tcpip_ttSendData(uint8 nDLCI, const uint8_t *data, unsigned length)
{
    const uint8 *p = data;
    
    KING_SysLog("%s: buffLen=%d, length=%d", __FUNCTION__,
                      gTTBypassBuf->buffLen, length);
    if (gTTBypassBuf->buffLen + length > gTTBypassBuf->buffSize) // overflow
    {
        KING_SysLog("tcpip_ttSendDataFromUart overflow,buffLen=%d, length=%d",
                          gTTBypassBuf->buffLen, length);
        length = (gTTBypassBuf->buffSize - gTTBypassBuf->buffLen);
    }
    memcpy(gTTBypassBuf->buff + gTTBypassBuf->buffLen, p, length);
    gTTBypassBuf->buffLen += length;
    KING_SysLog("%s: 11 buffLen=%d", __FUNCTION__, gTTBypassBuf->buffLen);
    if (gTTBypassBuf->buffLen >= tcpipConfig.transPktSize)
    {
        tcpip_ttTimer(FALSE);
        tcpip_ttBypassDataSend(0);
    }
}

static int tcpip_ttBypassDataRecv(void *param, const uint8_t *data, unsigned length)
{
    uint32 nDLCI = (uint32)param;

    KING_SysLog("tcpip_ttBypassDataRecv,length=%d, nDLCI=%u", length, nDLCI);
    KING_MutexLock(gTTBypassBuf->mutex, WAIT_OPT_INFINITE);
    tcpip_ttSendData((uint8)nDLCI, (uint8 *)data, (uint32)length);
    KING_MutexUnLock(gTTBypassBuf->mutex);

    return length;
}

static void tcpip_ttTimer(bool isSet)
{
    int ret = 0;
    
    if (isSet) 
    {
        TIMER_ATTR_S timerattr;
        
        if (ttTimer == NULL)
        {
            ret = KING_TimerCreate(&ttTimer);
            if(-1 == ret)
            {
                KING_SysLog("%s: Failed!", __FUNCTION__);
                return;
            }
        }
        memset(&timerattr, 0x00, sizeof(timerattr));
        timerattr.isPeriod = FALSE;
        timerattr.timeout = tcpipConfig.transWaitTm * 100;
        timerattr.timer_fun = tcpip_ttBypassDataSend;
        ret = KING_TimerActive(ttTimer, &timerattr);
        if(-1 == ret)
        {
            KING_SysLog("%s: Active Failed!", __FUNCTION__);
            return;
        }
    }
    else
    {
        if (ttTimer != NULL)
        {
            KING_TimerDeactive(ttTimer);
        }
    }
}

static void tcpip_ttStart(uint8 socketID)
{
    AT_DISPATCH_T *dispatch = at_DispatchGetByChannel(socketInfo[socketID].nDLCI);
    uint32 nDLCI = socketInfo[socketID].nDLCI;
    
    if (gTTBypassBuf == NULL)
    {
        gTTBypassBuf = tcpip_ttBufCreate();
    }
    if (NULL == gTTBypassBuf)
    {
        tcpip_atRspErr(socketInfo[socketID].atEngine, CME_TCPIP_UNKNOWN_ERROR);
        return;
    }
    at_DispatchSetPlusLeadingTick(TCPIP_HAL_TICK1S);
    at_DispatchSetPlusDurationTick(TCPIP_HAL_TICK900MS);
    at_DispatchSetPlusTrailingTick(TCPIP_HAL_TICK900MS);
    at_ModuleModeSwitch(AT_MODE_SWITCH_DATA_START, socketInfo[socketID].nDLCI);
    socketInfo[socketID].state = SOCKET_MODEM_SEND;
    socketInfo[socketID].ttDataMode = TRUE;
    socketInfo[socketID].isLastTT = TRUE;
    tcpip_ttTimer(TRUE);
    at_DataSetBypassMode(at_DispatchGetDataEngine(dispatch), tcpip_ttBypassDataRecv, (void *)nDLCI);
}

static void tcpip_socketConnected(uint8 socketID)
{
    if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP_SERVER
        || socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_UDP_SERVER)
    {
        socketInfo[socketID].state = SOCKET_MODEM_LISTENING;
    }
    else
    {
        socketInfo[socketID].state = SOCKET_MODEM_CONNECTED;
    }
    isRunning = FALSE;
    socketInfo[socketID].running = FALSE;
    socketInfo[socketID].totalSendNum = 0;
    socketInfo[socketID].ackNum = 0;

    if(socketInfo[socketID].accessMode == ACCESS_MODE_TRANSPARENT)
    {
        tcpip_ttStart(socketID);
    }
    else
    {
        socketInfo[socketID].isLastTT = FALSE;
        socketInfo[socketID].ttDataMode = FALSE;
        socketInfo[socketID].running = FALSE;
        tcpip_OpenRsp(socketInfo[socketID].atEngine, socketID, TCPIP_OPERATE_SUCC);
    }
}

static void tcpip_ReadData(uint16 actual_size, uint16 socketID, char *buff, bool isUrc)
{
    uint16 read_leng = 0, temp_len = 0, data_len = 0;
    bool isbreak = FALSE;
    char *actual_buff = NULL;

    actual_buff = (char*)malloc(actual_size * 2 + 1);
    if (actual_buff == NULL)
    {
        return;
    }
    memset(actual_buff, 0x00, actual_size * 2 + 1);
    if(tcpipConfig.recvDataFormat == 1)
    {
        atUtils_ConvertBinToHex((UINT8 *)buff, 
                            (UINT16)actual_size, 
                            (UINT8 *)actual_buff,
                            0);
        read_leng = actual_size * 2;
        socketInfo[socketID].readLen += actual_size;
    }
    else
    {
        if (strlen(buff) == actual_size<<1)
        {
            atUtils_ConvertHexToBin((char *)buff, 
                                (UINT16)(actual_size<<1), 
                                (char *)actual_buff);
        }
        else
        {
            memcpy((char*)actual_buff, buff, actual_size);
        }
        read_leng = actual_size;
        socketInfo[socketID].readLen += actual_size;
    }

    while(temp_len < read_leng)
    {
        memset(g_rspStr, 0x00, sizeof((char *)g_rspStr));
        if(read_leng - temp_len > TCPIP_RSP_MAX_LEN)
        {
            data_len = TCPIP_RSP_MAX_LEN;
            memcpy(g_rspStr, (actual_buff + temp_len), data_len);
            temp_len += TCPIP_RSP_MAX_LEN;
            isbreak = FALSE;
        }
        else
        {
            data_len = read_leng - temp_len;
            memcpy(g_rspStr, (actual_buff + temp_len), data_len);
            isbreak = TRUE;
        }
        if (socketInfo[socketID].atEngine != NULL)
        {
            at_CmdRespOutputNText(socketInfo[socketID].atEngine, g_rspStr, data_len);
        }
        if (isbreak)
        {
            break;
        }
    }
    if (isUrc && (socketInfo[socketID].atEngine != NULL))
    {
        at_CmdRespOutputText(socketInfo[socketID].atEngine, (uint8 *)"\r\n");
    }
    if (actual_buff != NULL)
    {
        free(actual_buff);
        actual_buff = NULL;
    }
}

static int tcpip_SocketRecvDataBufferMode(int socketfd, uint8 socketID, uint16 dataLen)
{
    char *read_buffer = NULL;
    uint16 read_cnt = 0;
    int num_bytes_read = 0, to_ringbuf_size = 0;
    int ret = -1;
    char *rev_buf = NULL;
    bool need_urc = TRUE;
  
    KING_SysLog("tcpip_SocketRecvDataBufferMode socketfd=%d\n", socketfd);
    KING_SysLog("tcpip_SocketRecvDataBufferMode socketID=%d\n", socketID);
    if (socketfd == TCPIP_SOCKET_ERROR)
    {
        KING_SysLog("tcpip_SocketRecvDataBufferMode Invalid sock fd!\n");
        return -1;
    }
  
    if (FALSE == AtRingBuf_IsInited(&(tcpipRxBuf[socketID])))
    {
        rev_buf = (char*)malloc(TCPIP_RX_BUF_MAX_SIZE);
        if (rev_buf != NULL)
        {
            AtRingBuf_Init(&(tcpipRxBuf[socketID]), rev_buf, TCPIP_RX_BUF_MAX_SIZE);
        }
        else
        {
            KING_SysLog("tcpip_SocketRecvDataBufferMode Can't alloc memory for buffer read!!!\n");
            return 1;
        }
    }

    if (AtRingBuf_DataSize(&tcpipRxBuf[socketID]) > 0)
    {
        need_urc = FALSE;
    }

    {
        int buf_space = AtRingBuf_Space(&tcpipRxBuf[socketID]);

        if (buf_space <= 0)
        {
            KING_SysLog("tcpip_SocketRecvDataBufferMode sock[%d] rx ring buffer is full!!!\n", socketID);
            ret = 1;
            goto EXIT;
        }

        read_cnt = (buf_space < dataLen) ? buf_space : dataLen;
        read_buffer = (char*)malloc(read_cnt + 1);
        if (read_buffer == NULL)
        {
            ret = 1;
            goto EXIT;
        }
        memset(read_buffer, 0x00, read_cnt + 1);
        if ((socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP) ||
            (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP_SERVER) ||
            (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP_INCOMING))
        {
            num_bytes_read = KING_SocketRecv(socketfd, read_buffer, read_cnt, 0);
            KING_SysLog("Tcpip KING_SocketRecv returned %d bytes", num_bytes_read);
        }
        else if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_UDP)
        {
            num_bytes_read = KING_SocketRecvfrom(socketfd, read_buffer, 
                                                read_cnt, 0, NULL, NULL);
            KING_SysLog("Tcpip KING_SocketRecvfrom returned %d bytes", num_bytes_read);
        }

        if (num_bytes_read > 0)
        {
            to_ringbuf_size += AtRingBuf_In(&(tcpipRxBuf[socketID]), read_buffer, num_bytes_read); 
            ret = 1;
        }
        else if (num_bytes_read == 0)
        {
            //to do close socket
            ret = 0;
            goto EXIT;
        }
        else if (num_bytes_read < 0)
        {
            ret = -1;
            goto EXIT;
        }
    }

EXIT:
    socketInfo[socketID].totalRecvSize += to_ringbuf_size;
    KING_SysLog("Tcpip recvfrom returned %d bytes, need_urc %d",
                to_ringbuf_size, need_urc);
    if ((to_ringbuf_size > 0) && (TRUE == need_urc))
    {
        char rspString[30] = {0};

        memset(rspString, 0x00, 30);
        sprintf(rspString, "\r\n+QIURC: \"recv\",%d\r\n", socketID);
        if (socketInfo[socketID].atEngine != NULL)
        {
            at_CmdRespOutputText(socketInfo[socketID].atEngine, (uint8 *)rspString);
        }
    }
    if (read_buffer != NULL)
    {
        free(read_buffer);
        read_buffer = NULL;
    }

    return ret;
}

static int tcpip_SocketRecvDataDirectMode(int socketfd, uint8 socketID, uint16 dataLen)
{
    char *read_buffer = NULL;
    uint16 read_cnt = TCPIP_RX_BUF_MAX_LEN;
    uint16 recv_cnt = 0, readSize = 0;
    int num_bytes_read = 0, ret = 1;

    if (socketfd == TCPIP_SOCKET_ERROR)
    {
        return -1;
    }
    read_buffer = (char*)malloc(read_cnt + 1);
    if (read_buffer == NULL)
    {
        return 1;
    }
    while (recv_cnt < dataLen)
    {
        memset(read_buffer, 0x00, read_cnt + 1);
        if(dataLen - recv_cnt > TCPIP_RX_BUF_MAX_LEN)
        {
            readSize = TCPIP_RX_BUF_MAX_LEN;
        }
        else
        {
            readSize = dataLen - recv_cnt;
        }
        KING_SysLog("Tcpip_SocketRecvDataDirectMode readSize %d bytes", readSize);
        if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_UDP)
        {
            num_bytes_read = KING_SocketRecvfrom(socketfd, read_buffer, readSize, 0,
                                                    NULL, NULL);
        }
        else
        {
            num_bytes_read = KING_SocketRecv(socketfd, read_buffer, readSize, 0);
        }
        KING_SysLog("Tcpip_SocketRecvDataDirectMode recv returned %d bytes", num_bytes_read);
        if (num_bytes_read < 0)
        {
            ret = -1;
            goto EXIT;
        }
        else if(num_bytes_read == 0)
        {
            //todo close socket
            ret = 0;
            goto EXIT;
        }
        recv_cnt += num_bytes_read;
        KING_SysLog("Tcpip_SocketRecvDataDirectMode recv_cnt %d bytes", recv_cnt);
        socketInfo[socketID].totalRecvSize += num_bytes_read;
        memset(g_rspStr, 0x00, sizeof((char *)g_rspStr));
        if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_UDP_SERVER)
        {
            if (tcpipConfig.viewMode == 0)
            {
                sprintf ((char *)g_rspStr, "\r\n+QIURC: \"recv\",%d,%d,\"%s\",%d\r\n",
                        socketID, num_bytes_read,
                        socketInfo[socketID].ipAddrStr,
                        socketInfo[socketID].remotePort);
            }
            else
            {
                sprintf ((char *)g_rspStr, "\r\n+QIURC: \"recv\",%d,%d,\"%s\",%d,",
                        socketID, num_bytes_read,
                        socketInfo[socketID].ipAddrStr,
                        socketInfo[socketID].remotePort);
            }
        }
        else
        {
            if (tcpipConfig.viewMode == 0)
            {
                sprintf ((char *)g_rspStr, "\r\n+QIURC: \"recv\",%d,%d\r\n",
                        socketID, num_bytes_read);
            }
            else
            {
                sprintf ((char *)g_rspStr, "\r\n+QIURC: \"recv\",%d,%d,",
                        socketID, num_bytes_read);
            }
        }
        if (socketInfo[socketID].atEngine != NULL)
        {
            at_CmdRespOutputText(socketInfo[socketID].atEngine, g_rspStr);
        }

        //todo display data
        tcpip_ReadData((uint16)num_bytes_read, socketID, read_buffer, TRUE);
    }
    
EXIT:
    if (read_buffer != NULL)
    {
        free(read_buffer);
        read_buffer = NULL;
    } 
    return ret;
}

static int tcpip_SocketRecvDataTTMode(int socketfd, uint8 socketID, uint16 dataLen)
{
    char *read_buffer = NULL;
    char *rev_buf = NULL;
    uint16 read_cnt = 0;
    int num_bytes_read = 0, ret = -1;
    int buf_space = 0;

    if (socketfd == TCPIP_SOCKET_ERROR)
    {
        return -1;
    }
    
    KING_SysLog("%s: socketfd=%d", __FUNCTION__, socketfd);
    KING_SysLog("%s: socketID=%d", __FUNCTION__, socketID);
    if (!socketInfo[socketID].ttDataMode)
    {
        if (FALSE == AtRingBuf_IsInited(&(tcpipRxBuf[socketID])))
        {
            rev_buf = malloc(TCPIP_RX_BUF_MAX_SIZE);
            if (rev_buf != NULL)
            {
                AtRingBuf_Init(&(tcpipRxBuf[socketID]), rev_buf, TCPIP_RX_BUF_MAX_SIZE);
            }
            else
            {
                KING_SysLog("%s: Can't alloc memory for buffer read!!!\n", __FUNCTION__);
                return 1;
            }
        }
        
        buf_space = AtRingBuf_Space(&tcpipRxBuf[socketID]);
        if (buf_space <= 0)
        {
            KING_SysLog("%s: sock[%d] rx ring buffer is full!!!\n", __FUNCTION__, socketID);
            return 1;
        }

        read_cnt = (buf_space < dataLen) ? buf_space : dataLen;
    }
    else
    {
        read_cnt = dataLen;
    }
    read_buffer = (char*)malloc(read_cnt + 1);
    if (read_buffer == NULL)
    {
        ret = 1;
        goto EXIT;
    }
    memset(read_buffer, 0x00, read_cnt + 1);
    if ((socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP) ||
        (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP_SERVER) ||
        (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP_INCOMING))
    {
        num_bytes_read = KING_SocketRecv(socketfd, read_buffer, read_cnt, 0);
    }
    else if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_UDP)
    {
        num_bytes_read = KING_SocketRecvfrom(socketfd, read_buffer, 
                                            read_cnt, 0, NULL, NULL);
    }
    KING_SysLog("%s: recv returned %d bytes", __FUNCTION__, num_bytes_read);
    if (num_bytes_read > 0)
    {
        socketInfo[socketID].totalRecvSize += num_bytes_read;
        if (!socketInfo[socketID].ttDataMode)
        {
            AtRingBuf_In(&(tcpipRxBuf[socketID]), read_buffer, num_bytes_read); 
        }
        else
        {
            int temp_len = 0, data_len = 0;
            BOOL isbreak = FALSE;
            AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(socketInfo[socketID].nDLCI);
            AT_DISPATCH_T *dispatch = at_DispatchGetByChannel(at_CmdGetChannel(engine));
            AT_DATA_ENGINE_T *dataEngine = at_DispatchGetDataEngine(dispatch);

            if (dataEngine == NULL)
            {
                ret = -1;
                goto EXIT;
            }
            socketInfo[socketID].readLen += num_bytes_read;
            while(temp_len < num_bytes_read)
            {
                memset(g_rspStr, 0x00, sizeof((char *)g_rspStr));
                if(num_bytes_read - temp_len > TCPIP_RSP_MAX_LEN)
                {
                    data_len = TCPIP_RSP_MAX_LEN;
                    memcpy(g_rspStr, read_buffer + temp_len,
                            data_len);
                    temp_len += TCPIP_RSP_MAX_LEN;
                    isbreak = FALSE;
                }
                else
                {
                    data_len = num_bytes_read - temp_len;
                    memcpy(g_rspStr, (read_buffer + temp_len), data_len);
                    isbreak = TRUE;
                }
                at_DataWrite(dataEngine, g_rspStr, data_len);
                if (isbreak)
                {
                    break;
                }
            }
        }
        ret = 1;            
    }
    else if (num_bytes_read == 0)
    {
        ret = 0;
    }
    else if (num_bytes_read < 0)
    {
        ret = -1;
    }

EXIT: 
    if (read_buffer != NULL)
    {
        free(read_buffer);
        read_buffer = NULL;
    }    
    return ret;
}

static void tcpip_socketRecvData(uint8 socketID, uint16 dataLen)
{
    KING_SysLog("tcpip_socketRecvData dataLen %d bytes", dataLen);
    if (dataLen == 0)
    {
        return;
    }
    tcpip_SocketLock(socketID);
    if (socketInfo[socketID].accessMode == ACCESS_MODE_BUFFER)
    {
        tcpip_SocketRecvDataBufferMode(socketInfo[socketID].socketfd, socketID, dataLen);
    }
    else if (socketInfo[socketID].accessMode == ACCESS_MODE_DIRECT)
    {
        tcpip_SocketRecvDataDirectMode(socketInfo[socketID].socketfd, socketID, dataLen);
    }
    else if (socketInfo[socketID].accessMode == ACCESS_MODE_TRANSPARENT)
    {
        tcpip_SocketRecvDataTTMode(socketInfo[socketID].socketfd, socketID, dataLen);
    }
    tcpip_SocketUnlock(socketID);
}

static void tcpip_MsgSend(TCPIP_MSG_ID_TYPE_E msgId, uint8 ctxID, uint8 socketID, TCPIP_CMD_E atCmd, uint16 len)
{
    int ret = 0;
    MSG_S msg;
    TCPIP_SEND_MSG_S *pSendMsg = NULL;
    
    KING_SysLog("tcpip_MsgSend ctxID=%d, socketID=%d!", ctxID, socketID);
    if (tcpipMsgHandle == NULL)
    {
        KING_SysLog("tcpip_MsgSend tcpipMsgHandle is NULL!");
        return;
    }
    
    pSendMsg = malloc(sizeof(TCPIP_SEND_MSG_S));
    if (pSendMsg == NULL)
    {
        KING_SysLog("tcpip_MsgSend pSendMsg is NULL!");
        return;
    }
    pSendMsg->socketID = socketID;
    pSendMsg->ctxID = ctxID;
    pSendMsg->atCmd = atCmd;
    pSendMsg->len = len;
    
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = msgId;
    msg.pData = (void *)pSendMsg;
    msg.DataLen = sizeof(TCPIP_SEND_MSG_S);

    ret = KING_MsgSend(&msg, tcpipMsgHandle);
    if(-1 == ret)
    {
        KING_SysLog("tcpip_MsgSend KING_MsgSend() Failed!");
        return;
    }
}

static void tcpip_closeUrcRsp(uint8 socketID, bool isTtMode)
{
    KING_SysLog("tcpip_closeUrcRsp state:%d", socketInfo[socketID].state);
    KING_SysLog("tcpip_closeUrcRsp accessMode:%d", socketInfo[socketID].accessMode);
    if (socketInfo[socketID].state == SOCKET_MODEM_OPENING)
    {
        tcpip_connectTimer(FALSE);
        KING_SocketClose(socketInfo[socketID].socketfd);
        socketInfo[socketID].socketfd = TCPIP_SOCKET_ERROR;
        socketInfo[socketID].state = SOCKET_MODEM_INITIAL;
        tcpip_OpenErrRsp(socketInfo[socketID].atEngine, socketID, socketInfo[socketID].accessMode, CME_TCPIP_SOCKET_CONNECT_FAILED, 0);
        return;
    }
    if (socketInfo[socketID].state == SOCKET_MODEM_CONNECTED)
    {
        if (isTtMode)
        {
            socketInfo[socketID].state = SOCKET_MODEM_CLOSING;
        }
        else
        {
            char buf[30] = {0};
            
            KING_SysLog("tcpip_closeUrcRsp socketID:%d", socketID);
            memset(buf, 0, 30);
            sprintf(buf, "\r\n+QIURC: \"closed\",%d\r\n", socketID);
            if (socketInfo[socketID].atEngine != NULL)
            {
                at_CmdRespOutputText(socketInfo[socketID].atEngine, (uint8 *)buf);
            }
            socketInfo[socketID].state = SOCKET_MODEM_CLOSING;
        }
    }
}

static void tcpip_connectTimeOut(uint32 tmrId)
{
    tcpip_connectTimer(FALSE);
    KING_SocketClose(socketInfo[gCurSocketID].socketfd);
    socketInfo[gCurSocketID].socketfd = TCPIP_SOCKET_ERROR;
    socketInfo[gCurSocketID].state = SOCKET_MODEM_INITIAL;
    tcpip_OpenErrRsp(socketInfo[gCurSocketID].atEngine, gCurSocketID, socketInfo[gCurSocketID].accessMode, CME_TCPIP_OPERATION_TIMEOUT, 0);
}

static void tcpip_connectTimer(bool isSet)
{
    int ret = 0;
    
    if (isSet) 
    {
        TIMER_ATTR_S timerattr;
        
        if (connectTimer == NULL)
        {
            ret = KING_TimerCreate(&connectTimer);
            if(-1 == ret)
            {
                KING_SysLog("%s: Failed!", __FUNCTION__);
                return;
            }
        }
        memset(&timerattr, 0x00, sizeof(timerattr));
        timerattr.isPeriod = FALSE;
        timerattr.timeout = TCPIP_CONNECT_MAX_TIMEOUT * 1000;
        timerattr.timer_fun = tcpip_connectTimeOut;
        ret = KING_TimerActive(connectTimer, &timerattr);
        if(-1 == ret)
        {
            KING_SysLog("%s: Active Failed!", __FUNCTION__);
            return;
        }
    }
    else
    {
        if (connectTimer != NULL)
        {
            KING_TimerDeactive(connectTimer);
            KING_TimerDelete(connectTimer);
            connectTimer = NULL;
        }
    }
}

static void tcpip_SocketEventProcess(uint32 eventID, void* pdata, uint32 len)
{
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pdata;
    uint8 socketFd = (uint8)data->socket.socketID;
    uint8 socketID = tcpip_GetSocketIDBySocketFd(socketFd);
    KING_SysLog("tcpip_SocketEventProcess() eventID=%d, dataLen=%d\r\n", eventID, len);
    KING_SysLog("tcpip_SocketEventProcess() socketID=%d\r\n", socketID);
    if (socketID == 0xff)
    {
        return;
    }
    switch (eventID)
    {
        case SOCKET_CONNECT_EVENT_RSP:
        {
            KING_SysLog("tcpip SOCKET_CONNECT_EVENT_RSP\r\n");
            tcpip_connectTimer(FALSE);
            tcpip_socketConnected(socketID);
        }
            break;

        case SOCKET_ACCEPT_EVENT_IND:
            KING_SysLog("tcpip SOCKET_ACCEPT_EVENT_IND\r\n");
            break;

        case SOCKET_CLOSE_EVENT_RSP:
        {
            KING_SysLog("tcpip SOCKET_CLOSE_EVENT_RSP\r\n");
            KING_SysLog("tcpip isClose = %d\r\n", isClose);
            if (isClose)
            {
                AT_CMD_ENGINE_T *atEngine = pTcpipAtEngine;
                
                tcpip_SocketClearOne(socketID);
                isClose = FALSE;
                tcpip_atRspOk(atEngine);
                pTcpipAtEngine = NULL;
            }
        }
            break;

        case SOCKET_ERROR_EVENT_IND:
            KING_SysLog("tcpip SOCKET_ERROR_EVENT_IND\r\n");
            if (isRunning && socketInfo[socketID].state == SOCKET_MODEM_OPENING)
            {
                tcpip_connectTimer(FALSE);
                KING_SocketClose(socketInfo[socketID].socketfd);
                socketInfo[socketID].socketfd = TCPIP_SOCKET_ERROR;
                tcpip_OpenErrRsp(socketInfo[socketID].atEngine, socketID, socketInfo[socketID].accessMode, CME_TCPIP_SOCKET_CONNECT_FAILED, 0);
            }
            else if(isClose  && (socketInfo[socketID].state == SOCKET_MODEM_CONNECTED 
                || socketInfo[socketID].state == SOCKET_MODEM_OPENING))
            {
                AT_CMD_ENGINE_T *atEngine = pTcpipAtEngine;

                tcpip_SocketClearOne(socketID);
                isClose = FALSE;
                tcpip_atRspErr(atEngine, CME_TCPIP_UNKNOWN_ERROR);
                pTcpipAtEngine = NULL;
            }
            else if (socketInfo[socketID].accessMode != ACCESS_MODE_TRANSPARENT && socketInfo[socketID].state == SOCKET_MODEM_SEND)
            {
                socketInfo[socketID].sendDataLen = 0;
                socketInfo[socketID].unsendLen = 0;
                if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP_SERVER)
                {
                    socketInfo[socketID].state = SOCKET_MODEM_LISTENING;
                }
                else
                {
                    socketInfo[socketID].state = SOCKET_MODEM_CONNECTED;
                }
                
                gTcpipCurErr = CME_TCPIP_SOCKET_WRITE_FAILED;
                if (socketInfo[socketID].atEngine != NULL)
                {
                    at_CmdRespOKText(socketInfo[socketID].atEngine, (uint8 *)"SEND FAIL");
                }
                return;
            }
            break;

        case SOCKET_WRITE_EVENT_IND:
            KING_SysLog("tcpip SOCKET_WRITE_EVENT_IND ack by peer uSendDataSize=%d\r\n", data->socket.availSize);
            break;

        case SOCKET_READ_EVENT_IND:
        {
            KING_SysLog("tcpip SOCKET_READ_EVENT_IND ack by peer availSize=%d\r\n", data->socket.availSize);
            tcpip_MsgSend(TCPIP_RECV_DATA, socketInfo[socketID].ctxID, socketInfo[socketID].socketID, CMD_QI_NONE, data->socket.availSize);
        }
            break;
            
        case SOCKET_FULL_CLOSED_EVENT_IND:
        {
            bool isTtMode = FALSE;
            
            KING_SysLog("tcpip SOCKET_FULL_CLOSED_EVENT_IND\r\n");
            if ((socketInfo[socketID].accessMode == ACCESS_MODE_TRANSPARENT)
                && (socketInfo[socketID].ttDataMode))
            {
                isTtMode = TRUE;
            }
            tcpip_exitDataMode(socketID);
            tcpip_closeUrcRsp(socketID, isTtMode);
        }
            break;
            
        default:
            KING_SysLog("tcpip TCPIP unexpect event/response %d\r\n", eventID);
            break;
    }
}

static int tcpip_SocketCreate(uint8 ctxID, uint8 socketID, uint16 localPort,
                                TCPIP_SOCKET_TYPE_E socketType, TCPIP_PDP_TYPE_E pdpType)
{
    int ret = 0;
    int dss_protocol = 0;
    int addr_family;
    int sock_type = 0;
    int val = 1;  
    int val_len = sizeof( int );
    union sockaddr_aligned local_addr;
    uint32 localaddr_size = 0;
    
    KING_SysLog("tcpip_SocketCreate ctxID=%d\r\n", ctxID);
    KING_SysLog("tcpip_SocketCreate socketID=%d\r\n", socketID);
    KING_SysLog("tcpip_SocketCreate socketType=%d\r\n", socketType);
    if(socketType >= TCPIP_SOCK_TYPE_NUM)
    {
        KING_SysLog("tcpip_SocketCreate() fail\n");
        tcpip_OpenErrRsp(socketInfo[socketID].atEngine, socketID, socketInfo[socketID].accessMode, CME_TCPIP_CREATE_SOCKET_FAILED, 0);
        return -1;
    }
    if((TCPIP_SOCK_TYPE_UDP == socketType) 
        || (TCPIP_SOCK_TYPE_UDP_SERVER == socketType))
    {
        sock_type = SOCK_DGRAM;
        dss_protocol = IPPROTO_UDP;
    }
    else if((TCPIP_SOCK_TYPE_TCP == socketType) 
        || (TCPIP_SOCK_TYPE_TCP_SERVER == socketType))
    {
        sock_type = SOCK_STREAM;
        dss_protocol = IPPROTO_TCP;
    }
    if(pdpType == TCPIP_PDP_TYPE_IPV6)
    {
        addr_family = AF_INET6;
    }
    else
    {
        addr_family = AF_INET;
    }

    ret = KING_SocketCreate(addr_family, sock_type, dss_protocol, &socketInfo[socketID].socketfd);
    KING_SysLog("tcpip_SocketCreate socketfd=%d\r\n", socketInfo[socketID].socketfd);
    if (ret == -1 || socketInfo[socketID].socketfd < 0)
    {
        KING_SysLog("tcpip_SocketCreate() KING_SocketCreate fail\n");
        socketInfo[socketID].socketfd = TCPIP_SOCKET_ERROR;
        socketInfo[socketID].state = SOCKET_MODEM_INITIAL;
        tcpip_OpenErrRsp(socketInfo[socketID].atEngine, socketID, socketInfo[socketID].accessMode, CME_TCPIP_CREATE_SOCKET_FAILED, 0);
        return -1;
    }
    
    KING_RegNotifyFun(DEV_CLASS_SOCKET, socketInfo[socketID].socketfd, tcpip_SocketEventProcess);
    ret = KING_SocketOptSet(socketInfo[socketID].socketfd, SOL_SOCKET,
                            SO_REUSEADDR, &val, val_len);
    if ( ret < 0 )
    {
        KING_SysLog("tcpip KING_SocketOptSet() SO_REUSEADDR fail!!!\n");
    }
    if(addr_family == AF_INET6)
    {
        struct sockaddr_in6 *stLocalAddr6 = (struct sockaddr_in6 *)&(local_addr);
        IPV6_ADDR_S ip6;

        ret = KING_NetIpv6AddrInfoGet(tcpipPdpInfo[ctxID - 1].hContext, &ip6, NULL, NULL);
        if (ret == 0) 
        {
            stLocalAddr6->sin6_len = sizeof(struct sockaddr_in6);
            stLocalAddr6->sin6_family = AF_INET6;
            stLocalAddr6->sin6_port = 0;
            stLocalAddr6->sin6_addr.addr[0] = ip6.addr[0];
            stLocalAddr6->sin6_addr.addr[1] = ip6.addr[1];
            stLocalAddr6->sin6_addr.addr[2] = ip6.addr[2];
            stLocalAddr6->sin6_addr.addr[3] = ip6.addr[3];

            localaddr_size = sizeof(struct sockaddr_in6);
        }
    }
    else
    {
        IPV4_ADDR_S ip;
        struct sockaddr_in *stLocalAddr = (struct sockaddr_in *)&(local_addr);

        ret = KING_NetIpv4AddrInfoGet(tcpipPdpInfo[ctxID - 1].hContext, &ip, NULL, NULL);
        if(ret == 0)
        {
            stLocalAddr->sin_len = sizeof(struct sockaddr_in);
            stLocalAddr->sin_family = AF_INET;
            stLocalAddr->sin_port = 0;
            stLocalAddr->sin_addr.addr = ip.addr;

            localaddr_size = sizeof(struct sockaddr_in);
        }
    }
    
    if (ret == 0)
    {
        ret = KING_SocketBind(socketInfo[socketID].socketfd, &local_addr, localaddr_size);
        KING_SysLog("KH_SocketBind() ret=%d\r\n", ret);
        if (ret < 0)
        {
            KING_SysLog("KH_SocketBind fail\r\n");
            KING_SocketClose(socketInfo[socketID].socketfd);
            socketInfo[socketID].socketfd = TCPIP_SOCKET_ERROR;
            socketInfo[socketID].state = SOCKET_MODEM_INITIAL;
            tcpip_OpenErrRsp(socketInfo[socketID].atEngine, socketID, socketInfo[socketID].accessMode, CME_TCPIP_SOCKET_BIND_FAILED, 0);
            return -1;
        }
    }
    else
    {
        KING_SysLog("tcpip_SocketCreate() bind fail\n");
        KING_SocketClose(socketInfo[socketID].socketfd);
        socketInfo[socketID].socketfd = TCPIP_SOCKET_ERROR;
        socketInfo[socketID].state = SOCKET_MODEM_INITIAL;
        tcpip_OpenErrRsp(socketInfo[socketID].atEngine, socketID, socketInfo[socketID].accessMode, CME_TCPIP_SOCKET_BIND_FAILED, 0);
        return -1;
    } 

    return 0;
}

static void tcpip_SocketConnect(SOCK_IN_ADDR_T *addr, uint8 socketID)
{
    int ret = 0;
    union sockaddr_aligned sock_addr;
    uint32 sockaddr_size = 0;
    
    if (addr->type == IP_ADDR_TYPE_V6)
    {
        struct sockaddr_in6 *sock_addr6 = (struct sockaddr_in6 *)&(sock_addr);
        
        sock_addr6->sin6_len = sizeof(struct sockaddr_in6);
        sock_addr6->sin6_family = AF_INET6;
        sock_addr6->sin6_port = KING_htons(socketInfo[socketID].remotePort);
        sock_addr6->sin6_addr.addr[0] = addr->u_addr.ip6.addr[0];
        sock_addr6->sin6_addr.addr[1] = addr->u_addr.ip6.addr[1];
        sock_addr6->sin6_addr.addr[2] = addr->u_addr.ip6.addr[2];
        sock_addr6->sin6_addr.addr[3] = addr->u_addr.ip6.addr[3];
        
        sockaddr_size = sizeof(struct sockaddr_in6);
    }
    else
    {
        struct sockaddr_in *sock_addr4 = (struct sockaddr_in *)&(sock_addr);
        
        sock_addr4->sin_len = sizeof(struct sockaddr_in);
        sock_addr4->sin_family = AF_INET;
        sock_addr4->sin_port = KING_htons(socketInfo[socketID].remotePort);
        sock_addr4->sin_addr.addr = addr->u_addr.ip4.addr;
        KING_SysLog("tcpip_SocketConnect ip_addr=%u\r\n", sock_addr4->sin_addr.addr);

        sockaddr_size = sizeof(SOCK_ADDR_IN_T);
    }

    ret = KING_SocketConnect(socketInfo[socketID].socketfd, &sock_addr, sockaddr_size);
    if (ret < 0)
    {
        KING_SysLog("tcpip_SocketConnect fail\r\n");
        KING_SocketClose(socketInfo[socketID].socketfd);
        socketInfo[socketID].socketfd = TCPIP_SOCKET_ERROR;
        socketInfo[socketID].state = SOCKET_MODEM_INITIAL;
        tcpip_OpenErrRsp(socketInfo[socketID].atEngine, socketID, socketInfo[socketID].accessMode, CME_TCPIP_SOCKET_CONNECT_FAILED, 0);
        return;
    }
    tcpip_connectTimer(TRUE);
}

static void tcpip_socketDnsReqCb(void* pData, uint32 len)
{
    ASYN_DNS_PARSE_T *dnsParse = (ASYN_DNS_PARSE_T *)pData;
    
    KING_SysLog("tcpip_socketDnsReqCb hostname=%s\r\n", dnsParse->hostname);
    KING_SysLog("tcpip_socketDnsReqCb dnsParse->result=%d\n", dnsParse->result);
    if (dnsParse->result == DNS_PARSE_SUCCESS)
    {
        int ret = 0;
        uint8 socketID = gCurSocketID;
        SOCK_IN_ADDR_T ipaddr = dnsParse->addr;
        char *AddrStr = NULL;
        
        KING_SysLog("tcpip_socketDnsReqCb socketID=%d\r\n", socketID);
        ret = KING_ipntoa(ipaddr, &AddrStr);
        if (ret == -1)
        {
            tcpip_OpenErrRsp(socketInfo[socketID].atEngine, socketID, socketInfo[socketID].accessMode, CME_TCPIP_DNS_PARSE_FAILED, 0);
            return;
        }
        memset(socketInfo[socketID].ipAddrStr, 0x00, TCPIP_NETWORK_ADDR_MAX_LEN);
        strcpy(socketInfo[socketID].ipAddrStr, AddrStr);
        ret = tcpip_SocketCreate(socketInfo[socketID].ctxID, socketID, socketInfo[socketID].localPort, 
            socketInfo[socketID].socketType, tcpipPdpInfo[socketInfo[socketID].ctxID - 1].pdpType);
        if (ret == -1)
        {
            return;
        }
        if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_UDP)
        {
            tcpip_socketConnected(socketID);
        }
        else
        {
            tcpip_SocketConnect(&ipaddr, socketID);
        }
    }
    else
    {
        tcpip_OpenErrRsp(socketInfo[gCurSocketID].atEngine, gCurSocketID, socketInfo[gCurSocketID].accessMode, CME_TCPIP_DNS_PARSE_FAILED, 0);
    }
}

static void tcpip_GetIPAddrByHostname(uint8 socketID, uint8 ctxID)
{
    int ret = -1;
    SOCK_IN_ADDR_T destAddr;
    
    KING_SysLog("tcpip_GetIPAddrByHostname() start");
    KING_SysLog("tcpip_GetIPAddrByHostname() socketID=%d", socketID);
    KING_SysLog("tcpip_GetIPAddrByHostname() ctxID=%d", ctxID);
    if (socketInfo[socketID].socketID != socketID 
        || socketInfo[socketID].ctxID != ctxID 
        || tcpipPdpInfo[ctxID - 1].hContext == -1)
    {
        tcpip_OpenErrRsp(socketInfo[socketID].atEngine, socketID, socketInfo[socketID].accessMode, CME_TCPIP_DNS_PARSE_FAILED, 0);
        return;
    }
    memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
    ret = KING_NetGetHostByName(tcpipPdpInfo[ctxID - 1].hContext, 
                            socketInfo[socketID].addrStr, &destAddr, 
                            10000, (DNS_CALLBACK)tcpip_socketDnsReqCb);
    KING_SysLog("tcpip_GetIPAddrByHostname() ret=%d", ret);
    if (ret == 0)
    {
        gCurSocketID = socketID;
        if (((destAddr.type == IP_ADDR_TYPE_V6) && (0 == destAddr.u_addr.ip6.addr[0]) 
            && (0 == destAddr.u_addr.ip6.addr[1]) && (0 == destAddr.u_addr.ip6.addr[2])
            && (0 == destAddr.u_addr.ip6.addr[3])) || ((destAddr.type == IP_ADDR_TYPE_V4) 
            && (0 == destAddr.u_addr.ip4.addr)) )
        {
            KING_SysLog("tcpip_GetIPAddrByHostname ipv6 query QUEUED\r\n");
        }
        else
        {
            KING_SysLog("tcpip_GetIPAddrByHostname ipv4 query QUEUED\r\n");
        }
    }
    else
    {
        KING_SysLog("tcpip_GetIPAddrByHostname FAIL\r\n");
    }

    return;
}

static void tcpip_QisendCmdErrReset(uint8 socketID)
{
    socketInfo[socketID].inputData = FALSE;
    socketInfo[socketID].sendDataLen = 0;
    socketInfo[socketID].unsendLen = 0;
    if(socketInfo[socketID].state == SOCKET_MODEM_SEND)
    {
        if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP_SERVER)
        {
            socketInfo[socketID].state = SOCKET_MODEM_LISTENING;
        }
        else
        {
            socketInfo[socketID].state = SOCKET_MODEM_CONNECTED;
        }
    }
}

static void tcpip_QisendCmdSendMsg(AT_CMD_ENGINE_T *atEngine, uint8 socketID, char *msg, int len)
{
    int send_ret = 0;
    
    if (!socketInfo[socketID].inputData)
    {
        tcpip_atRspErr(atEngine, CME_TCPIP_UNKNOWN_ERROR);
        return;
    }
    KING_SysLog("tcpip_QisendCmdSendMsg socketID=%d\n", socketID);
    if (socketInfo[socketID].state == SOCKET_MODEM_CLOSING)
    {
        goto ERROR;
    }
    if((len <= 0) || msg == NULL)
    {
        goto ERROR;
    }

    socketInfo[socketID].inputData = FALSE;
    if (tcpipConfig.sendDataFormat == 1)
    {
        if ((0 != len % 2) || ((len >> 1) > TCPIP_TX_BUF_MAX_LEN))
        {
            goto ERROR;
        }
    }
    else
    {
        if (len > TCPIP_TX_BUF_MAX_LEN)
        {
            goto ERROR;
        }
    }

    memset(socketInfo[socketID].sendBuf, 0, TCPIP_TX_BUF_MAX_LEN);
    if (tcpipConfig.sendDataFormat == 1)
    {
        if (FALSE == atUtils_ConvertHexToBin((char *)msg, len, socketInfo[socketID].sendBuf))
        {
            goto ERROR; 
        }
        socketInfo[socketID].sendDataLen = (len >> 1);
    }
    else
    {
        memcpy(socketInfo[socketID].sendBuf, (char*) msg, len);
        socketInfo[socketID].sendDataLen = len;
    }
    socketInfo[socketID].unsendLen = socketInfo[socketID].sendDataLen;

    send_ret = tcpip_SendData(socketID);
    socketInfo[socketID].sendDataLen = 0;
    socketInfo[socketID].unsendLen = 0;
    if (socketInfo[socketID].state == SOCKET_MODEM_SEND)
    {
        if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP_SERVER)
        {
            socketInfo[socketID].state = SOCKET_MODEM_LISTENING;
        }
        else
        {
            socketInfo[socketID].state = SOCKET_MODEM_CONNECTED;
        }
    }
    memset(g_rspStr, 0x00, 1024);
    if (-1 == send_ret)
    {
        gTcpipCurErr = CME_TCPIP_SOCKET_WRITE_FAILED;
        sprintf((char *)g_rspStr, "SEND FAIL");
        AT_CMD_RETURN(at_CmdRespOKText(atEngine, g_rspStr));
    }
    else
    {
        gTcpipCurErr = TCPIP_OPERATE_SUCC;
        sprintf((char *)g_rspStr, "SEND OK");
        AT_CMD_RETURN(at_CmdRespOKText(atEngine, g_rspStr));
    }

ERROR: 
    gTcpipCurErr = CME_TCPIP_SOCKET_WRITE_FAILED;

    socketInfo[socketID].inputData = FALSE;
    socketInfo[socketID].sendDataLen = 0;
    socketInfo[socketID].unsendLen = 0;
    if(socketInfo[socketID].state == SOCKET_MODEM_SEND)
    {
        if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP_SERVER)
        {
            socketInfo[socketID].state = SOCKET_MODEM_LISTENING;
        }
        else
        {
            socketInfo[socketID].state = SOCKET_MODEM_CONNECTED;
        }
    }
    gTcpipCurErr = CME_TCPIP_SOCKET_WRITE_FAILED;
    sprintf((char *)g_rspStr, "SEND FAIL");
    AT_CMD_RETURN(at_CmdRespOKText(atEngine, g_rspStr));
}

static void tcpip_QisendEnterInputDataMode(AT_CMD_PARA *pParam, uint8 socketID, uint16 length, uint16 argCount)
{
    uint16 tempLen = length;
    
    if ((socketInfo[socketID].accessMode == ACCESS_MODE_TRANSPARENT)
        || (socketInfo[socketID].state != SOCKET_MODEM_CONNECTED))
    {
        KING_SysLog("tcpip_QisendEnterInputDataMode is transparent!");
        tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_ALLOWED);
        return;
    }

    if (argCount == 1)
    {
        tempLen = TCPIP_TX_BUF_MAX_LEN;
    }
    socketInfo[socketID].state = SOCKET_MODEM_SEND;
    socketInfo[socketID].inputData = TRUE;
    if (tcpipConfig.sendDataFormat == 1)
    {
        tcpipSendInfo.length = (tempLen<<1);
    }
    else
    {
        tcpipSendInfo.length = tempLen;
    }
    KING_SysLog("tcpip_QisendEnterInputDataMode: pParam->iExDataLen=%d, length=%d",
                      pParam->iExDataLen, tcpipSendInfo.length);
    if (!pParam->iExDataLen)
    {
        at_CmdRespOutputPrompt(pParam->engine);
        KING_SysLog("tcpip_QisendEnterInputDataMode: pParam->iExDataLen=%d, length=%d",
                          pParam->iExDataLen, tcpipSendInfo.length);
        if (tcpipSendInfo.length > 0)
        {
            at_CmdSetRawMode(pParam->engine, tcpipSendInfo.length);
        }
        else
        {
            at_CmdSetPromptMode(pParam->engine);
        }
        return;
    }
    else
    {
        KING_SysLog("tcpip_QisendEnterInputDataMode: 11 pParam->iExDataLen=%d, length=%d",
                          pParam->iExDataLen, tcpipSendInfo.length);
        if (pParam->pExData[pParam->iExDataLen - 1] == 27)
        {
            KING_SysLog("tcpip_QisendEnterInputDataMode: end with esc, cancel send");
            tcpip_atRspOk(pParam->engine);
            return;
        }
        else if (pParam->pExData[pParam->iExDataLen - 1] != 26 || pParam->iExDataLen <= 1)
        {
            KING_SysLog("tcpip_QisendEnterInputDataMode: not end with ctl+z, err happen");
            gTcpipCurErr = ERR_AT_CMS_INVALID_LEN;
            AT_CMD_RETURN(at_CmdRespError(pParam->engine));
        }
        if (tcpipSendInfo.length != pParam->iExDataLen)
        {
            KING_SysLog("tcpip_QisendEnterInputDataMode: 22 pParam->iExDataLen=%d, length=%d",
                              pParam->iExDataLen, tcpipSendInfo.length);
            gTcpipCurErr = ERR_AT_CMS_INVALID_LEN;
            AT_CMD_RETURN(at_CmdRespError(pParam->engine));
        }
        
        tcpip_QisendCmdSendMsg(pParam->engine, socketID, (char *)pParam->pExData, (int)(pParam->iExDataLen - 1));
        
    }

    return;
}

static void tcpip_QisendQuery(AT_CMD_ENGINE_T *atEngine, uint8 socketID)
{
    char rspString[50] = {0};
    int32 ackSize = 0, totalSentSize = 0;
    int ret = 0;
    
    memset(rspString, 0x00, 50);
    if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP
        || socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP_SERVER
        || socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP_INCOMING)
    {
        ret = KING_TcpipSocketAckedAndTotalSentSizeGet(socketInfo[socketID].socketfd, &ackSize, &totalSentSize);
        if (ret != 0)
        {
            ackSize = socketInfo[socketID].sendDataLen;
            totalSentSize = socketInfo[socketID].totalSendNum;
        }
    }
    else
    {
        ackSize = 0;
        totalSentSize = socketInfo[socketID].totalSendNum;
    }
    sprintf (rspString, "+QISEND: %u,%u,%u",
            totalSentSize, ackSize,
            totalSentSize - ackSize);
    
    at_CmdRespInfoText(atEngine, (uint8 *)rspString);
    tcpip_atRspOk(atEngine);
}

static void tcpip_QideactResetPdpInfo(uint8 ctxID)
{
    KING_SysLog("tcpip_QideactResetPdpInfo ctxID:%d", ctxID);
    if (ctxID <= 0 || ctxID > TCPIP_CONTEXT_ID_MAX_NUM)
    {
        return;
    }
    if (tcpipPdpInfo[ctxID - 1].hContext != -1)
    {
        KING_ConnectionRelease(tcpipPdpInfo[ctxID - 1].hContext);
    }

    memset(&tcpipPdpInfo[ctxID - 1], 0, sizeof(TCPIP_PDP_INFO_S));
    tcpipPdpInfo[ctxID - 1].ctxID = 0;
    tcpipPdpInfo[ctxID - 1].netState = TCPIP_STATE_PDP_DEACT;
    tcpipPdpInfo[ctxID - 1].actStatus = TCPIP_PDP_DEACTIVATE;
    tcpipPdpInfo[ctxID - 1].pdpType = TCPIP_PDP_TYPE_NONE;
    tcpipPdpInfo[ctxID - 1].hContext = -1;
    memset(tcpipPdpInfo[ctxID - 1].ipAddr, 0x00, TCPIP_IP_ADDR_STRINT_LEN);
}

static void tcpip_deactUrcRsp(uint8 ctxID)
{
    if (tcpipPdpInfo[ctxID - 1].atEngine != NULL)
    {
        char buf[30] = {0};

        KING_SysLog("tcpip_deactUrcRsp ctxID:%d", ctxID);
        memset(buf, 0, 30);
        sprintf(buf, "\r\n+QIURC: \"pdpdeact\",%d\r\n", ctxID);
        at_CmdRespOutputText(tcpipPdpInfo[ctxID - 1].atEngine, (uint8 *)buf);
    }
}

static void tcpip_networkDownUrcReport(uint8 ctxID)
{
    KING_SysLog("tcpip_networkDownUrcReport netState:%d",tcpipPdpInfo[ctxID - 1].netState);
    if(tcpipPdpInfo[ctxID - 1].netState == TCPIP_STATE_GPRSACT)
    {
        KING_SysLog("tcpip_networkDownUrcReport ctxID:%d", ctxID);
        if (http_isEnterDataMode())
        {
            http_networkDownExit(ctxID);
        }
        tcpip_exitDataMode(tcpip_getEnterDataModeSocketID(ctxID));
        tcpip_CloseSocketByContext(ctxID);
        tcpip_deactUrcRsp(ctxID);
        tcpip_QideactResetPdpInfo(ctxID);
    }
}

static void tcpip_NetCallBack(uint32 eventID, void* pdata, uint32 len)
{
    int ret = -1;
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pdata;

    KING_SysLog("tcpip_NetCallBack() eventID=0x%x, len=%d, pdata=%p\r\n", eventID, len, pdata);
    switch(eventID)
    {
    case NW_EVENT_GSM_REGISTERED_IND:
        KING_SysLog("[tcpip]NET registered\r\n");
        break;

    case NW_EVENT_GSM_UNREGISTERED_IND:
        KING_SysLog("[tcpip]NET unregistered\r\n");
        break;

    case NW_EVENT_GPRS_ATTACH_IND:
    {
    }
        break;

    case NW_EVENT_GPRS_DEATTACH_IND:
        KING_SysLog("[tcpip]GPRS deattached\r\n");
        break;

    case NW_EVENT_PS_ONOFF_RSP:
        KING_SysLog("[tcpip]GPRS attach rsp\r\n");
        break;

    case NW_EVENT_ATTACH_RSP:
        KING_SysLog("[tcpip]GPRS attach rsp\r\n");
        break;

    case NW_EVENT_PDP_ACT_RSP:
    {
        uint8 pdpActCid = (uint8)(data->net.info.value);
        uint32 result = data->net.info.result;
        NET_ACTION action = data->net.info.action;
        
        if (tcpipPdpInfo[pdpActCid - 1].atEngine == NULL)
        {
            return;
        }
        KING_SysLog("[tcpip] NW_EVENT_PDP_ACT_RSP pdpActCid=%d\r\n", pdpActCid);
        KING_SysLog("[tcpip] NW_EVENT_PDP_ACT_RSP action=%d\r\n", action);
        KING_SysLog("[tcpip] NW_EVENT_PDP_ACT_RSP result=%d\r\n", result);
        if (action == NET_ACTION_ACTIVE && tcpipAction == NET_ACTION_ACTIVE)
        {
            if (result != 0)
            {
                if (activeType < TCPIP_SOCKET_ID_MAX_NUM)
                {
                    tcpip_OpenErrRsp(socketInfo[gCurSocketID].atEngine, gCurSocketID, socketInfo[gCurSocketID].accessMode, CME_TCPIP_OPEN_PDP_CONTEXT_FAILED, 1);
                }
                else if (activeType == PDP_ACTIVE_SOCK_JUST_ACTIVE)
                {
                    tcpip_atRspErr(tcpipPdpInfo[pdpActCid - 1].atEngine, CME_TCPIP_OPEN_PDP_CONTEXT_FAILED);
                }
                else if (activeType == PDP_ACTIVE_SOCK_HTTP_ACTIVE)
                {
                    http_activePdpCnf(FALSE);
                }
                else if (activeType == PDP_ACTIVE_SOCK_QIDNSGIP_ACTIVE)
                {
                    at_CmdRespOK(tcpipPdpInfo[pdpActCid - 1].atEngine);
                    tcpip_ErrRsp(tcpipPdpInfo[pdpActCid - 1].atEngine, "+QIURC: \"dnsgip\",", CME_TCPIP_OPEN_PDP_CONTEXT_FAILED);
                }
                activeType = 13;
                tcpipAction = NET_ACTION_MAX;
                return;
            }
            KING_Sleep(300);
            ret = tcpip_qiactActSet(pdpActCid);
            if (ret == -1)
            {
                if (activeType < TCPIP_SOCKET_ID_MAX_NUM)
                {
                    tcpip_OpenErrRsp(socketInfo[gCurSocketID].atEngine, gCurSocketID, socketInfo[gCurSocketID].accessMode, CME_TCPIP_OPEN_PDP_CONTEXT_FAILED, 1);
                }
                else if (activeType == PDP_ACTIVE_SOCK_JUST_ACTIVE)
                {
                    tcpip_atRspErr(tcpipPdpInfo[pdpActCid - 1].atEngine, CME_TCPIP_OPEN_PDP_CONTEXT_FAILED);
                }
                else if (activeType == PDP_ACTIVE_SOCK_HTTP_ACTIVE)
                {
                    http_activePdpCnf(FALSE);
                }
                else if (activeType == PDP_ACTIVE_SOCK_QIDNSGIP_ACTIVE)
                {
                    at_CmdRespOK(tcpipPdpInfo[pdpActCid - 1].atEngine);
                    tcpip_ErrRsp(tcpipPdpInfo[pdpActCid - 1].atEngine, "+QIURC: \"dnsgip\",", CME_TCPIP_OPEN_PDP_CONTEXT_FAILED);
                }
                activeType = 13;
                tcpipAction = NET_ACTION_MAX;
                return;
            }
            
            if (activeType < TCPIP_SOCKET_ID_MAX_NUM)
            {
                if (socketInfo[gCurSocketID].accessMode != ACCESS_MODE_TRANSPARENT)
                {
                    tcpip_atRspOk(socketInfo[gCurSocketID].atEngine);
                }
                tcpip_MsgSend(TCPIP_DNS_PARSE_REQ, pdpActCid, socketInfo[gCurSocketID].socketID, CMD_QIOPEN, 0);
            }
            else if (activeType == PDP_ACTIVE_SOCK_JUST_ACTIVE)
            {
                tcpip_atRspOk(tcpipPdpInfo[pdpActCid - 1].atEngine);
            }
            else if (activeType == PDP_ACTIVE_SOCK_HTTP_ACTIVE)
            {
                http_activePdpCnf(TRUE);
            }
            else if (activeType == PDP_ACTIVE_SOCK_QIDNSGIP_ACTIVE)
            {
                tcpip_atRspOk(tcpipPdpInfo[pdpActCid - 1].atEngine);
                tcpip_MsgSend(TCPIP_DNS_PARSE_REQ, pdpActCid, PDP_ACTIVE_SOCK_QIDNSGIP_ACTIVE, CMD_QIDNSGIP, 0);
            }
            activeType = 13;
            tcpipAction = NET_ACTION_MAX;
        }
        else if (action == NET_ACTION_DEACTIVE && tcpipAction == NET_ACTION_DEACTIVE)
        {
            if (result != 0)
            {
                tcpip_atRspErr(tcpipPdpInfo[pdpActCid - 1].atEngine, CME_TCPIP_CLOSE_PDP_CONTEXT_FAILED);
            }
            else
            {
                AT_CMD_ENGINE_T *atEngine = tcpipPdpInfo[pdpActCid - 1].atEngine;
                
                tcpip_QideactResetPdpInfo(pdpActCid);
                tcpip_atRspOk(atEngine);
            }
            
            tcpipAction = NET_ACTION_MAX;
        }
        break;
    }

    case NW_EVENT_PDP_DEACT_IND:
    {
        uint8 pdpActCid = (uint8)(data->net.value);
        
        KING_SysLog("[tcpip]PDP deactive ind\r\n");
        tcpip_networkDownUrcReport(pdpActCid);
    }
        break;

    default:
        break;
    }
}

static int tcpip_isSetApn(char * apnStr, uint8 ctxID)
{
    uint8 i = 0;
    
    for (i = 0; i < TCPIP_CONTEXT_ID_MAX_NUM; i++)
    {
        if (strcasecmp(apnStr, qicsgpInfo[i].apn) == 0
            && qicsgpInfo[i].cid != ctxID)
        {
            return 1;
        }
    }

    return 0;
}

static void tcpip_qicsgpSet(uint8 cid, uint8 type, char *apn, char *usrName, char *pwd, uint8 authType)
{
    if (cid < 1 || cid > TCPIP_CONTEXT_ID_MAX_NUM)
    {
        return;
    }
    memset(&qicsgpInfo[cid - 1], 0, sizeof(TCPIP_QICSGP_PDP_CTX_S));
    qicsgpInfo[cid - 1].cid = cid;
    qicsgpInfo[cid - 1].type = type;
    qicsgpInfo[cid - 1].authType = authType;
    if (strlen(apn) > 0)
    {
        memcpy(qicsgpInfo[cid - 1].apn, apn, strlen(apn));
    }
    if (strlen(usrName) > 0)
    {
        memcpy(qicsgpInfo[cid - 1].userName, usrName, strlen(usrName));
    }
    if (strlen(pwd) > 0)
    {
        memcpy(qicsgpInfo[cid - 1].password, pwd, strlen(pwd));
    }
}

static void tcpip_qicsgpReadRsp(AT_CMD_PARA *pParam, uint8 cid)
{
    char strRsp[TCPIP_MAX_STRING_RSP] = {0};

    memset(&qicsgpInfo[cid - 1], 0x00, sizeof(TCPIP_QICSGP_PDP_CTX_S));
    qicsgpInfo[cid - 1].type = 1;
    qicsgpInfo[cid - 1].cid = cid;
    KING_TcpipNetApnGet(cid, qicsgpInfo[cid - 1].apn, qicsgpInfo[cid - 1].userName,
        qicsgpInfo[cid - 1].password, &(qicsgpInfo[cid - 1].authType), &(qicsgpInfo[cid - 1].type));

    memset(strRsp, 0x00, TCPIP_MAX_STRING_RSP);
    sprintf(strRsp, "+QICSGP: %d,\"%s\",\"%s\",\"%s\",%d", 
        qicsgpInfo[cid - 1].type, qicsgpInfo[cid - 1].apn, qicsgpInfo[cid - 1].userName,
        qicsgpInfo[cid - 1].password, qicsgpInfo[cid - 1].authType);
    at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
    tcpip_atRspOk(pParam->engine);
}

static void tcpip_qiactReadRsp(AT_CMD_PARA *pParam)
{
    uint8 i = 0, count = 0;
    char strRsp[TCPIP_MAX_STRING_RSP] = {0};

    for (i = 0; i < TCPIP_CONTEXT_ID_MAX_NUM; i++)
    {
        if ((tcpipPdpInfo[i].netState != TCPIP_STATE_GPRSACT) &&
            (tcpipPdpInfo[i].actStatus != TCPIP_PDP_ACTIVATE))
        {
            continue;
        }  
        memset((char *)strRsp, 0x00, TCPIP_MAX_STRING_RSP);
        if (count == 0)
        {
            at_CmdRespOutputText(pParam->engine, (uint8 *)"\r\n");
        }
        sprintf(strRsp, "%s: %d,%d,%d,\"%s\"\r\n",
                "+QIACT",
                tcpipPdpInfo[i].ctxID,
                tcpipPdpInfo[i].actStatus,
                tcpipPdpInfo[i].pdpType,
                tcpipPdpInfo[i].ipAddr
                );
        at_CmdRespOutputText(pParam->engine, (uint8 *)strRsp);
        count++;
    }
    tcpip_atRspOk(pParam->engine);
}

static int tcpip_qiactActSet(uint8 cid)
{
    int ret = 0;
    
    if (cid < 1 || cid > 4)
    {
        return -1;
    }
    ret = tcpip_getContextHandle(cid);
    if (ret == -1)
    {
        return -1;
    }
    ret = tcpip_getLocalIp(cid);
    if (ret == -1)
    {
        return -1;
    }
    tcpipPdpInfo[cid - 1].netState = TCPIP_STATE_GPRSACT;
    tcpipPdpInfo[cid - 1].actStatus = TCPIP_PDP_ACTIVATE;
    tcpipPdpInfo[cid - 1].ctxID = cid;
    
    return ret;
}

static void tcpip_qiopenCmdSet
    (
        AT_CMD_ENGINE_T *atEngine, uint8 nDLCI, uint8 ctxID, uint8 socketID,
        TCPIP_SOCKET_TYPE_E socketType, char *domainName,
        uint32 remotePort, uint32 localPort, uint8 accessMode
    )
{
    if ((accessMode == ACCESS_MODE_TRANSPARENT) 
        && ((socketType != TCPIP_SOCK_TYPE_TCP) 
        && (socketType != TCPIP_SOCK_TYPE_UDP)))
    {
        tcpip_atRspErr(atEngine, CME_TCPIP_OPERATION_NOT_ALLOWED);
        return;
    }
    if (tcpip_IsEnterDataMode())
    {
        tcpip_atRspErr(atEngine, CME_TCPIP_OPERATION_NOT_ALLOWED);
        return;
    }

    if (socketType > TCPIP_SOCK_TYPE_TCP_SERVER)
    {
        at_CmdRespOK(atEngine);
        tcpip_OpenRsp(atEngine, socketID, CME_TCPIP_OPERATION_NOT_SUPPORTED);
        return;
    }
    if (isRunning)
    {
        gTcpipCurErr = CME_TCPIP_OPERATION_BUSY;
        if (accessMode == ACCESS_MODE_TRANSPARENT)
        {
            at_CmdRespError(atEngine);
        }
        else
        {
            at_CmdRespOK(atEngine);
            memset(g_rspStr, 0x00, 1024);
            sprintf((char *)g_rspStr, "+QIOPEN: %d,%d", socketID, CME_TCPIP_OPERATION_BUSY);
            at_CmdRespUrcText(atEngine, g_rspStr);
        }
        return;
    }
    if (((socketInfo[socketID].socketfd != TCPIP_SOCKET_ERROR) &&
        ((socketInfo[socketID].state >= SOCKET_MODEM_OPENING) &&
        (socketInfo[socketID].state <= SOCKET_MODEM_CLOSING)) &&
        (socketInfo[socketID].socketID == socketID)) ||
        socketInfo[socketID].running)
    {
        if (accessMode == ACCESS_MODE_TRANSPARENT)
        {
            tcpip_atRspErr(atEngine, CME_TCPIP_SOCKET_ID_USED);
        }
        else
        {
            at_CmdRespOK(atEngine);
            tcpip_OpenRsp(atEngine, socketID, CME_TCPIP_SOCKET_ID_USED);
        }
        return;
    }
    
    tcpipPdpInfo[ctxID - 1].atEngine = atEngine;
    isRunning = TRUE;
    gCurSocketID = socketID;
    gTcpipCurErr = TCPIP_OPERATE_SUCC;
    socketInfo[socketID].running = TRUE;
    socketInfo[socketID].ctxID = ctxID;
    socketInfo[socketID].socketID = socketID;
    socketInfo[socketID].localPort = localPort;
    socketInfo[socketID].socketType = socketType;
    socketInfo[socketID].remotePort = remotePort;
    memset(socketInfo[socketID].addrStr, 0, TCPIP_NETWORK_ADDR_MAX_LEN);
    strcpy(socketInfo[socketID].addrStr, domainName);
    socketInfo[socketID].accessMode = accessMode;
    socketInfo[socketID].state = SOCKET_MODEM_OPENING;
    socketInfo[socketID].serverID = socketID;
    socketInfo[socketID].atEngine = atEngine;
    socketInfo[socketID].nDLCI = nDLCI;

    if(socketInfo[socketID].accessMode == ACCESS_MODE_TRANSPARENT)
    {
        tcpip_IsLastTTInit();
        tcpip_TransparentInit(socketID);
        socketInfo[socketID].ttDataMode = TRUE;
    }
    else
    {
        AtRingBuf_Uninit(&(tcpipTtTxBuf[socketID]));
        socketInfo[socketID].isLastTT = FALSE;
    } 
    //if (strlen(qicsgpInfo[ctxID - 1].apn) == 0)
    //{
        //tcpip_OpenErrRsp(atEngine, socketID, accessMode, CME_TCPIP_APN_NOT_CONFIGURED, 1);
        //return;
    //}
    if (tcpipPdpInfo[ctxID - 1].netState == TCPIP_STATE_INITIAL 
        || tcpipPdpInfo[ctxID - 1].netState == TCPIP_STATE_PDP_DEACT)
    {
        int ret = 0;

        if (tcpip_ActivedMax() >= TCPIP_ALLOWED_ACTIVATED_MAX)
        {
            tcpip_OpenErrRsp(atEngine, socketID, accessMode, CME_TCPIP_OPERATION_NOT_ALLOWED, 1);
            return;
        }
        
        ret = tcpip_activeByCtxID(atEngine, ctxID, socketID);
        if (ret == -1)
        {
            tcpip_OpenErrRsp(atEngine, socketID, accessMode, CME_TCPIP_OPEN_PDP_CONTEXT_FAILED, 1);
            return;
        }
        else if (ret == 1)
        {
            if (accessMode != ACCESS_MODE_TRANSPARENT)
            {
                tcpip_atRspOk(atEngine);
            }
            tcpip_MsgSend(TCPIP_DNS_PARSE_REQ, ctxID, socketID, CMD_QIOPEN, 0);
        }
    }
    else if (tcpipPdpInfo[ctxID - 1].netState == TCPIP_STATE_GPRSACT)
    {
        if (accessMode != ACCESS_MODE_TRANSPARENT)
        {
            tcpip_atRspOk(atEngine);
        }
        tcpip_MsgSend(TCPIP_DNS_PARSE_REQ, ctxID, socketID, CMD_QIOPEN, 0);
    }
    else
    {
        tcpip_OpenErrRsp(atEngine, socketID, accessMode, CME_TCPIP_OPERATION_NOT_ALLOWED, 1);
    }
    
    return;
}

static void tcpip_qicloseCmdSet(AT_CMD_ENGINE_T *atEngine, uint8 socketID, uint32 timeout)
{
    int ret = -1;
    struct linger lingerVal;
    uint32 len = sizeof(struct linger);

    KING_SysLog("tcpip_qicloseCmdSet state=%d\n", socketInfo[socketID].state);
    KING_SysLog("tcpip_qicloseCmdSet socketID=%d socketfd=%d\n", socketID, socketInfo[socketID].socketfd);
    if (socketInfo[socketID].socketfd == TCPIP_SOCKET_ERROR)
    {
        tcpip_atRspOk(atEngine);
        return;
    }

    if (socketInfo[socketID].socketID != socketID)
    {
        tcpip_atRspErr(atEngine, CME_TCPIP_OPERATION_NOT_ALLOWED);
        return;
    }
    if (socketInfo[socketID].state != SOCKET_MODEM_CONNECTED
        && socketInfo[socketID].state != SOCKET_MODEM_CLOSING)
    {
        tcpip_atRspErr(atEngine, CME_TCPIP_OPERATION_NOT_ALLOWED);
        return;
    }
    lingerVal.l_onoff = TRUE;
    if(timeout > 0)
    {
        lingerVal.l_linger = timeout;
    }
    else
    {
        lingerVal.l_linger = 0;
    }
    ret = KING_SocketOptSet( socketInfo[socketID].socketfd,
                    SOL_SOCKET, SO_LINGER, &lingerVal, len);

    if (ret == -1)
    {
        KING_SysLog("tcpip_qicloseCmdSet SocketOptSet SO_LINGER fail\n");
    }
    
    isClose = TRUE;
    pTcpipAtEngine = atEngine;
    ret = KING_SocketClose(socketInfo[socketID].socketfd);
    if (ret == -1)
    {
        KING_SysLog("qtTcpip_QicloseCmdSet closesocket fail\n");
        isClose = FALSE;
        tcpip_atRspErr(atEngine, CME_TCPIP_UNKNOWN_ERROR);
        return;
    }
}

static void tcpip_QisendexSendMsg(AT_CMD_ENGINE_T *atEngine, uint8 socketID, char  *msg, uint16 len)
{
    int send_ret = 0;
  
    if ((socketInfo[socketID].state != SOCKET_MODEM_CONNECTED)
        || (socketInfo[socketID].socketID != socketID)
        || (socketInfo[socketID].socketfd == TCPIP_SOCKET_ERROR)
        || (socketInfo[socketID].unsendLen > 0)
        || ((TCPIP_SOCK_TYPE_TCP != socketInfo[socketID].socketType) 
        && (TCPIP_SOCK_TYPE_UDP != socketInfo[socketID].socketType))
        || (socketInfo[socketID].accessMode == ACCESS_MODE_TRANSPARENT))
    {
        tcpip_atRspErr(atEngine, CME_TCPIP_OPERATION_NOT_ALLOWED);
        return;
    }
    if ((len <= 0) || msg == NULL)
    {
        tcpip_atRspErr(atEngine, CME_TCPIP_OPERATION_NOT_ALLOWED);
        return;
    }

    if ((0 != len % 2) || (len > TCPIP_TX_HEX_BUF_MAX_LEN))
    {
        tcpip_atRspErr(atEngine, CME_TCPIP_OPERATION_NOT_ALLOWED);
        return;
    }
  
    memset(socketInfo[socketID].sendBuf, 0, TCPIP_TX_BUF_MAX_LEN);
    if (FALSE == atUtils_ConvertHexToBin(msg, len, socketInfo[socketID].sendBuf))
    {
        tcpip_atRspErr(atEngine, CME_TCPIP_INVALID_PARAMETERS);
        return;
    }
  
    socketInfo[socketID].sendDataLen = (len >> 1);
    socketInfo[socketID].unsendLen = socketInfo[socketID].sendDataLen;
    socketInfo[socketID].state = SOCKET_MODEM_SEND;

    send_ret = tcpip_SendData(socketID);
    socketInfo[socketID].sendDataLen = 0;
    socketInfo[socketID].unsendLen = 0;
    if (socketInfo[socketID].state == SOCKET_MODEM_SEND)
    {
        if (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP_SERVER)
        {
            socketInfo[socketID].state = SOCKET_MODEM_LISTENING;
        }
        else
        {
            socketInfo[socketID].state = SOCKET_MODEM_CONNECTED;
        }
    }
    
    memset(g_rspStr, 0x00, 1024);
    if (-1 == send_ret)
    {
        gTcpipCurErr = CME_TCPIP_SOCKET_WRITE_FAILED;
        sprintf((char *)g_rspStr, "SEND FAIL");
        AT_CMD_RETURN(at_CmdRespOKText(atEngine, g_rspStr));
    }
    else
    {
        gTcpipCurErr = TCPIP_OPERATE_SUCC;
        sprintf((char *)g_rspStr, "SEND OK");
        AT_CMD_RETURN(at_CmdRespOKText(atEngine, g_rspStr));
    }

    return;
}

static void tcpip_getRxDataFromBuf(AT_CMD_ENGINE_T *atEngine, uint8 socketID, uint16 length, uint8 argCount)
{
    char *pBuff = NULL;
    int actual_size = 0;
    uint16 reqlen = 0;

    if(TRUE == AtRingBuf_IsInited(&(tcpipRxBuf[socketID])))
    {
        if (argCount == 1)
        {
            reqlen = TCPIP_RX_BUF_MAX_LEN;
        }
        else
        {
            reqlen = length;
        }

        pBuff = (char*)malloc(TCPIP_RX_BUF_MAX_LEN * 2 + 1);
        if (pBuff == NULL)
        {
            tcpip_atRspErr(atEngine, CME_TCPIP_MEMORY_NOT_ENOUGH);
            return;
        }
        memset(pBuff, 0x00, TCPIP_RX_BUF_MAX_LEN * 2 + 1);
        actual_size = AtRingBuf_Out(&(tcpipRxBuf[socketID]), pBuff, reqlen);
    }
    if (actual_size == 0)
    {
        char rspString[30] = {0};

        memset(rspString, 0, 30);
        snprintf(rspString, 30, "+QIRD: 0");
        at_CmdRespInfoText(atEngine, (uint8 *)rspString);
    }
    else if (actual_size > 0)
    {
        char rspString[300] = {0};

        memset(rspString, 0x00, 300);
        if ((socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP) ||
            (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_UDP) ||
            (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_TCP_INCOMING))
        {
            if (tcpipConfig.viewMode == 0)
            {
                snprintf (rspString, 300, "\r\n+QIRD: %d\r\n", actual_size);
            }
            else
            {
                snprintf (rspString, 300, "\r\n+QIRD: %d,", actual_size);
            }
        }
        if ((socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_UDP_SERVER) &&
            (argCount == 1))
        {
            if (tcpipConfig.viewMode == 0)
            {
                snprintf (rspString, 300, "\r\n+QIRD: %d,\"%s\",%d\r\n",
                            actual_size,
                            socketInfo[socketID].ipAddrStr,
                            socketInfo[socketID].remotePort);
            }
            else
            {
                snprintf (rspString, 300, "\r\n+QIRD: %d,\"%s\",%d,",
                            actual_size,
                            socketInfo[socketID].ipAddrStr,
                            socketInfo[socketID].remotePort);
            }
        }
        at_CmdRespOutputNText(atEngine, (uint8 *)rspString, strlen(rspString));
        tcpip_ReadData((uint16)actual_size, socketID, pBuff, FALSE);
    }
    
    if (pBuff != NULL)
    {
        free(pBuff);
        pBuff = NULL;
    }
    return;
}

static void tcpip_qirdCmdSet(AT_CMD_ENGINE_T *atEngine, uint8 socketID, uint16 length, uint8 argCount)
{
    if ((socketInfo[socketID].socketID != socketID)
        || (socketInfo[socketID].socketfd == TCPIP_SOCKET_ERROR))
    {
        tcpip_atRspErr(atEngine, CME_TCPIP_OPERATION_NOT_ALLOWED);
        return;
    }
  
    if ((socketInfo[socketID].state != SOCKET_MODEM_CONNECTED) &&
        (socketInfo[socketID].state != SOCKET_MODEM_SEND) &&
        (socketInfo[socketID].state != SOCKET_MODEM_LISTENING) &&
        (socketInfo[socketID].state != SOCKET_MODEM_CLOSING))
    {
        tcpip_atRspErr(atEngine, CME_TCPIP_UNKNOWN_ERROR);
        return;
    }

    if ((argCount == 2) && (length == 0))
    {
        int32 unread_len = socketInfo[socketID].totalRecvSize - socketInfo[socketID].readLen;
        char rspString[80] = {0};

        memset(rspString, 0, 80);
        snprintf (rspString, 80, "+QIRD: %d,%d,%d",
                socketInfo[socketID].totalRecvSize,
                socketInfo[socketID].readLen,
                unread_len);
        at_CmdRespInfoText(atEngine, (uint8 *)rspString);
        tcpip_atRspOk(atEngine);
    }
    else
    {
        uint32 availSize = 0;
        
        if ((socketInfo[socketID].accessMode == ACCESS_MODE_TRANSPARENT)
            || (socketInfo[socketID].accessMode == ACCESS_MODE_DIRECT))
        {
            tcpip_atRspErr(atEngine, CME_TCPIP_OPERATION_NOT_ALLOWED);
            return;
        }
        tcpip_getRxDataFromBuf(atEngine, socketID, length, argCount);
        
        KING_SocketOptGet(socketInfo[socketID].socketfd, 0, SO_RXDATA, &availSize, 0);
        KING_SysLog("tcpip_qirdCmdSet() availSize=%d", availSize);
        if (availSize > 0)
        {
            tcpip_socketRecvData(socketID, (uint16)availSize);
        }
        tcpip_atRspOk(atEngine);
    }

    return;
}

static char *tcpip_getPortString(AT_CMD_ENGINE_T *atEngine, uint8 nDLCI)
{
    int dlci = nDLCI;
    
    KING_SysLog("tcpip_getPortString dlci = %d!", dlci);
    if (at_CmdGetDispatch(atEngine) == g_dispatch[AT_DISPATCH_ID_USB])
    {
        return "usbat";
    }
    else
    {
        if (dlci == 0)
        {
            return "uart1";
        }
        else if (dlci == 1)
        {
            return "cmux1";
        }
        else if(dlci == 2)
        {
            return "cmux2";
        }
        else if(dlci == 3)
        {
            return "cmux3";
        }
        else if(dlci == 4)
        {
            return "cmux4";
        }
        else if(dlci == 5)
        {
            return "cmux5";
        }
        else if(dlci == 6)
        {
            return "cmux6";
        }
        else if(dlci == 7)
        {
            return "cmux7";
        }
        else if(dlci == 8)
        {
            return "cmux8";
        }
        else if(dlci == 9)
        {
            return "cmux9";
        }
        else if(dlci == 10)
        {
            return "cmux10";
        }
        else
        {
            return "";
        }
    }
}

static void tcpip_QistateByCtxID(AT_CMD_PARA *pParam, uint8 ctxID)
{
    int i = 0, count = 0;
    char buf[150] = {0};
    
    KING_SysLog("tcpip_QistateBySocketID ctxID = %d!", ctxID);
    KING_SysLog("tcpip_QistateBySocketID netState = %d!", tcpipPdpInfo[ctxID - 1].netState);
    if (tcpipPdpInfo[ctxID - 1].netState == TCPIP_STATE_GPRSACT)
    {
        for (i = 0; i < TCPIP_SOCKET_ID_MAX_NUM; i++)
        {
            if ((socketInfo[i].socketfd != TCPIP_SOCKET_ERROR) &&
                (socketInfo[i].state <= SOCKET_MODEM_CLOSING) &&
                (socketInfo[i].ctxID == ctxID))
            {
                memset((char *)buf, 0, 150);
                if (count == 0)
                {
                    at_CmdRespOutputText(pParam->engine, (uint8 *)"\r\n");
                }
                sprintf(buf, "%s: %d,\"%s\",\"%s\",%d,%d,%d,%d,%d,%d,\"%s\"\r\n",
                            "+QISTATE", i,
                            sockTypeString[socketInfo[i].socketType],
                            socketInfo[i].ipAddrStr,
                            socketInfo[i].remotePort,
                            socketInfo[i].localPort,
                            socketInfo[i].state,
                            ctxID,
                            socketInfo[i].serverID,
                            socketInfo[i].accessMode,
                            tcpip_getPortString(socketInfo[i].atEngine, socketInfo[i].nDLCI));
                count++;
                at_CmdRespOutputText(pParam->engine, (uint8 *)buf);
            }
        }
    }
    
    tcpip_atRspOk(pParam->engine);
}

static void tcpip_QistateBySocketID(AT_CMD_PARA *pParam, uint8 socketID)
{
    char buf[150] = {0};
    
    KING_SysLog("tcpip_QistateBySocketID state = %d!", socketInfo[socketID].state);
    KING_SysLog("tcpip_QistateBySocketID socketID = %d!", socketID);
    KING_SysLog("tcpip_QistateBySocketID socketfd = %d!", socketInfo[socketID].socketfd);
    memset((char *)buf, 0, 150);
    if ((socketInfo[socketID].socketfd != TCPIP_SOCKET_ERROR) &&
        (socketInfo[socketID].state <= SOCKET_MODEM_CLOSING) &&
        (socketInfo[socketID].socketID == socketID))
    {
        sprintf(buf, "\r\n%s: %d,\"%s\",\"%s\",%d,%d,%d,%d,%d,%d,\"%s\"\r\n",
                "+QISTATE", socketID,
                sockTypeString[socketInfo[socketID].socketType],
                socketInfo[socketID].ipAddrStr,
                socketInfo[socketID].remotePort,
                socketInfo[socketID].localPort,
                socketInfo[socketID].state,
                socketInfo[socketID].ctxID,
                socketInfo[socketID].serverID,
                socketInfo[socketID].accessMode,
                tcpip_getPortString(socketInfo[socketID].atEngine, socketInfo[socketID].nDLCI));
        at_CmdRespOutputText(pParam->engine, (uint8 *)buf);
    }
  
    tcpip_atRspOk(pParam->engine);
}

static void tcpip_QistateCmdRead(AT_CMD_PARA *pParam)
{
    int i = 0, count = 0;
    char buf[150] = {0};

    for (i = 0; i < TCPIP_SOCKET_ID_MAX_NUM; i++)
    {
        if ((socketInfo[i].socketfd != TCPIP_SOCKET_ERROR) &&
            (socketInfo[i].state <= SOCKET_MODEM_CLOSING) &&
            (socketInfo[i].socketID == i))
        {
            memset((char *)buf, 0, 150);
            if (count == 0)
            {
                at_CmdRespOutputText(pParam->engine, (uint8 *)"\r\n");
            }
            snprintf(buf, 150, "%s: %d,\"%s\",\"%s\",%d,%d,%d,%d,%d,%d,\"%s\"\r\n",
                    "+QISTATE", i,
                    sockTypeString[socketInfo[i].socketType],
                    socketInfo[i].ipAddrStr,
                    socketInfo[i].remotePort,
                    socketInfo[i].localPort,
                    socketInfo[i].state,
                    socketInfo[i].ctxID,
                    socketInfo[i].serverID,
                    socketInfo[i].accessMode,
                    tcpip_getPortString(socketInfo[i].atEngine, socketInfo[i].nDLCI));
            count++;
            at_CmdRespOutputText(pParam->engine, (uint8 *)buf);
        }
    }
    
    tcpip_atRspOk(pParam->engine);
}

static void tcpip_dnsReqCb(void* pData, uint32 len)
{
    ASYN_DNS_PARSE_T *dnsParse = (ASYN_DNS_PARSE_T *)pData;
    
    KING_SysLog("tcpip_dnsReqCb hostname=%s\r\n", dnsParse->hostname);
    KING_SysLog("tcpip_dnsReqCb dnsParse->result=%d\n", dnsParse->result);
    if (dnsParse->result == DNS_PARSE_SUCCESS)
    {
        int ret = 0;
        SOCK_IN_ADDR_T ipaddr = dnsParse->addr;
        char *AddrStr = NULL;
        char buf[100] = {0};
        
        KING_SysLog("tcpip_socketDnsReqCb curCid=%d\r\n", curCid);
        ret = KING_ipntoa(ipaddr, &AddrStr);
        if (ret == -1)
        {
            tcpip_ErrRsp(tcpipPdpInfo[curCid - 1].atEngine, "+QIURC: \"dnsgip\",", CME_TCPIP_DNS_PARSE_FAILED);
            return;
        }
        memset(buf, 0, 100);
        snprintf(buf, 100, "\r\n+QIURC: \"dnsgip\",%d,%d\r\n", gTcpipCurErr, 1);
        at_CmdRespOutputText( tcpipPdpInfo[curCid - 1].atEngine, (uint8 *)buf);
        memset(buf, 0, 100);
        snprintf(buf, 100, "\r\n+QIURC: \"dnsgip\",\"%s\"\r\n", AddrStr);
        at_CmdRespOutputText( tcpipPdpInfo[curCid - 1].atEngine, (uint8 *)buf);
    }
    else
    {
        tcpip_ErrRsp(tcpipPdpInfo[curCid - 1].atEngine, "+QIURC: \"dnsgip\",", CME_TCPIP_DNS_PARSE_FAILED);
    }
}

static void tcpip_qidnsgipGetIpAddr(uint8 ctxID)
{
    int ret = -1;
    SOCK_IN_ADDR_T destAddr;
    
    KING_SysLog("tcpip_qidnsgipGetIpAddr() start");
    KING_SysLog("tcpip_qidnsgipGetIpAddr() ctxID=%d", ctxID);
    if (tcpipPdpInfo[ctxID - 1].hContext == -1)
    {
        tcpip_ErrRsp(tcpipPdpInfo[ctxID - 1].atEngine, "+QIURC: \"dnsgip\",", CME_TCPIP_DNS_PARSE_FAILED);
        return;
    }
    curCid = ctxID;
    memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
    ret = KING_NetGetHostByName(tcpipPdpInfo[ctxID - 1].hContext, 
                            qidnsgipHostname, &destAddr, 
                            10000, (DNS_CALLBACK)tcpip_dnsReqCb);
    KING_SysLog("tcpip_qidnsgipGetIpAddr() ret=%d", ret);
    if (ret == 0)
    {
        if (((destAddr.type == IP_ADDR_TYPE_V6) && (0 == destAddr.u_addr.ip6.addr[0]) 
            && (0 == destAddr.u_addr.ip6.addr[1]) && (0 == destAddr.u_addr.ip6.addr[2])
            && (0 == destAddr.u_addr.ip6.addr[3])) || ((destAddr.type == IP_ADDR_TYPE_V4) 
            && (0 == destAddr.u_addr.ip4.addr)) )
        {
            KING_SysLog("tcpip_qidnsgipGetIpAddr ipv6 query QUEUED\r\n");
        }
        else
        {
            KING_SysLog("tcpip_qidnsgipGetIpAddr ipv4 query QUEUED\r\n");
        }
    }
    else
    {
        KING_SysLog("tcpip_qidnsgipGetIpAddr KING_NetGetHostByName error\r\n");
    }
}

static void tcpip_qidnsgipCmdSet(AT_CMD_ENGINE_T *atEngine, uint8 ctxID, char *pHostNamePtr)
{
    int ret = 0;
    
    if (tcpip_ActivedMax() >= TCPIP_ALLOWED_ACTIVATED_MAX)
    {
        at_CmdRespOK(atEngine);
        tcpip_ErrRsp(atEngine, "+QIURC: \"dnsgip\",", CME_TCPIP_OPERATION_NOT_ALLOWED);
        return;
    }
    memset(qidnsgipHostname, 0x00, TCPIP_NETWORK_ADDR_MAX_LEN);
    memcpy(qidnsgipHostname, pHostNamePtr, strlen(pHostNamePtr));
    ret = tcpip_activeByCtxID(atEngine, ctxID, PDP_ACTIVE_SOCK_QIDNSGIP_ACTIVE);
    if (ret == -1)
    {
        at_CmdRespOK(atEngine);
        tcpip_ErrRsp(atEngine, "+QIURC: \"dnsgip\",", CME_TCPIP_OPEN_PDP_CONTEXT_FAILED);
        return;
    }
    else if (ret == 1)
    {
        at_CmdRespOK(atEngine);
        tcpip_MsgSend(TCPIP_DNS_PARSE_REQ, ctxID, PDP_ACTIVE_SOCK_QIDNSGIP_ACTIVE, CMD_QIDNSGIP, 0);
    }
}

static void tcpip_socketThread(void *argv)
{
    MSG_S msg;
    int ret = -1;
    
    KING_SysLog("tcpip_socketThread start!\r\n");
    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        ret = KING_MsgRcv(&msg, tcpipMsgHandle, WAIT_OPT_INFINITE);
        if (ret == -1)
        {
            KING_SysLog("tcpip_socketThread ret=%d\r\n", ret);
            continue;
        }
        
        KING_SysLog("tcpip_socketThread msg.messageID=%d\r\n", msg.messageID);
        switch(msg.messageID)
        {
            case TCPIP_DNS_PARSE_REQ:
            {
                TCPIP_SEND_MSG_S sendMsg = {0};

                memcpy((void*)&sendMsg, (TCPIP_SEND_MSG_S*)msg.pData, msg.DataLen);
                free(msg.pData);
                msg.pData = NULL;
                gTcpipCurErr = TCPIP_OPERATE_SUCC;
                if (sendMsg.atCmd == CMD_QIOPEN)
                {
                    tcpip_GetIPAddrByHostname(sendMsg.socketID, sendMsg.ctxID);
                }
                else if (sendMsg.atCmd == CMD_QIDNSGIP)
                {
                    tcpip_qidnsgipGetIpAddr(sendMsg.ctxID);
                }
            }
                break;

            case TCPIP_RECV_DATA:
            {
                TCPIP_SEND_MSG_S sendMsg = {0};

                memcpy((void*)&sendMsg, (TCPIP_SEND_MSG_S*)msg.pData, msg.DataLen);
                free(msg.pData);
                msg.pData = NULL;
                tcpip_socketRecvData(sendMsg.socketID, sendMsg.len);
            }
                break;
                
            default:
                break;
        }
    }
    
    tcpipThreadID = NULL;
    KING_ThreadExit();
}

static int tcpip_CreatThreadAndMsg(void)
{
    int ret = SUCCESS;
    THREAD_ATTR_S tcpipThreadAttr;

    ret = KING_MsgCreate(&tcpipMsgHandle);
    if(-1 == ret)
    {
        KING_SysLog("tcpip_CreatThreadAndMsg KING_MsgCreate fail!");
        tcpipMsgHandle = NULL;
        return -1;
    }
    if (tcpipMsgHandle == NULL)
    {
        KING_SysLog("tcpip_CreatThreadAndMsg msgHandle is NULL");
        return -1;
    }
    memset(&tcpipThreadAttr, 0x00, sizeof(THREAD_ATTR_S));
    tcpipThreadAttr.fun = tcpip_socketThread;
    tcpipThreadAttr.priority = THREAD_PRIORIT1;
    tcpipThreadAttr.stackSize = 1024 * 4;
    tcpipThreadAttr.queueNum = 64; 
    
    ret = KING_ThreadCreate("TcpipThread", &tcpipThreadAttr, &tcpipThreadID);
    if (-1 == ret)
    {
        KING_SysLog("tcpip_CreatThreadAndMsg Create Thread Failed!!");
        tcpipThreadID = NULL;
        return -1;
    }
    if (tcpipThreadID == NULL)
    {
        KING_SysLog("tcpip_CreatThreadAndMsg tcpipThreadID is NULL");
        return -1;
    }

    return 0;
}

/*******************************************************************************
 ** Functions
 ******************************************************************************/
void tcpip_init(void)
{
    tcpip_CreatThreadAndMsg();
    tcpip_SetDefaultConfig();
    tcpip_QicsgpInit();
    tcpip_PdpInfoInit();
    tcpip_SocketInfoInit();
    tcpip_SocketMutexInit();
    tcpip_getNicHandle();
    KING_RegNotifyFun(DEV_CLASS_NW, 0, tcpip_NetCallBack);
}

int tcpip_IsPdpActiveAllowed(void)
{
    if (tcpip_ActivedMax() >= TCPIP_ALLOWED_ACTIVATED_MAX)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int tcpip_IsPdpActived(uint8 ctxID)
{
    uint8 i = 0, ret = 0;

    for (i = 0; i < TCPIP_CONTEXT_ID_MAX_NUM; i++)
    {
        if(tcpipPdpInfo[i].ctxID == ctxID)
        {
            if(tcpipPdpInfo[i].netState == TCPIP_STATE_GPRSACT)
            {
                ret = 1;
                break;
            }
        }
    }

    return ret;
}

int tcpip_activeByCtxID(AT_CMD_ENGINE_T *atEngine, uint8 ctxID, uint16 actType)
{
    uint32 status = 0;
    int ret = 0;
    
    //if (strlen(qicsgpInfo[ctxID - 1].apn) == 0)
    //{
        //gTcpipCurErr = CME_TCPIP_APN_NOT_CONFIGURED;
        //return -1;
    //}
    tcpipPdpInfo[ctxID - 1].atEngine = atEngine;
    ret = KING_NetPdpStatusGet((uint32)ctxID, &status);
    if (ret == -1)
    {
        return -1;
    }
    if (status == 1)
    {
        ret = tcpip_qiactActSet(ctxID);
        if (ret == -1)
        {
            return -1;
        }
        return 1;
    }
    else
    {
        activeType = actType;
        ret = KING_NetPdpActive(ctxID);
        if (ret == -1)
        {
            return -1;
        }
        tcpipAction = NET_ACTION_ACTIVE;
        AT_SetAsyncTimerMux(atEngine, TCPIP_PDP_ACTIVE_MAX_TIMEOUT);
    }
    return ret;
}

int32 tcpip_getSwHandleByCID(uint8 ctxID)
{
    uint8 i = 0;

    for (i = 0; i < TCPIP_CONTEXT_ID_MAX_NUM; i++)
    {
        if(tcpipPdpInfo[i].ctxID == ctxID)
        {
            if(tcpipPdpInfo[i].netState == TCPIP_STATE_GPRSACT 
                && tcpipPdpInfo[i].hContext != -1 )
            {
                return tcpipPdpInfo[i].hContext;
            }
        }
    }

    return -1;
}

bool tcpip_ttConnected(void)
{
    uint8 socketID = tcpip_ttGetModeSocketID();
    
    KING_SysLog("%s: socketID=%d", __FUNCTION__, socketID);
    if (socketID == 0xff)
    {
        return FALSE;
    }
    return TRUE;
}

void tcpip_ttEscapeDataMode(uint8 nDLCI)
{
    uint8 socketID = tcpip_ttGetModeSocketID();
    
    KING_SysLog("%s: socketID=%d", __FUNCTION__, socketID);
    
    tcpip_ttTimer(FALSE);
    socketInfo[socketID].state = SOCKET_MODEM_CONNECTED;
    socketInfo[socketID].ttDataMode = FALSE;
    tcpip_ttBufDestroy(gTTBypassBuf);
    gTTBypassBuf = NULL;

    return;
}

void tcpip_ttResumeDataMode(uint8 nDLCI)
{
    uint8 socketID = tcpip_ttGetLastIdx();
    
    KING_SysLog("%s: socketID=%d", __FUNCTION__, socketID);

    tcpip_ttTimer(FALSE);
    socketInfo[socketID].state = SOCKET_MODEM_SEND;
    socketInfo[socketID].ttDataMode = TRUE;
    socketInfo[socketID].isLastTT = TRUE;

    tcpip_ttTimer(TRUE);
    return;
}

void AT_CmdFunc_QICSGP(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[TCPIP_MAX_STRING_RSP] = {0};

            memset(strRsp, 0x00, TCPIP_MAX_STRING_RSP);
            sprintf(strRsp, "+QICSGP: (1-4),(1,2,3),<APN>,<username>,<password>,(0-2)");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            tcpip_atRspOk(pParam->engine);
        }
            break;

        case AT_CMD_SET:
        {
            int ret = SUCCESS;
            bool paramok = true;    
            uint8 idx = 0, paramCount = pParam->paramCount;
            const uint8_t *apn = NULL;
            const uint8_t *userName = NULL;
            const uint8_t *pwd = NULL;
            uint32 ipTpye = 0, cid = 0, pdpType = 0, authType = 0;
            
            if (paramCount < 1 || paramCount > 6 || paramCount == 2)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                break;
            }

            cid = at_ParamUintInRange(pParam->params[idx], 1, 4, &paramok);
            if (!paramok)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }

            if (paramCount == 1)
            {
                tcpip_qicsgpReadRsp(pParam, cid);
                return;
            }
            
            idx++; 
            pdpType = at_ParamUintInRange(pParam->params[idx], 1, 3, &paramok);
            if (!paramok)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }

            idx++;
            apn = at_ParamOptStr(pParam->params[idx], &paramok);
            if (!paramok)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }
            if (strlen((char *)apn) > TCPIP_MAX_STRING_LENGTH)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }
            if (tcpip_isSetApn((char *)apn, (uint8)cid))
            {
                KING_SysLog("tcpip_isSetApn() is error");
                tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_SUPPORTED);
                return;
            }
            if (paramCount > 3)
            {
                idx++;
                userName = at_ParamOptStr(pParam->params[idx], &paramok);
                if (!paramok)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                    return;
                }
                if (strlen((char *)userName) > TCPIP_MAX_STRING_LENGTH)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                    return;
                }
            }
            if (paramCount > 4)
            {
                idx++;
                pwd = at_ParamOptStr(pParam->params[idx], &paramok);
                if (!paramok)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                    return;
                }
                if (strlen((char *)pwd) > TCPIP_MAX_STRING_LENGTH)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                    return;
                }
            }
            if (paramCount > 5)
            {
                idx++; 
                authType = at_ParamUintInRange(pParam->params[idx], 0, 2, &paramok);
                if (!paramok)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                    return;
                }
            }
            if (pdpType == TCPIP_PDP_TYPE_IP)
            {
                ipTpye = IP_ADDR_TYPE_V4;
            }
            else if (pdpType == TCPIP_PDP_TYPE_IPV6)
            {
                ipTpye = IP_ADDR_TYPE_V6;
            }
            else if (pdpType == TCPIP_PDP_TYPE_IPV4V6)
            {
                ipTpye = IP_ADDR_TYPE_ANY;
            }
            else
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }
            ret = KING_NetApnSet(cid, (char *)apn, (char *)userName, (char *)pwd, authType, ipTpye);
            if (ret != 0)
            {
                KING_SysLog("KING_NetApnSet() is error");
                tcpip_atRspErr(pParam->engine, ERR_AT_CME_EXE_FAIL);
                return;
            }
            tcpip_qicsgpSet((uint8)cid, (uint8)pdpType, (char *)apn, (char *)userName, (char *)pwd, (uint8)authType);
            
            tcpip_atRspOk(pParam->engine);
        }
            break;

        default:
            tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_ALLOWED);
            break;
    }

    return;
}

void AT_CmdFunc_QIACT(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[30] = {0};

            memset(strRsp, 0x00, 30);
            sprintf(strRsp, "+QIACT: (1-4)");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            tcpip_atRspOk(pParam->engine);
        }
            break;

        case AT_CMD_READ:
            tcpip_qiactReadRsp(pParam);
            break;

        case AT_CMD_SET:
        {
            int ret = SUCCESS;
            bool paramok = true;
            uint32 cid = 0;

            if (pParam->paramCount != 1)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                break;
            }
            
            cid = at_ParamUintInRange(pParam->params[0], 1, 4, &paramok);
            if (!paramok)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }

            if (tcpip_ActivedMax() >= TCPIP_ALLOWED_ACTIVATED_MAX)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_ALLOWED);
                return;
            }
            if (tcpipPdpInfo[cid - 1].netState != TCPIP_STATE_GPRSACT)
            {
                ret = tcpip_activeByCtxID(pParam->engine, cid, PDP_ACTIVE_SOCK_JUST_ACTIVE);
                if (ret == -1)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_OPEN_PDP_CONTEXT_FAILED);
                    return;
                }
                else if (ret == 1)
                {
                    tcpip_atRspOk(pParam->engine);
                }
            }
            else
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_UNKNOWN_ERROR);
            }
        }
            break;

        default:
            tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_ALLOWED);
            break;
    }

    return;
}

void AT_CmdFunc_QIDEACT(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[30] = {0};

            memset(strRsp, 0x00, 30);
            sprintf(strRsp, "+QIDEACT: (1-4)");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            tcpip_atRspOk(pParam->engine);
        }
            break;
            
        case AT_CMD_SET:
        {
            int ret = SUCCESS;
            bool paramok = true;
            uint32 cid = 0;
        
            if (pParam->paramCount != 1)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                break;
            }
            
            cid = at_ParamUintInRange(pParam->params[0], 1, 4, &paramok);
            if (!paramok)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }

            tcpipPdpInfo[cid - 1].atEngine = pParam->engine;
            if (tcpipPdpInfo[cid - 1].netState == TCPIP_STATE_GPRSACT)
            {
                uint32 status = 0;
                
                if (tcpipPdpInfo[cid - 1].ctxID != cid)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_ALLOWED);
                    return;
                }
                ret = KING_NetPdpStatusGet(cid, &status);
                if (ret == -1)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_UNKNOWN_ERROR);
                    return;
                }
                if (status == 1)
                {
                    ret = tcpip_CloseSocketByContext((uint8)cid);
                    if (ret == -1)
                    {
                        tcpip_atRspErr(pParam->engine, CME_TCPIP_CLOSE_PDP_CONTEXT_FAILED);
                        return;
                    }
                    ret = KING_NetPdpDeactive(cid);
                    if (ret == -1)
                    {
                        tcpip_atRspErr(pParam->engine, CME_TCPIP_CLOSE_PDP_CONTEXT_FAILED);
                        return;
                    }
                    tcpipAction = NET_ACTION_DEACTIVE;
                    AT_SetAsyncTimerMux(pParam->engine, TCPIP_PDP_DEACTIVE_MAX_TIMEOUT);
                }
                else
                {
                    tcpip_QideactResetPdpInfo(cid);
                    tcpip_atRspOk(pParam->engine);
                }
            }
            else
            {
                tcpip_atRspOk(pParam->engine);
            }
        }
            break;
        
        default:
            tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_ALLOWED);
            break;
    }
}

void AT_CmdFunc_QIOPEN(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[TCPIP_MAX_STRING_RSP] = {0};

            memset(strRsp, 0x00, TCPIP_MAX_STRING_RSP);
            sprintf(strRsp, "+QIOPEN: (1-4),(0-4),\"TCP/UDP/TCP LISTENER/UDP SERVICE\",\"<IP_address>/<domain_name>\",<remote_port>,<local_port>,(0-2)");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            tcpip_atRspOk(pParam->engine);
        }
            break;

        case AT_CMD_READ:
            tcpip_atRspOk(pParam->engine);
            break;

        case AT_CMD_SET:
        {
            bool paramok = true;
            TCPIP_SOCKET_TYPE_E socketType = TCPIP_SOCK_TYPE_NUM;
            uint8 cid = 0, socketID = 0xff, accessMode = 0;
            uint32 remotePort = 0, localPort = 0;
            const uint8_t *argStr = NULL;
            const uint8_t *domainName = NULL;
            
            if (pParam->paramCount < 4 || pParam->paramCount > 7)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                break;
            }
            
            cid = at_ParamUintInRange(pParam->params[0], 1, 4, &paramok);
            if (!paramok)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }
            
            socketID = at_ParamUintInRange(pParam->params[1], 0, 4, &paramok);
            if (!paramok)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }
            
            argStr = at_ParamOptStr(pParam->params[2], &paramok);
            if (!paramok || strlen((char *)argStr) > 20 || strlen((char *)argStr) == 0)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }
            socketType = tcpip_QiopenServiceType((char *)argStr);
            if (socketType == TCPIP_SOCK_TYPE_NUM)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }
            
            domainName = at_ParamOptStr(pParam->params[3], &paramok);
            if (!paramok || strlen((char *)domainName) > TCPIP_NETWORK_ADDR_MAX_LEN || strlen((char *)domainName) == 0)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }
            
            remotePort = at_ParamUintInRange(pParam->params[4], 0, 65535, &paramok);
            if (!paramok)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }

            if (pParam->paramCount >= 6)
            {
                localPort = at_ParamUintInRange(pParam->params[5], 0, 65535, &paramok);
                if (!paramok)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                    return;
                }
            }
            else
            {
                localPort = 0;
                accessMode = 1;
            }
            if (pParam->paramCount >= 7)
            {
                accessMode = at_ParamUintInRange(pParam->params[6], 0, 2, &paramok);
                if (!paramok)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                    return;
                }
            }
            else
            {
                accessMode = 1;
            }

            tcpip_qiopenCmdSet(pParam->engine, pParam->nDLCI, cid, socketID, socketType, 
                        (char *)domainName, remotePort, localPort, accessMode);
        }
            break;

        default:
            tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_ALLOWED);
            break;
    }

    return;
}

void AT_CmdFunc_QICLOSE(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[30] = {0};

            memset(strRsp, 0x00, 30);
            sprintf(strRsp, "+QICLOSE: (0-4),(0-65535)");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            tcpip_atRspOk(pParam->engine);
        }
            break;

        case AT_CMD_EXE:
            tcpip_atRspOk(pParam->engine);
            break;
            
        case AT_CMD_SET:
        {
            bool paramok = true;
            uint8 socketID = 0xff;
            uint32 timeout = 0;

            if (pParam->paramCount < 1 || pParam->paramCount > 2)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                break;
            }
            
            socketID = at_ParamUintInRange(pParam->params[0], 0, 4, &paramok);
            if (!paramok)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }

            if (pParam->paramCount == 2)
            {
                timeout = at_ParamUintInRange(pParam->params[1], 0, 65535, &paramok);
                if (!paramok)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                    return;
                }
            }
            else
            {
                timeout = 10;
            }
            tcpip_qicloseCmdSet(pParam->engine, socketID, timeout);
        }
            break;
            
        default:
            tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_ALLOWED);
            break;

    }
}

void AT_CmdFunc_QISEND(AT_CMD_PARA *pParam)
{
    uint16 uLength = 0, sendLen = 0;
    
    KING_SysLog("AT_CmdFunc_QISEND: pParam->paramCount=%d", pParam->paramCount);
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[30] = {0};

            memset(strRsp, 0x00, 30);
            sprintf(strRsp, "+QISEND: (0-4),(0-1460)");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            tcpip_atRspOk(pParam->engine);
        }
            break;
            
        case AT_CMD_SET:
        {
            bool paramok = true;
            uint8 socketID = 0xff;
            uint32 remotePort = 0;
            const uint8_t *ipAddr = NULL;
            
            KING_SysLog("AT_CmdFunc_QISEND pParam->iExDataLen = %u!", pParam->iExDataLen);
            KING_SysLog("AT_CmdFunc_QISEND pParam->paramCount = %d!", pParam->paramCount);
            if (pParam->paramCount < 1 || pParam->paramCount > 4)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                break;
            }
            
            socketID = at_ParamUintInRange(pParam->params[0], 0, 4, &paramok);
            if (!paramok)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }

            if (pParam->paramCount >= 2)
            {
                uLength = at_ParamUintInRange(pParam->params[1], 0, TCPIP_TX_BUF_MAX_LEN, &paramok);
                if (!paramok)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                    return;
                }
            }
            if ((uLength == 0 && pParam->paramCount > 2) || pParam->paramCount == 3)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }
            if (pParam->paramCount > 2)
            {
                ipAddr = at_ParamOptStr(pParam->params[2], &paramok);
                if (!paramok)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                    return;
                }
                if (strlen((char *)ipAddr) == 0 ||  strlen((char *)ipAddr) > TCPIP_NETWORK_ADDR_MAX_LEN)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                    return;
                }
                
                remotePort = at_ParamUintInRange(pParam->params[3], 0, 65535, &paramok);
                if (!paramok)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                    return;
                }
            }
            
            if (!pParam->iExDataLen)
            {
                if ((tcpip_IsEnterDataMode())
                    || (socketInfo[socketID].socketID != socketID)
                    || (socketInfo[socketID].socketfd == TCPIP_SOCKET_ERROR)
                    || (socketInfo[socketID].unsendLen > 0))
                {
                    KING_SysLog("AT_CmdFunc_QISEND is enter data mode!");
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_ALLOWED);
                    return;
                }
            }

            if (pParam->paramCount == 2 && uLength == 0)
            {
                tcpip_QisendQuery(pParam->engine, socketID);
                return;
            }
            if (!pParam->iExDataLen)
            {
                if ((socketInfo[socketID].accessMode == ACCESS_MODE_TRANSPARENT)
                    || (socketInfo[socketID].state != SOCKET_MODEM_CONNECTED))
                {
                    KING_SysLog("AT_CmdFunc_QISEND is transparent!");
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_ALLOWED);
                    return;
                }
            }
            if (pParam->paramCount == 1)
            {
                //uLength = TCPIP_TX_BUF_MAX_LEN;
            }
            socketInfo[socketID].state = SOCKET_MODEM_SEND;
            socketInfo[socketID].inputData = TRUE;
            if (tcpipConfig.sendDataFormat == 1)
            {
                tcpipSendInfo.length = (uLength<<1);
            }
            else
            {
                tcpipSendInfo.length = uLength;
            }
            if (pParam->paramCount == 1 || pParam->paramCount == 2)
            {
                KING_SysLog("AT_CmdFunc_QISEND: pParam->iExDataLen=%u, length=%d",
                                  pParam->iExDataLen, tcpipSendInfo.length);
                if (!pParam->iExDataLen)
                {
                    at_CmdRespOutputPrompt(pParam->engine);
                    if (tcpipSendInfo.length > 0)
                    {
                        at_CmdSetRawMode(pParam->engine, tcpipSendInfo.length);
                    }
                    else
                    {
                        at_CmdSetPromptMode(pParam->engine);
                    }
                    return;
                }
                else
                {
                    KING_SysLog("tcpip_QisendEnterInputDataMode: 11 pParam->iExDataLen=%d, length=%d",
                                      pParam->iExDataLen, tcpipSendInfo.length);
                    if ((tcpipSendInfo.length == 0) && pParam->pExData[pParam->iExDataLen - 1] == 27)
                    {
                        KING_SysLog("tcpip_QisendEnterInputDataMode: end with esc, cancel send");
                        tcpip_QisendCmdErrReset(socketID);
                        tcpip_atRspOk(pParam->engine);
                        return;
                    }
                    else if ((tcpipSendInfo.length == 0) && (pParam->pExData[pParam->iExDataLen - 1] != 26 || pParam->iExDataLen <= 1))
                    {
                        KING_SysLog("tcpip_QisendEnterInputDataMode: not end with ctl+z, err happen");
                        gTcpipCurErr = ERR_AT_CMS_INVALID_LEN;
                        tcpip_QisendCmdErrReset(socketID);
                        AT_CMD_RETURN(at_CmdRespError(pParam->engine));
                    }
                    if ((tcpipSendInfo.length != 0) && (pParam->paramCount == 2) && (tcpipSendInfo.length != pParam->iExDataLen))
                    {
                        KING_SysLog("tcpip_QisendEnterInputDataMode: 22 pParam->iExDataLen=%d, length=%d",
                                          pParam->iExDataLen, tcpipSendInfo.length);
                        gTcpipCurErr = ERR_AT_CMS_INVALID_LEN;
                        tcpip_QisendCmdErrReset(socketID);
                        AT_CMD_RETURN(at_CmdRespError(pParam->engine));
                    }
                    sendLen = (uint16)pParam->iExDataLen - (uLength == 0);
                    if(sendLen > TCPIP_TX_BUF_MAX_LEN)
                    {
                        sendLen = TCPIP_TX_BUF_MAX_LEN;
                    }
                    tcpip_QisendCmdSendMsg(pParam->engine, socketInfo[socketID].socketID, (char *)pParam->pExData, (int)sendLen);
                    
                }
                //tcpip_QisendEnterInputDataMode(pParam, socketID, length, pParam->paramCount);
            }
            else if ((pParam->paramCount == 4) && (uLength > 0) && (socketInfo[socketID].socketType == TCPIP_SOCK_TYPE_UDP_SERVER))
            {
                memset(&tcpipSendInfo, 0x00, sizeof(TCPIP_SEND_INFO_S));
                tcpipSendInfo.argCount = pParam->paramCount;
                tcpipSendInfo.socketID = socketID;
                
                tcpipSendInfo.remotePort = remotePort;
                memset(tcpipSendInfo.remoteAddr, 0, TCPIP_NETWORK_ADDR_MAX_LEN);
                strcpy(tcpipSendInfo.remoteAddr, (char *)ipAddr);
                tcpip_QisendEnterInputDataMode(pParam, socketID, uLength, pParam->paramCount);
            }
            else
            {
                tcpip_QisendCmdErrReset(socketID);
                tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_ALLOWED);
            }
        }
            break;
            
        default:
            tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_ALLOWED);
            break;

    }

    return;
}

void AT_CmdFunc_QISENDEX(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[30] = {0};

            memset(strRsp, 0x00, 30);
            sprintf(strRsp, "+QISENDEX: (0-4),<hex_string>");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            tcpip_atRspOk(pParam->engine);
        }
            break;
            
        case AT_CMD_EXE:
            tcpip_atRspOk(pParam->engine);
            break;
            
        case AT_CMD_SET:
        {
            bool paramok = true;
            uint8 socketID = 0xff;
            const uint8_t *hexStr = NULL;
            
            if (pParam->paramCount != 2)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                break;
            }
            
            socketID = at_ParamUintInRange(pParam->params[0], 0, 4, &paramok);
            if (!paramok)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }
                
            hexStr = at_ParamOptStr(pParam->params[1], &paramok);
            if (!paramok)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }
            if (strlen((char *)hexStr) == 0 ||  strlen((char *)hexStr) > TCPIP_TX_HEX_BUF_MAX_LEN)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }
            tcpip_QisendexSendMsg(pParam->engine, socketID, (char *)hexStr, strlen((char *)hexStr));
        }
            break;
            
        default:
            tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_ALLOWED);
            break;

    }

    return;
}

void AT_CmdFunc_QIRD(AT_CMD_PARA *pParam)
{
    if (pParam == NULL)
    {
        return;
    }
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[30] = {0};

            memset(strRsp, 0x00, 30);
            sprintf(strRsp, "+QIRD: (0-4),(0-1500)");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            tcpip_atRspOk(pParam->engine);
        }
            break;
            
        case AT_CMD_EXE:
        case AT_CMD_READ:
            tcpip_atRspOk(pParam->engine);
            break;
            
        case AT_CMD_SET:
        {
            bool paramok = true;
            uint8 socketID = 0xff, argCount = 0;
            uint16 length = 0;
            
            if (pParam->paramCount < 1 || pParam->paramCount > 2)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                break;
            }
            
            socketID = at_ParamUintInRange(pParam->params[0], 0, 4, &paramok);
            if (!paramok)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }
            argCount++;

            if(pParam->paramCount == 2) 
            {
                length = at_ParamUintInRange(pParam->params[1], 0, 1500, &paramok);
                if (!paramok)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                    return;
                }
                argCount++;
            }

            tcpip_qirdCmdSet(pParam->engine, socketID, length, argCount);
        }
            break;
            
        default:
            tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_ALLOWED);
            break;

    }

    return;
}

void AT_CmdFunc_QISTATE(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            tcpip_atRspOk(pParam->engine);
            break;
            
        case AT_CMD_READ:
            tcpip_QistateCmdRead(pParam);
            break;
            
        case AT_CMD_SET:
        {
            bool paramok = true;
            uint8 query = 0xff, param = 0;
            
            if (pParam->paramCount != 2)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                break;
            }
            
            query = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
            if (!paramok)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }

            if (query == 0)
            {
                param = at_ParamUintInRange(pParam->params[1], 1, 4, &paramok);
                if (!paramok)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                    return;
                }
            }
            else if (query == 1)
            {
                param = at_ParamUintInRange(pParam->params[1], 0, 4, &paramok);
                if (!paramok)
                {
                    tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                    return;
                }
            }
            else
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }
            KING_SysLog("AT_CmdFunc_QISTATE query = %d!", query);
            KING_SysLog("AT_CmdFunc_QISTATE param = %d!", param);
            if (query == 1)
            {
                tcpip_QistateBySocketID(pParam, param);
            }
            else if (query == 0)
            {
                tcpip_QistateByCtxID(pParam, param);
            }
            else
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }
        }
            break;
            
        default:
            tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_ALLOWED);
            break;

    }

    return;
}

void AT_CmdFunc_QIDNSGIP(AT_CMD_PARA *pParam)
{
    if (pParam == NULL)
    {
        return;
    }
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[30] = {0};
        
            memset(strRsp, 0x00, 30);
            sprintf(strRsp, "+QIDNSGIP: (1-4),<hostname>");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            tcpip_atRspOk(pParam->engine);
        }
            break;
            
        case AT_CMD_SET:
        {
            bool paramok = true;
            uint32 cid = 0;
            const uint8_t *domainName = NULL;
            
            if (pParam->paramCount != 2)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                break;
            }
            
            cid = at_ParamUintInRange(pParam->params[0], 1, 4, &paramok);
            if (!paramok)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }

            domainName = at_ParamOptStr(pParam->params[1], &paramok);
            if (!paramok || strlen((char *)domainName) > TCPIP_NETWORK_ADDR_MAX_LEN || strlen((char *)domainName) == 0)
            {
                tcpip_atRspErr(pParam->engine, CME_TCPIP_INVALID_PARAMETERS);
                return;
            }
            
            tcpipPdpInfo[cid - 1].atEngine = pParam->engine;
            tcpip_qidnsgipCmdSet(pParam->engine, cid, (char *)domainName);
        }
            break;
            
        default:
            tcpip_atRspErr(pParam->engine, CME_TCPIP_OPERATION_NOT_ALLOWED);
            break;

    }

    return;    
}

