/*******************************************************************************
 ** File Name:   at_cmd_http.c
 ** Author:      Allance.Chen
 ** Date:        2020-08-05
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains ds functions about http cmd.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-08-05     Allance.Chen         Create.
 ******************************************************************************/
/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "At_common.h"
#include "At_errcode.h"
#include "At_module.h"
#include "AT_define.h"
#include "AT_cmd_tcpip.h"

#include "kingsocket.h"
#include "httpclient.h"
#include "kingrtos.h"
#include "kingcstd.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define HTTP_STR_RSP_LEN                200
#define HTTP_URL_INPUT_MAX_LEN          300
#define HTTP_RESPONSE_SIZE              1024

#define HTTP_HAL_TICK1S                 1000
#define HTTP_HAL_TICK900MS              900

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef enum _httpcfgType {
    QHTTPCFG_NONE_TYPE = 0,
    QHTTPCFG_CONTEXT_ID_TYPE,
    QHTTPCFG_REQ_HEADER_TYPE,
    QHTTPCFG_RSP_HEADER_TYPE,
    QHTTPCFG_SSL_CTXID_TYPE,
    QHTTPCFG_CONTENT_TYPE,  
    QHTTPCFG_MAX_TYPE
} HTTP_CFG_TYPE_E;

typedef enum atHttpActionType {
    AT_HTTP_ACTION_NONE = 0,
    AT_HTTP_ACTION_GET,
    AT_HTTP_ACTION_READ,

    AT_HTTP_ACTION_NUM
} AT_HTTP_ACTION_TYPE_E;

typedef enum HttpMsgIdType {
    HTTP_MSG_ID_MIN,
    HTTP_GET_REQ,
    
    HTTP_MSG_ID_MAX
} HTTP_MSG_ID_TYPE_E;

typedef struct _httpcfgInfo {
    uint8 ctxID;
    uint8 reqHeader;
    uint8 rspHeader;  
    uint8 sslCtxid;
    uint8 contentType;
} HTTP_CFG_INFO_S;

typedef struct _httpInfo {
    AT_CMD_ENGINE_T *atEngine;
    AT_HTTP_ACTION_TYPE_E actionType;
    
    char url[HTTP_URL_INPUT_MAX_LEN + 1];
    volatile bool urlDataStatus;
    
    volatile uint32 uartDataLen;
    volatile uint32 uartDataGot;
    volatile bool getDataStatus;

    uint32 inputTimeout;
    uint32 rspTimeout;
    uint32 waitTimeout;
    
    char *pHeader;
    uint16 headerLen;

    bool isRunning;
} HTTP_INFO_S;

/*******************************************************************************
 ** External Variables 
 ******************************************************************************/
extern UINT8 g_rspStr[];

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static HTTP_CFG_INFO_S httpCfgConfig;
static HTTP_INFO_S ghttpInfo;
static TIMER_HANDLE inputTimer;
static MUTEX_HANDLE httpMutex = NULL;
static uint32 httpLastRxTick = 0;
static uint32 httpCurTick = 0;
static bool httpPlusTimeOut = FALSE;
static TIMER_HANDLE httpPlusTimer = NULL;
static char httpPlusBuf[4] = {0};
static THREAD_HANDLE httpThreadID = NULL;
static MSG_HANDLE    httpMsgHandle = NULL;

/*******************************************************************************
 ** Local Function Delcarations
 ******************************************************************************/
static void http_plusTimer(bool isSet);
static void http_exitDataMode(bool isOK);
static void http_msgSend(HTTP_MSG_ID_TYPE_E msgId);

/*******************************************************************************
 ** Local Function Delcarations
 ******************************************************************************/
static void http_setDefaultConfig(void)
{
    httpCfgConfig.ctxID = 1;
    httpCfgConfig.reqHeader = 0;
    http_reqHeaderSet(httpCfgConfig.reqHeader);
    httpCfgConfig.rspHeader = 0;
    http_rspHeaderSet(httpCfgConfig.rspHeader);
    httpCfgConfig.sslCtxid = 1;  
    httpCfgConfig.contentType = 0;   
    http_contentTypeSet(httpCfgConfig.contentType);
}

static char* http_cmdName(void)
{
    if (ghttpInfo.actionType == AT_HTTP_ACTION_GET)
    {
        return "+QHTTPGET";
    }
    else if (ghttpInfo.actionType == AT_HTTP_ACTION_READ)
    {
        return "+QHTTPREAD";
    }
    else
    {
        return "";
    }
}

static void http_urcRsp(char *cmdString, uint16 errCode)
{
    ghttpInfo.isRunning = FALSE;
    memset(g_rspStr, 0x00, sizeof((char *)g_rspStr));
    snprintf((char *)g_rspStr, 30, "\r\n%s: %d\r\n", cmdString, errCode);
    at_CmdRespOutputText(ghttpInfo.atEngine, g_rspStr);
}

static void http_inputTimeOut(uint32 tmrId)
{
    http_exitDataMode(FALSE);
    return;
}

static void http_inputTimer(bool isSet)
{
    int ret = 0;
    
    if (isSet) 
    {
        TIMER_ATTR_S timerattr;
        
        if (inputTimer == NULL)
        {
            ret = KING_TimerCreate(&inputTimer);
            if(-1 == ret)
            {
                KING_SysLog("%s: Failed!", __FUNCTION__);
                return;
            }
        }
        memset(&timerattr, 0x00, sizeof(timerattr));
        timerattr.isPeriod = FALSE;
        timerattr.timeout = ghttpInfo.inputTimeout * 1000;
        timerattr.timer_fun = http_inputTimeOut;
        ret = KING_TimerActive(inputTimer, &timerattr);
        if(-1 == ret)
        {
            KING_SysLog("%s: Active Failed!", __FUNCTION__);
            return;
        }
    }
    else
    {
        if (inputTimer != NULL)
        {
            KING_TimerDeactive(inputTimer);
            KING_TimerDelete(inputTimer);
            inputTimer = NULL;
        }
    }
}

