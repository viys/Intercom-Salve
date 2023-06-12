/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "kingnet.h"
#include "kingsocket.h"
#include "kingcstd.h"
#include "kingrtos.h"
#include "kingplat.h"
#include "kingCbData.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define CHEERZING_TEST_SERVER   "sockettest.cheerzing.com"
#define CHEERZING_TEST_PORT     12001

#define LogPrintf(fmt, args...)    do { KING_SysLog("socket: "fmt, ##args); } while(0)

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static hardwareHandle hSocketNIC = -1;
static softwareHandle hSocketContext = -1;
static int s_socketFd = -1;
static bool isIpv6 = 0;

static THREAD_HANDLE socketThreadID = NULL;
static MSG_HANDLE    socketMsgHandle = NULL;
static SOCK_IN_ADDR_T socketHostAddr;

/*******************************************************************************
 ** Local Function Delcarations
 ******************************************************************************/
static void socket_connect(SOCK_IN_ADDR_T *addr, uint16 port);
static void socket_enter(int pdpType, char *hostName);

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
static void socket_pingCallBack(
            TCPIP_PING_RESULT_E     res,          ///< ping result, 0 - succeed; other - fail
            uint32                  time_delay,   ///< ping time delay, only valid when success, unit: ms
            PING_HANDLE             ping_handle,  ///< ping handle
            uint8                   ttl,
            char*                   ipaddr        ///< uint16 ipaddr_size*/
            )
{
    LogPrintf("socket_pingCallBack res=%d, time_delay=%d, ttl=%d, ipaddr=%s\r\n", res, time_delay, ttl, ipaddr);
}

