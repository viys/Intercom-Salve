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
#define LogPrintf(fmt, args...)    do { KING_SysLog("multi_dns: "fmt, ##args); } while(0)

#define DNS_TEST_SERVER   "www.baidu.com"


/*******************************************************************************
 ** Variables
 ******************************************************************************/
static hardwareHandle hDnsNIC = -1;
static softwareHandle hDnsContext = -1;

static THREAD_HANDLE dnsThreadID = NULL;
static MSG_HANDLE    dnsMsgHandle = NULL;
static TIMER_HANDLE dnsTimerHandle = 0;
static bool isSecond = FALSE;

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
static int multiDns_getNicHandle(void)
{
    int ret = 0;
    NIC_INFO info;

    memset(&info, 0x00, sizeof(NIC_INFO));
    info.NIC_Type = NIC_TYPE_WWAN;
    ret = KING_HardwareNICRequest(info, &hDnsNIC);
    LogPrintf("dns_getNicHandle ret=%d, hDnsNIC=%d", ret, hDnsNIC);

    return ret;
}

static int multiDns_getContextHandle(int isIpv6)
{
    int ret = 0;
    NIC_CONTEXT ApnInfo;

    if (hDnsNIC == -1)
    {
        return -1;
    }
    memset(&ApnInfo, 0x00, sizeof(NIC_CONTEXT));
    if (isIpv6)
    {
        //strcpy((char *)ApnInfo.APN_Name, "CMNET");
        ApnInfo.IpType = IP_ADDR_TYPE_V6;
    }
    else
    {
        //strcpy((char *)ApnInfo.APN_Name, "CMNET");
        ApnInfo.IpType = IP_ADDR_TYPE_V4;
    }
    ret = KING_ConnectionAcquire(hDnsNIC, ApnInfo, &hDnsContext);
    LogPrintf("multiDns_getContextHandle ret=%d, hDnsContext=%d", ret, hDnsContext);

    return ret;
}

static int multiDns_releaseContextHandle(void)
{
    int ret = 0;

    if (hDnsContext == -1)
    {
        return -1;
    }
    
    ret = KING_ConnectionRelease(hDnsContext);
    LogPrintf("multiDns_releaseContextHandle ret=%d", ret);

    return ret;
}

