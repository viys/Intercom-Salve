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
#include "httpTls_api.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("https: "fmt, ##args); } while(0)

#define HTTPS_URL_AUTHMODE_1 "https://test.cheerzing.com:9451/same" //单向认证
#define HTTPS_URL_AUTHMODE_2 "https://test.cheerzing.com:9449/same" //双向认证

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static hardwareHandle hHttpsNIC = -1;
static softwareHandle hHttpsContext = -1;
static THREAD_HANDLE httpsThreadID = NULL;
static MSG_HANDLE    httpsMsgHandle = NULL;

const char https_test_ca_crt[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDdTCCAl2gAwIBAgIJAMzI/v8QATp2MA0GCSqGSIb3DQEBCwUAMFExGjAYBgNV\r\n"
"BAMMEXd3dy5jaGVlcnppbmcuY29tMQswCQYDVQQGEwJRSjETMBEGA1UECAwKU29t\r\n"
"ZS1TdGF0ZTERMA8GA1UECgwISW50ZXJuZXQwHhcNMjAwNzI4MDA1ODAwWhcNMzAw\r\n"
"NzI2MDA1ODAwWjBRMRowGAYDVQQDDBF3d3cuY2hlZXJ6aW5nLmNvbTELMAkGA1UE\r\n"
"BhMCUUoxEzARBgNVBAgMClNvbWUtU3RhdGUxETAPBgNVBAoMCEludGVybmV0MIIB\r\n"
"IjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4lxjX9JY4rrK1nlOWVJZpUhC\r\n"
"gODzp2SbX+WREml4pRWSBjyEB4Uj63da5kgynjXevUgJbuNEnMpBcRxDWoeOG3z+\r\n"
"naRFadJxbKaam0EPVEPraSYzGX9lSQd9oKFpidwRQ98OiN5eduOYDhFtrESI7jHZ\r\n"
"iMzCUDf7ZTk9Rp87VkbpABG8feqi9cM8hxp0pBcyIYZ+IEVENEG9I2zz6G2ikNHu\r\n"
"d3hzUky/vH/D7/OQgawDlWJsrWDWeagqmzkXFceOabaXdmtYpnKGah3uLGHOtSYh\r\n"
"AI8yNMjpF6TwbrcVLa63+2qnuuL7rL738fugrZX3UYhSkxW4Bb/kvHduFlA2QQID\r\n"
"AQABo1AwTjAdBgNVHQ4EFgQU3vaKWeYsqTC9QdyEgqojx3N3Mf4wHwYDVR0jBBgw\r\n"
"FoAU3vaKWeYsqTC9QdyEgqojx3N3Mf4wDAYDVR0TBAUwAwEB/zANBgkqhkiG9w0B\r\n"
"AQsFAAOCAQEAvi4mLcMXSt9XkFLBAoXGYhu0CUDruVr188/q/53Bif7gjBT8gvnJ\r\n"
"Y3aj8w9gC9BYCKyVQYAsmfM9pGEN0YfCiKjY1OV9NJtQstBv2JcPLuXy6OFw/TR6\r\n"
"neIM0WyBTF7PPjE2zqN0cKivc1n2RKQPD/tNgPKGP79HJEX2sNa76wHHYrswAH33\r\n"
"g7uWmshJciI97IKMJMnTUsh+aJ/PXqSiYgOD/4yAcVVeyKZ17m/jOhRTvjmwNga+\r\n"
"iUwHKdlUvoIhoebch0Pu4HJhE0i661mIKTAVfyEIwH3PzmXcHrl3UZh1tIYMKLpQ\r\n"
"QquGQ5aB8yKjQKve+eANLQCysAEvxgamqQ==\r\n"
"-----END CERTIFICATE-----";

const uint32 https_test_ca_crt_len = sizeof( https_test_ca_crt );

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
static int https_getNicHandle(void)
{
    int ret = 0;
    NIC_INFO info;

    memset(&info, 0x00, sizeof(NIC_INFO));
    info.NIC_Type = NIC_TYPE_WWAN;
    ret = KING_HardwareNICRequest(info, &hHttpsNIC);
    LogPrintf("https_getNicHandle ret=%d, hHttpsNIC=%d", ret, hHttpsNIC);

    return ret;
}

static int https_getContextHandle(int isIpv6)
{
    int ret = 0;
    NIC_CONTEXT ApnInfo;

    if (hHttpsNIC == -1)
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
    ret = KING_ConnectionAcquire(hHttpsNIC, ApnInfo, &hHttpsContext);
    LogPrintf("https_getContextHandle ret=%d, hHttpsContext=%d", ret, hHttpsContext);

    return ret;
}

static void https_get_callback(uint16 errCode, int16 httprspcode, uint32 contentLen)
{
    LogPrintf("https_get_callback errCode : %d\r\n", errCode);
    LogPrintf("https_get_callback httprspcode : %d\r\n", httprspcode);
    LogPrintf("https_get_callback contentLen : %u\r\n", contentLen);
    if (errCode == 0 && httprspcode == HTTP_RESPONSE_CODE_OK)
    {
        MSG_S msg;
        int ret = -1;
        
        memset(&msg, 0x00, sizeof(MSG_S));
        msg.messageID = 122;
        ret = KING_MsgSend(&msg, httpsMsgHandle);
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

static void https_getTest(void)
{
    int ret = -1;
    HTTPTLS_CRT_T httptls_crt = {0};   
    
    ret = httpTls_client_init();
    if (ret == -1)
    {
        LogPrintf("httpTls_client_init fail!");
        return;
    }
    ret = httpTls_setAuthmode(1);//设置单向认证
    if (ret == -1)
    {
        LogPrintf("httpTls_setAuthmode fail!");
        return;
    }
    httptls_crt.crt = (unsigned char *)https_test_ca_crt;
    httptls_crt.len = https_test_ca_crt_len;
    ret = httpTls_setCert((HTTPTLS_CRT_T *)(&httptls_crt));
    if (ret == -1)
    {
        LogPrintf("httpTls_setCert fail!");
        return;
    }
    ret = http_urlParse(HTTPS_URL_AUTHMODE_1);
    if (ret == -1)
    {
        LogPrintf("https_getTest ret: %d", ret);
        return;
    }
    http_get(hHttpsContext, NULL, 0, 60, (HTTP_RSP_CALLBACK)https_get_callback);
}

static void https_test(int isIpv6)
{
    int ret = -1;
    NET_STATE_E state;
    
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

            if (isIpv6)
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
            https_getNicHandle();
            https_getContextHandle(isIpv6);
            KING_Sleep(1000);
            https_getTest();
            LogPrintf("https_test complete\r\n");
            break;
        }
        
        KING_Sleep(1000);
    }
}

static void https_test_thread(void *param)
{
    MSG_S msg;
    int ret = -1;

    LogPrintf("https_test_thread is running. httpsThreadID = 0x%X\r\n", httpsThreadID);

    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        LogPrintf("https_test_thread!\r\n");
        if (httpsMsgHandle != NULL)
        {
            LogPrintf("https_test_thread KING_MsgRcv!\r\n");
            ret = KING_MsgRcv(&msg, httpsMsgHandle, WAIT_OPT_INFINITE);
            if (ret == -1)
            {
                LogPrintf("https_test_thread ret=%d\r\n", ret);
                continue;
            }
            LogPrintf("https_test_thread receive msg. MsgId=%d\r\n", msg.messageID);
            if (msg.messageID == 120)
            {
                https_test(0);
            }
            else if (msg.messageID == 121)
            {
                https_getTest();
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
                        LogPrintf("https_test_thread pHeaderStr : %s", pHeaderStr);
                        LogPrintf("https_test_thread headLen : %d", headLen);
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
                    LogPrintf("https_test_thread readEnd : %d", readEnd);
                    LogPrintf("https_test_thread pData : %s", pData);
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

void https_startTest(void)
{
    MSG_S msg;
    int ret = -1;
    THREAD_ATTR_S threadAttr;
    
    ret = KING_MsgCreate(&httpsMsgHandle);
    LogPrintf("https_startTest is KING_MsgCreate ret %d", ret);
    if(-1 == ret)
    {
        LogPrintf("KING_MsgCreate, errcode=0x%x\r\n", KING_GetLastErrCode());
    }

    KING_Sleep(1000);

    LogPrintf("https_startTest is KING_ThreadCreate ");
    memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
    threadAttr.fun = https_test_thread;
    threadAttr.priority = THREAD_PRIORIT1;
    threadAttr.stackSize = 1024*5;
    threadAttr.queueNum = 64; 
    
    LogPrintf("https_startTest is create thread 0x%08x", &httpsThreadID);
    ret = KING_ThreadCreate("httpsTestThread", &threadAttr, &httpsThreadID);
    if (-1 == ret)
    {
        LogPrintf("Create Test Thread Failed!! Errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    KING_Sleep(1000);
    
    LogPrintf("https_startTest is KING_MsgSend ");
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = 120;
    ret = KING_MsgSend(&msg, httpsMsgHandle);
    if(-1 == ret)
    {
        LogPrintf("KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
}
