/*******************************************************************************
 ** File Name:   httpClient.c
 ** Author:      Allance.Chen
 ** Date:        2020-06-24
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains functions about http.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-06-24     Allance.Chen         Create.
 ******************************************************************************/
/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "kingRtos.h"
#include "kingCStd.h"
#include "kingPlat.h"
#include "kingNet.h"
#include "kingsocket.h"
#include "kingCbData.h"
#include "httpclient.h"
#include "httptls_api.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define HTTP_URL_MAX_LEN                1024

#define HTTP_DEFAULT_PORT               80
#define HTTPS_DEFAULT_PORT              443

#define HTTP_DEF_HEAD_MAX_SIZE          (400 + HTTP_URL_MAX_LEN)
#define HTTP_TX_BUF_DEFAULT_SIZE        (1460 * 3)
#define HTTP_RX_BUF_DEFAULT_SIZE        1460
#define HTTP_BODY_MAX_SIZE              2048
#define HTTP_RX_RSP_LEN                 2048
#define HTTP_HEX_STR_MAX_SIZE           8
#define HTTP_CHUNK_TAIL_SIZE            (HTTP_HEX_STR_MAX_SIZE + 4 + 1) //the tail data is less than sizeof(CRLF + check_hex + CRLF)

#define HTTP_OPERATE_SUCC               0

#define HTTP_SOCK_RECV_TIMEOUT          10000

#define HTTP_HEADER_KEY_LEN             64
#define HTTP_HEADER_VALUE_LEN           128
#define HTTP_HEADER_KV_MAX              8


/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef enum httpActionType{
    HTTP_ACTION_NONE = 0,
    HTTP_ACTION_URL,
    HTTP_ACTION_GET,
    HTTP_ACTION_POST,
    HTTP_ACTION_READ,

    HTTP_ACTION_NUM
} HTTP_ACTION_TYPE_E;

typedef enum httpReceiveStatusType{
    HTTP_RECEIVE_HEADER = 0,
    HTTP_RECEIVE_CONTENT,
    HTTP_RECEIVE_CHUNK,
    HTTP_RECEIVE_COMPLETE
} HTTP_RECEIVE_STATUS_TYPE_E;

typedef struct uriInfo {
    char* url;
    char* uri;
    char* host;
    uint32 port;
    bool isHttps;
} URI_INFO_S;

typedef struct {
    char key[HTTP_HEADER_KEY_LEN];
    char value[HTTP_HEADER_VALUE_LEN];
} HTTP_HEADER_KV_S;

typedef struct httpStatusInfo {
    int socketFd;
    softwareHandle hContext;

    HTTP_ACTION_TYPE_E actionType;
    HTTP_STATE_TYPE_E  stateType;

    int16 rspcode;

    char *pBody;
    volatile uint32 bodySize;
    HTTP_CONTENT_TYPE_E contentType;

    uint8 reqHeader;
    uint8 rspHeader;

    HTTP_HEADER_KV_S reqHeaderKv[HTTP_HEADER_KV_MAX];

    char *pHeaderptr;
    uint32 headerlen;

    uint32 sendTotalLen;
    uint32 unsendLen;

    TIMER_HANDLE rspTimer;
    uint32 rspTimeout;
    bool rspReach;
    uint32 waitTimeout;

    uint32 receiveSize;
    uint32 bodyBufLen;
    bool isContentLen;
    HTTP_RECEIVE_STATUS_TYPE_E receiveStatus;

    bool isRunning;

    HTTP_RSP_CALLBACK rspCb;
} HTTP_STATUS_INFO_S;

typedef struct httpChunkTailBuf {
    char buf[HTTP_CHUNK_TAIL_SIZE];
    uint32 cnt;
} HTTP_CHUNK_TAIL_BUF_T;


/*******************************************************************************
 ** Variables
 ******************************************************************************/
static bool isHttpInited = FALSE;
static URI_INFO_S urlInfo;
static HTTP_STATUS_INFO_S httpInfo;
static SEM_HANDLE httpSem = NULL;
static int isHttpRunning = 0;
static int isHttpSucc = 0;
static SOCK_IN_ADDR_T httpHostAddr;
static char *pHttpTxBuf = NULL;
static char *pHttpRxBuf = NULL;
static uint32 httpRxLen = 0;
static uint32 httpCurChunkLen = 0;
static uint16 httpErrCode = 0;
static uint32 readCurLen = 0;
static char *pReadData = NULL;
static uint32 gReadLen = 0;
static HTTP_CHUNK_TAIL_BUF_T gHttpChunkTail; //to handle tail size;


/*******************************************************************************
 ** Local Function Delcarations
 ******************************************************************************/
static void http_getOrPostReqHandle(void);
static bool http_decodeRspHeader(char *rxBuf, uint32 bufsize);


/*******************************************************************************
 ** Local Function
 ******************************************************************************/
static void http_urlClear(URI_INFO_S *pUrlInfo)
{
    if (pUrlInfo == NULL)
    {
        return;
    }

    if (pUrlInfo->url!= NULL)
    {
        free(pUrlInfo->url);
        pUrlInfo->url = NULL;
    }

    if (pUrlInfo->host != NULL)
    {
        free(pUrlInfo->host);
        pUrlInfo->host = NULL;
    }

    if (pUrlInfo->uri != NULL)
    {
        free(pUrlInfo->uri);
        pUrlInfo->uri = NULL;
    }

    pUrlInfo->port = 80;
    pUrlInfo->isHttps = FALSE;
}

static int http_createSeam(void)
{
    int ret = 0;

    if (NULL == httpSem)
    {
        ret = KING_SemCreate("HTTP SEM", 0, (SEM_HANDLE *)&httpSem);
        if (ret < 0 || httpSem == NULL)
        {
            KING_SysLog("%s: KING_SemCreate() fail", __FUNCTION__);
            return -1;
        }
    }

    return ret;
}

static int http_getSem(void)
{
    isHttpRunning = 1;
    KING_SemGet(httpSem, WAIT_OPT_INFINITE);
    isHttpRunning = 0;
    KING_SemDelete(httpSem);
    httpSem = NULL;

    if (isHttpSucc == 0)
    {
        return -1;
    }

    isHttpSucc = 0;

    return 0;
}

static void http_clearBuf(void)
{
    if (httpInfo.pHeaderptr != NULL)
    {
        free(httpInfo.pHeaderptr);
        httpInfo.pHeaderptr = NULL;
        httpInfo.headerlen = 0;
    }

    if (httpInfo.pBody != NULL)
    {
        free(httpInfo.pBody);
        httpInfo.pBody = NULL;
        httpInfo.bodySize = 0;
        httpInfo.bodyBufLen = 0;
    }
}

static bool http_closeSocket(bool isSslclose)
{
    int result = 0;

    if (httpInfo.socketFd == -1)
    {
        return TRUE;
    }

#ifdef KING_SDK_MBEDTLS_SUPPORT    
    if (isSslclose)
    {
        result = httpTls_close();
        if(-1 == result)
        {
            return FALSE;
        }
    }
#endif

    result = KING_SocketClose(httpInfo.socketFd);
    if(-1 == result)
    {
        return FALSE;
    }

    KING_UnRegNotifyFun(DEV_CLASS_SOCKET, httpInfo.socketFd, NULL);
    httpInfo.socketFd = -1;

    return TRUE;
}

static void http_rspTimeOut(uint32 tmrId)
{
    if (isHttpRunning)
    {
        isHttpSucc = 0;
        KING_SemPut(httpSem);
    }

    httpInfo.rspReach = TRUE;
}

static void http_rspTimer(bool isSet)
{
    int ret = 0;

    httpInfo.rspReach = FALSE;

    if (isSet)
    {
        TIMER_ATTR_S timerattr;

        if (httpInfo.rspTimer == NULL)
        {
            ret = KING_TimerCreate(&httpInfo.rspTimer);
            if(-1 == ret)
            {
                KING_SysLog("%s: Failed!", __FUNCTION__);
                return;
            }
        }
        memset(&timerattr, 0x00, sizeof(timerattr));
        timerattr.isPeriod = FALSE;
        timerattr.timeout = httpInfo.rspTimeout * 1000;
        timerattr.timer_fun = http_rspTimeOut;
        ret = KING_TimerActive(httpInfo.rspTimer, &timerattr);
        if (-1 == ret)
        {
            KING_SysLog("%s: Active Failed!", __FUNCTION__);
        }
    }
    else
    {
        if (httpInfo.rspTimer != NULL)
        {
            KING_TimerDeactive(httpInfo.rspTimer);
            KING_TimerDelete(httpInfo.rspTimer);
            httpInfo.rspTimer = NULL;
        }
    }
}

static void http_resetStatus(bool isSslclose)
{
    http_rspTimer(FALSE);
    http_clearBuf();

    httpInfo.isRunning = FALSE;

    if (pHttpRxBuf != NULL)
    {
        free(pHttpRxBuf);
        pHttpRxBuf = NULL;
        httpRxLen = 0;
    }
    if (pHttpTxBuf != NULL)
    {
        free(pHttpTxBuf);
        pHttpTxBuf = NULL;
    }

    if (httpInfo.stateType >= HTTP_STATE_URL_READY)
    {
        httpInfo.stateType = HTTP_STATE_URL_READY;
    }
    else
    {
        httpInfo.stateType = HTTP_STATE_NONE;
    }

    httpInfo.receiveSize = 0;

    http_closeSocket(isSslclose);
}

