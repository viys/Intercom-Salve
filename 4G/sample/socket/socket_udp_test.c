#include "KingDef.h"
#include "KingPlat.h"
#include "KingRtos.h"
#include "KingSocket.h"
#include "KingCSTD.h"
#include "KingCbData.h"

/*******************************************************************************
 ** Variables
 ******************************************************************************/
enum Msg_Evt
{
    MSG_EVT_BASE = 0,
    MSG_EVT_SOCKET_READ,
    MSG_EVT_SOCKET_SENDTO,
    MSG_EVT_END,
};

#define LogPrintf(fmt, args...)    do { KING_SysLog("socket: "fmt, ##args); } while(0)
#define CHEERZING_TEST_SERVER       "110.80.1.51"
#define CHEERZING_TEST_PORT_UDP     12003

static MSG_HANDLE msgHandle = NULL; 
static THREAD_HANDLE testThreadH = NULL;
static int s_socketFd = 0;
static hardwareHandle hSocketNIC = -1;
static softwareHandle hSocketContext = -1;

static int Socket_Recvfrom(void)
{
    int ret = -1;
    uint8 buf[256] = {0};
    union sockaddr_aligned sock_addr;
    SOCK_IN_ADDR_T ipAddr;
    uint32 addr_size = 0;
    LogPrintf("Socket_Recvfrom");
    if (s_socketFd < 0)
    {
        LogPrintf("no s_socketFd");
        return -1;
    }

    memset(buf, 0x00, 256);
    memset(&ipAddr, 0, sizeof(SOCK_IN_ADDR_T));
    KING_ipaton("127.0.0.1",&ipAddr);    

    struct sockaddr_in *sock_addr4 = (struct sockaddr_in *)&(sock_addr);
    sock_addr4->sin_len = sizeof(struct sockaddr_in);
    sock_addr4->sin_family = AF_INET;
    sock_addr4->sin_port = KING_htons(6666);
    sock_addr4->sin_addr.addr = ipAddr.u_addr.ip4.addr;
    
    ret = KING_SocketRecvfrom(s_socketFd, (void *)buf, 256, 0, &sock_addr, &addr_size);
    if (-1 == ret)
    {
        LogPrintf("KING_SocketRecvfrom error:%x", KING_GetLastErrCode());
        KING_SocketClose(s_socketFd);
        s_socketFd = -1;
        return -1;
    }

    LogPrintf("Socket_Recvfrom succees len:%d data:%s",ret, buf);
    return 0;
}

static int Socket_Sendto(uint8 *buf, size_t buf_len)
{   
    int ret = -1;
    union sockaddr_aligned sock_addr;
    SOCK_IN_ADDR_T ipAddr;

    LogPrintf("Socket_Sendto");
    if (s_socketFd < 0)
    {
        LogPrintf("no s_socketFd");
        return -1;
    }
    memset(&ipAddr, 0, sizeof(SOCK_IN_ADDR_T));
    KING_ipaton(CHEERZING_TEST_SERVER,&ipAddr);    
    
    struct sockaddr_in *sock_addr4 = (struct sockaddr_in *)&(sock_addr);
    sock_addr4->sin_len = sizeof(struct sockaddr_in);
    sock_addr4->sin_family = AF_INET;
    sock_addr4->sin_port = KING_htons(CHEERZING_TEST_PORT_UDP);
    sock_addr4->sin_addr.addr = ipAddr.u_addr.ip4.addr;

    ret = KING_SocketSendto(s_socketFd, (void *)buf, buf_len, 0, &sock_addr, sizeof(SOCK_ADDR_IN_T));
    if (-1 == ret)
    {
        LogPrintf("KING_SocketSendto error:%x", KING_GetLastErrCode());
        KING_SocketClose(s_socketFd);
        s_socketFd = -1;
        return -1;
    }
    LogPrintf("KING_SocketSendto succees");
    return 0;
}

