/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "kingcstd.h"
#include "kingsocket.h"
#include "kingplat.h"
#include "kingrtos.h"
#include "kingnet.h"
#include "httpclient.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("http: "fmt, ##args); } while(0)

#define HTTP_URL "http://117.29.183.70:13783/http.php"
#define HTTPS_URL "https://117.29.183.70:8003/http.php"

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static hardwareHandle hHttpNIC = -1;
static softwareHandle hHttpContext = -1;
static THREAD_HANDLE httpThreadID = NULL;
static MSG_HANDLE    httpMsgHandle = NULL;
static int type = 1;
static int isHttps = 0;

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
static int http_getNicHandle(void)
{
    int ret = 0;
    NIC_INFO info;

    memset(&info, 0x00, sizeof(NIC_INFO));
    info.NIC_Type = NIC_TYPE_WWAN;
    ret = KING_HardwareNICRequest(info, &hHttpNIC);
    LogPrintf("http_getNicHandle ret=%d, hHttpNIC=%d", ret, hHttpNIC);

    return ret;
}

static int http_getContextHandle(int isIpv6)
{
    int ret = 0;
    NIC_CONTEXT ApnInfo;

    if (hHttpNIC == -1)
    {
        return -1;
    }
    memset(&ApnInfo, 0x00, sizeof(NIC_CONTEXT));
    if (isIpv6)
    {
        ApnInfo.IpType = IP_ADDR_TYPE_V6;
    }
    else
    {
        ApnInfo.IpType = IP_ADDR_TYPE_V4;
    }
    ret = KING_ConnectionAcquire(hHttpNIC, ApnInfo, &hHttpContext);
    LogPrintf("http_getContextHandle ret=%d, hHttpContext=%d", ret, hHttpContext);

    return ret;
}