static void multiDns_Timer_Callback(uint32 tmrId)
{
    MSG_S msg;
    int ret = SUCCESS;
    
    LogPrintf("multiDns_Timer_Callback");
    
    //send msg
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = 221;
    ret = KING_MsgSend(&msg, dnsMsgHandle);
    if(-1 == ret)
    {
        LogPrintf("KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }   
}

static void multiDns_Timer(void)
{
    int ret = FAIL;
    TIMER_ATTR_S timerattr;
    
    LogPrintf("multiDns_Timer start!");
    isSecond = TRUE;
    //timer create 
    ret = KING_TimerCreate(&dnsTimerHandle);
    if(FAIL == ret)
    {
        LogPrintf("KING_TimerCreate() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }

    //timer active
    memset(&timerattr, 0x00, sizeof(timerattr));
    timerattr.isPeriod= FALSE;
    timerattr.timeout = 5000;
    timerattr.timer_fun = multiDns_Timer_Callback;
    ret = KING_TimerActive(dnsTimerHandle, &timerattr);
    if(FAIL == ret)
    {
        LogPrintf("KING_TimerActive() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        KING_TimerDelete(dnsTimerHandle);
        return;
    }
    LogPrintf("multiDns_Timer end!");
}

static void multiDns_dnsReqCb(void* pData, uint32 len)
{
    ASYN_MULTI_DNS_PARSE_T *dnsParse = (ASYN_MULTI_DNS_PARSE_T *)pData;
    
    LogPrintf("multiDns_dnsReqCb hostname=%s\r\n", dnsParse->hostname);
    LogPrintf("multiDns_dnsReqCb dnsParse->result=%d\n", dnsParse->result);
    if (dnsParse->result == DNS_PARSE_SUCCESS)
    {
        char *pIpAddr = NULL;
        int i = 0, ret = -1;

        for (i = 0; i < DNS_MAX_ADDR_COUNT; i++)
        {
            SOCK_IN_ADDR_T *ipaddr = &(dnsParse->addr[i]);
            
            if (!ip_addr_isany(ipaddr))
            {
                ret = KING_ipntoa(dnsParse->addr[i], &pIpAddr);
                if (ret == 0 && pIpAddr != NULL)
                {
                    LogPrintf("multiDns_dnsReqCb i:%d, ipaddr=%s", i, pIpAddr);
                }
            }
            else
            {
                break;
            }
        } 
        if (!isSecond)
        {
            multiDns_Timer();
        }
    }
    else if (dnsParse->result == DNS_PARSE_ERROR)
    {
        LogPrintf("socket_dnsReqCb fail!\n");
    }
}

static void multiDns_enter(char *hostName)
{
    int ret = -1;
    SOCK_IN_ADDR_T destAddr[DNS_MAX_ADDR_COUNT];

    LogPrintf("multiDns_enter ... ...");
    if (hDnsContext == -1)
    {
        return;
    }
    memset((void *)(&destAddr), 0x00, sizeof(SOCK_IN_ADDR_T) * DNS_MAX_ADDR_COUNT);
    ret = KING_NetGetAllHostByName(hDnsContext, hostName, &destAddr[0], 10000, (DNS_CALLBACK)multiDns_dnsReqCb);
    LogPrintf("multiDns_enter ret=%d", ret);
    if (ret != 0)
    {
        LogPrintf("multiDns_enter KING_NetGetAllHostByName fail\r\n");
    }
}

static void multiDns_startTest(void)
{
    int ret;
    NET_STATE_E state;

    LogPrintf("\r\n======== multiDns_startTest start... ========\r\n");
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

            multiDns_getNicHandle();
            multiDns_getContextHandle(0);
            KING_Sleep(1000);
            multiDns_enter(DNS_TEST_SERVER);
            LogPrintf("socket_enter complete\r\n");
            break;
        }
        
        KING_Sleep(1000);
    }

    LogPrintf("\r\n[KING]======== Socket_test complete ========\r\n");
}

static void multiDns_test_thread(void *param)
{
    MSG_S msg;
    int ret = -1;

    LogPrintf("KING multiDns_test_thread is running. dnsThreadID = 0x%X\r\n", dnsThreadID);
    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        LogPrintf("multiDns_test_thread!\r\n");
        if (dnsMsgHandle != NULL)
        {
            LogPrintf("multiDns_test_thread KING_MsgRcv!\r\n");
            ret = KING_MsgRcv(&msg, dnsMsgHandle, WAIT_OPT_INFINITE);
            if (ret == -1)
            {
                LogPrintf("multiDns_test_thread ret=%d\r\n", ret);
                continue;
            }
            LogPrintf("multiDns_test_thread receive msg. MsgId=%d\r\n", msg.messageID);
            if (msg.messageID == 220)
            {
                multiDns_startTest();
            }
            else if (msg.messageID == 221)
            {
                multiDns_enter(DNS_TEST_SERVER);
            }
        }
    }
    
    multiDns_releaseContextHandle();
    if (dnsMsgHandle != NULL)
    {
        KING_MsgDelete(dnsMsgHandle);
        dnsMsgHandle = NULL;
    }
    if (dnsThreadID != NULL)
    {
        dnsThreadID = NULL; 
    }
    KING_ThreadExit();
}

void multiDns_test(void)
{
    int ret = -1;
    MSG_S msg;
    THREAD_ATTR_S threadAttr;
    
    LogPrintf("multiDns_test is KING_MsgCreate ");
    ret = KING_MsgCreate(&dnsMsgHandle);
    LogPrintf("multiDns_test is KING_MsgCreate ret %d", ret);
    if(-1 == ret)
    {
        LogPrintf("multiDns_test KING_MsgCreate, errcode=0x%x\r\n", KING_GetLastErrCode());
    }

    KING_Sleep(1000);

    LogPrintf("multiDns_test is KING_ThreadCreate ");
    memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
    threadAttr.fun = multiDns_test_thread;
    threadAttr.priority = THREAD_PRIORIT1;
    threadAttr.stackSize = 1024 * 4;
    threadAttr.queueNum = 64; 
    
    LogPrintf("multiDns_test is create socketThreadID 0x%08x", &dnsThreadID);
    ret = KING_ThreadCreate("multiDnsThread", &threadAttr, &dnsThreadID);
    if (-1 == ret)
    {
        LogPrintf("[KING]Create Test Thread Failed!! Errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    KING_Sleep(1000);
    
    LogPrintf("APP Main is KING_MsgSend ");
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = 220;
    ret = KING_MsgSend(&msg, dnsMsgHandle);
    if(-1 == ret)
    {
        LogPrintf("KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
}