static void socket_eventProcessCb(uint32 eventID, void* pdata, uint32 len)
{
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pdata;
    MSG_S msg;
    int ret = -1;
    LogPrintf("socket_eventProcessCb() eventID=%d, dataLen=%d", eventID, len);
    
    switch (eventID)
    {
    case SOCKET_CONNECT_EVENT_RSP:
        LogPrintf("SOCKET_CONNECT_EVENT_RSP");
        break;

    case SOCKET_ACCEPT_EVENT_IND:
        LogPrintf("EV_CFW_TCPIP_ACCEPT_IND");
        break;

    case SOCKET_CLOSE_EVENT_RSP:
        LogPrintf("SOCKET_CLOSE_EVENT_RSP");
        break;

    case SOCKET_ERROR_EVENT_IND:
        LogPrintf("EV_CFW_TCPIP_ERR_IND, errcode=0x%x", KING_GetLastErrCode());
        if (s_socketFd != -1)
        {
            KING_SocketClose(s_socketFd);
            s_socketFd = -1;
        }
        break;

    case SOCKET_WRITE_EVENT_IND:
        LogPrintf("SOCKET_WRITE_EVENT_IND ack by peer uSendDataSize=%d", data->socket.availSize);
        break;

    case SOCKET_READ_EVENT_IND:
        LogPrintf("SOCKET_READ_EVENT_IND");
        memset(&msg, 0x00, sizeof(MSG_S));
        msg.messageID = MSG_EVT_SOCKET_READ;
        ret = KING_MsgSend(&msg, msgHandle);
        if(-1 == ret)
        {
            LogPrintf("KING_MsgSend() Failed! errcode=0x%x", KING_GetLastErrCode());
            return;
        }
        break;

    default:
        LogPrintf("TCPIP unexpect event/response %d", eventID);
        if (s_socketFd != -1)
        {
            KING_SocketClose(s_socketFd);
            s_socketFd = -1;
        }
        break;
    }

}

static int socket_getNicHandle(void)
{
    int ret = 0;
    NIC_INFO info;

    memset(&info, 0x00, sizeof(NIC_INFO));
    info.NIC_Type = NIC_TYPE_WWAN;
    ret = KING_HardwareNICRequest(info, &hSocketNIC);
    LogPrintf("socket_getNicHandle ret=%d, hSocketNIC=%d", ret, hSocketNIC);

    return ret;
}

static int socket_getContextHandle(void)
{
    int ret = 0;
    NIC_CONTEXT ApnInfo;

    if (hSocketNIC == -1)
    {
        return -1;
    }
    memset(&ApnInfo, 0x00, sizeof(NIC_CONTEXT));
    ret = KING_ConnectionAcquire(hSocketNIC, ApnInfo, &hSocketContext);
    LogPrintf("socket_getContextHandle ret=%d, hSocketContext=%d", ret, hSocketContext);
    return ret;
}

static void socket_netstate(void)
{
    int ret;
    NET_STATE_E state;

    LogPrintf("\r\n======== socket_netstate start... ========\r\n");
    while (1)
    {
        ret = KING_NetStateGet(&state);
        LogPrintf("Get net state %d, ret : %d\r\n", state, ret);
        if (ret == 0 && state > NET_NO_SERVICE && state < NET_EMERGENCY)
        {
            NET_CESQ_S Cesq;
            ret = KING_NetSignalQualityGet(&Cesq);
            LogPrintf("Get sinagal ret is %d. rxlev:%d, ber:%d, rscp:%d, ecno:%d, rsrq:%d, rsrp:%d\r\n",
                ret,Cesq.rxlev,Cesq.ber,Cesq.rscp,Cesq.ecno,Cesq.rsrq,Cesq.rsrp);
            socket_getNicHandle();
            socket_getContextHandle();
            KING_Sleep(1000);
            break;
        }
        KING_Sleep(1000);
    }
}

static void Socket_UDP_Thread(void *param)
{
    int ret = -1;
    MSG_S msg;

    socket_netstate();
    
    ret = KING_SocketCreate(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &s_socketFd);    
    if (ret < 0)
    {
        LogPrintf("KING_SocketCreate Fail");
        return;
    }
    LogPrintf("s_socketFd = %d",s_socketFd);
    KING_RegNotifyFun(DEV_CLASS_SOCKET, s_socketFd, socket_eventProcessCb);
    
    //msg
    ret = KING_MsgCreate(&msgHandle);
    if(FAIL == ret)
    {
        LogPrintf("KING_MsgCreate Fail");
        return;
    }
    
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = MSG_EVT_SOCKET_SENDTO;
    ret = KING_MsgSend(&msg, msgHandle);
    if(-1 == ret)
    {
        LogPrintf("KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        KING_MsgRcv(&msg, msgHandle, WAIT_OPT_INFINITE);
        switch (msg.messageID)
        {
            case MSG_EVT_SOCKET_READ:
            {
                Socket_Recvfrom();
                break;
            }
            case MSG_EVT_SOCKET_SENDTO:
            {
                Socket_Sendto((uint8 *)"123456789",sizeof("123456789"));
                break;
            }
            default:
            {
                LogPrintf("msg no know");
                break;
            }
        }
    }
}

void Socket_UDP_Test(void)
{
    int ret = SUCCESS;
    THREAD_ATTR_S threadAttr;
    LogPrintf("socket_udp_test");
    //Thread create 
    memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
    threadAttr.fun = Socket_UDP_Thread;
    threadAttr.priority = THREAD_PRIORIT3;
    threadAttr.stackSize = 5*1024;
    ret = KING_ThreadCreate("Socket_UDP_test", &threadAttr, &testThreadH);
    if(ret != 0)
    {
        LogPrintf("KING_ThreadCreate() testThreadH Fail!");
    }
}