static void http_socketProcessCb(uint32 eventID, void* pdata, uint32 len)
{
    //SOCKET_EVENT_DATA_S *data = (SOCKET_EVENT_DATA_S *)pdata;

    KING_SysLog("%s: eventID=%d", __FUNCTION__, eventID);
    switch (eventID)
    {
        case SOCKET_CONNECT_EVENT_RSP:
        {
            KING_SysLog("http SOCKET_CONNECT_EVENT_RSP");
            isHttpSucc = 1;
            KING_SemPut(httpSem);
        }
            break;

        case SOCKET_ACCEPT_EVENT_IND:
            //KING_SysLog("http SOCKET_ACCEPT_EVENT_IND");
            break;

        case SOCKET_CLOSE_EVENT_RSP:
        {
            KING_SysLog("http SOCKET_CLOSE_EVENT_RSP");
        }
            break;

        case SOCKET_ERROR_EVENT_IND:
            KING_SysLog("http SOCKET_ERROR_EVENT_IND\r\n");
            if (isHttpRunning)
            {
                isHttpSucc = 0;
                KING_SemPut(httpSem);
            }
            break;

        case SOCKET_WRITE_EVENT_IND:
            //KING_SysLog("http SOCKET_WRITE_EVENT_IND ack by peer uSendDataSize=%d\r\n", data->availSize);
            break;

        case SOCKET_READ_EVENT_IND:
            //KING_SysLog("http SOCKET_READ_EVENT_IND ack by peer uSendDataSize=%d\r\n", data->availSize);
            break;

        case SOCKET_FULL_CLOSED_EVENT_IND:
            KING_SysLog("http SOCKET_FULL_CLOSED_EVENT_IND\r\n");
            if (isHttpRunning)
            {
                isHttpSucc = 0;
                KING_SemPut(httpSem);
            }
            break;

        default:
            KING_SysLog("http unexpect event/response %d\r\n", eventID);
            break;
    }
}

static int http_socketCreate(int addrType)
{
    int ret = 0;
    int family = 0;
    int val = 1;
    int val_len = sizeof(int);
    union sockaddr_aligned local_addr;
    uint32 localaddr_size = 0;

    //KING_SysLog("%s: family=%d\r\n", __FUNCTION__, family);
    if (httpInfo.socketFd != -1)
    {
        http_closeSocket(TRUE);
    }
    
#ifdef KING_SDK_MBEDTLS_SUPPORT
    if (urlInfo.isHttps)
    {
        if (httTls_getState() < HTTP_TLS_STATE_CREATED)
        {
            httpTls_client_init();
        }
    }
#endif

    if (addrType == IP_ADDR_TYPE_V6)
    {
        family = AF_INET6;
    }
    else
    {
        family = AF_INET;
    }
    ret = KING_SocketCreate(family, SOCK_STREAM, IPPROTO_TCP, &httpInfo.socketFd);
    if (ret == -1 || httpInfo.socketFd < 0)
    {
        KING_SysLog("%s: KING_SocketCreate fail!", __FUNCTION__);
        return -1;
    }

    val = 0;
    ret = KING_SocketOptSet( httpInfo.socketFd, 0, SO_BIO, &val, 0 );
    if (ret < 0)
    {
        KING_SysLog("http KING_SocketOptSet() SO_BIO fail !!!");
    }

    KING_RegNotifyFun(DEV_CLASS_SOCKET, httpInfo.socketFd, http_socketProcessCb);
    //KING_Sleep(200);

    val = 1;
    ret = KING_SocketOptSet(httpInfo.socketFd, IPPROTO_TCP, TCP_NODELAY, &val, val_len);
    if (ret < 0)
    {
        KING_SysLog("http KING_SocketOptSet() TCP_NODELAY fail !!!");
    }
    //KING_Sleep(200);

    val = HTTP_SOCK_RECV_TIMEOUT;
    ret = KING_SocketOptSet(httpInfo.socketFd, SOL_SOCKET, SO_RCVTIMEO, &val, val_len);
    if (ret < 0)
    {
        KING_SysLog("http KING_SocketOptSet() SO_RCVTIMEO fail !!!");
    }

    if (family == AF_INET6)
    {
        struct sockaddr_in6 *stLocalAddr6 = (struct sockaddr_in6 *)&(local_addr);
        IPV6_ADDR_S ip6;

        ret = KING_NetIpv6AddrInfoGet(httpInfo.hContext, &ip6, NULL, NULL);
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

        ret = KING_NetIpv4AddrInfoGet(httpInfo.hContext, &ip, NULL, NULL);
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
        ret = KING_SocketBind(httpInfo.socketFd, &local_addr, localaddr_size);
        if (ret < 0)
        {
            KING_SysLog("http KING_SocketBind fail !!!");
            return -1;
        }
    }
    else
    {
        KING_SysLog("%s: bind fail\n", __FUNCTION__);
        return -1;
    }

    return 0;
}

static int http_socketConnect(SOCK_IN_ADDR_T *addr)
{
    int ret = 0;
    union sockaddr_aligned sock_addr;
    uint32 sockaddr_size = 0;

    if (addr->type == IP_ADDR_TYPE_V6)
    {
        struct sockaddr_in6 *sock_addr6 = (struct sockaddr_in6 *)&(sock_addr);

        sock_addr6->sin6_len = sizeof(struct sockaddr_in6);
        sock_addr6->sin6_family = AF_INET6;
        sock_addr6->sin6_port = KING_htons(urlInfo.port);
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
        sock_addr4->sin_port = KING_htons(urlInfo.port);
        sock_addr4->sin_addr.addr = addr->u_addr.ip4.addr;

        sockaddr_size = sizeof(SOCK_ADDR_IN_T);
    }

    if (isHttpRunning == 1)
    {
        return -1;
    }

    ret = http_createSeam();
    if (ret != 0)
    {
        return -1;
    }

    ret = KING_SocketConnect(httpInfo.socketFd, &sock_addr, sockaddr_size);
    if (ret < 0)
    {
        KING_SysLog("%s: connect fail !!!", __FUNCTION__);
        return -1;
    }

    ret = http_getSem();
    if (ret != 0)
    {
        return -1;
    }

    return 0;
}

static bool http_rxBuf(char* input, int32 size)
{
    if (input == NULL || size == 0)
    {
        return FALSE;
    }

    if (pHttpRxBuf != NULL)
    {
        free(pHttpRxBuf);
        pHttpRxBuf = NULL;
        httpRxLen = 0;
    }

    pHttpRxBuf = malloc(size + 1);
    if (pHttpRxBuf == NULL)
    {
        return FALSE;
    }

    httpRxLen = size;
    memset(pHttpRxBuf, 0x00, (httpRxLen + 1));
    memcpy(pHttpRxBuf, input, httpRxLen);

    return TRUE;
}

