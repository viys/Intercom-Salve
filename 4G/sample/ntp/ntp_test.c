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
#define NTP_SERVER      "ntp1.aliyun.com"
#define NTP_PORT        123

static MSG_HANDLE msgHandle = NULL; 
static THREAD_HANDLE testThreadH = NULL;
static int s_socketFd = 0;
static hardwareHandle hSocketNIC = -1;
static softwareHandle hSocketContext = -1;

typedef struct ntpPacket {
    uint8  li_vn_mode;
    uint8  stratum;
    uint8  poll;
    uint8  precision;
    uint32 root_delay;
    uint32 root_dispersion;
    int8   ref_id[4];
    uint32 reftimestamp_h;
    uint32 reftimestamp_l;
    uint32 oritimestamp_h;
    uint32 oritimestamp_l;
    uint32 recvtimestamp_h;
    uint32 recvtimestamp_l;
    uint32 trantimestamp_h;
    uint32 trantimestamp_l;
} NTP_PACKET_T;

void get_month_and_day(int nDays,int*nMonth,int*nDay,bool IsLeapYear)
{
    static const int MON1[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};	//non-leap year
    static const int MON2[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};	//leap year
    const int *pMonths = IsLeapYear?MON2:MON1;
    //The cycle subtracts the days of each month in 12 months until the remaining days are less than or equal to 0, and the corresponding month is found
    for (int i=0; i<12; ++i)
    {
        int nTemp = nDays - pMonths[i];
        if (nTemp<=0)
        {
            *nMonth = i + 1;
            if (nTemp == 0)
            {
                //It means the last day of the month, so the number of days is the total number of days of the month
                *nDay = pMonths[i];
            }
            else
            {
                *nDay = nDays;
            }
            break;
        }
        else
        {   
            *nDay = nDays;  
        }
        nDays = nTemp;
    }
}


int gmtime(uint32 t,CALENDAR_TIME*new_time)
{   
    static const int FOURYEARS = (365 + 365 +366 +365);	//Total number of days per four years
    static const int DAYMS = 24*3600;	//Seconds per day
    int nTime = t;//Get the current system time
    int nDays = nTime/DAYMS + 1;	//The time function gets the number of milliseconds since 1970, so you need to get the number of days first
    int nYear4 = nDays/FOURYEARS;	//Get the number of cycles (4 years) since 1970
    int nRemain = nDays%FOURYEARS;	//Get the number of days in less than one cycle
    int nDesYear = 1970 + nYear4*4;
    int nDesMonth = 0, nDesDay = 0;
    int nDesHour=0,nDesMinute=0,nDesSecond=0;
    bool bLeapYear = 0;

    if ( nRemain<365 ) //In a cycle, the first year 
    {

    }
    else if ( nRemain<(365+365) ) //In one cycle, in the second year
    {
        nDesYear += 1;
        nRemain -= 365;
    }
    else if ( nRemain<(365+365+366) )//In a cycle, the third year, leap year。
    {
        nDesYear += 2;
        nRemain -= (365+365);
        bLeapYear = 1;
    } 
    else//In a cycle, the fourth year, which is a leap year
    {
        nDesYear += 3;
        nRemain -= (365+365+366);
    }
    
    get_month_and_day(nRemain, &nDesMonth, &nDesDay,bLeapYear);
    
    nDesHour=(nTime%(24*3600))/3600;
    nDesMinute=((nTime%(24*3600))%3600)/60;
    nDesSecond=((nTime%(24*3600))%3600)%60;
    LogPrintf("nRemain=%d,nDesYear=%d,nDesMonth=%d, nDesDay=%d,nDesHour=%d,nDesMinute=%d,nDesSecond=%d bLeapYear=%d",nRemain,nDesYear,nDesMonth,nDesDay,nDesHour,nDesMinute,nDesSecond,bLeapYear);
    new_time->yy=BIN2BCD((nDesYear-2000));
    new_time->MM=BIN2BCD(nDesMonth);
    new_time->dd=BIN2BCD(nDesDay);
    new_time->hh=BIN2BCD(nDesHour);
    new_time->mm=BIN2BCD(nDesMinute);
    new_time->ss_l=BIN2BCD(nDesSecond%10);
    new_time->ss_h=BIN2BCD(nDesSecond/10);

    if((new_time->yy<0)||(new_time->MM<0&&new_time->MM>12)||(new_time->dd<0)||(new_time->hh<0&&new_time->yy>24)||(new_time->mm<0&&new_time->yy>60)||(new_time->ss_h<0&&new_time->ss_h>60))
    {
        LogPrintf("recv time value is error!!!");
        return -1;
    }
    return 0;
}


static int ntp_recvfrom(void)
{
    int ret = -1;
    uint8 buf[256] = {0};
    union sockaddr_aligned sock_addr;
    SOCK_IN_ADDR_T ipAddr;
    uint32 addr_size = 0;
    NTP_PACKET_T recv_ntp_packet;
    uint32 t = 0;
    CALENDAR_TIME system_time;
    CALENDAR_TIME get_time;
        
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
    LogPrintf("Socket_Recvfrom succees len:%d",ret);
    memset(&recv_ntp_packet, 0x00, sizeof(NTP_PACKET_T));
    memcpy(&recv_ntp_packet, buf, sizeof(NTP_PACKET_T));

    t = KING_ntohl(recv_ntp_packet.trantimestamp_h)-(uint32)2208988800;//获取为0时区的时间

    KING_LocalTimeGet(&system_time);
    if (0 == system_time.ew)
    {
        t += system_time.tz*15*60;
    }
    else
    {
        t += system_time.tz*15*60;
    }
    LogPrintf("NTP time:%u",t);
    
    gmtime(t, &get_time);
    
    return 0;
}