static void http_plusTimerCb(uint32 tmrId)
{
    if (httpPlusTimeOut)
    {
        httpPlusTimeOut = FALSE;
        memset(httpPlusBuf, 0x00, 4);
        http_plusTimer(FALSE);
        http_exitDataMode(TRUE);
    }

    return;
}

static void http_plusTimer(bool isSet)
{
    int ret = -1;

    if (isSet)
    {
        TIMER_ATTR_S timerattr;
        
        if (httpPlusTimer == NULL)
        {
            ret = KING_TimerCreate(&httpPlusTimer);
            if(-1 == ret)
            {
                KING_SysLog("%s: Failed!", __FUNCTION__);
                return;
            }
        }
        memset(&timerattr, 0x00, sizeof(timerattr));
        timerattr.isPeriod = FALSE;
        timerattr.timeout = 900;
        timerattr.timer_fun = http_plusTimerCb;
        ret = KING_TimerActive(httpPlusTimer, &timerattr);
        if(-1 == ret)
        {
            KING_SysLog("%s: Active Failed!", __FUNCTION__);
            return;
        }
        httpPlusTimeOut = TRUE;
    }
    else
    {
        if (httpPlusTimer != NULL)
        {
            KING_TimerDeactive(httpPlusTimer);
            KING_TimerDelete(httpPlusTimer);
            httpPlusTimer = NULL;
        }
    }

}

static HTTP_CFG_TYPE_E http_qHttpCfgInputType(char *configName, char *cfgName)
{
    if(strcasecmp((char *)configName, (char *)"contextid") == 0)
    {
        strncpy(cfgName,(char*)"contextid", strlen("contextid"));
        return QHTTPCFG_CONTEXT_ID_TYPE;
    }
    else if(strcasecmp((char *)configName, (char *)"requestheader") == 0)
    {
        strncpy(cfgName,(char*)"requestheader", strlen("requestheader"));
        return QHTTPCFG_REQ_HEADER_TYPE;
    }
    else if(strcasecmp((char *)configName, (char *)"responseheader") == 0)
    {
        strncpy(cfgName,(char*)"responseheader", strlen("responseheader"));
        return QHTTPCFG_RSP_HEADER_TYPE;
    } 
    else if(strcasecmp((char *)configName, (char *)"sslctxid") == 0)
    {
        strncpy(cfgName,(char*)"sslctxid", strlen("sslctxid"));
        return QHTTPCFG_SSL_CTXID_TYPE;
    } 
    else if(strcasecmp((char *)configName, (char *)"contenttype") == 0)
    {
        strncpy(cfgName,(char*)"contenttype", strlen("contenttype"));
        return QHTTPCFG_CONTENT_TYPE;
    }
    else
    {
        strcpy(cfgName,(char*)"");
        return QHTTPCFG_NONE_TYPE;
    }
}

static int http_qHttpCfgSetAndSave(HTTP_CFG_TYPE_E cfgType, uint8 param)
{
    int ret = 0;

    if (cfgType == QHTTPCFG_CONTEXT_ID_TYPE)
    {
        httpCfgConfig.ctxID = param;
    }
    else if (cfgType == QHTTPCFG_REQ_HEADER_TYPE)
    {
        httpCfgConfig.reqHeader = param;
        http_reqHeaderSet(httpCfgConfig.reqHeader);
    }
    else if (cfgType == QHTTPCFG_RSP_HEADER_TYPE)
    {
        httpCfgConfig.rspHeader = param;
        http_rspHeaderSet(httpCfgConfig.rspHeader);
    }
    else if (cfgType == QHTTPCFG_SSL_CTXID_TYPE)
    {
        httpCfgConfig.sslCtxid = param;
    } 
    else if (cfgType == QHTTPCFG_CONTENT_TYPE)
    {
        httpCfgConfig.contentType = param;
        http_contentTypeSet(httpCfgConfig.contentType);
    }
    else
    {
        return -1;
    }
    
    return ret;
}

static void http_qHttpCfgGetConfig(HTTP_CFG_TYPE_E cfgType, uint8 *param)
{
    if (cfgType == QHTTPCFG_CONTEXT_ID_TYPE)
    {
        *param = httpCfgConfig.ctxID;
    }
    else if (cfgType == QHTTPCFG_REQ_HEADER_TYPE)
    {
        *param = httpCfgConfig.reqHeader;
    }
    else if (cfgType == QHTTPCFG_RSP_HEADER_TYPE)
    {
        *param = httpCfgConfig.rspHeader;
    }
    else if (cfgType == QHTTPCFG_SSL_CTXID_TYPE)
    {
        *param = httpCfgConfig.sslCtxid;
    }
    else if (cfgType == QHTTPCFG_CONTENT_TYPE)
    {
        *param = httpCfgConfig.contentType;
    }
}

static void http_qHttpCfgCmdRead(AT_CMD_ENGINE_T *atEngine)
{
    uint8 param = 0;
    char buf[50] = {0};
    uint8 i = 0;
    uint8 * httpcfgname[] =
    {
        (uint8 *)"contextid",(uint8 *)"requestheader",
        (uint8 *)"responseheader",(uint8 *)"sslctxid",
        (uint8 *)"contenttype"
    };
    
    for (i = 0; i < 5; i++)
    {
        if (i == 0)
        { 
            param = httpCfgConfig.ctxID;
        }
        else if (i == 1)
        {
            param = httpCfgConfig.reqHeader;
        }
        else if (i == 2)
        {
            param = httpCfgConfig.rspHeader;
        }
        else if (i == 3)
        {
            param = httpCfgConfig.sslCtxid;
        }
        else if (i == 4)
        {
            param = httpCfgConfig.contentType;
        }


        if (i == 0)
        {
            at_CmdRespOutputText(atEngine, (uint8 *)"\r\n");
        }
        snprintf(buf, 50, "+QHTTPCFG: \"%s\",%d\r\n",
                (char *)httpcfgname[i],
                param);
        at_CmdRespOutputText(atEngine, (uint8 *)buf);
    }
    
    at_CmdRespOK(atEngine);
    return;
}