static void socket_ping6CallBack(
            TCPIP_PING_RESULT_E     res,          ///< ping result, 0 - succeed; other - fail
            uint32                  time_delay,   ///< ping time delay, only valid when success, unit: ms
            PING_HANDLE             ping_handle,  ///< ping handle
            uint8                   ttl,
            char*                   ipaddr        ///< uint16 ipaddr_size*/
            )
{
    LogPrintf("socket_pingCallBack res=%d, time_delay=%d, ttl=%d, ipaddr=%s\r\n", res, time_delay, ttl, ipaddr);
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

static int socket_getContextHandle(int isIpv6)
{
    int ret = 0;
    NIC_CONTEXT ApnInfo;

    if (hSocketNIC == -1)
    {
        return -1;
    }
    memset(&ApnInfo, 0x00, sizeof(NIC_CONTEXT));
    if (isIpv6)
    {
        strcpy((char *)ApnInfo.APN_Name, "");
        ApnInfo.IpType = IP_ADDR_TYPE_V6;
    }
    else
    {
        strcpy((char *)ApnInfo.APN_Name, "");
        ApnInfo.IpType = IP_ADDR_TYPE_V4;
    }
    ret = KING_ConnectionAcquire(hSocketNIC, ApnInfo, &hSocketContext);
    LogPrintf("socket_getContextHandle ret=%d, hSocketContext=%d", ret, hSocketContext);

    return ret;
}

static int socket_releaseContextHandle(void)
{
    int ret = 0;

    if (hSocketContext == -1)
    {
        return -1;
    }
    
    ret = KING_ConnectionRelease(hSocketContext);
    LogPrintf("socket_releaseContextHandle ret=%d", ret);

    return ret;
}

static void socket_pingTest(void)
{
    int ret =0;
    PING_HANDLE pingH;
    
    LogPrintf("socket_pingTest ... ...");
    if (hSocketContext == -1)
    {
        return;
    }

    ret = KING_NetPingV4Request(hSocketContext, "www.baidu.com", 20, 3000, socket_pingCallBack, &pingH);
    if(ret == -1)
    {
        LogPrintf("socket_pingTest KH_NetPingV4Request fail\r\n");
    }
}

static void socket_ping6Test(void)
{
    int ret =0;
    PING_HANDLE pingH;
    
    LogPrintf("socket_ping6Test ... ...");
    if (hSocketContext == -1)
    {
        return;
    }

    ret = KING_NetPingV6Request(hSocketContext, "ipv6.test-ipv6.com", 20, 3000, socket_ping6CallBack, &pingH);
    if(ret == -1)
    {
        LogPrintf("socket_ping6Test KING_NetPingV6Request fail\r\n");
    }
}

static void socket_Ipv4NetInfo(void)
{
    int ret = 0;
    IPV4_ADDR_S ip;
    IPV4_ADDR_S gw;
    IPV4_DNS dns;
    char *ipAddr = NULL;
    SOCK_IN_ADDR_T destAddr;
    
    LogPrintf("socket_Ipv4NetInfo ... ...");
    if (hSocketContext == -1)
    {
        return;
    }
    memset(&ip, 0, sizeof(IPV4_ADDR_S));
    memset(&gw, 0, sizeof(IPV4_ADDR_S));
    memset(&dns, 0, sizeof(IPV4_DNS));
    ret = KING_NetIpv4AddrInfoGet(hSocketContext, &ip, &dns, &gw);
    if(ret == 0)
    {
        memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
        destAddr.type = IP_ADDR_TYPE_V4;
        destAddr.u_addr.ip4.addr = ip.addr;
        KING_ipntoa(destAddr, &ipAddr);
        LogPrintf("socket_Ipv4NetInfo ip=%s\r\n", ipAddr);

        memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
        destAddr.type = IP_ADDR_TYPE_V4;
        destAddr.u_addr.ip4.addr = gw.addr;
        KING_ipntoa(destAddr, &ipAddr);
        LogPrintf("socket_Ipv4NetInfo gateway=%s\r\n", ipAddr);

        memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
        destAddr.type = IP_ADDR_TYPE_V4;
        destAddr.u_addr.ip4.addr = dns.primary_dns.addr;
        KING_ipntoa(destAddr, &ipAddr);
        LogPrintf("socket_Ipv4NetInfo primary_dns=%s\r\n", ipAddr);

        memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
        destAddr.type = IP_ADDR_TYPE_V4;
        destAddr.u_addr.ip4.addr = dns.secondary_dns.addr;
        KING_ipntoa(destAddr, &ipAddr);
        LogPrintf("socket_Ipv4NetInfo secondary_dns=%s\r\n", ipAddr);

        LogPrintf("socket_Ipv4NetInfo success\r\n");
    }
    else
    {
        LogPrintf("socket_Ipv4NetInfo fail\r\n");
    }
}

static void socket_Ipv6NetInfo(void)
{
    int ret = 0;
    IPV6_ADDR_S ip;
    IPV6_ADDR_S gw;
    IPV6_DNS dns;
    char *ipAddr = NULL;
    SOCK_IN_ADDR_T destAddr;
    
    LogPrintf("socket_Ipv6NetInfo ... ...");
    if (hSocketContext == -1)
    {
        return;
    }

    memset(&ip, 0, sizeof(IPV6_ADDR_S));
    memset(&gw, 0, sizeof(IPV6_ADDR_S));
    memset(&dns, 0, sizeof(IPV6_DNS));
    ret = KING_NetIpv6AddrInfoGet(hSocketContext, &ip, &dns, &gw);
    if(ret == 0)
    {
        memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
        destAddr.type = IP_ADDR_TYPE_V6;
        destAddr.u_addr.ip6.addr[0] = ip.addr[0];
        destAddr.u_addr.ip6.addr[1] = ip.addr[1];
        destAddr.u_addr.ip6.addr[2] = ip.addr[2];
        destAddr.u_addr.ip6.addr[3] = ip.addr[3];
        KING_ipntoa(destAddr, &ipAddr);
        LogPrintf("socket_Ipv6NetInfo ip=%s\r\n", ipAddr);

        memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
        destAddr.type = IP_ADDR_TYPE_V6;
        destAddr.u_addr.ip6.addr[0] = gw.addr[0];
        destAddr.u_addr.ip6.addr[1] = gw.addr[1];
        destAddr.u_addr.ip6.addr[2] = gw.addr[2];
        destAddr.u_addr.ip6.addr[3] = gw.addr[3];
        KING_ipntoa(destAddr, &ipAddr);
        LogPrintf("socket_Ipv6NetInfo gateway=%s\r\n", ipAddr);

        memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
        destAddr.type = IP_ADDR_TYPE_V6;
        destAddr.u_addr.ip6.addr[0] = dns.primary_dns.addr[0];
        destAddr.u_addr.ip6.addr[1] = dns.primary_dns.addr[1];
        destAddr.u_addr.ip6.addr[2] = dns.primary_dns.addr[2];
        destAddr.u_addr.ip6.addr[3] = dns.primary_dns.addr[3];
        KING_ipntoa(destAddr, &ipAddr);
        LogPrintf("socket_Ipv6NetInfo primary_dns=%s\r\n", ipAddr);

        memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
        destAddr.type = IP_ADDR_TYPE_V6;
        destAddr.u_addr.ip6.addr[0] = dns.secondary_dns.addr[0];
        destAddr.u_addr.ip6.addr[1] = dns.secondary_dns.addr[1];
        destAddr.u_addr.ip6.addr[2] = dns.secondary_dns.addr[2];
        destAddr.u_addr.ip6.addr[3] = dns.secondary_dns.addr[3];
        KING_ipntoa(destAddr, &ipAddr);
        LogPrintf("socket_Ipv6NetInfo secondary_dns=%s\r\n", ipAddr);

        LogPrintf("socket_Ipv6NetInfo success\r\n");
    }
    else
    {
        LogPrintf("socket_Ipv6NetInfo fail\r\n");
    }
}

static bool socket_httpSend(void)
{
    char *httpTxBuf = NULL;
    uint16 sendTotalLen = 0, send_len = 0;
    
    httpTxBuf = malloc(sizeof(char) * (400 + 1));
    if(httpTxBuf != NULL)
    {
        memset(httpTxBuf, 0x00, (sizeof(char) * (400 + 1)));
    }
    else
    {
        return FALSE;
    }

    sprintf(httpTxBuf, "GET %s HTTP/1.1\r\n", "/");

    sprintf(httpTxBuf + strlen(httpTxBuf), "Host: %s", "ipv6.baidu.com");

    sprintf(httpTxBuf + strlen(httpTxBuf), ":");
    sprintf(httpTxBuf + strlen(httpTxBuf), "%d", 80);
    sprintf(httpTxBuf + strlen(httpTxBuf), "\r\n");

    sprintf(httpTxBuf + strlen(httpTxBuf), "Connection: keep-alive\r\n");
    sprintf(httpTxBuf + strlen(httpTxBuf), "Content-Type: %s\r\n", "application/x-www-form-urlencoded");
    sprintf(httpTxBuf + strlen(httpTxBuf), "\r\n");
    
    sendTotalLen = strlen(httpTxBuf);
    send_len = KING_SocketSend(s_socketFd,
                    (void *)httpTxBuf,
                    (size_t)sendTotalLen,
                    0);
    LogPrintf("socket_httpSend send_len=%d\r\n", send_len);
    free(httpTxBuf);
    httpTxBuf = NULL;
    if (send_len > 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

#if 0
static void socket_eventProcessCb(uint32 eventID, void* pdata, uint32 len)
{
    int ret;
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pdata;

    LogPrintf("socket_eventProcessCb() eventID=%d, dataLen=%d\r\n", eventID, len);
    switch (eventID)
    {
    case SOCKET_CONNECT_EVENT_RSP:
    {
    #if 1
        MSG_S msg;
    
        LogPrintf("SOCKET_CONNECT_EVENT_RSP KING_MsgSend ");
        memset(&msg, 0x00, sizeof(MSG_S));
        msg.messageID = 121;
        ret = KING_MsgSend(&msg, socketMsgHandle);
        if(-1 == ret)
        {
            LogPrintf("SOCKET_CONNECT_EVENT_RSP KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
            return;
        }
    #else   
        uint8 buf[128] = {0};
    
        LogPrintf("[KING]SOCKET_CONNECT_EVENT_RSP\r\n");

        memset(buf, 0x00,sizeof(buf));
        sprintf((char*)buf, "%s", "socket send and recv test");
        LogPrintf("[KING]Send data: %s\r\n", (char*)buf);

        ret = KING_SocketSend(s_sock, buf, KING_strlen((char*)buf), 0);
        if (-1 == ret)
        {
            LogPrintf("[KING]Socket send fail\r\n");
            if (s_sock != -1)
            {
                KING_SocketClose(s_sock);
                s_sock = -1;
            }
        }
        else
        {
            LogPrintf("[KING]Socket send succ\r\n");
        }
    #endif
    }
        break;

    case SOCKET_ACCEPT_EVENT_IND:
        LogPrintf("EV_CFW_TCPIP_ACCEPT_IND\r\n");
        break;

    case SOCKET_CLOSE_EVENT_RSP:
        LogPrintf("SOCKET_CLOSE_EVENT_RSP\r\n");
        break;

    case SOCKET_ERROR_EVENT_IND:
        LogPrintf("EV_CFW_TCPIP_ERR_IND, errcode=0x%x\r\n", KING_GetLastErrCode());
        if (s_socketFd != -1)
        {
            KING_SocketClose(s_socketFd);
            s_socketFd = -1;
        }
        break;

    case SOCKET_WRITE_EVENT_IND:
        LogPrintf("SOCKET_WRITE_EVENT_IND ack by peer uSendDataSize=%d\r\n", data->socket.availSize);
        break;

    case SOCKET_READ_EVENT_IND:
    {
        uint16 uDataSize = 0;
    #if 1
        MSG_S msg;
    #else
        uint8 *pRecv     = NULL;
    #endif
    
        LogPrintf("SOCKET_READ_EVENT_IND\r\n");
        uDataSize = data->socket.availSize;
        LogPrintf("recv datalen=%d\r\n", uDataSize);
        
    #if 1
        memset(&msg, 0x00, sizeof(MSG_S));
        msg.messageID = 122;
        msg.DataLen = uDataSize;
        ret = KING_MsgSend(&msg, socketMsgHandle);
        if(-1 == ret)
        {
            LogPrintf("SOCKET_READ_EVENT_IND KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
            return;
        }
    #else 
        if(KING_MemAlloc((void**)(&pRecv), uDataSize+1) < 0)
        {
            LogPrintf("[KING]SOCKET_READ_EVENT_IND KING_MemAlloc fail\r\n");
            break;
        }
        memset(pRecv, 0, uDataSize+1);
        ret = KING_SocketRecv(s_sock, pRecv, uDataSize, 0);
        if(ret == -1)
        {
            LogPrintf("[KING]KING_SocketRecv recv fail\r\n");
        }
        else
        {
            LogPrintf("[KING]KING_SocketRecv recv data=%s\r\n",pRecv);
        }
        KING_MemFree(pRecv);
        if (s_sock != -1)
        {
            KING_SocketClose(s_sock);
            s_sock = -1;
        }
    #endif    
    }
        break;

    default:
        LogPrintf("TCPIP unexpect event/response %d\r\n", eventID);
        if (s_socketFd != -1)
        {
            KING_SocketClose(s_socketFd);
            s_socketFd = -1;
        }
        break;
    }

}
#endif

static int socket_send(void)
{
    int ret = -1;
    
    if (isIpv6)
    {
        ret = socket_httpSend();
    }
    else
    {
        uint8 buf[128] = {0};
        
        memset(buf, 0x00,sizeof(buf));
        sprintf((char*)buf, "%s", "socket send and recv test");
    
        ret = KING_SocketSend(s_socketFd, buf, strlen((char*)buf), 0);
        if (-1 == ret)
        {
            LogPrintf("Socket send fail\r\n");
            if (s_socketFd != -1)
            {
                KING_SocketClose(s_socketFd);
                s_socketFd = -1;
            }
            ret = -1;
        }
        else
        {
            LogPrintf("Socket send succ\r\n");
            ret = 0;
        }
    }
    return ret;
}

static void socket_recvData(void)
{
    int ret = -1;
    int selectfd = -1, nfds = -1, nCount = 0;
    fd_set Fdset;
    struct timeval timev = { 5, 0 };
    char *pRecv = NULL;
    
    pRecv = malloc(1460 + 1);
    if(pRecv == NULL)
    {
        LogPrintf("socket_recv malloc fail\r\n");
        return;
    }

    while(1)
    {
        if (s_socketFd == -1)
        {
            break;
        }
        selectfd = s_socketFd + 1;
        FD_ZERO(&Fdset);
        FD_SET(s_socketFd, &Fdset);
        if((nfds = KING_SocketSelect(selectfd, &Fdset, NULL, NULL, &timev)) == -1)
        {
            LogPrintf("socket_recv Select nfds=%d\n", nfds);
            break;
        }

        if(nfds == 0)
        {
            LogPrintf("socket_recv nfds=%d\n", nfds);
            nCount++;
            if (nCount > 5)
            {
                break;
            }
            else 
            {
                continue;
            }
        }
        
        memset((void *)pRecv, 0x00, 1460 + 1);
        ret = KING_SocketRecv(s_socketFd, (void *)pRecv, 1460, 0);
        LogPrintf("socket_recv ret=%d\n", ret);
        if(ret == -1)
        {
            LogPrintf("KING_SocketRecv recv fail\r\n");
            break;
        }
        else
        {
            LogPrintf("KING_SocketRecv recv succ\r\n");
        }

    }
    if (pRecv != NULL)
    {
        free(pRecv);
        pRecv = NULL;
    }
    return;
}

static int socket_recv(void)
{
    MSG_S msg;
    int ret = -1;
    
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = 123;
    ret = KING_MsgSend(&msg, socketMsgHandle);
    if(-1 == ret)
    {
        LogPrintf("socket_recv KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return -1;
    }

    return 0;
}

static void socket_connect(SOCK_IN_ADDR_T *addr, uint16 port)
{
    int ret = 0;
    union sockaddr_aligned sock_addr;
    uint32 sockaddr_size = 0;
    union sockaddr_aligned local_addr;
    uint32 localaddr_size = 0;
    
    if (isIpv6)
    {
        ret = KING_SocketCreate(AF_INET6, SOCK_STREAM, IPPROTO_TCP, &s_socketFd);
    }
    else
    {
        ret = KING_SocketCreate(AF_INET, SOCK_STREAM, IPPROTO_TCP, &s_socketFd);
    }
    if (ret < 0)
    {
        LogPrintf("socket_connect KING_SocketCreate() fail\r\n");
        return;
    }
    //KING_RegNotifyFun(DEV_CLASS_SOCKET, s_socketFd, socket_eventProcessCb);

    if (hSocketContext == -1)
    {
        return;
    }

    if (isIpv6)
    {
        IPV6_ADDR_S ip6;
        struct sockaddr_in6 *sock_addr6 = (struct sockaddr_in6 *)&(sock_addr);

        ret = KING_NetIpv6AddrInfoGet(hSocketContext, NULL, NULL, NULL);
        LogPrintf("KING_NetIpv6AddrInfoGet() ret=%d\r\n", ret);
        memset(&ip6, 0, sizeof(IPV6_ADDR_S));
        ret = KING_NetIpv6AddrInfoGet(hSocketContext, &ip6, NULL, NULL);
        if(ret == 0)
        {
            struct sockaddr_in6 *stLocalAddr6 = (struct sockaddr_in6 *)&(local_addr);
            
            stLocalAddr6->sin6_len = sizeof(struct sockaddr_in6);
            stLocalAddr6->sin6_family = AF_INET6;
            stLocalAddr6->sin6_port = 0;
            stLocalAddr6->sin6_addr.addr[0] = ip6.addr[0];
            stLocalAddr6->sin6_addr.addr[1] = ip6.addr[1];
            stLocalAddr6->sin6_addr.addr[2] = ip6.addr[2];
            stLocalAddr6->sin6_addr.addr[3] = ip6.addr[3];

            localaddr_size = sizeof(struct sockaddr_in6);
        }
        
        sock_addr6->sin6_len = sizeof(struct sockaddr_in6);
        sock_addr6->sin6_family = AF_INET6;
        sock_addr6->sin6_port = KING_htons(port);
        sock_addr6->sin6_addr.addr[0] = addr->u_addr.ip6.addr[0];
        sock_addr6->sin6_addr.addr[1] = addr->u_addr.ip6.addr[1];
        sock_addr6->sin6_addr.addr[2] = addr->u_addr.ip6.addr[2];
        sock_addr6->sin6_addr.addr[3] = addr->u_addr.ip6.addr[3];
        
        sockaddr_size = sizeof(struct sockaddr_in6);
    }
    else
    {
        IPV4_ADDR_S ip;
        struct sockaddr_in *sock_addr4 = (struct sockaddr_in *)&(sock_addr);
        
        ret = KING_NetIpv4AddrInfoGet(hSocketContext, NULL, NULL, NULL);
        LogPrintf("KING_NetIpv4AddrInfoGet() ret=%d\r\n", ret);
        memset(&ip, 0, sizeof(IPV4_ADDR_S));
        ret = KING_NetIpv4AddrInfoGet(hSocketContext, &ip, NULL, NULL);
        if(ret == 0)
        {
            struct sockaddr_in *stLocalAddr = (struct sockaddr_in *)&(local_addr);
            
            stLocalAddr->sin_len = sizeof(struct sockaddr_in);
            stLocalAddr->sin_family = AF_INET;
            stLocalAddr->sin_port = 0;
            stLocalAddr->sin_addr.addr = ip.addr;

            localaddr_size = sizeof(struct sockaddr_in);
        }
        
        sock_addr4->sin_len = sizeof(struct sockaddr_in);
        sock_addr4->sin_family = AF_INET;
        sock_addr4->sin_port = KING_htons(port);
        sock_addr4->sin_addr.addr = addr->u_addr.ip4.addr;
        LogPrintf("socket_connect ip_addr=%u, s_socketFd=%d\r\n", sock_addr4->sin_addr.addr, s_socketFd);

        sockaddr_size = sizeof(SOCK_ADDR_IN_T);
    }

    if (ret == 0)
    {
        ret = KING_SocketBind(s_socketFd, &local_addr, localaddr_size);
        LogPrintf("KING_SocketBind() ret=%d\r\n", ret);
        if (ret < 0)
        {
            LogPrintf("KING_SocketBind fail\r\n");
            KING_SocketClose(s_socketFd);
            return;
        }
    }
    
    ret = KING_SocketConnect(s_socketFd, &sock_addr, sockaddr_size);
    LogPrintf("KING_SocketConnect() ret=%d\r\n", ret);
    if (ret < 0)
    {
        LogPrintf("KING_SocketConnect fail\r\n");
        KING_SocketClose(s_socketFd);
        return;
    }
    
    ret = socket_send();
    if (ret < 0)
    {
        LogPrintf("socket_send fail\r\n");
        return;
    }

    ret = socket_recv();
    if (ret < 0)
    {
        LogPrintf("socket_recv fail\r\n");
        return;
    }
}

static void socket_dnsReqCb(void* pData, uint32 len)
{
    ASYN_DNS_PARSE_T *dnsParse = (ASYN_DNS_PARSE_T *)pData;
    
    LogPrintf("socket_dnsReqCb hostname=%s\r\n", dnsParse->hostname);
    LogPrintf("socket_dnsReqCb dnsParse->result=%d\n", dnsParse->result);
    if (dnsParse->result == DNS_PARSE_SUCCESS)
    {
        MSG_S msg;
        int ret = -1;
        
        memset(&socketHostAddr, 0x00, sizeof(SOCK_IN_ADDR_T));
        socketHostAddr = dnsParse->addr;
        LogPrintf("socket DNS_PARSE_SUCCESS KING_MsgSend ");
        memset(&msg, 0x00, sizeof(MSG_S));
        msg.messageID = 121;
        ret = KING_MsgSend(&msg, socketMsgHandle);
        if(-1 == ret)
        {
            LogPrintf("socket DNS_PARSE_SUCCESS KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
            return;
        }
    }
    else if (dnsParse->result == DNS_PARSE_ERROR)
    {
        LogPrintf("socket_dnsReqCb fail!\n");
    }
}

static void socket_enter(int pdpType, char *hostName)
{
    int ret = -1;
    SOCK_IN_ADDR_T destAddr;
    SOCK_IN_ADDR_T *ip = &destAddr;

    LogPrintf("socket_enter ... ...");
    if (pdpType == 1)
    {
        socket_Ipv6NetInfo();
        socket_ping6Test();
    }
    else
    {
        socket_Ipv4NetInfo();
        socket_pingTest();
    }
    if (hSocketContext == -1)
    {
        return;
    }
    memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
    ret = KING_NetGetHostByName(hSocketContext, hostName, ip, 10000, (DNS_CALLBACK)socket_dnsReqCb);
    LogPrintf("socket_enter ret=%d", ret);
    if (ret == 0)
    {
        if (ip != NULL)
        {
            if (destAddr.type == IP_ADDR_TYPE_V6)
            {
                if (0 == destAddr.u_addr.ip6.addr[0] && 0 == destAddr.u_addr.ip6.addr[1]
                    && 0 == destAddr.u_addr.ip6.addr[2] && 0 == destAddr.u_addr.ip6.addr[3])
                {
                    LogPrintf("socket_enter ipv6 query QUEUED\r\n");
                }
            }
            else
            {
                if (0 == destAddr.u_addr.ip4.addr)
                {
                    LogPrintf("socket_enter query QUEUED\r\n");
                }
            }
        }
    }
    else
    {
        LogPrintf("socket_enter KING_NetGetHostByName fail\r\n");
    }
}

static void socket_startTest(int pdpType)
{
    int ret;
    NET_STATE_E state;

    LogPrintf("\r\n======== socket_startTest start... ========\r\n");
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

            if (pdpType == 1)
            {
                ret = KING_NetApnSet(1, "", NULL, NULL, 0, IP_ADDR_TYPE_V6);
            }
            else
            {
                ret = KING_NetApnSet(1, "", NULL, NULL, 0, IP_ADDR_TYPE_V4);
            }
            LogPrintf("KING_NetApnSet ret : %d\r\n", ret);
            if (ret != 0)
            {
                LogPrintf("KING_NetApnSet fail\r\n");
                break;
            }

            socket_getNicHandle();
            socket_getContextHandle(pdpType);
            KING_SocketNativeApiSet(1);
            KING_Sleep(1000);
            if (pdpType == 1)
            {
                socket_enter(pdpType, "ipv6.test-ipv6.com");
            }
            else
            {
                socket_enter(pdpType, CHEERZING_TEST_SERVER);
            }
            LogPrintf("socket_enter complete\r\n");
            break;
        }
        
        KING_Sleep(1000);
    }

    LogPrintf("\r\n[KING]======== Socket_test complete ========\r\n");
}

static void socket_test_thread(void *param)
{
    MSG_S msg;
    int ret = -1;

    LogPrintf("KING socket_test_thread is running. socketThreadID = 0x%X\r\n", socketThreadID);
    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        LogPrintf("socket_test_thread!\r\n");
        if (socketMsgHandle != NULL)
        {
            LogPrintf("socket_test_thread KING_MsgRcv!\r\n");
            ret = KING_MsgRcv(&msg, socketMsgHandle, WAIT_OPT_INFINITE);
            if (ret == -1)
            {
                LogPrintf("socket_test_thread ret=%d\r\n", ret);
                continue;
            }
            LogPrintf("socket_test_thread receive msg. MsgId=%d\r\n", msg.messageID);
            if (msg.messageID == 120)
            {
                if (isIpv6)
                {
                    socket_startTest(1);
                }
                else
                {
                    socket_startTest(0);
                }
            }
            else if (msg.messageID == 121)
            {
                if (socketHostAddr.type == IP_ADDR_TYPE_V6)
                {
                    socket_connect(&socketHostAddr, 80);
                }
                else
                {
                    socket_connect(&socketHostAddr, CHEERZING_TEST_PORT);
                }
            }
            else if (msg.messageID == 122)
            {
                if (isIpv6)
                {
                    socket_httpSend();
                }
                else
                {
                    uint8 buf[128] = {0};
                    
                    memset(buf, 0x00,sizeof(buf));
                    sprintf((char*)buf, "%s", "socket send and recv test");

                    ret = KING_SocketSend(s_socketFd, buf, strlen((char*)buf), 0);
                    if (-1 == ret)
                    {
                        LogPrintf("Socket send fail\r\n");
                        if (s_socketFd != -1)
                        {
                            KING_SocketClose(s_socketFd);
                            s_socketFd = -1;
                        }
                    }
                    else
                    {
                        LogPrintf("Socket send succ\r\n");
                    }
                }
            }
            else if (msg.messageID == 123)
            {
                socket_recvData();
                if (s_socketFd != -1)
                {
                    KING_SocketClose(s_socketFd);
                    s_socketFd = -1;
                }
                break;
            }
        }
    }
    
    socket_releaseContextHandle();
    if (socketMsgHandle != NULL)
    {
        KING_MsgDelete(socketMsgHandle);
        socketMsgHandle = NULL;
    }
    if (socketThreadID != NULL)
    {
        socketThreadID = NULL; 
    }
    KING_ThreadExit();
}

void socket_test(void)
{
#if 1    
    int ret = -1;
    MSG_S msg;
    THREAD_ATTR_S threadAttr;
    
    LogPrintf("socket_test is KING_MsgCreate ");
    ret = KING_MsgCreate(&socketMsgHandle);
    LogPrintf("socket_test is KING_MsgCreate ret %d", ret);
    if(-1 == ret)
    {
        LogPrintf("socket_test KING_MsgCreate, errcode=0x%x\r\n", KING_GetLastErrCode());
    }

    KING_Sleep(1000);

    LogPrintf("socket_test is KING_ThreadCreate ");
    memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
    threadAttr.fun = socket_test_thread;
    threadAttr.priority = THREAD_PRIORIT1;
    threadAttr.stackSize = 1024*4;
    threadAttr.queueNum = 64; 
    
    LogPrintf("socket_test is create socketThreadID 0x%08x", &socketThreadID);
    ret = KING_ThreadCreate("socketTestThread", &threadAttr, &socketThreadID);
    if (-1 == ret)
    {
        LogPrintf("[KING]Create Test Thread Failed!! Errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    KING_Sleep(1000);
    
    LogPrintf("APP Main is KING_MsgSend ");
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = 120;
    ret = KING_MsgSend(&msg, socketMsgHandle);
    if(-1 == ret)
    {
        LogPrintf("KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
#else    
    socket_startTest(0);
#endif
}