static bool http_parseRedirection(char *buf, char *pLocbuf, int len)
{
    char *ptr;
    char *endptr;

    if (buf == NULL || strlen(buf) == 0)
    {
        return FALSE;
    }

    ptr = strstr(buf, "Location:");
    if (ptr != NULL)
    {
        ptr += strlen("Location:");
        while ((ptr != NULL) && (*ptr == ' '))
            ptr++;

        endptr = strstr(ptr, "\r\n");
        if ((endptr - ptr) <= len)
        {
            strncpy(pLocbuf, ptr, endptr - ptr);
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

static void http_resultRsp(bool isSucc)
{
    //KING_SysLog("%s: isSucc=%d, isContentLen=%d", __FUNCTION__, isSucc, httpInfo.isContentLen);
    if (httpInfo.isContentLen)
    {
        httpInfo.rspCb(httpErrCode, httpInfo.rspcode, httpInfo.bodySize);
    }
    else
    {
        httpInfo.rspCb(httpErrCode, httpInfo.rspcode, 0);
    }
}

static void http_resultRspHandler(bool isSucc)
{
    //KING_SysLog("%s: isSucc=%d", __FUNCTION__, isSucc);
    httpInfo.isRunning = FALSE;
    //if (!isSucc)
    //{
        //http_resetStatus(urlInfo.isHttps);
    //}
    //else
    //{
        if (httpInfo.receiveStatus == HTTP_RECEIVE_COMPLETE)
        {
            http_closeSocket(urlInfo.isHttps);
        }
    //}
    httpInfo.stateType = HTTP_STATE_URL_READY;

    http_resultRsp(isSucc);
}

static void http_redirectRspStatus(void)
{
    char *pLocation = NULL;

    pLocation = malloc(HTTP_URL_MAX_LEN + 1);
    if (pLocation == NULL)
    {
        httpErrCode = HTTP_ALLOC_MEM_FAIL;
        http_resultRspHandler(FALSE);
        return;
    }

    memset(pLocation, 0x00, HTTP_URL_MAX_LEN + 1);
    if (http_parseRedirection(pHttpRxBuf, pLocation, HTTP_URL_MAX_LEN))
    {
        if (strcmp(pLocation, urlInfo.url) != 0)
        {
            if (http_urlParse(pLocation) != 0)
            {
                if (pLocation != NULL)
                {
                    free(pLocation);
                    pLocation = NULL;
                }
                http_resultRspHandler(FALSE);
                return;
            }

            if (pLocation != NULL)
            {
                free(pLocation);
                pLocation = NULL;
            }

            http_getOrPostReqHandle();
        }
        else
        {
            if (pLocation != NULL)
            {
                free(pLocation);
                pLocation = NULL;
            }
            httpInfo.rspcode = HTTP_RESPONSE_CODE_DNS_ERROR;
            http_resultRspHandler(FALSE);
            return;
        }
    }
    else
    {
        if (pLocation != NULL)
        {
            free(pLocation);
            pLocation = NULL;
        }
        httpInfo.rspcode = HTTP_RESPONSE_CODE_METHOD_NOT_ALLOWED;
        http_resultRspHandler(FALSE);
        return;
    }
}

static void http_rspStatus(void)
{
    //KING_SysLog("%s: start! rspcode=%d", __FUNCTION__, httpInfo.rspcode);
    http_rspTimer(FALSE);
    switch(httpInfo.rspcode)
    {
        case HTTP_RESPONSE_CODE_OK: // Success
            http_resultRspHandler(TRUE);
            break;

        case HTTP_RESPONSE_CODE_MOVED_PERMANENTLY: /* Permanent redirect */
        case HTTP_RESPONSE_CODE_PERMANENTLY_REDIRECT: /* Permanent redirect */
        case HTTP_RESPONSE_CODE_MULTIPLE_CHOICES: /* Multiple choices - Choose first one; treat it as temp redirect */
        case HTTP_RESPONSE_CODE_FOUND: /* temp redirect */
        case HTTP_RESPONSE_CODE_SEE_OTHER: /* temp redirect */
        case HTTP_RESPONSE_CODE_TEMPORARY_REDIRECT: /* temp redirect */
            http_redirectRspStatus();
            break;

        case HTTP_RESPONSE_CODE_UNAUTHORIZED: // fallthrough
        case HTTP_RESPONSE_CODE_PROXY_AUTHENTICATION_REQUIRED: // Authenticate
            http_resultRspHandler(FALSE);
            break;

        default:
            http_resultRspHandler(FALSE);
            break;
    }

    if (pHttpRxBuf != NULL)
    {
        free(pHttpRxBuf);
        pHttpRxBuf = NULL;
        httpRxLen = 0;
    }
}

static bool http_contentAppend(char* input, int32 size)
{
    int32 new_size = 0;

    if (input == NULL || size == 0)
    {
        httpErrCode = HTTP_UNKNOWN_ERR;
        return FALSE;
    }

    new_size = httpInfo.receiveSize + size;
    //KING_SysLog("%s: new_size=%u", __FUNCTION__, new_size);
    if (new_size > httpInfo.bodySize)
    {
        httpErrCode = HTTP_UNKNOWN_ERR;
        return FALSE;
    }
    //KING_SysLog("%s: stateType=%u", __FUNCTION__, httpInfo.stateType);
    if (httpInfo.stateType == HTTP_STATE_READ)
    {
        if (httpInfo.actionType == HTTP_ACTION_READ)
        {
            //KING_SysLog("%s: gReadLen=%u", __FUNCTION__, gReadLen);
            memcpy(pReadData + gReadLen, input, size);
            gReadLen += size;
        }
    }
    else
    {
        memcpy(httpInfo.pBody + httpInfo.receiveSize, input, size);
    }
    httpInfo.receiveSize += size;

    //KING_SysLog("%s: 11 receiveSize=%u", __FUNCTION__, httpInfo.receiveSize);
    if (httpInfo.receiveSize == httpInfo.bodySize)
    {
        httpInfo.receiveStatus = HTTP_RECEIVE_COMPLETE;
    }

    if (httpInfo.receiveSize > httpInfo.bodySize)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static bool http_chunkAppend(char* input, int32 size)
{
    char hex_str[HTTP_HEX_STR_MAX_SIZE + 1] = {0};
    char *iss = NULL;
    char *token = NULL;
    uint32 remaining = 0;
    uint32 hex_value = 0;
    uint32 offset = 0, tempLen = 0;
    uint32 tail_space;

    //KING_SysLog("%s: size=%u\n", __FUNCTION__, size);
    if (input == NULL || size == 0)
    {
        httpErrCode = HTTP_UNKNOWN_ERR;
        return FALSE;
    }

    iss = input;
    remaining = size;
    //KING_SysLog("%s: remaining=%u\n", __FUNCTION__, remaining);

    if (gHttpChunkTail.cnt > 0)
    {
        //there is tail data in the buffer;
        tail_space = (HTTP_CHUNK_TAIL_SIZE - 1) - gHttpChunkTail.cnt;
        tempLen = (tail_space < size) ? tail_space : size;
        memcpy(gHttpChunkTail.buf + gHttpChunkTail.cnt, iss, tempLen);
        gHttpChunkTail.buf[gHttpChunkTail.cnt + tempLen] = '\0';
        if (('\r' == gHttpChunkTail.buf[0]) && ('\n' == gHttpChunkTail.buf[1]))
        {
            KING_SysLog("%s: tail leading with 0x0D0A", __FUNCTION__);
            //this happens only when the last time only '\r' is left
            //so the '\n' is received this time.
            iss++;
            memset(&gHttpChunkTail, 0x00, sizeof(HTTP_CHUNK_TAIL_BUF_T));
            size -= 1;
            remaining = size;
        }
        else
        {
            token = strstr(gHttpChunkTail.buf, "\r\n");
            if (NULL != token)
            {
                if(HTTP_HEX_STR_MAX_SIZE < (token - gHttpChunkTail.buf))
                {
                    KING_SysLog("%s: tail Hex string too long len:%d\n", __FUNCTION__, (token - gHttpChunkTail.buf));
                    return FALSE;
                }
                memset(hex_str, 0, HTTP_HEX_STR_MAX_SIZE + 1);
                memcpy(hex_str, gHttpChunkTail.buf, token - gHttpChunkTail.buf);
                hex_value = strtoul(hex_str, NULL, 16);
                KING_SysLog("%s: tail chunk size :%s(%d), ptr = %d, cnt = %d\n", __FUNCTION__,
                            hex_str, hex_value, token - gHttpChunkTail.buf, gHttpChunkTail.cnt);
                httpCurChunkLen = hex_value;
                httpInfo.bodySize = hex_value;

                //calcute iss start point
                tempLen = token + 2 - gHttpChunkTail.buf - gHttpChunkTail.cnt;
                iss += tempLen;
                size -= tempLen;
                remaining = size;
                memset(&gHttpChunkTail, 0x00, sizeof(HTTP_CHUNK_TAIL_BUF_T));
            }
            else
            {
                //no enough data
                KING_SysLog("%s: no enough tail data\n", __FUNCTION__);
                gHttpChunkTail.cnt += tempLen;
                return TRUE;
            }
        }
    }

    if (httpCurChunkLen > 0)
    {
        uint32 rxLen = 0;

        KING_SysLog("%s: httpCurChunkLen=%u\n", __FUNCTION__, httpCurChunkLen);
        if (httpCurChunkLen <= size)
        {
            rxLen = httpCurChunkLen;
        }
        else
        {
            rxLen = size;
        }
        if (httpInfo.stateType == HTTP_STATE_READ)
        {
            if (httpInfo.actionType == HTTP_ACTION_READ)
            {
                memcpy(pReadData + gReadLen, iss, rxLen);
                gReadLen += rxLen;
            }
        }
        else
        {
            memcpy(httpInfo.pBody + httpInfo.receiveSize, iss, rxLen);
        }
        iss += rxLen;
        httpCurChunkLen -= rxLen;
        httpInfo.receiveSize += rxLen;
        remaining -= rxLen;
        if (httpCurChunkLen == 0)
        {
            if (remaining >= 2)
            {
                iss += 2;
                remaining -= 2;
            }
        }
    }

    // Loop <chunk_hex>\r\n<content>\r\n
    while ((httpCurChunkLen == 0) && (NULL != (token = strstr((const char*)iss, "\r\n"))))
    {
        if (httpInfo.rspReach)
        {
            httpErrCode = HTTP_WAIT_HTTP_RSP_TIMEOUT;
            return FALSE;
        }
        if (HTTP_HEX_STR_MAX_SIZE < (token - iss))
        {
            KING_SysLog("%s: - Hex string too long len:%d\n", __FUNCTION__, (token - iss));
            httpErrCode = HTTP_UNKNOWN_ERR;
            return FALSE;
        }
        if (token == iss)
        {
            KING_SysLog("%s: lead with 0D0A\n", __FUNCTION__);
            iss += 2;
            remaining -= 2;
            continue;
        }
        memset(hex_str, 0, HTTP_HEX_STR_MAX_SIZE+1);
        memcpy(hex_str, iss, (token - iss));
        hex_value = strtoul(hex_str, NULL, 16);
        KING_SysLog("%s: hex_value = %u", __FUNCTION__, hex_value);
        httpCurChunkLen = hex_value;
        httpInfo.bodySize = hex_value;
        // offset = hex str len + chunk size + CRLF*2
        offset = ((token - iss) + hex_value + 4);
        if (hex_value == 0 && remaining == offset)
        {
            KING_SysLog("%s - Chunked transfer complete\n", __FUNCTION__);
            httpInfo.receiveStatus = HTTP_RECEIVE_COMPLETE;
            return TRUE;
        }

        if (remaining < offset)
        {
            if (remaining >= (offset - 2))
            {
                tempLen = hex_value;
                remaining = remaining - (token - iss) - 2 - hex_value;
            }
            else
            {
                tempLen = remaining - (token - iss) - 2;
                remaining = 0;
            }
        }
        else
        {
            tempLen = hex_value;
            remaining = remaining - hex_value - (token - iss) - 2;
        }

        token += 2; // skip CRLF
        if (tempLen > 0)
        {
            uint32 read_size = (tempLen < httpCurChunkLen) ? tempLen : httpCurChunkLen;

            //KING_SysLog("%s: read_size =%u", __FUNCTION__, read_size);
            //KING_SysLog("%s: receiveSize =%u", __FUNCTION__, httpInfo.receiveSize);
            if (httpInfo.stateType == HTTP_STATE_READ)
            {
                if (httpInfo.actionType == HTTP_ACTION_READ)
                {
                    memcpy(pReadData + gReadLen, token, read_size);
                    gReadLen += read_size;
                }
            }
            else
            {
                memcpy(httpInfo.pBody + httpInfo.receiveSize, token, read_size);
            }
            httpInfo.receiveSize += read_size;
            //KING_SysLog("%s: 11 receiveSize = %u, bodySize = %u", __FUNCTION__, httpInfo.receiveSize, httpInfo.bodySize);
            httpCurChunkLen -= read_size;
            tempLen -= read_size;
            token += read_size;
        }

        iss = token;
        if (httpCurChunkLen == 0)
        {
            KING_SysLog("%s: 11 remaining=%u\n", __FUNCTION__, remaining);
            if (remaining >= 2)
            {
                iss += 2;
                remaining -= 2;
            }
        }
    }

    if (remaining > 0)
    {
        if (remaining > (HTTP_CHUNK_TAIL_SIZE - 1))
        {
            KING_SysLog("%s: remain data error!!!\n", __FUNCTION__);
            for (tempLen = 0; tempLen < remaining; tempLen++)
            {
                KING_SysLog("%s: remain data [%d]=%02X!!!\n", __FUNCTION__, tempLen, iss[tempLen]);
            }
        }
        else
        {
            //one chunk has been received, but there are tailing data in the buffer
            memcpy(gHttpChunkTail.buf, iss, remaining);
            gHttpChunkTail.cnt += remaining;
            KING_SysLog("%s: save tail len=%u, d[0]=0x%02X\n", __FUNCTION__, gHttpChunkTail.cnt, iss[0]);
        }
    }
    KING_SysLog("%s: - Chunked transfer incomplete", __FUNCTION__);
    return TRUE;
}

static bool http_appendRsp(char* input, int32 size)
{
    char *err_info = "";

    //KING_SysLog("%s: receiveStatus = %d", __FUNCTION__, httpInfo.receiveStatus);
    switch (httpInfo.receiveStatus)
    {
        case HTTP_RECEIVE_CONTENT:
        {
            if (!http_contentAppend(input, size))
            {
                err_info = "Content append failed";
                break;
            }
        }
            break;

        case HTTP_RECEIVE_CHUNK:
        {
            if (!http_chunkAppend(input, size))
            {
                err_info = "Content append failed";
                break;
            }
        }
            break;

        case HTTP_RECEIVE_COMPLETE:
            break;

        default:
            break;
    }

    if (strlen(err_info) > 0)
    {
        return FALSE;
    }

    return TRUE;
}

static int http_ParseRspCode(char* buf)
{
    char *ptr;
    char *endptr;
    int rspcode = 0;

    if ((0 != strncmp(buf, "HTTP/1.0", 8)) &&
        (0 != strncmp(buf, "HTTP/1.1", 8)))
    {
        return HTTP_RESPONSE_CODE_UNKNOW;
    }
    ptr = buf + strlen("HTTP/1.1");
    while(*ptr == ' ')
        ptr++;
    if (ptr != NULL)
    {
        rspcode = strtol(ptr, &endptr, 10);
    }
    else
    {
        rspcode = HTTP_RESPONSE_CODE_UNKNOW;
    }

    return rspcode;
}

static bool http_decodeRspHeader(char *rxBuf, uint32 bufsize)
{
    char *ptr = NULL;
    char *endptr = NULL;
    char *header_end = NULL;
    int32 header_size = 0;

    //KING_SysLog("%s: bufsize=%u", __FUNCTION__, bufsize);
    if (rxBuf == NULL || bufsize == 0)
    {
        httpErrCode = HTTP_UNKNOWN_ERR;
        return FALSE;
    }

    ptr = strstr(rxBuf, "Content-Length:");
    if (ptr != NULL)
    {
        ptr += strlen("Content-Length:");
        while ((ptr != NULL) && (*ptr == ' '))
            ptr++;
        if (ptr != NULL)
        {
            httpInfo.bodySize = strtoul(ptr, &endptr, 10);
            //KING_SysLog("%s: bodySize = %u", __FUNCTION__, httpInfo.bodySize);
            httpInfo.receiveStatus = HTTP_RECEIVE_CONTENT;
            httpInfo.isContentLen = TRUE;
        }
    }
    else if (NULL != (ptr = strstr(rxBuf, "Transfer-Encoding:")))
    {
        ptr += strlen("Transfer-Encoding:");
        while ((ptr != NULL) && (*ptr == ' '))
            ptr++;
        if (0 == strncmp(ptr, "chunked", strlen("chunked")))
        {
            httpInfo.receiveStatus = HTTP_RECEIVE_CHUNK;
            ptr = strstr(ptr, "\r\n\r\n");
            if (ptr != NULL)
            {
                ptr += strlen("\r\n\r\n");
                if (ptr != NULL)
                {
                    httpInfo.bodySize = strtoul(ptr, &endptr, 16);
                    //KING_SysLog("%s: bodySize = %u", __FUNCTION__, httpInfo.bodySize);
                }
            }

            httpInfo.isContentLen = FALSE;
        }
    }

    header_end = strstr(rxBuf, "\r\n\r\n");
    if (httpInfo.pHeaderptr != NULL)
    {
        free(httpInfo.pHeaderptr);
        httpInfo.pHeaderptr = NULL;
        httpInfo.headerlen = 0;
    }
    httpInfo.headerlen = header_end - rxBuf;

    if (httpInfo.headerlen > 0)
    {
        httpInfo.pHeaderptr= malloc(httpInfo.headerlen + 1);
        if (httpInfo.pHeaderptr == NULL)
        {
            KING_SysLog("http malloc for header fail !!");
            httpInfo.headerlen = 0;
            httpErrCode = HTTP_ALLOC_MEM_FAIL;
            httpInfo.rspcode = HTTP_RESPONSE_CODE_NO_MEMORY;
            return FALSE;
        }
        KING_SysLog("http set head: len = %d", httpInfo.headerlen);
        memset(httpInfo.pHeaderptr, 0x00, (httpInfo.headerlen + 1));
        memcpy(httpInfo.pHeaderptr, rxBuf, httpInfo.headerlen);
    }
    header_end += 4;
    header_size = header_end - rxBuf;

    //KING_SysLog("%s: receiveStatus = %d", __FUNCTION__, httpInfo.receiveStatus);
    if (httpInfo.receiveStatus == HTTP_RECEIVE_CONTENT)
    {
        if (httpInfo.bodySize == 0)
        {
            httpInfo.receiveStatus = HTTP_RECEIVE_COMPLETE;
            return TRUE;
        }
    }
    else if (httpInfo.receiveStatus == HTTP_RECEIVE_COMPLETE)
    {
        return TRUE;
    }

    //KING_SysLog("%s: header_size=%u", __FUNCTION__, header_size);
    if (bufsize >= header_size)
    {
        int32 body_buf = 0;

        if (httpInfo.pBody != NULL)
        {
            free(httpInfo.pBody);
            httpInfo.pBody = NULL;
        }

        if (httpInfo.receiveStatus == HTTP_RECEIVE_CHUNK)
        {
            body_buf = HTTP_TX_BUF_DEFAULT_SIZE + 8;
        }
        else
        {
            body_buf = (httpInfo.bodySize > HTTP_TX_BUF_DEFAULT_SIZE) ? (HTTP_TX_BUF_DEFAULT_SIZE + 8) : (httpInfo.bodySize + 8);
        }
        httpInfo.pBody = malloc(body_buf + 1);
        if (httpInfo.pBody == NULL)
        {
            httpInfo.rspcode = HTTP_RESPONSE_CODE_NO_MEMORY;
            httpErrCode = HTTP_ALLOC_MEM_FAIL;
            return FALSE;
        }
        memset(httpInfo.pBody, 0x00, (body_buf + 1));

        if (bufsize > header_size)
        {
            http_appendRsp(header_end, bufsize - header_size);
        }
    }

    return TRUE;
}

static void http_parsePostData(void)
{
    char *ptr;
    char *tempBody = NULL;

    if (httpInfo.bodySize == 0 || httpInfo.pBody == NULL)
    {
        return;
    }

    ptr = strstr(httpInfo.pBody, "\r\n\r\n");
    if (ptr != NULL)
    {
        if (httpInfo.pHeaderptr != NULL)
        {
            free(httpInfo.pHeaderptr);
            httpInfo.pHeaderptr = NULL;
            httpInfo.headerlen = 0;
        }
        httpInfo.headerlen = ptr - httpInfo.pBody + 2;
        httpInfo.pHeaderptr = malloc(httpInfo.headerlen + 1);
        if (httpInfo.pHeaderptr == NULL)
        {
            httpInfo.headerlen = 0;
            return;
        }
        memset(httpInfo.pHeaderptr, 0x00, httpInfo.headerlen + 1);
        memcpy(httpInfo.pHeaderptr, httpInfo.pBody, httpInfo.headerlen);

        ptr += strlen("\r\n\r\n");
        httpInfo.bodySize = httpInfo.bodySize - (ptr - httpInfo.pBody);
        if (httpInfo.bodySize == 0)
        {
            return;
        }

        tempBody = malloc(httpInfo.bodySize + 1);
        if (tempBody == NULL)
        {
            return;
        }
        memset(tempBody, 0x00, httpInfo.bodySize + 1);
        memcpy(tempBody, ptr, httpInfo.bodySize);
        memset(httpInfo.pBody, 0x00, httpInfo.headerlen + httpInfo.bodySize + 1);
        memcpy(httpInfo.pBody, tempBody, httpInfo.bodySize);
        if (tempBody != NULL)
        {
            free(tempBody);
            tempBody = NULL;
        }
    }
}

static int http_constructRequestPack(void)
{
    int size = 0;
    int i = 0;

    if (pHttpTxBuf != NULL)
    {
        free(pHttpTxBuf);
        pHttpTxBuf = NULL;
    }

    if (httpInfo.actionType == HTTP_ACTION_GET)
    {
        pHttpTxBuf = malloc(HTTP_DEF_HEAD_MAX_SIZE + httpInfo.headerlen + 1);
        if (pHttpTxBuf != NULL)
        {
            memset(pHttpTxBuf, 0x00, (HTTP_DEF_HEAD_MAX_SIZE + httpInfo.headerlen + 1));
        }
    }
    else
    {
        pHttpTxBuf = malloc(HTTP_DEF_HEAD_MAX_SIZE + httpInfo.bodySize + 1);
        if (pHttpTxBuf != NULL)
        {
            memset(pHttpTxBuf, 0x00, (HTTP_DEF_HEAD_MAX_SIZE + httpInfo.bodySize + 1));
        }
    }
    if (pHttpTxBuf == NULL)
    {
        return -1;
    }

    if (httpInfo.reqHeader == 0)
    {
        if (httpInfo.actionType == HTTP_ACTION_GET)
        {
            size = sprintf(pHttpTxBuf, "GET %s HTTP/1.1\r\n", urlInfo.uri);
        }
        else
        {
            size = sprintf(pHttpTxBuf, "POST %s HTTP/1.1\r\n", urlInfo.uri);
        }
        size += sprintf(pHttpTxBuf + size, "Host: %s", urlInfo.host);

        if ((!urlInfo.isHttps && urlInfo.port != HTTP_DEFAULT_PORT) ||
            (urlInfo.isHttps && urlInfo.port != HTTPS_DEFAULT_PORT))
        {
            size += sprintf(pHttpTxBuf + size, ":");
            size += sprintf(pHttpTxBuf + size, "%u", urlInfo.port);
        }
        size += sprintf(pHttpTxBuf + size, "\r\n");

        size += sprintf(pHttpTxBuf + size, "Connection: keep-alive\r\n");

        if (httpInfo.contentType == HTTP_CONTENT_APPLICATION_URLENCODED)
        {
            size += sprintf(pHttpTxBuf + size, "Content-Type: %s\r\n", "application/x-www-form-urlencoded");
        }
        else if (httpInfo.contentType == HTTP_CONTENT_TEXT_PLAIN)
        {
            size += sprintf(pHttpTxBuf + size, "Content-Type: %s\r\n", "text/plain");
        }
        else if (httpInfo.contentType == HTTP_CONTENT_APPLICATION_OCTET_STREAM)
        {
            size += sprintf(pHttpTxBuf + size, "Content-Type: %s\r\n", "application/octet-stream");
        }
        else if (httpInfo.contentType == HTTP_CONTENT_MULIPART_FORM_DATA)
        {
            size += sprintf(pHttpTxBuf + size, "Content-Type: %s\r\n", "multipart/form-data; boundary="HTTP_BOUNDARY_STR);
        }
        else if (httpInfo.contentType == HTTP_CONTENT_APPLICATION_JSON)
        {
            size += sprintf(pHttpTxBuf + size, "Content-Type: %s\r\n", "application/json");
        }
        else if (httpInfo.contentType == HTTP_CONTENT_AUDIO_MP3)
        {
            size += sprintf(pHttpTxBuf + size, "Content-Type: %s\r\n", "audio/mp3");
        }

        for (i = 0; i < HTTP_HEADER_KV_MAX; i++)
        {
            if (0 == strlen(httpInfo.reqHeaderKv[i].key))
            {
                break;
            }
            size += sprintf(pHttpTxBuf + size, "%s: %s\r\n", httpInfo.reqHeaderKv[i].key, httpInfo.reqHeaderKv[i].value);
        }
    }

    if (httpInfo.actionType == HTTP_ACTION_GET)
    {
        if (httpInfo.reqHeader == 1)
        {
            if (httpInfo.pHeaderptr != NULL && httpInfo.headerlen > 0)
            {
                memcpy(pHttpTxBuf + size, httpInfo.pHeaderptr, httpInfo.headerlen);
                size += httpInfo.headerlen;
            }
        }
    }
    else
    {
        if (httpInfo.reqHeader == 1)
        {
            http_parsePostData();
            if (httpInfo.pHeaderptr != NULL && httpInfo.headerlen > 0)
            {
                memcpy(pHttpTxBuf + size, httpInfo.pHeaderptr, httpInfo.headerlen);
                size += httpInfo.headerlen;
            }
        }
        else // 如果调用者提供整个 HTTP 报文，自然包含 Content-Length 域。
        {
            size += sprintf(pHttpTxBuf + size, "Content-Length: %u\r\n", httpInfo.bodySize);
        }
    }

    size += sprintf(pHttpTxBuf + size, "\r\n");  // end of http header

    if (httpInfo.actionType == HTTP_ACTION_POST)
    {
        if (httpInfo.pBody != NULL && httpInfo.bodySize > 0)
        {
            memcpy(pHttpTxBuf + size, httpInfo.pBody, httpInfo.bodySize);
            size += httpInfo.bodySize;
        }
    }

    httpInfo.sendTotalLen = size;
    httpInfo.unsendLen = httpInfo.sendTotalLen;
    KING_SysLog("%s: sendTotalLen = %u", __FUNCTION__, httpInfo.sendTotalLen);
    //KING_SysLog("%s:  pHttpTxBuf:%s", __FUNCTION__, pHttpTxBuf);
    httpInfo.stateType = HTTP_STATE_SEND;

    return 0;
}

static int http_sendRequestPack(void)
{
    int ret = -1;
    int send_len = 0;
    char *send_ptr;

    KING_SysLog("%s: sendTotalLen = %u, unsendLen = %u", __FUNCTION__,
                httpInfo.sendTotalLen, httpInfo.unsendLen);

    if (httpInfo.socketFd == -1)
    {
        return -1;
    }

    while (httpInfo.unsendLen > 0)
    {
        send_ptr = pHttpTxBuf + (httpInfo.sendTotalLen - httpInfo.unsendLen);
    #ifdef KING_SDK_MBEDTLS_SUPPORT
        if (urlInfo.isHttps)
        {
            send_len = httpTls_write(httpInfo.socketFd, send_ptr, httpInfo.unsendLen);
            //KING_SysLog("%s: send_len = %d, unsendLen = %u", __FUNCTION__,
                            //send_len, httpInfo.unsendLen);
        }
        else
    #endif        
        {
            send_len = KING_SocketSend(httpInfo.socketFd, (void *)send_ptr,
                                       (size_t)httpInfo.unsendLen, 0);
            //KING_SysLog("%s: send_len = %d, unsendLen = %u", __FUNCTION__,
                            //send_len, httpInfo.unsendLen);
        }

        if (send_len != -1)
        {
            if (httpInfo.rspReach)
            {
                httpInfo.sendTotalLen = 0;
                httpInfo.unsendLen = 0;

                if (pHttpTxBuf != NULL)
                {
                    free(pHttpTxBuf);
                    pHttpTxBuf = NULL;
                }
                return -1;
            }

            httpInfo.unsendLen -= send_len;
            //KING_SysLog("%s: unsend_len = %u", __FUNCTION__, httpInfo.unsendLen);
            if (httpInfo.unsendLen == 0)
            {
                httpInfo.sendTotalLen = 0;
                httpInfo.unsendLen = 0;

                if (pHttpTxBuf != NULL)
                {
                    free(pHttpTxBuf);
                    pHttpTxBuf = NULL;
                }
                return 0;
            }
            else
            {
                KING_SysLog("%s: sendTotalLen = %u, unsendLen = %u", __FUNCTION__,
                            httpInfo.sendTotalLen, httpInfo.unsendLen);
                KING_Sleep(200);
            }
        }
        else
        {
            KING_SysLog("%s: send fail", __FUNCTION__);
            httpInfo.sendTotalLen = 0;
            httpInfo.unsendLen = 0;
            if (pHttpTxBuf != NULL)
            {
                free(pHttpTxBuf);
                pHttpTxBuf = NULL;
            }
            ret = -1;
        }
    }

    return ret;
}

static void http_receiveRsp(void)
{
    int bytes_read = 0;
    uint16 errcode = 0;
    char *pBuffer = NULL;
    char *ptr = NULL;
    char *pTempBuffer = NULL;
    uint32 tempLen = 0;
    
    //KING_SysLog("%s: start!", __FUNCTION__);
    pBuffer = malloc(HTTP_RX_BUF_DEFAULT_SIZE + 1);
    if (pBuffer == NULL)
    {
        http_resetStatus(urlInfo.isHttps);
        httpErrCode = HTTP_ALLOC_MEM_FAIL;
        httpInfo.rspCb(httpErrCode, HTTP_RESPONSE_CODE_UNKNOW, 0);
        return;
    }

    while(1)
    {
        if (httpInfo.socketFd == -1)
        {
            errcode = HTTP_NW_ERR;
            break;
        }

        if (httpInfo.rspReach)
        {
            errcode = HTTP_WAIT_HTTP_RSP_TIMEOUT;
            break;
        }

        memset(pBuffer, 0x00, HTTP_RX_BUF_DEFAULT_SIZE + 1);
    #ifdef KING_SDK_MBEDTLS_SUPPORT        
        if(urlInfo.isHttps)
        {
            bytes_read = httpTls_read(httpInfo.socketFd, pBuffer, HTTP_RX_BUF_DEFAULT_SIZE, 10 * 1000);
            //KING_SysLog("%s: recv returned %d bytes", __FUNCTION__, bytes_read);
        }
        else
    #endif            
        {
            bytes_read = KING_SocketRecv(httpInfo.socketFd, pBuffer, HTTP_RX_BUF_DEFAULT_SIZE, 0);
            //KING_SysLog("%s: recv returned %d bytes", __FUNCTION__, bytes_read);
        }

        if (bytes_read == 0)
        {
            errcode = HTTP_SOCK_CLOSE;
            break;
        }
        else if (bytes_read < 0)
        {
            errcode = HTTP_SOCK_RD_ERR;
            break;
        }

        if (httpInfo.receiveStatus == HTTP_RECEIVE_HEADER)
        {
            if (pTempBuffer == NULL)
            {
                pTempBuffer = malloc(tempLen + bytes_read + 1);
            }
            else
            {
                pTempBuffer = realloc(pTempBuffer, tempLen + bytes_read + 1);
            }
            memset(pTempBuffer + tempLen, 0x00, (bytes_read + 1));
            memcpy(pTempBuffer + tempLen, pBuffer, bytes_read);
            tempLen += bytes_read;
            ptr = strstr(pTempBuffer, "\r\n\r\n");
            if (ptr == NULL)
            {
                continue;
            }
            
            httpInfo.rspcode = http_ParseRspCode(pTempBuffer);
            if((httpInfo.rspcode == HTTP_RESPONSE_CODE_OK) ||
                (httpInfo.rspcode == HTTP_RESPONSE_CODE_PARTIAL_CONTENT))
            {
                if (!http_decodeRspHeader(pTempBuffer, tempLen))
                {
                    errcode = HTTP_NW_ERR;
                    goto EXIT;
                }
                if ((httpInfo.receiveStatus == HTTP_RECEIVE_COMPLETE) ||
                    (httpInfo.receiveStatus == HTTP_RECEIVE_CONTENT &&
                        httpInfo.receiveSize >= HTTP_RX_RSP_LEN &&
                        httpInfo.bodySize >= httpInfo.receiveSize &&
                        httpInfo.receiveStatus != HTTP_RECEIVE_COMPLETE) ||
                    (httpInfo.receiveStatus == HTTP_RECEIVE_CHUNK &&
                        httpInfo.receiveSize >= HTTP_RX_RSP_LEN &&
                        httpInfo.receiveStatus != HTTP_RECEIVE_COMPLETE))
                {
                    httpErrCode = HTTP_OPERATE_SUCC;
                    httpInfo.bodyBufLen = httpInfo.receiveSize;
                    //KING_SysLog("%s: bodyBufLen=%d", __FUNCTION__, httpInfo.bodyBufLen);
                    http_rspStatus();
                    break;
                }
            }
            else
            {
                http_rxBuf(pTempBuffer, tempLen);
                http_rspStatus();
                break;
            }
        }
        else
        {
            if (!http_appendRsp(pBuffer, bytes_read))
            {
                errcode = HTTP_NW_ERR;
                goto EXIT;
            }
            if ((httpInfo.receiveStatus == HTTP_RECEIVE_COMPLETE) ||
                (httpInfo.receiveStatus == HTTP_RECEIVE_CONTENT &&
                    httpInfo.receiveSize >= HTTP_RX_RSP_LEN &&
                    httpInfo.bodySize >= httpInfo.receiveSize &&
                    httpInfo.receiveStatus != HTTP_RECEIVE_COMPLETE) ||
                (httpInfo.receiveStatus == HTTP_RECEIVE_CHUNK &&
                    httpInfo.receiveSize >= HTTP_RX_RSP_LEN &&
                    httpInfo.receiveStatus != HTTP_RECEIVE_COMPLETE))
            {
                httpErrCode = HTTP_OPERATE_SUCC;
                httpInfo.bodyBufLen = httpInfo.receiveSize;
                //KING_SysLog("%s: bodyBufLen=%d", __FUNCTION__, httpInfo.bodyBufLen);
                http_rspStatus();
                break;
            }
        }
    }

    if (pBuffer != NULL)
    {
        free(pBuffer);
        pBuffer = NULL;
    }
    if(pTempBuffer != NULL)
    {
        free(pTempBuffer);
        pTempBuffer = NULL;
        tempLen = 0;
    }
    //KING_SysLog("%s: errcode=%d", __FUNCTION__, errcode);
    if (errcode != 0)
    {
        http_resetStatus(urlInfo.isHttps);
        httpErrCode = errcode;
        httpInfo.rspCb(httpErrCode, HTTP_RESPONSE_CODE_UNKNOW, 0);
        return;
    }
    return;

EXIT:
    if(pBuffer != NULL)
    {
        free(pBuffer);
        pBuffer = NULL;
    }
    if(pTempBuffer != NULL)
    {
        free(pTempBuffer);
        pTempBuffer = NULL;
        tempLen = 0;
    }
    http_resetStatus(urlInfo.isHttps);
    httpInfo.rspcode = HTTP_RESPONSE_CODE_NETWORK_ERROR;
    httpErrCode = errcode;
    httpInfo.rspCb(httpErrCode, httpInfo.rspcode, 0);
}

static int http_readreceiveRsp(uint32 dataLen, uint32 *realSize)
{
    int bytes_read = 0;
    uint16 errcode = HTTP_OPERATE_SUCC;
    uint32 recv_cnt = 0, readSize = 0;
    char *pBuffer = NULL;
    int selectfd = -1, nfds = -1;
    fd_set Fdset;
    struct timeval timev = {httpInfo.waitTimeout, 0};

    //KING_SysLog("%s: start! dataLen=%d", __FUNCTION__, dataLen);
    pBuffer = malloc(HTTP_RX_BUF_DEFAULT_SIZE + 1);
    if(pBuffer == NULL)
    {
        http_resetStatus(urlInfo.isHttps);
        return HTTP_ALLOC_MEM_FAIL;
    }
    gReadLen = 0;
    //KING_SysLog("%s: socketFd=%d!\n", __FUNCTION__, httpInfo.socketFd);
    while(recv_cnt < dataLen)
    {
        if (httpInfo.socketFd == -1)
        {
            errcode = HTTP_NW_ERR;
            break;
        }
        selectfd = httpInfo.socketFd + 1;
        FD_ZERO(&Fdset);
        FD_SET(httpInfo.socketFd, &Fdset);
        if ((nfds = KING_SocketSelect(selectfd, &Fdset, NULL, NULL, &timev)) == -1)
        {
            KING_SysLog("s: nfds=%d", __FUNCTION__, nfds);
            errcode = HTTP_NW_ERR;
            break;
        }

        if (nfds == 0)
        {
            KING_SysLog("%s: nfds=%d", __FUNCTION__, nfds);
            errcode = HTTP_TIMEOUT;
            break;
        }
        memset(pBuffer, 0x00, HTTP_RX_BUF_DEFAULT_SIZE + 1);
        if (dataLen - recv_cnt > HTTP_RX_BUF_DEFAULT_SIZE)
        {
            readSize = HTTP_RX_BUF_DEFAULT_SIZE;
        }
        else
        {
            readSize = dataLen - recv_cnt;
        }
    #ifdef KING_SDK_MBEDTLS_SUPPORT        
        if(urlInfo.isHttps)
        {
            bytes_read = httpTls_read(httpInfo.socketFd, pBuffer, readSize, httpInfo.waitTimeout * 1000);
            //KING_SysLog("%s: recv returned %d bytes", __FUNCTION__, bytes_read);
        }
        else
    #endif            
        {
            bytes_read = KING_SocketRecv(httpInfo.socketFd, pBuffer, readSize, 0);
            //KING_SysLog("%s: recv returned %d bytesd", __FUNCTION__, bytes_read);
        }

        if (bytes_read == 0)
        {
            httpInfo.receiveStatus = HTTP_RECEIVE_COMPLETE;
            if (httpInfo.stateType == HTTP_STATE_READ)
            {
                goto RESULT_OK;
            }
            break;
        }
        else if (bytes_read == -1)
        {
            KING_SysLog("%s: read fail", __FUNCTION__);
            errcode = HTTP_SOCK_RD_ERR;
            break;
        }
        recv_cnt += bytes_read;
        KING_SysLog("%s: read=%d, cnt=%d", __FUNCTION__, bytes_read, recv_cnt);
        if (!http_appendRsp(pBuffer, bytes_read))
        {
            errcode = httpErrCode;
            break;
        }

        if (httpInfo.stateType == HTTP_STATE_READ && httpInfo.receiveStatus == HTTP_RECEIVE_COMPLETE)
        {
            *realSize = gReadLen;
            goto RESULT_OK;
        }
    }

    if(pBuffer != NULL)
    {
        free(pBuffer);
        pBuffer = NULL;
    }
    if (errcode != HTTP_OPERATE_SUCC)
    {
        http_resetStatus(urlInfo.isHttps);
        return errcode;
    }
    *realSize = gReadLen;
    //KING_SysLog("%s: read return 1", __FUNCTION__);
    return 1;

RESULT_OK:
    if(pBuffer != NULL)
    {
        free(pBuffer);
        pBuffer = NULL;
    }
    http_resetStatus(urlInfo.isHttps);
    return 0;
}

static void http_actionSendData(void)
{
    int ret = -1;

    //KING_SysLog("%s: actionType=%d\n", __FUNCTION__, httpInfo.actionType);
    switch (httpInfo.actionType)
    {
        case HTTP_ACTION_GET:
        case HTTP_ACTION_POST:
            if (http_constructRequestPack() != 0)
            {
                KING_SysLog("%s: http_constructRequestPack ret=%d\n", __FUNCTION__, ret);
                http_resetStatus(urlInfo.isHttps);
                httpErrCode = HTTP_ALLOC_MEM_FAIL;
                ret = -1;
                break;
            }

            ret = http_sendRequestPack();
            if (httpInfo.rspReach)
            {
                KING_SysLog("%s: timeout ret=%d\n", __FUNCTION__, ret);
                http_resetStatus(urlInfo.isHttps);
                httpErrCode = HTTP_WAIT_HTTP_RSP_TIMEOUT;
                ret = -1;
                break;
            }

            if (ret == 0)
            {
                httpInfo.stateType = HTTP_STATE_RECEIVE_START;
                httpInfo.receiveStatus = HTTP_RECEIVE_HEADER;
                httpInfo.receiveSize = 0;
                http_receiveRsp();
            }
            else if (ret == -1)
            {
                KING_SysLog("%s: http_sendRequestPack ret=%d\n", __FUNCTION__, ret);
                http_resetStatus(urlInfo.isHttps);
                httpErrCode = HTTP_SOCK_WR_ERR;
                ret = -1;
            }
            break;

        default:
            break;
    }

    if (ret == -1)
    {
        httpInfo.rspCb(httpErrCode, HTTP_RESPONSE_CODE_UNKNOW, 0);
    }
}

static int http_socketCreatAndConnect(void)
{
    uint16 errcode = 0;
    int ret = -1, retVal = -1;

    ret = http_socketCreate(httpHostAddr.type);
    if (ret == -1)
    {
        KING_SysLog("%s: create ret=%d\n", __FUNCTION__, ret);
        errcode = HTTP_SOCK_CREATE_ERR;
        retVal = -1;
        goto EXIT;
    }

    if (httpInfo.rspReach)
    {
        errcode = HTTP_WAIT_HTTP_RSP_TIMEOUT;
        retVal = -1;
        goto EXIT;
    }

    ret = http_socketConnect(&httpHostAddr);
    if (ret == 0)
    {
        if (httpInfo.rspReach)
        {
            errcode = HTTP_WAIT_HTTP_RSP_TIMEOUT;
            retVal = -1;
            goto EXIT;
        }
        httpInfo.stateType = HTTP_STATE_CONNECTED;
        
    #ifdef KING_SDK_MBEDTLS_SUPPORT
        if (urlInfo.isHttps)
        {
            retVal = httpTls_connect(httpInfo.socketFd);
            if (retVal < 0)
            {
                errcode = HTTP_SOCK_CONNECT_ERR;
                retVal = -1;
                goto EXIT;
            }
            retVal = 0;
            http_actionSendData();
        }
        else
    #endif            
        {
            retVal = 0;
            http_actionSendData();
        }
    }
    else
    {
        KING_SysLog("%s: connect ret = %d", __FUNCTION__, ret);
        errcode = HTTP_SOCK_CONNECT_ERR;
        retVal = -1;
    }

EXIT:
    if (errcode != 0)
    {
        httpErrCode = errcode;
        http_resetStatus(urlInfo.isHttps);
        return retVal;
    }

    return retVal;
}

static void http_socketDnsReqCb(void* pData, uint32 len)
{
    ASYN_DNS_PARSE_T *dnsParse = (ASYN_DNS_PARSE_T *)pData;

    //KING_SysLog("%s: hostname = %s, result = %d", __FUNCTION__, dnsParse->hostname, dnsParse->result);
    if (dnsParse->result == DNS_PARSE_SUCCESS)
    {
        httpHostAddr = dnsParse->addr;
        isHttpSucc = 1;
    }
    else
    {
        KING_SysLog("%s: fail!\n", __FUNCTION__);
        isHttpSucc = 0;
    }
    KING_SemPut(httpSem);
}

static int http_getIPAddrByHostname(void)
{
    int ret = -1;

    //KING_SysLog("%s: start", __FUNCTION__);
    if (isHttpRunning == 1)
    {
        return -1;
    }

    ret = http_createSeam();
    if (ret != 0)
    {
        return -1;
    }

    memset(&httpHostAddr, 0x00, sizeof(SOCK_IN_ADDR_T));
    ret = KING_NetGetHostByName(httpInfo.hContext, urlInfo.host, &httpHostAddr,
                            10000, (DNS_CALLBACK)http_socketDnsReqCb);
    if (ret != 0)
    {
        KING_SysLog("%s: ret=%d", __FUNCTION__, ret);
        return -1;
    }

    ret = http_getSem();
    if (ret != 0)
    {
        return -1;
    }

    httpInfo.stateType = HTTP_STATE_IP_GOT;
    return 0;
}

static void http_getOrPostReqHandle(void)
{
    int ret = -1;

    ret = http_getIPAddrByHostname();
    if (ret != 0)
    {
        KING_SysLog("%s: ret : %d", __FUNCTION__, ret);
        httpErrCode = HTTP_DNS_ERR;
        httpInfo.stateType = HTTP_STATE_URL_READY;
        goto EXIT;
    }

    http_rspTimer(TRUE);
    ret = http_socketCreatAndConnect();
    if (ret != 0)
    {
        goto EXIT;
    }

    return;

EXIT:
    httpInfo.isRunning = FALSE;
    httpInfo.rspCb(httpErrCode, HTTP_RESPONSE_CODE_UNKNOW, 0);
}

void http_init(void)
{
    if (!isHttpInited)
    {
        httpInfo.actionType = HTTP_ACTION_NUM;
        httpInfo.stateType = HTTP_STATE_NONE;

        httpInfo.sendTotalLen = 0;
        httpInfo.unsendLen = 0;
        httpInfo.receiveStatus = HTTP_RECEIVE_HEADER;
        httpInfo.receiveSize = 0;
        httpInfo.isContentLen = FALSE;
        httpInfo.isRunning = FALSE;
        httpInfo.pHeaderptr = NULL;
        httpInfo.pBody = NULL;
        httpInfo.socketFd = -1;

        isHttpInited = TRUE;
    }

    http_urlClear(&urlInfo);
    http_clearBuf();

    memset(&(httpInfo.reqHeaderKv), 0, sizeof(HTTP_HEADER_KV_S) * HTTP_HEADER_KV_MAX);
}

void http_reqHeaderSet(uint8 reqHeader)
{
    if (reqHeader != 0 && reqHeader != 1)
    {
        httpInfo.reqHeader = 0;
    }
    else
    {
        httpInfo.reqHeader = reqHeader;
    }
}

uint8 http_reqHeaderGet(void)
{
    return httpInfo.reqHeader;
}

int http_addHeaderKv(char *key, char *value)
{
    int i = 0;

    if ((NULL == key) || (NULL == value))
    {
        return -1;
    }

    if ((strlen(key) >= HTTP_HEADER_KEY_LEN) || (strlen(value) >= HTTP_HEADER_VALUE_LEN))
    {
        return -1;
    }

    for (i = 0; i < HTTP_HEADER_KV_MAX; i++)
    {
        if (0 == strlen(httpInfo.reqHeaderKv[i].key))
        {
            break;
        }
    }

    strcpy(httpInfo.reqHeaderKv[i].key, key);
    strcpy(httpInfo.reqHeaderKv[i].value, value);

    return 0;
}

void http_rspHeaderSet(uint8 rspHeader)
{
    if (rspHeader != 0 && rspHeader != 1)
    {
        httpInfo.rspHeader = 0;
    }
    else
    {
        httpInfo.rspHeader = rspHeader;
    }
}

uint8 http_rspHeaderGet(void)
{
    return httpInfo.rspHeader;
}

void http_contentTypeSet(HTTP_CONTENT_TYPE_E contentType)
{
    httpInfo.contentType = contentType;
}

void http_stateTypeSet(HTTP_STATE_TYPE_E  stateType)
{
    httpInfo.stateType = stateType;
}

HTTP_STATE_TYPE_E http_stateTypeGet(void)
{
    return httpInfo.stateType;
}

int http_urlParse(char *pUrl)
{
    char *srcptr = NULL;
    char *ptr;
    char *endptr;

    if (pUrl == NULL || strlen(pUrl) == 0 || strlen(pUrl) > HTTP_URL_MAX_LEN)
    {
        KING_SysLog("%s: empty url or url exceeded maximum length", __FUNCTION__);
        httpErrCode = HTTP_EMPTY_URL;
        return httpErrCode;
    }

    if (!isHttpInited)
    {
        http_init();
    }

    httpErrCode = HTTP_OPERATE_SUCC;
    urlInfo.url = malloc(HTTP_URL_MAX_LEN + 1);
    if (urlInfo.url == NULL)
    {
        KING_SysLog("%s: url malloc is fail", __FUNCTION__);
        httpErrCode = HTTP_ALLOC_MEM_FAIL;
        return httpErrCode;
    }
    memset(urlInfo.url, 0x00, HTTP_URL_MAX_LEN + 1);
    memcpy(urlInfo.url, pUrl, strlen(pUrl));

    srcptr = urlInfo.url;
    if (!strncasecmp(srcptr, "http://", strlen("http://")))
    {
        urlInfo.isHttps = FALSE;
        srcptr += strlen("http://");
    }
    else if (!strncasecmp(srcptr, "https://", strlen("https://")))
    {
        urlInfo.isHttps = TRUE;
        srcptr += strlen("https://");
    }
    else
    {
        KING_SysLog("%s: is invalid url", __FUNCTION__);
        httpErrCode = HTTP_URL_ERR;
        return httpErrCode;
    }

    urlInfo.host = malloc(HTTP_URL_MAX_LEN + 1);
    if (urlInfo.host == NULL)
    {
        KING_SysLog("%s: host malloc is fail", __FUNCTION__);
        httpErrCode = HTTP_ALLOC_MEM_FAIL;
        return httpErrCode;
    }
    memset(urlInfo.host, 0x00, HTTP_URL_MAX_LEN + 1);

    urlInfo.uri = malloc(HTTP_URL_MAX_LEN + 1);
    if (urlInfo.uri == NULL)
    {
        KING_SysLog("%s: uri malloc is fail", __FUNCTION__);
        httpErrCode = HTTP_ALLOC_MEM_FAIL;
        return httpErrCode;
    }
    memset(urlInfo.uri, 0x00, HTTP_URL_MAX_LEN + 1);

    ptr = strchr(srcptr, ':');
    if (ptr != NULL)
    {
        memcpy(urlInfo.host, srcptr, ptr - srcptr);
        urlInfo.port = (uint32)strtol(ptr + 1, &endptr, 10);

        srcptr = endptr;
        ptr = strchr(srcptr, '/');
        if(ptr != NULL)
        {
            memcpy(urlInfo.uri, srcptr, strlen(srcptr));
        }
        else
        {
            urlInfo.uri[0] = '/';
        }
    }
    else
    {
        if (urlInfo.isHttps)
        {
            urlInfo.port = HTTPS_DEFAULT_PORT;
        }
        else
        {
            urlInfo.port = HTTP_DEFAULT_PORT;
        }
        urlInfo.uri[0] = '/';
        ptr = strchr(srcptr, '/');
        if(ptr != NULL)
        {
            memcpy(urlInfo.host, srcptr, ptr - srcptr);
            strcpy(urlInfo.uri, ptr);
        }
        else
        {
            strcpy(urlInfo.host, srcptr);
        }
    }

    KING_SysLog("HTTP HOST:%s, URI:%s, PORT:%u",urlInfo.host, urlInfo.uri, urlInfo.port);
    httpInfo.stateType = HTTP_STATE_URL_READY;

    return 0;
}


void http_get(
    softwareHandle hContext,
    char *pHeaderStr, uint32 headLen,
    uint16 rspTimeout, HTTP_RSP_CALLBACK rspCb
)
{
    httpInfo.rspCb = rspCb;

    if (httpInfo.isRunning)
    {
        httpErrCode = HTTP_BUSY;
        httpInfo.rspCb(httpErrCode, HTTP_RESPONSE_CODE_UNKNOW, 0);
        return;
    }

    if (httpInfo.stateType != HTTP_STATE_URL_READY)
    {
        httpErrCode = HTTP_EMPTY_URL;
        goto EXIT;
    }

    if (strlen(urlInfo.host) == 0)
    {
        httpErrCode = HTTP_URL_ERR;
        goto EXIT;
    }

    httpInfo.hContext = hContext;
    httpInfo.isRunning = TRUE;
    httpInfo.rspTimeout = rspTimeout;
    httpInfo.actionType = HTTP_ACTION_GET;
    httpCurChunkLen = 0;
    readCurLen = 0;
    httpInfo.bodySize = 0;
    httpInfo.headerlen = 0;
    httpErrCode = HTTP_OPERATE_SUCC;
    memset(&gHttpChunkTail, 0x00, sizeof(HTTP_CHUNK_TAIL_BUF_T));

    if (httpInfo.reqHeader == 1)
    {
        if (pHeaderStr == NULL || headLen == 0)
        {
            httpErrCode = HTTP_INVALID_PARAM;
            goto EXIT;
        }

        if (httpInfo.pHeaderptr != NULL)
        {
            free(httpInfo.pHeaderptr);
            httpInfo.pHeaderptr = NULL;
            httpInfo.headerlen = 0;
        }
        httpInfo.pHeaderptr = malloc(headLen + 1);
        if (httpInfo.pHeaderptr == NULL)
        {
            httpErrCode = HTTP_ALLOC_MEM_FAIL;
            goto EXIT;
        }
        memset(httpInfo.pHeaderptr, 0x00, headLen + 1);
        memcpy(httpInfo.pHeaderptr, pHeaderStr, headLen);
        httpInfo.headerlen = headLen;
    }

    http_getOrPostReqHandle();
    return;

EXIT:
    httpInfo.isRunning = FALSE;
    httpInfo.rspCb(httpErrCode, HTTP_RESPONSE_CODE_UNKNOW, 0);
}

void http_post(
    softwareHandle hContext,
    char *pData, uint32 dataLen,
    uint16 rspTimeout, HTTP_RSP_CALLBACK rspCb
)
{
    httpInfo.rspCb = rspCb;

    if (httpInfo.isRunning)
    {
        httpErrCode = HTTP_BUSY;
        httpInfo.rspCb(httpErrCode, HTTP_RESPONSE_CODE_UNKNOW, 0);
        return;
    }

    if (httpInfo.stateType != HTTP_STATE_URL_READY)
    {
        httpErrCode = HTTP_EMPTY_URL;
        goto EXIT;
    }

    if (strlen(urlInfo.host) == 0)
    {
        httpErrCode = HTTP_URL_ERR;
        goto EXIT;
    }

    if (pData == NULL || dataLen == 0)
    {
        httpErrCode = HTTP_INVALID_PARAM;
        goto EXIT;
    }

    httpInfo.hContext = hContext;
    httpInfo.isRunning = TRUE;
    httpInfo.rspTimeout = rspTimeout;
    httpInfo.actionType = HTTP_ACTION_POST;
    httpCurChunkLen = 0;
    readCurLen = 0;
    httpInfo.bodySize = 0;
    httpInfo.headerlen = 0;
    httpErrCode = HTTP_OPERATE_SUCC;
    memset(&gHttpChunkTail, 0x00, sizeof(HTTP_CHUNK_TAIL_BUF_T));

    if (httpInfo.pBody!= NULL)
    {
        free(httpInfo.pBody);
        httpInfo.pBody = NULL;
        httpInfo.bodySize = 0;
    }
    httpInfo.pBody = malloc(dataLen + 1);
    if (httpInfo.pBody == NULL)
    {
        httpErrCode = HTTP_ALLOC_MEM_FAIL;
        goto EXIT;
    }
    memset(httpInfo.pBody, 0x00, dataLen + 1);
    memcpy(httpInfo.pBody, pData, dataLen);
    httpInfo.bodySize = dataLen;

    http_getOrPostReqHandle();
    return;

EXIT:
    httpInfo.isRunning = FALSE;
    httpInfo.rspCb(httpErrCode, HTTP_RESPONSE_CODE_UNKNOW, 0);
}

int http_canReadData(void)
{
    if (((httpInfo.rspHeader == 0) && (httpInfo.pBody != NULL)) ||
        ((httpInfo.rspHeader == 1) && (httpInfo.pHeaderptr != NULL) && (httpInfo.pBody != NULL)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void http_readHeaderData(char **ppHeaderStr, uint32 *headerLen)
{
    if (httpInfo.pHeaderptr != NULL && httpInfo.rspHeader == 1)
    {
        *ppHeaderStr = httpInfo.pHeaderptr;
        *headerLen = httpInfo.headerlen;
    }
    else
    {
        *ppHeaderStr = NULL;
        *headerLen = 0;
    }
}

int http_readContentData(uint32 waitTimeout, uint32 dataLen, char *pData, uint8 *readEnd)
{
    uint32 readLen = 0;
    uint32 realSize = 0;
    int ret = -1;

    if (pData == NULL || dataLen == 0)
    {
        return HTTP_INVALID_PARAM;
    }

    httpInfo.isRunning = TRUE;
    httpErrCode = HTTP_OPERATE_SUCC;
    httpInfo.waitTimeout = waitTimeout;
    httpInfo.actionType = HTTP_ACTION_READ;

    memset(pData, 0x00, dataLen);
    //KING_SysLog("%s: bodyBufLen=%d", __FUNCTION__, httpInfo.bodyBufLen);
    if (httpInfo.pBody != NULL && (httpInfo.bodyBufLen - readCurLen) > 0)
    {
        if (dataLen <= (httpInfo.bodyBufLen - readCurLen))
        {
            readLen = dataLen;
        }
        else
        {
            readLen = (httpInfo.bodyBufLen - readCurLen);
        }

        //KING_SysLog("%s: readLen=%d", __FUNCTION__, readLen);
        if (readLen > 0)
        {
            memcpy(pData, httpInfo.pBody + readCurLen, readLen);
            readCurLen += readLen;
        }
    }

    if (readCurLen < httpInfo.bodyBufLen && httpInfo.stateType != HTTP_STATE_READ)
    {
        httpInfo.isRunning = FALSE;
        *readEnd = 0;
        return 0;
    }
    else
    {
        if (httpInfo.receiveStatus == HTTP_RECEIVE_COMPLETE)
        {
            http_resetStatus(urlInfo.isHttps);
            readCurLen = 0;
            *readEnd = 1;
            return 0;
        }
        else
        {
            httpInfo.stateType = HTTP_STATE_READ;

            if (httpInfo.pHeaderptr != NULL)
            {
                free(httpInfo.pHeaderptr);
                httpInfo.pHeaderptr = NULL;
                httpInfo.headerlen = 0;
            }

            if (httpInfo.pBody != NULL)
            {
                free(httpInfo.pBody);
                httpInfo.pBody = NULL;
                httpInfo.bodyBufLen = 0;
            }

            //KING_SysLog("%s: readLen=%d", __FUNCTION__, readLen);
            pReadData = malloc(dataLen - readLen + 1);
            if (pReadData == NULL)
            {
                http_resetStatus(urlInfo.isHttps);
                readCurLen = 0;
                return HTTP_ALLOC_MEM_FAIL;
            }
            memset(pReadData, 0x00, dataLen - readLen + 1);

            ret = http_readreceiveRsp(dataLen - readLen, &realSize);
            if (ret == 0)
            {
                memcpy(pData + readLen, pReadData, realSize);
                readCurLen = 0;
                *readEnd = 1;
                ret = 0;
            }
            else if (ret == 1)
            {
                memcpy(pData + readLen, pReadData, realSize);
                *readEnd = 0;
                ret = 0;
            }

            if (pReadData != NULL)
            {
                free(pReadData);
                pReadData = NULL;
            }
            return ret;
        }
    }
}

int http_readContentDataAndLen(
    uint32 waitTimeout,
    uint32 dataLen, char *pData,
    uint32 *actualDataLen, uint8 *readEnd
)
{
    uint32 readLen = 0;
    uint32 realSize = 0;
    int ret = -1;

    if (pData == NULL || dataLen == 0)
    {
        return HTTP_INVALID_PARAM;
    }

    httpInfo.isRunning = TRUE;
    httpErrCode = HTTP_OPERATE_SUCC;
    httpInfo.waitTimeout = waitTimeout;
    httpInfo.actionType = HTTP_ACTION_READ;

    memset(pData, 0x00, dataLen);
    //KING_SysLog("%s: bodyBufLen=%d", __FUNCTION__, httpInfo.bodyBufLen);
    if (httpInfo.pBody != NULL && (httpInfo.bodyBufLen - readCurLen) > 0)
    {
        if (dataLen <= (httpInfo.bodyBufLen - readCurLen))
        {
            readLen = dataLen;
        }
        else
        {
            readLen = (httpInfo.bodyBufLen - readCurLen);
        }

        //KING_SysLog("%s: readLen=%d", __FUNCTION__, readLen);
        if (readLen > 0)
        {
            memcpy(pData, httpInfo.pBody + readCurLen, readLen);
            readCurLen += readLen;
        }
    }
    if (readCurLen < httpInfo.bodyBufLen && httpInfo.stateType != HTTP_STATE_READ)
    {
        httpInfo.isRunning = FALSE;
        *readEnd = 0;
        *actualDataLen = readLen;
        return 0;
    }
    else
    {
        if (httpInfo.receiveStatus == HTTP_RECEIVE_COMPLETE)
        {
            http_resetStatus(urlInfo.isHttps);
            readCurLen = 0;
            *readEnd = 1;
            *actualDataLen = readLen;
            return 0;
        }
        else
        {
            httpInfo.stateType = HTTP_STATE_READ;

            if (httpInfo.pHeaderptr != NULL)
            {
                free(httpInfo.pHeaderptr);
                httpInfo.pHeaderptr = NULL;
                httpInfo.headerlen = 0;
            }

            if (httpInfo.pBody != NULL)
            {
                free(httpInfo.pBody);
                httpInfo.pBody = NULL;
                httpInfo.bodyBufLen = 0;
            }

            //KING_SysLog("%s: readLen=%d", __FUNCTION__, readLen);
            pReadData = malloc(dataLen - readLen + 1);
            if (pReadData == NULL)
            {
                http_resetStatus(urlInfo.isHttps);
                readCurLen = 0;
                return HTTP_ALLOC_MEM_FAIL;
            }
            memset(pReadData, 0x00, dataLen - readLen + 1);

            ret = http_readreceiveRsp(dataLen - readLen, &realSize);
            if (ret == 0)
            {
                memcpy(pData + readLen, pReadData, realSize);
                readCurLen = 0;
                *readEnd = 1;
                *actualDataLen = realSize + readLen;
                ret = 0;
            }
            else if (ret == 1)
            {
                memcpy(pData + readLen, pReadData, realSize);
                *readEnd = 0;
                *actualDataLen = realSize + readLen;
                ret = 0;
            }

            if (pReadData != NULL)
            {
                free(pReadData);
                pReadData = NULL;
            }
            return ret;
        }
    }
}

void http_clearStatus(void)
{
    http_resetStatus(urlInfo.isHttps);
}