static void http_qHttpUrlCmdRead(AT_CMD_ENGINE_T *atEngine)
{
    if (strlen(ghttpInfo.url) > 0)
    {
        char rsp_string[HTTP_URL_INPUT_MAX_LEN + 12] = {0};

        memset(rsp_string, 0x00, HTTP_URL_INPUT_MAX_LEN + 12);
        snprintf(rsp_string, HTTP_URL_INPUT_MAX_LEN + 12, "+QHTTPURL: %s", ghttpInfo.url);
        at_CmdRespInfoText(atEngine, (uint8 *)rsp_string);
    }
    
    at_CmdRespOK(atEngine);
    return;
}

static void http_exitDataMode(bool isErr)
{
    at_CmdSetLineMode(ghttpInfo.atEngine);
    http_inputTimer(FALSE);
    ghttpInfo.isRunning = FALSE;
    if(ghttpInfo.urlDataStatus == TRUE)
    {
        memset(ghttpInfo.url, 0x00, HTTP_URL_INPUT_MAX_LEN + 1);
        if (isErr)
        {
            at_CmdRespCmeError(ghttpInfo.atEngine, HTTP_UNKNOWN_ERR);
        }
        else
        {
            at_CmdRespCmeError(ghttpInfo.atEngine, HTTP_TIMEOUT);
        }
    }
    else if (ghttpInfo.getDataStatus == TRUE)
    {
        if(ghttpInfo.pHeader != NULL)
        {
            free(ghttpInfo.pHeader);
            ghttpInfo.pHeader = NULL;
            ghttpInfo.headerLen = 0;
        }
    }
    if(http_stateTypeGet() >= HTTP_STATE_URL_READY)
    {
        http_stateTypeSet(HTTP_STATE_URL_READY);
    }
    else
    {
        http_stateTypeSet(HTTP_STATE_NONE);
    }

    ghttpInfo.getDataStatus = FALSE;
    ghttpInfo.urlDataStatus = FALSE;
    ghttpInfo.uartDataLen = 0;
    ghttpInfo.uartDataGot = 0;
    if (ghttpInfo.actionType == AT_HTTP_ACTION_GET)
    {
        char *parseName = http_cmdName();
        
        at_CmdRespOK(ghttpInfo.atEngine);
        //urc error
        if (isErr)
        {
            http_urcRsp(parseName, HTTP_UNKNOWN_ERR);
        }
        else
        {
            http_urcRsp(parseName, HTTP_TIMEOUT);
        }
    }
    
    ghttpInfo.actionType = AT_HTTP_ACTION_NUM;
}

static void http_getCb(uint16 errCode, int16 httprspcode, uint32 contentLen)
{
    char *parseName = http_cmdName();

    ghttpInfo.isRunning = FALSE;
    http_stateTypeSet(HTTP_STATE_URL_READY);

    memset(g_rspStr, 0x00, sizeof((char *)g_rspStr));
    if (errCode == 0)
    {
        if (((httprspcode == HTTP_RESPONSE_CODE_OK) 
            ||(httprspcode == HTTP_RESPONSE_CODE_PARTIAL_CONTENT)) 
            && contentLen > 0)
        {
            snprintf((char *)g_rspStr, 40, "\r\n%s: %d,%d,%u\r\n", parseName,
                    errCode, httprspcode,
                    contentLen);
        }
        else
        {
            snprintf((char *)g_rspStr, 40, "\r\n%s: %d,%d\r\n", parseName,
                    errCode, httprspcode);
        }
    }
    else
    {
        snprintf((char *)g_rspStr, 40, "\r\n%s: %d\r\n", parseName,
                errCode);
    }

    at_CmdRespOutputText(ghttpInfo.atEngine, g_rspStr);
}

static void http_GetDataFromUartOpt(uint8 *pData, uint32 dataLength)
{
    uint32 needLen;
    int ret = -1;

    if(ghttpInfo.uartDataLen == 0)
    {
        http_exitDataMode(TRUE);
        return;
    }
    
    if((ghttpInfo.uartDataLen - ghttpInfo.uartDataGot) < dataLength)
    {
        needLen = ghttpInfo.uartDataLen - ghttpInfo.uartDataGot;
    }
    else
    {
        needLen = dataLength;
    } 
    if(ghttpInfo.urlDataStatus == TRUE)
    {
        memcpy(ghttpInfo.url + ghttpInfo.uartDataGot, pData, needLen);
        ghttpInfo.uartDataGot += needLen;

        KING_SysLog("http_GetDataFromUart: uartDataLen=%d, uartDataGot=%d",
                    ghttpInfo.uartDataLen, ghttpInfo.uartDataGot);

        if(ghttpInfo.uartDataGot >= ghttpInfo.uartDataLen)
        {
            at_CmdSetLineMode(ghttpInfo.atEngine);
            http_inputTimer(FALSE);
            ghttpInfo.urlDataStatus = FALSE;
            ghttpInfo.uartDataGot = 0;
            ghttpInfo.uartDataLen = 0;
            ret = http_urlParse(ghttpInfo.url);
            if (ret == 0)
            {
                at_CmdRespOK(ghttpInfo.atEngine);
            }
            else
            {
                memset(ghttpInfo.url, 0, HTTP_URL_INPUT_MAX_LEN + 1);
                at_CmdRespCmeError(ghttpInfo.atEngine, ret);
            }
            ghttpInfo.isRunning = FALSE;
            return;
        }
    }
    else if (ghttpInfo.getDataStatus == TRUE)
    {
        memcpy(ghttpInfo.pHeader + ghttpInfo.headerLen, pData, needLen);
        ghttpInfo.uartDataGot += needLen;
        ghttpInfo.headerLen += needLen;
        KING_SysLog("http_GetDataFromUart: pHeader=%s, headerlen=%d",
                        ghttpInfo.pHeader, ghttpInfo.headerLen);
        if(ghttpInfo.headerLen >= ghttpInfo.uartDataLen)
        {
            http_inputTimer(FALSE);
            ghttpInfo.getDataStatus = FALSE;
            ghttpInfo.uartDataGot = 0;
            ghttpInfo.uartDataLen = 0;
            at_CmdSetLineMode(ghttpInfo.atEngine);
            if (httpCfgConfig.reqHeader == 1)
            {
                at_CmdRespOK(ghttpInfo.atEngine); 
            }
            http_msgSend(HTTP_GET_REQ);
        }
    }
}