static void http_get_callback(uint16 errCode, int16 httprspcode, uint32 contentLen)
{
    LogPrintf("http_get_callback errCode : %d\r\n", errCode);
    LogPrintf("http_get_callback httprspcode : %d\r\n", httprspcode);
    LogPrintf("http_get_callback contentLen : %u\r\n", contentLen);
    if (errCode == 0 && httprspcode == HTTP_RESPONSE_CODE_OK)
    {
        MSG_S msg;
        int ret = -1;
        
        memset(&msg, 0x00, sizeof(MSG_S));
        msg.messageID = 122;
        ret = KING_MsgSend(&msg, httpMsgHandle);
        if(-1 == ret)
        {
            LogPrintf("KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
            return;
        }
        #if 0
        if (type == 1)
        {
            MSG_S msg;
            
            memset(&msg, 0x00, sizeof(MSG_S));
            msg.messageID = 121;
            ret = KING_MsgSend(&msg, httpMsgHandle);
            if(-1 == ret)
            {
                LogPrintf("KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
                return;
            }
        }
        #endif
    }
}

static void http_getTest(void)
{
    int ret = -1;

    if (isHttps)
    {
        ret = http_urlParse(HTTPS_URL);
    }
    else
    {
        ret = http_urlParse(HTTP_URL);
    }
    LogPrintf("http_getTest ret : %d\r\n", ret);
    if (ret == -1)
    {
        return;
    }
    type = 0;
    http_get(hHttpContext, NULL, 0, 60, (HTTP_RSP_CALLBACK)http_get_callback);
}

static void http_postTest(void)
{
    char *pData = NULL;
    uint8 count = 19;
    uint32 dataLen = 0;
    int ret = -1;
    
    if (isHttps)
    {
        ret = http_urlParse(HTTPS_URL);
    }
    else
    {
        ret = http_urlParse(HTTP_URL);
    }
    LogPrintf("http_postTest ret : %d\r\n", ret);
    if (ret == -1)
    {
        return;
    }
    pData = malloc(1024 * 2 + 1);
    if (pData == NULL)
    {
        return;
    }
    memset(pData, 0x00, 1024 * 2 + 1);
    memcpy(pData + dataLen, "Message=qwertyuioplkmg34fdgasrt09876554321qazxcvbnmjfjjjjljgdsswerzxcvasdfgasdfgsdfgsdfgsdfgsdfg", 96);
    while(count)
    {
        dataLen +=96;
        memcpy(pData + dataLen, "Mesmjyreqwertyuioplkmh34fdg1qwe09876554321qazxcvbnmhfjjjjljgdsswerzxcvasdfgasdfgsdfgsdfgsdfgsdfg", 96);
        count--;
    }
    dataLen +=96;
    LogPrintf("http_postTest dataLen : %d\r\n", dataLen);
    type = 1;
    http_post(hHttpContext, pData, dataLen, 60, (HTTP_RSP_CALLBACK)http_get_callback);
    if (pData != NULL)
    {
        free(pData);
        pData = NULL;
    }
}

static void http_test(int isIpv6)
{
    int ret = -1;
    NET_STATE_E state;
    
    while (1)
    {
        ret = KING_NetStateGet(&state);
        LogPrintf("http Get net state %d, ret : %d\r\n", state, ret);
        if (ret == 0 && state > NET_NO_SERVICE && state < NET_EMERGENCY)
        {
            NET_CESQ_S Cesq;
            ret = KING_NetSignalQualityGet(&Cesq);
            LogPrintf("http Get sinagal ret is %d. rxlev:%d, ber:%d, rscp:%d, ecno:%d, rsrq:%d, rsrp:%d\r\n",
                ret,Cesq.rxlev,Cesq.ber,Cesq.rscp,Cesq.ecno,Cesq.rsrq,Cesq.rsrp);

            if (isIpv6)
            {
                ret = KING_NetApnSet(1, "", NULL, NULL, 0, IP_ADDR_TYPE_V6);
            }
            else
            {
                ret = KING_NetApnSet(1, "", NULL, NULL, 0, IP_ADDR_TYPE_V4);
            }
            LogPrintf("http KING_NetApnSet ret : %d\r\n", ret);
            if (ret != 0)
            {
                LogPrintf("http KING_NetApnSet fail\r\n");
                break;
            }
            http_getNicHandle();
            http_getContextHandle(isIpv6);
            KING_Sleep(1000);
            if (type == 0)
            {
                http_getTest();
            }
            else
            {
                http_postTest();
            }
            LogPrintf("http_test complete\r\n");
            break;
        }
        
        KING_Sleep(1000);
    }
}

static void http_test_thread(void *param)
{
    MSG_S msg;
    int ret = -1;

    LogPrintf("http_test_thread is running. httpThreadID = 0x%X\r\n", httpThreadID);

    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        LogPrintf("http_test_thread!\r\n");
        if (httpMsgHandle != NULL)
        {
            LogPrintf("http_test_thread KING_MsgRcv!\r\n");
            ret = KING_MsgRcv(&msg, httpMsgHandle, WAIT_OPT_INFINITE);
            if (ret == -1)
            {
                LogPrintf("http_test_thread ret=%d\r\n", ret);
                continue;
            }
            LogPrintf("http_test_thread receive msg. MsgId=%d\r\n", msg.messageID);
            if (msg.messageID == 120)
            {
                http_test(0);
            }
            else if (msg.messageID == 121)
            {
                if (type == 0)
                {
                    http_getTest();
                }
                else
                {
                    http_postTest();
                }
            }
            else if (msg.messageID == 122)
            {
                char *pData = NULL;
                uint8 readEnd = 0;
                
                if(http_rspHeaderGet() == 1)
                {
                    char *pHeaderStr = NULL;
                    uint32 headLen = 0;
                    
                    http_readHeaderData(&pHeaderStr, &headLen);
                    if (headLen > 0)
                    {
                        LogPrintf("http_get_callback pHeaderStr : %s", pHeaderStr);
                        LogPrintf("http_get_callback headLen : %d", headLen);
                    }
                }
                
                pData = malloc(1024 + 1);
                if (pData == NULL)
                {
                    return;
                }
                while (!readEnd)
                {
                    memset(pData, 0x00, 1024 + 1);
                    ret = http_readContentData(60, 1024, pData, &readEnd);
                    if (ret != 0)
                    {
                        free(pData);
                        pData = NULL;
                        break;
                    }
                    LogPrintf("http_get_callback readEnd : %d", readEnd);
                    LogPrintf("http_get_callback pData : %s", pData);
                }
                if (pData != NULL)
                {
                    free(pData);
                    pData = NULL;
                }
            }
        }
    }
}

void http_startTest(void)
{
    MSG_S msg;
    int ret = -1;
    THREAD_ATTR_S threadAttr;
    
    ret = KING_MsgCreate(&httpMsgHandle);
    LogPrintf("http_startTest is KING_MsgCreate ret %d", ret);
    if(-1 == ret)
    {
        LogPrintf("KING_MsgCreate, errcode=0x%x\r\n", KING_GetLastErrCode());
    }

    KING_Sleep(1000);

    LogPrintf("http_startTest is KING_ThreadCreate ");
    memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
    threadAttr.fun = http_test_thread;
    threadAttr.priority = THREAD_PRIORIT1;
    threadAttr.stackSize = 1024*5;
    threadAttr.queueNum = 64; 
    
    LogPrintf("http_startTest is create thread 0x%08x", &httpThreadID);
    ret = KING_ThreadCreate("httpTestThread", &threadAttr, &httpThreadID);
    if (-1 == ret)
    {
        LogPrintf("Create Test Thread Failed!! Errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    KING_Sleep(1000);
    
    LogPrintf("http_startTest is KING_MsgSend ");
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = 120;
    ret = KING_MsgSend(&msg, httpMsgHandle);
    if(-1 == ret)
    {
        LogPrintf("http KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
}