static int ntp_sendto(SOCK_IN_ADDR_T ipAddr,uint16 post)
{   
    int ret = -1;
    union sockaddr_aligned sock_addr;
    NTP_PACKET_T ntp_packet;
    
    LogPrintf("ntp_sendto");
    if (s_socketFd < 0)
    {
        LogPrintf("no s_socketFd");
        return -1;
    }

    struct sockaddr_in *sock_addr4 = (struct sockaddr_in *)&(sock_addr);
    sock_addr4->sin_len = sizeof(struct sockaddr_in);
    sock_addr4->sin_family = AF_INET;
    sock_addr4->sin_port = KING_htons(post);
    sock_addr4->sin_addr.addr = ipAddr.u_addr.ip4.addr;

    memset(&ntp_packet, 0x00, sizeof(NTP_PACKET_T));
    ntp_packet.li_vn_mode = 0x1B;
    ntp_packet.oritimestamp_h = 0x0ffffff;

    ret = KING_SocketSendto(s_socketFd, (void *)&ntp_packet, sizeof(NTP_PACKET_T), 0, &sock_addr, sizeof(SOCK_ADDR_IN_T));
    if (-1 == ret)
    {
        LogPrintf("KING_SocketSendto error:%x", KING_GetLastErrCode());
        KING_SocketClose(s_socketFd);
        s_socketFd = -1;
        return -1;
    }
    LogPrintf("ntp_sendto succees");
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

static void socket_dnsReqCb(void* pData, uint32 len)
{
    ASYN_DNS_PARSE_T *dnsParse = (ASYN_DNS_PARSE_T *)pData;
    
    LogPrintf("socket_dnsReqCb hostname=%s\r\n", dnsParse->hostname);
    LogPrintf("socket_dnsReqCb dnsParse->result=%d\n", dnsParse->result);
    if (dnsParse->result == DNS_PARSE_SUCCESS)
    {
        SOCK_IN_ADDR_T ipaddr = dnsParse->addr;
        
        ntp_sendto(ipaddr,NTP_PORT);
    }
    else if (dnsParse->result == DNS_PARSE_ERROR)
    {
        LogPrintf("socket_dnsReqCb fail!\n");
    }
}

static void socket_getHostByName(char *hostName)
{
    int ret = -1;
    SOCK_IN_ADDR_T destAddr;
    SOCK_IN_ADDR_T *ip = &destAddr;

    if (hSocketContext == -1)
    {
        return;
    }
    memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
    ret = KING_NetGetHostByName(hSocketContext, hostName, ip, 10000, (DNS_CALLBACK)socket_dnsReqCb);
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

static void Sim_IsReady(void)
{
    int ret;
    uint32 simRdy = 0;

    while (simRdy == 0)
    {
        ret = KING_SimReadyGet(&simRdy);
        if (FAIL == ret)
        { 
            LogPrintf("KING_SimReadyGet() errcode=0x%x\r\n", KING_GetLastErrCode());
        }
        LogPrintf("Get SIM status: %d\r\n", simRdy);
        KING_Sleep(1000);
    }
}

static void net_state(void)
{
    int ret;
    NET_STATE_E state;

    LogPrintf("======== net_state start... ========");
    while (1)
    {
        ret = KING_NetStateGet(&state);
        LogPrintf("Get net state %d, ret : %d", state, ret);
        if (ret == 0 && state > NET_NO_SERVICE && state < NET_EMERGENCY)
        {
            NET_CESQ_S Cesq;
            ret = KING_NetSignalQualityGet(&Cesq);
            LogPrintf("Get sinagal ret is %d. rxlev:%d, ber:%d, rscp:%d, ecno:%d, rsrq:%d, rsrp:%d",
                ret,Cesq.rxlev,Cesq.ber,Cesq.rscp,Cesq.ecno,Cesq.rsrq,Cesq.rsrp);
            socket_getNicHandle();
            socket_getContextHandle();//这边已激活pdp
            KING_Sleep(1000);
            break;
        }
        KING_Sleep(1000);
    }
}

static void ntp_Thread(void *param)
{
    int ret = -1;
    MSG_S msg;

    Sim_IsReady();
    net_state();
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

    socket_getHostByName(NTP_SERVER);
    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        KING_MsgRcv(&msg, msgHandle, WAIT_OPT_INFINITE);
        switch (msg.messageID)
        {
            case MSG_EVT_SOCKET_READ:
            {
                ntp_recvfrom();
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

void ntp_test(void)
{
    int ret = SUCCESS;
    THREAD_ATTR_S threadAttr;
    LogPrintf("ntp_test");
    //Thread create 
    memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
    threadAttr.fun = ntp_Thread;
    threadAttr.priority = THREAD_PRIORIT3;
    threadAttr.stackSize = 5*1024;
    ret = KING_ThreadCreate("ntp_Thread", &threadAttr, &testThreadH);
    if(ret != 0)
    {
        LogPrintf("KING_ThreadCreate() Fail!");
    }
}