static int http_GetDataFromUart(uint8 *pData, uint32 dataLength)
{
    KING_SysLog("http_GetDataFromUart dataLength[%d]", dataLength);
    if (dataLength > 0)
    {
        uint32 rx_interval = 0;
        uint8 httpPlusLen = 0;

        KING_CurrentTickGet(&httpCurTick);
        KING_SysLog("http_GetDataFromUart httpLastRxTick[%ld]", httpLastRxTick);
        KING_SysLog("http_GetDataFromUart httpCurTick[%ld]", httpCurTick);
        rx_interval = (httpCurTick >= httpLastRxTick ? httpCurTick - httpLastRxTick : 0xffffffff - httpLastRxTick + httpCurTick);
        KING_SysLog("http_GetDataFromUart httpPlusTimeOut[%d]", httpPlusTimeOut);
        httpPlusLen = strlen(httpPlusBuf);
        if (httpPlusTimeOut)
        {
            http_plusTimer(FALSE);
            httpPlusTimeOut = FALSE;
            if (httpPlusLen > 0)
            {
                http_GetDataFromUartOpt((uint8 *)httpPlusBuf, httpPlusLen);
            }
            memset(httpPlusBuf, 0x00, 4);
            httpPlusLen = 0;
            httpLastRxTick = httpCurTick;
        }

        KING_SysLog("http_GetDataFromUart data_ptr[0]:%c", pData[0]);
        if ((dataLength == 1) && (pData[0] == '+'))
        {
            KING_SysLog("http_GetDataFromUart httpPlusBuf len[%d]", strlen(httpPlusBuf));
            if (httpPlusLen == 0)
            {
                if (rx_interval > HTTP_HAL_TICK900MS)
                {
                    httpPlusBuf[httpPlusLen] = '+';
                }
                else
                {
                    http_GetDataFromUartOpt((uint8 *)"+", 1);
                }
            }
            else if (httpPlusLen == 1)
            {
                if (rx_interval > HTTP_HAL_TICK1S)
                {
                    http_GetDataFromUartOpt((uint8 *)httpPlusBuf, httpPlusLen);
                    memset(httpPlusBuf, 0x00, 4);
                    httpPlusLen = 0;
                    httpPlusBuf[httpPlusLen] = '+';
                }
                else
                {
                    httpPlusBuf[httpPlusLen] = '+';
                }
            }
            else if (httpPlusLen == 2)
            {
                if (rx_interval > HTTP_HAL_TICK1S)
                {
                    http_GetDataFromUartOpt((uint8 *)httpPlusBuf, httpPlusLen);
                    memset(httpPlusBuf, 0x00, 4);
                    httpPlusLen = 0;
                    httpPlusBuf[httpPlusLen] = '+';
                }
                else
                {
                    memset(httpPlusBuf, 0x00, 4);
                    memset(httpPlusBuf, '+', 3);
                    http_plusTimer(TRUE);
                }
            }

            httpLastRxTick = httpCurTick;
            return 0;
        }
        else if(dataLength == 2 && pData[0] == '+' && pData[1] == '+')
        {
            if (httpPlusLen == 0)
            {
                if (rx_interval > HTTP_HAL_TICK900MS)
                {
                    memset(httpPlusBuf, '+', 2);
                    httpLastRxTick = httpCurTick;
                    return 0;
                }
            }
            else if(httpPlusLen == 1)
            {
                if (rx_interval > HTTP_HAL_TICK1S)
                {
                    http_GetDataFromUartOpt((uint8 *)httpPlusBuf, httpPlusLen);
                    memset(httpPlusBuf, 0x00, 4);
                    httpPlusLen = 0;
                    memset(httpPlusBuf, '+', 2);
                }
                else
                {
                    memset(httpPlusBuf, 0x00, 4);
                    memset(httpPlusBuf, '+', 3);
                    http_plusTimer(TRUE);
                }

                httpLastRxTick = httpCurTick;
                return 0;
            }
            else if (httpPlusLen == 2)
            {
                http_GetDataFromUartOpt((uint8 *)httpPlusBuf, httpPlusLen);
                memset(httpPlusBuf, 0x00, 4);
                httpPlusLen = 0;
                if (rx_interval > HTTP_HAL_TICK900MS)
                {
                    memset(httpPlusBuf, '+', 2);
                    httpLastRxTick = httpCurTick;
                    return 0;
                }
            }
        }  
        else if(dataLength == 3 && pData[0] == '+' && pData[1] == '+' && pData[2] == '+')
        {
            if (httpPlusLen > 0)
            {
                http_GetDataFromUartOpt((uint8 *)httpPlusBuf, httpPlusLen);
                memset(httpPlusBuf, 0x00, 4);
                httpPlusLen = 0;
            }
          
            if (rx_interval >= HTTP_HAL_TICK900MS)
            {
                memset(httpPlusBuf, 0x00, 4);
                memset(httpPlusBuf, '+', 3);
                http_plusTimer(TRUE);
                httpLastRxTick = httpCurTick;
                return 0;
            }
        }
        
        if (httpPlusLen > 0)
        {
            http_GetDataFromUartOpt((uint8 *)httpPlusBuf, httpPlusLen);
            memset(httpPlusBuf, 0x00, 4);
        }
        httpLastRxTick = httpCurTick;
        http_GetDataFromUartOpt(pData, dataLength);
    }
    
    return 0;
}

static int http_BypassDataRecv(void *param, uint8_t *data, unsigned length)
{
    int ret = -1;

    if (httpMutex == NULL)
    {
        ret = KING_MutexCreate("httpCmdmutex", 0, &httpMutex);
        if (ret != 0 )
        {
            KING_SysLog("KING_MutexCreate fail");
            return -1;
        }
    }

    KING_MutexLock(httpMutex, WAIT_OPT_INFINITE);
    http_GetDataFromUart(data, length);
    KING_MutexUnLock(httpMutex);

    return length;
}

static void http_getReqHandler(void)
{
    KING_SysLog("http_getReqHandler stateType=%d", http_stateTypeGet());
    if(http_stateTypeGet() == HTTP_STATE_URL_READY)
    {
        if(strlen(ghttpInfo.url) == 0)
        {
            if (httpCfgConfig.reqHeader == 1)
            {
                at_CmdRespOK(ghttpInfo.atEngine);
            }
            
            http_urcRsp("+QHTTPGET", HTTP_URL_ERR);
            return;
        }
        if (httpCfgConfig.reqHeader == 1)
        {
            if(ghttpInfo.pHeader != NULL)
            {
                free(ghttpInfo.pHeader);
                ghttpInfo.pHeader = NULL;
                ghttpInfo.headerLen = 0;
            }
            ghttpInfo.pHeader = malloc((sizeof(char) * (ghttpInfo.uartDataLen + 1)));
            if(ghttpInfo.pHeader == NULL)
            {
                ghttpInfo.uartDataLen = 0;
                ghttpInfo.inputTimeout = 0;
                at_CmdRespOK(ghttpInfo.atEngine);
                http_urcRsp("+QHTTPGET", HTTP_ALLOC_MEM_FAIL);
                return;
            }
            memset(ghttpInfo.pHeader, 0x00, (sizeof(char) * (ghttpInfo.uartDataLen + 1)));
            ghttpInfo.headerLen = 0;
            ghttpInfo.getDataStatus = TRUE;
            
            at_CmdRespOutputText(ghttpInfo.atEngine, (uint8 *)"\r\nCONNECT\r\n");
            http_inputTimer(TRUE);
            KING_CurrentTickGet(&httpLastRxTick);
            at_CmdSetBypassMode(ghttpInfo.atEngine, (AT_CMD_BYPASS_CALLBACK_T)http_BypassDataRecv, NULL);
        }
        else
        {
            http_msgSend(HTTP_GET_REQ);
        }
    }
    else
    {
        if (httpCfgConfig.reqHeader == 1)
        {
            at_CmdRespOK(ghttpInfo.atEngine); 
        }
        http_urcRsp("+QHTTPGET", HTTP_EMPTY_URL);
    }
    return;
}

static void http_qHttpGetCmdSet(AT_CMD_ENGINE_T *atEngine)
{
    int ret = -1;
   
    if (!tcpip_IsPdpActived(httpCfgConfig.ctxID))
    {
        if (tcpip_IsPdpActiveAllowed())
        {
            ghttpInfo.isRunning = FALSE;
            at_CmdRespCmeError(atEngine, HTTP_NW_DEACTIVATED);
            return;
        }
        ret = tcpip_activeByCtxID(atEngine, httpCfgConfig.ctxID, PDP_ACTIVE_SOCK_HTTP_ACTIVE);
        if (ret == -1)
        {
            ghttpInfo.isRunning = FALSE;
            at_CmdRespCmeError(atEngine, CME_TCPIP_OPEN_PDP_CONTEXT_FAILED);
        }
        else if (ret == 1)
        {
            if (httpCfgConfig.reqHeader == 0)
            {
                at_CmdRespOK(ghttpInfo.atEngine);
            }
            http_getReqHandler();
        }
    }
    else
    {
        if (httpCfgConfig.reqHeader == 0)
        {
            at_CmdRespOK(ghttpInfo.atEngine);
        }
        http_getReqHandler();
    }
}

static void http_qHttpReadCmdSet(AT_CMD_ENGINE_T *atEngine)
{
    int ret = -1;
    char *pHeaderStr = NULL;
    uint32 headLen = 0;
    char *pReadData = NULL;
    uint8 readEnd = 0;
    uint32 readLen = 0;

    if (!http_canReadData())
    {
        ghttpInfo.isRunning = FALSE;
        at_CmdRespCmeError(atEngine, HTTP_NO_GET_POST_REQ);
        return;
    }
    
    at_CmdRespOutputText(atEngine, (uint8 *)"\r\nCONNECT\r\n");
    
    if(http_rspHeaderGet() == 1)
    {
        http_readHeaderData(&pHeaderStr, &headLen);
        if (headLen == 0 || pHeaderStr == NULL)
        {
            ghttpInfo.isRunning = FALSE;
            http_clearStatus();
            at_CmdRespCmeError(atEngine, HTTP_NO_GET_POST_REQ);
            return;
        }
        memset(g_rspStr, 0x00, sizeof((char *)g_rspStr));
        sprintf((char *)g_rspStr, "%s", pHeaderStr);
        at_CmdRespOutputText(atEngine, g_rspStr);
        at_CmdRespOutputText(atEngine, (uint8 *)"\r\n\r\n");
    }
    
    pReadData = malloc(HTTP_RESPONSE_SIZE + 1);
    if (pReadData == NULL)
    {
        ghttpInfo.isRunning = FALSE;
        http_clearStatus();
        at_CmdRespCmeError(atEngine, HTTP_ALLOC_MEM_FAIL);
        return;
    }
    while (!readEnd)
    {
        KING_SysLog("http_qHttpReadCmdSet start");
        memset(pReadData, 0x00, HTTP_RESPONSE_SIZE + 1);
        ret = http_readContentDataAndLen(ghttpInfo.waitTimeout, HTTP_RESPONSE_SIZE, pReadData, &readLen, &readEnd);
        KING_SysLog("http_qHttpReadCmdSet ret = %d,readEnd=%d", ret, readEnd);
        if (ret != 0)
        {
            if (pReadData != NULL)
            {
                free(pReadData);
                pReadData = NULL;
            }
            at_CmdRespOK(ghttpInfo.atEngine);
            http_urcRsp("+QHTTPREAD", ret);
            return;
        }
        memset(g_rspStr, 0x00, sizeof((char *)g_rspStr));
        memcpy(g_rspStr, pReadData, readLen);
        at_CmdRespOutputNText(atEngine, g_rspStr, readLen);
        KING_Sleep(200);
        
        KING_SysLog("http_qHttpReadCmdSet end");
    }
    if (pReadData != NULL)
    {
        free(pReadData);
        pReadData = NULL;
    }
    
    at_CmdRespOK(ghttpInfo.atEngine);
    http_urcRsp("+QHTTPREAD", 0);    
}

static void http_msgSend(HTTP_MSG_ID_TYPE_E msgId)
{
    int ret = 0;
    MSG_S msg;
    
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = msgId;

    ret = KING_MsgSend(&msg, httpMsgHandle);
    if(-1 == ret)
    {
        KING_SysLog("http_msgSend KING_MsgSend() Failed!");
        return;
    }
}

static void http_getReq(void)
{
    int hContext = tcpip_getSwHandleByCID(httpCfgConfig.ctxID);
    
    if (hContext == -1)
    {
        http_urcRsp("+QHTTPGET", HTTP_NW_OPEN_FAIL);
        return;
    }
    if (httpCfgConfig.reqHeader == 1)
    {
        http_get(hContext, ghttpInfo.pHeader, ghttpInfo.headerLen, (uint16)ghttpInfo.rspTimeout, http_getCb);
    }
    else
    {
        http_get(hContext, NULL, 0, (uint16)ghttpInfo.rspTimeout, http_getCb);
    }
}

static void http_evtThread(void *argv)
{
    MSG_S msg;
    int ret = -1;
    
    KING_SysLog("http_evtThread start!\r\n");
    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        ret = KING_MsgRcv(&msg, httpMsgHandle, WAIT_OPT_INFINITE);
        if (ret == -1)
        {
            KING_SysLog("http_evtThread ret=%d\r\n", ret);
            continue;
        }
        
        KING_SysLog("http_evtThread msg.messageID=%d\r\n", msg.messageID);
        switch(msg.messageID)
        {
            case HTTP_GET_REQ:
                http_getReq();
                break;
                
            default:
                break;
        }
    }
    
    httpMsgHandle = NULL;
    KING_ThreadExit();
}

static int http_creatThreadAndMsg(void)
{
    int ret = SUCCESS;
    THREAD_ATTR_S httpThreadAttr;

    ret = KING_MsgCreate(&httpMsgHandle);
    if(-1 == ret)
    {
        KING_SysLog("http_creatThreadAndMsg KING_MsgCreate fail!");
        httpMsgHandle = NULL;
        return -1;
    }
    if (httpMsgHandle == NULL)
    {
        KING_SysLog("http_creatThreadAndMsg msgHandle is NULL");
        return -1;
    }
    memset(&httpThreadAttr, 0x00, sizeof(THREAD_ATTR_S));
    httpThreadAttr.fun = http_evtThread;
    httpThreadAttr.priority = THREAD_PRIORIT1;
    httpThreadAttr.stackSize = 1024 * 5;
    httpThreadAttr.queueNum = 64; 
    
    ret = KING_ThreadCreate("httpThread", &httpThreadAttr, &httpThreadID);
    if (-1 == ret)
    {
        KING_SysLog("http_creatThreadAndMsg Create Thread Failed!!");
        httpThreadID = NULL;
        return -1;
    }
    if (httpThreadID == NULL)
    {
        KING_SysLog("http_creatThreadAndMsg httpThreadID is NULL");
        return -1;
    }

    return 0;
}

/*******************************************************************************
 ** Functions
 ******************************************************************************/
void AT_http_init(void)
{
    http_creatThreadAndMsg();
    http_setDefaultConfig();
    memset(&ghttpInfo, 0x00, sizeof(HTTP_INFO_S));
    memset(httpPlusBuf, 0x00, 4);
}

void http_activePdpCnf(bool isActive)
{
    char *parse_name = http_cmdName();

    KING_SysLog("http_activePdpCnf:isActive=%d", isActive);
    if (isActive)
    {
        if (ghttpInfo.actionType == AT_HTTP_ACTION_GET)
        {
            if (httpCfgConfig.reqHeader == 0)
            {
                at_CmdRespOK(ghttpInfo.atEngine);
            }
            http_getReqHandler();
        }
    }
    else
    {
        if (ghttpInfo.actionType == AT_HTTP_ACTION_GET)
        {
            at_CmdRespOK(ghttpInfo.atEngine);
            http_urcRsp(parse_name, HTTP_NW_OPEN_FAIL);
        }
        else
        {
            at_CmdRespOK(ghttpInfo.atEngine); 
            http_urcRsp(parse_name, HTTP_UNKNOWN_ERR);
        }
    }
}

bool http_isEnterDataMode(void)
{
    bool ret = FALSE;

    if(TRUE == ghttpInfo.getDataStatus)
    {
        ret = TRUE;
    }

    return ret;
}

void http_networkDownExit(uint8 ctxID)
{
    if (ctxID == httpCfgConfig.ctxID)
    {
        http_inputTimer(FALSE);
        if (ghttpInfo.getDataStatus == TRUE)
        {
            if(ghttpInfo.pHeader != NULL)
            {
                free(ghttpInfo.pHeader);
                ghttpInfo.pHeader = NULL;
                ghttpInfo.headerLen = 0;
            }
        }
        if(http_stateTypeGet() >= HTTP_STATE_URL_READY)
        {
            http_stateTypeSet(HTTP_STATE_URL_READY);
        }
        else
        {
            http_stateTypeSet(HTTP_STATE_NONE);
        }
        ghttpInfo.getDataStatus = FALSE;
        ghttpInfo.urlDataStatus = FALSE;
        ghttpInfo.uartDataLen = 0;

        if (ghttpInfo.actionType == AT_HTTP_ACTION_GET)
        {
            char *parseName = http_cmdName();

            at_CmdRespOK(ghttpInfo.atEngine);
            http_urcRsp(parseName, HTTP_SOCK_CLOSE);
        }
        
        ghttpInfo.actionType = AT_HTTP_ACTION_NUM;
    }
}

void AT_CmdFunc_QHTTPCFG(AT_CMD_PARA *pParam)
{
    if (pParam == NULL)
    {
        return;
    }
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[HTTP_STR_RSP_LEN] = {0};

            memset(strRsp, 0x00, HTTP_STR_RSP_LEN);
            sprintf(strRsp, "+QHTTPCFG: \"contextid\",(1-4)\r\n+QHTTPCFG: \"requestheader\",(0,1)\r\n+QHTTPCFG: \"responseheader\",(0,1)\r\n+QHTTPCFG: \"sslctxid\",(0-5)\r\n+QHTTPCFG: \"contenttype\",(0-4)");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;

        case AT_CMD_SET:
        {
            bool paramok = true;
            HTTP_CFG_TYPE_E cfgType = QHTTPCFG_NONE_TYPE;
            const uint8_t *argStr = NULL;
            char cfgName[20] = {0};
            int param = 0, ret = 0;
            uint16 minVal = 0, maxVal = 0;
            
            if (pParam->paramCount < 1 || pParam->paramCount > 2)
            {
                at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
                break;
            }

            argStr = at_ParamOptStr(pParam->params[0], &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
                return;
            }
            if (strlen((char *)argStr) == 0 ||  strlen((char *)argStr) > 20)
            {
                at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
                return;
            }
            memset(cfgName, 0x00, 20);
            cfgType = http_qHttpCfgInputType((char *)argStr, cfgName);
            if (cfgType == QHTTPCFG_NONE_TYPE)
            {
                at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
                break;
            }
            if (pParam->paramCount == 1)
            {
                char buff[50] = {0};
                
                http_qHttpCfgGetConfig(cfgType, (uint8 *)&param);
                memset(buff, 0x00, 50);
                sprintf(buff, "+QHTTPCFG: \"%s\",%d", cfgName, (uint8)param);
                at_CmdRespInfoText(pParam->engine, (uint8 *)buff);
                at_CmdRespOK(pParam->engine);
                break;
            }
            if (cfgType == QHTTPCFG_CONTEXT_ID_TYPE)
            {
                minVal = 1;
                maxVal = 4;
            }
            else if (cfgType == QHTTPCFG_REQ_HEADER_TYPE)
            {
                minVal = 0;
                maxVal = 1;
            }
            else if (cfgType == QHTTPCFG_RSP_HEADER_TYPE)
            {
                minVal = 0;
                maxVal = 1;
            }
            else if (cfgType == QHTTPCFG_SSL_CTXID_TYPE)
            {
                minVal = 0;
                maxVal = 5;
            }
            else if (cfgType == QHTTPCFG_CONTENT_TYPE)
            {
                minVal = 0;
                maxVal = 4;
            }
            param = at_ParamUintInRange(pParam->params[1], minVal, maxVal, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
                return;
            }
            ret = http_qHttpCfgSetAndSave(cfgType, (uint8)param);
            if (ret == 0)
            {
                at_CmdRespOK(pParam->engine);
            }
            else
            {
                at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
            }
        }
            break;

        case AT_CMD_READ:
            http_qHttpCfgCmdRead(pParam->engine);
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
            break;
    }
    
    return;
}

void AT_CmdFunc_QHTTPURL(AT_CMD_PARA *pParam)
{
    if (pParam == NULL)
    {
        return;
    }
    
    ghttpInfo.atEngine = pParam->engine;
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[40] = {0};

            memset(strRsp, 0x00, 40);
            sprintf(strRsp, "+QHTTPURL: (1-300),(1-65535)");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;
            
        case AT_CMD_READ:
            http_qHttpUrlCmdRead(pParam->engine);
            break;

        case AT_CMD_SET:
        {
            bool paramok = true;
            int urlLen = 0, inputTime = 0;
            
            if (pParam->paramCount < 1 || pParam->paramCount > 2)
            {
                at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
                break;
            }
            
            urlLen = at_ParamUintInRange(pParam->params[0], 1, 300, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
                return;
            }

            if (pParam->paramCount == 2)
            {
                inputTime = at_ParamUintInRange(pParam->params[1], 1, 65535, &paramok);
                if (!paramok)
                {
                    at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
                    return;
                }
            }
            else
            {
                inputTime = 60;
            } 
            if (ghttpInfo.isRunning)
            {
                at_CmdRespCmeError(pParam->engine, HTTP_BUSY);
                return;
            }
            http_init();
            ghttpInfo.uartDataLen = urlLen;
            ghttpInfo.inputTimeout = inputTime;
            ghttpInfo.urlDataStatus = TRUE;
            ghttpInfo.isRunning = TRUE;
            ghttpInfo.uartDataGot = 0;
            ghttpInfo.actionType = AT_HTTP_ACTION_NONE;
            memset(ghttpInfo.url, 0x00, HTTP_URL_INPUT_MAX_LEN + 1);
            http_stateTypeSet(HTTP_STATE_URL_GETTING);
            
            at_CmdRespOutputText(pParam->engine, (uint8 *)"\r\nCONNECT\r\n");
            
            http_inputTimer(TRUE);
            KING_CurrentTickGet(&httpLastRxTick);
            at_CmdSetBypassMode(pParam->engine, (AT_CMD_BYPASS_CALLBACK_T)http_BypassDataRecv, NULL);
        }
            break;
            
        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
            break;
    }
    
    return;
}

void AT_CmdFunc_QHTTPGET(AT_CMD_PARA *pParam)
{
    if (pParam == NULL)
    {
        return;
    }
    
    ghttpInfo.atEngine = pParam->engine;
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[50] = {0};

            memset(strRsp, 0x00, 50);
            sprintf(strRsp, "+QHTTPGET: (1-65535),(1-1024),(1-65535)");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;
            
        case AT_CMD_EXE: 
            if (httpCfgConfig.reqHeader == 0)
            {
                if (ghttpInfo.isRunning)
                {
                    at_CmdRespCmeError(pParam->engine, HTTP_BUSY);
                    return;
                }
                ghttpInfo.rspTimeout = 60;
                ghttpInfo.actionType = AT_HTTP_ACTION_GET;
                ghttpInfo.isRunning = TRUE;
                http_qHttpGetCmdSet(pParam->engine);
            }
            else
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
            }
            break;
            
        case AT_CMD_SET:
        {
            int rspTime = 0, dataLen = 0, inputTime = 0; 
            bool paramok = true;

            if (httpCfgConfig.reqHeader == 1)
            {
                if (pParam->paramCount < 2 || pParam->paramCount > 3)
                {
                    at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
                    return;
                }
                rspTime = at_ParamUintInRange(pParam->params[0], 1, 65535, &paramok);
                if (!paramok)
                {
                    at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
                    return;
                }
                dataLen = at_ParamUintInRange(pParam->params[1], 1, 1024, &paramok);
                if (!paramok)
                {
                    at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
                    return;
                }
                if (pParam->paramCount >= 3)
                {
                    inputTime = at_ParamUintInRange(pParam->params[2], 1, 65535, &paramok);
                    if (!paramok)
                    {
                        at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
                        return;
                    }
                }
                else
                {
                    inputTime = 60;
                }
                
            }
            else if (httpCfgConfig.reqHeader == 0)
            {
                if (pParam->paramCount > 1)
                {
                    at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
                    return;
                }
                if (pParam->paramCount == 1)
                {
                    rspTime = at_ParamUintInRange(pParam->params[0], 1, 65535, &paramok);
                    if (!paramok)
                    {
                        at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
                        return;
                    }
                }
                else
                {
                    rspTime = 60;
                }
            }
            else
            {
                at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
            }
            if (ghttpInfo.isRunning)
            {
                at_CmdRespCmeError(pParam->engine, HTTP_BUSY);
                return;
            }
            ghttpInfo.inputTimeout = inputTime;
            ghttpInfo.rspTimeout = rspTime;
            ghttpInfo.uartDataLen = dataLen;
            ghttpInfo.actionType = AT_HTTP_ACTION_GET;
            ghttpInfo.isRunning = TRUE;
            ghttpInfo.headerLen = 0;
            http_qHttpGetCmdSet(pParam->engine);
        }
            break;
            
        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
            break;
    }
    
    return;
}

void AT_CmdFunc_QHTTPREAD(AT_CMD_PARA *pParam)
{
    if (pParam == NULL)
    {
        return;
    }
    
    ghttpInfo.atEngine = pParam->engine;
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[40] = {0};

            memset(strRsp, 0x00, 40);
            sprintf(strRsp, "+QHTTPREAD: (1-65535)");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;
            
        case AT_CMD_SET:
        {
            int waitTime = 0; 
            bool paramok = true;

            if (pParam->paramCount > 1)
            {
                at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
                return;
            }
            if (pParam->paramCount == 1)
            {
                waitTime = at_ParamUintInRange(pParam->params[0], 1, 65535, &paramok);
                if (!paramok)
                {
                    at_CmdRespCmeError(pParam->engine, HTTP_INVALID_PARAM);
                    return;
                }
            }
            else
            {
                ghttpInfo.waitTimeout = 60;
            }
            ghttpInfo.waitTimeout = waitTime;
            ghttpInfo.actionType = AT_HTTP_ACTION_READ;
            http_qHttpReadCmdSet(pParam->engine);
        }
            break;
            
        case AT_CMD_EXE:
            ghttpInfo.waitTimeout = 60;
            ghttpInfo.actionType = AT_HTTP_ACTION_READ;
            http_qHttpReadCmdSet(pParam->engine);
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
            break;
    }

    return;
}

