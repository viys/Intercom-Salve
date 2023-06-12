/*******************************************************************************
 ** File Name:   at_cmd_icamqtt.c
 ** Author:      Allance.Chen
 ** Date:        2020-06-10
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains ds functions about ICA MQTT.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-06-10     Allance.Chen         Create.
 ******************************************************************************/
/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "At_common.h"
#include "At_errcode.h"
#include "At_module.h"
#include "AT_define.h"

#include "at_cmd_utils.h"

#include "kingsocket.h"
#include "kingplat.h"
#include "kingcstd.h"
#include "kingNet.h"
#include "kingCbData.h"

#include "dev_sign_api.h"
#include "infra_defs.h"
#include "mqtt_utils.h"
#include "mqtt_client.h"
#include "at_cmd_utils.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define ICAMQTT_STR_RSP_LEN                 200
#define ICAMQTT_STR_LEN                     128

#define ICAMQTT_MSG_LEN_MAX                 (1500)
#define ICAMQTT_PUB_MSG_BUF_SIZE            1024
#define ICAMQTT_NORMAL_RECONN_MAX_COUNT     60

#define ICAMQTT_FRAGMENT_ID_MAX             (11)

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef enum {
    ICAMQTT_NONE_TYPE = 0,
    ICAMQTT_TIMEOUT_TYPE,
    ICAMQTT_CLEAN_TYPE,
    ICAMQTT_KEEPALIVE_TYPE,
    ICAMQTT_VERSION_TYPE,
    ICAMQTT_AUTHMODE_TYPE,
    ICAMQTT_USER_TYPE,
    ICAMQTT_PASSWD_TYPE,
    ICAMQTT_CLIENTID_TYPE,
    ICAMQTT_MAX_TYPE
}   icaMqttParaType;

typedef enum {
    ICAMQTT_CONN_STATE_IDLE_TYPE = 0,
    ICAMQTT_CONN_STATE_CONNECTING_TYPE,
    ICAMQTT_CONN_STATE_CONNECTED_TYPE,
} icaMqttConnStateType;

typedef enum {
    ICAMQTT_PUB_NONE_TYPE = 0,
    ICAMQTT_PUB_TYPE,
    ICAMQTT_PUB_BIN_TYPE,
    ICAMQTT_PUB_MAX
} icaMqttPubType;

typedef struct {
    bool used;
    char topic[ICAMQTT_STR_LEN];
    uint32 packetId;
    uint8 qos;
} icaMqttSubMsgInfo;

typedef struct {
    uint8 qos;
    uint8 format;
    uint8 fragNum;
    uint16 fragMsgMaxLen;
    bool isFirst;
    char topic[ICAMQTT_STR_LEN];
} icaMqttPubTempFragInfo;

typedef struct {
    uint8 fragId;
    char *fragMsg;
    bool isUsed;
    uint16 len;
} icaMqttPubMsgFragInfo;

typedef struct {
    uint8 fragId;
    uint16 fragLen;
} icaMqttPubFragInfo;

typedef struct {
    char topic[ICAMQTT_STR_LEN];
    uint8 qos;
    //uint8 used;
    uint32 packetId;
    char msg[ICAMQTT_PUB_MSG_BUF_SIZE + 1];
    uint8 format;
    uint8 fragNum;
    icaMqttPubFragInfo fragInfo[ICAMQTT_FRAGMENT_ID_MAX];
} icaMqttPubMsgInfo;

typedef struct
{
    char topic[ICAMQTT_STR_LEN];
    uint8 qos;
    //uint8 used;
    uint32 packetId;
    char msg[ICAMQTT_PUB_MSG_BUF_SIZE + 1];
    uint16 len;
} icaMqttPubBinMsgInfo;

typedef struct {
    char topic[ICAMQTT_STR_LEN];
    uint32 packetId;
    char msg[ICAMQTT_PUB_MSG_BUF_SIZE + 1];
    uint16 rcvDataLen;
} icaMqttRecvMsgInfo;

typedef struct {
    char topic[ICAMQTT_STR_LEN];
    bool used;
    uint32 packetId;
} icaMqttUnsubMsgInfo;

typedef struct {  
    bool cleanFlag;  
    bool timeoutFlag;  
    bool keepaliveFlag;  
    bool versionFlag;
    bool authModeFlag;
} icaMqttConnParaInfo;

typedef struct {
    uint16 timeout;
    uint16 clean;
    uint16 keepalive;  
    char vesion[6];
    uint8 authMode;
} icaMqttParaInfo;

typedef struct {
    AT_CMD_ENGINE_T *atEngine;
    uint8 mode;
    char productKey[IOTX_PRODUCT_KEY_LEN + 1];
    char deviceName[IOTX_DEVICE_NAME_LEN + 1];
    char deviceSecret[IOTX_DEVICE_SECRET_LEN + 1];
    char hostname[DEV_SIGN_HOSTNAME_MAXLEN + 1];
    uint16 port;
    bool isMqttAuth;
    icaMqttConnParaInfo connParaFlag;

    bool isReconn;
    uint8 reconnTryCnt;
    icaMqttConnStateType connState;

    uint8 rcvFormat;
    uint16 rcvFragLen;
} icaMqttInfo;

/*******************************************************************************
 ** External Variables 
 ******************************************************************************/
extern UINT8 g_rspStr[];

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static icaMqttInfo mqttInfo;
static mqtt_client_param_t mqttParaInfo;
static iotx_sign_mqtt_t mqttConnInfo;
static icaMqttParaInfo mqttParaCmdInfo;
static icaMqttSubMsgInfo mqttSubMsgInfo;
static icaMqttPubMsgInfo mqttPubMsgInfo;
static icaMqttRecvMsgInfo mqttRcvMsgInfo;
static icaMqttUnsubMsgInfo mqttUnsubMsgInfo;
static icaMqttPubMsgFragInfo mqttPubFragInfo[ICAMQTT_FRAGMENT_ID_MAX];
static icaMqttPubTempFragInfo mqttPubTempInfo;
static icaMqttPubBinMsgInfo mqttPubBinMsgInfo;
static icaMqttRecvMsgInfo mqttRcvBinMsgInfo;

static mqtt_client_t *pIcaMqttClient = NULL;
static hardwareHandle hIcaMqttNIC = -1;
static softwareHandle hIcaMqttContext = -1;
static THREAD_HANDLE pIcaMqttThreadID = NULL;
static volatile bool icamqttRunning = FALSE;
static SEM_HANDLE icamqttTlsSem = NULL;
static MUTEX_HANDLE icamqttMutex = NULL;
static uint8* icamqttBypassData = NULL;
static uint16 icamqttBypassLen = 0;
static icaMqttPubType pubType = ICAMQTT_PUB_TYPE;

const char *icamqtt_ca_crt = \
{
    \
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\r\n" \
    "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n" \
    "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\r\n" \
    "MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n" \
    "YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\r\n" \
    "aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\r\n" \
    "jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\r\n" \
    "xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\r\n" \
    "1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\r\n" \
    "snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\r\n" \
    "U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\r\n" \
    "9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\r\n" \
    "BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\r\n" \
    "AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\r\n" \
    "yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\r\n" \
    "38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\r\n" \
    "AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\r\n" \
    "DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\r\n" \
    "HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\r\n" \
    "-----END CERTIFICATE-----"
};

/*******************************************************************************
 ** Local Function Delcarations
 ******************************************************************************/
static void icamqtt_disconnectNetwork(bool isReconn);
static int icamqtt_connect(AT_CMD_ENGINE_T *atEngine, bool isReconn);

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
static void icamqtt_setDefaultPara(void)
{
    mqttParaCmdInfo.timeout = 75;
    mqttParaCmdInfo.clean = 0;
    mqttParaCmdInfo.keepalive = 120;
    memset(mqttParaCmdInfo.vesion, 0x00, 6); 
    strcpy(mqttParaCmdInfo.vesion, "3.1.1");
    mqttParaCmdInfo.authMode = 1;
}

static void icamqtt_resetPubInfo(void)
{
    uint8 i = 0;

    for (i = 0; i < ICAMQTT_FRAGMENT_ID_MAX; i++)
    {
        if (mqttPubFragInfo[i].fragMsg != NULL)
        {
            free(mqttPubFragInfo[i].fragMsg);
            mqttPubFragInfo[i].fragMsg = NULL;
        }
        mqttPubFragInfo[i].fragId = 0;
        mqttPubFragInfo[i].isUsed = FALSE;
        mqttPubFragInfo[i].len = 0;
    }
}

static int icamqtt_catPubInfo(char *msg)
{
    uint8 i = 0;

    if (msg == NULL)
    {
        return -1;
    }
    for (i = 0; i <= mqttPubTempInfo.fragNum; i++)
    {
        strcat(msg, mqttPubFragInfo[i].fragMsg);
    }
    return 0;
}

static void icamqtt_getPubfragInfo(void)
{
    uint8 i = 0;

    for (i = 0; i <= mqttPubTempInfo.fragNum; i++)
    {
        mqttPubMsgInfo.fragInfo[i].fragId = mqttPubFragInfo[i].fragId;
        mqttPubMsgInfo.fragInfo[i].fragLen = mqttPubFragInfo[i].len;
    }
}

static void icamqtt_urcRsp(char *cmdString, uint32 packetId, uint16 status)
{
    memset(g_rspStr, 0x00, sizeof((char *)g_rspStr));
    snprintf((char *)g_rspStr, 40, "\r\n%s: %u,%d\r\n", cmdString, packetId, status);
    at_CmdRespOutputText( mqttInfo.atEngine, g_rspStr);
    KING_SysLog("icamqtt_urcRsp end.");
}

static void icamqtt_urcStrRsp(char *urcStr)
{
    memset(g_rspStr, 0x00, sizeof((char *)g_rspStr));
    sprintf((char *)g_rspStr, "\r\n%s\r\n", urcStr);
    at_CmdRespOutputText( mqttInfo.atEngine, g_rspStr);
}

static void icamqtt_eventHandle(void *pcontext, void *pclient, mqtt_event_msg_t *msg)
{
    uint32 packetId = (uint32)msg->msg;
    mqtt_client_topic_info_t *topic_info = (mqtt_client_topic_info_t *)msg->msg;

    KING_SysLog("icaMqtt_eventHandle,begin.msg->event_type=%d", msg->event_type);
    switch (msg->event_type) 
    {
        case MQTT_EVENT_UNDEF:
            KING_SysLog("MQTT undefined event occur.");
            break;

        case MQTT_EVENT_DISCONNECT:
            KING_SysLog("MQTT disconnect.");
            if (mqttInfo.connState == ICAMQTT_CONN_STATE_CONNECTED_TYPE)
            {
                icamqtt_urcStrRsp("+IMQTTDISCONN");
                mqttInfo.connState = ICAMQTT_CONN_STATE_IDLE_TYPE;
                mqttInfo.reconnTryCnt = 0;
                mqttInfo.isReconn = TRUE;
            }
            break;

        case MQTT_EVENT_RECONNECT:
            KING_SysLog("MQTT reconnect.");
            mqttInfo.isReconn = FALSE;
            mqttInfo.connState = ICAMQTT_CONN_STATE_CONNECTED_TYPE;
            mqttInfo.reconnTryCnt = 0;
            icamqtt_urcStrRsp("+IMQTTCONN");
            break;

        case MQTT_EVENT_SUBCRIBE_SUCCESS:
            KING_SysLog("MQTT subscribe success, packetId-id=%u", (unsigned int)packetId);
            icamqtt_urcRsp("+IMQTTSUB", packetId, mqttSubMsgInfo.qos);
            mqttSubMsgInfo.used = FALSE;
            break;

        case MQTT_EVENT_SUBCRIBE_TIMEOUT:
            KING_SysLog("MQTT subscribe wait ack timeout, packetId=%u", (unsigned int)packetId);
            icamqtt_urcRsp("+IMQTTSUB", packetId, 128);
            memset(&(mqttSubMsgInfo), 0x00, sizeof(icaMqttSubMsgInfo));
            break;

        case MQTT_EVENT_SUBCRIBE_NACK:
            KING_SysLog("MQTT subscribe nack, packetId=%u", (unsigned int)packetId);
            icamqtt_urcRsp("+IMQTTSUB", packetId, 128);
            memset(&(mqttSubMsgInfo), 0x00, sizeof(icaMqttSubMsgInfo));
            break;

        case MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            KING_SysLog("MQTT unsubscribe success, packetId=%u", (unsigned int)packetId);
            icamqtt_urcRsp("+IMQTTUNSUB", packetId, 0);
            mqttUnsubMsgInfo.used = FALSE;
            break;

        case MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            KING_SysLog("MQTT unsubscribe timeout, packetId=%u", (unsigned int)packetId);
            icamqtt_urcRsp("+IMQTTUNSUB", packetId, 1);
            memset(&(mqttUnsubMsgInfo), 0x00, sizeof(icaMqttUnsubMsgInfo));
            break;

        case MQTT_EVENT_UNSUBCRIBE_NACK:
            KING_SysLog("MQTT unsubscribe nack, packetId=%u", (unsigned int)packetId);
            icamqtt_urcRsp("+IMQTTUNSUB", packetId, 2);
            memset(&(mqttUnsubMsgInfo), 0x00, sizeof(icaMqttUnsubMsgInfo));
            break;

        case MQTT_EVENT_PUBLISH_SUCCESS:
            KING_SysLog("MQTT publish success, packetId=%u", (unsigned int)packetId);
            if (pubType == ICAMQTT_PUB_BIN_TYPE)
            {
                icamqtt_urcRsp("+IMQTTPUBIN", packetId, 0);
                //mqttPubBinMsgInfo.used = FALSE;
            }
            else
            {
                icamqtt_urcRsp("+IMQTTPUB", packetId, 0);
                //mqttPubMsgInfo.used = FALSE;
            }
            break;

        case MQTT_EVENT_PUBLISH_TIMEOUT:
            KING_SysLog("MQTT publish timeout, packetId=%u", (unsigned int)packetId);
            if (pubType == ICAMQTT_PUB_BIN_TYPE)
            {
                icamqtt_urcRsp("+IMQTTPUBIN", packetId, 1);
                memset(&(mqttPubBinMsgInfo), 0x00, sizeof(icaMqttPubBinMsgInfo));
            }
            else
            {
                icamqtt_urcRsp("+IMQTTPUB", packetId, 1);
                memset(&(mqttPubMsgInfo), 0x00, sizeof(icaMqttPubMsgInfo));
            }
            break;

        case MQTT_EVENT_PUBLISH_NACK:
            KING_SysLog("MQTT publish nack, packetId=%u", (unsigned int)packetId);
            if (pubType == ICAMQTT_PUB_BIN_TYPE)
            {
                icamqtt_urcRsp("+IMQTTPUBIN", packetId, 2);
                memset(&(mqttPubBinMsgInfo), 0x00, sizeof(icaMqttPubBinMsgInfo));
            }
            else
            {
                icamqtt_urcRsp("+IMQTTPUB", packetId, 2);
                memset(&(mqttPubMsgInfo), 0x00, sizeof(icaMqttPubMsgInfo));
            }
            break;

        case MQTT_EVENT_PUBLISH_RECEIVED:
            KING_SysLog("MQTT topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
                            topic_info->topic_len,
                            topic_info->ptopic,
                            topic_info->payload_len,
                            topic_info->payload);
            break;

        default:
            KING_SysLog("MQTT Should NOT arrive here.");
            break;
    }
}

static void icamqtt_paramsSet(void)
{
    KING_SysLog("icamqttSetParams,begin!");

    memset(&mqttParaInfo, 0x0, sizeof(mqttParaInfo));
    
    mqttParaInfo.port = mqttConnInfo.port;
    mqttParaInfo.host = mqttConnInfo.hostname;
    mqttParaInfo.client_id = mqttConnInfo.clientid;
    mqttParaInfo.username = mqttConnInfo.username;
    mqttParaInfo.password = mqttConnInfo.password;
    mqttParaInfo.sslMode = mqttInfo.mode;
    
    mqttParaInfo.request_timeout_ms = mqttParaCmdInfo.timeout*1000;
    mqttParaInfo.clean_session = mqttParaCmdInfo.clean;
    mqttParaInfo.keepalive_interval_ms = mqttParaCmdInfo.keepalive*1000;

    mqttParaInfo.read_buf_size = ICAMQTT_MSG_LEN_MAX;
    mqttParaInfo.write_buf_size = ICAMQTT_MSG_LEN_MAX;

    mqttParaInfo.handle_event.h_fp = icamqtt_eventHandle;
    mqttParaInfo.handle_event.pcontext = NULL;

    KING_SysLog("icamqttSetParams,end!");
}

static bool icamqtt_checkConnPara(void)
{
    if(mqttInfo.connParaFlag.keepaliveFlag == TRUE &&
        mqttInfo.connParaFlag.cleanFlag == TRUE &&
        mqttInfo.connParaFlag.timeoutFlag == TRUE &&
        mqttInfo.connParaFlag.versionFlag == TRUE &&
        mqttInfo.connParaFlag.authModeFlag == TRUE)
    {
        return TRUE;
    }
    return FALSE;
}

static int icamqtt_getNicHandle(void)
{
    int ret = 0;

    if (hIcaMqttNIC == -1)
    {
        NIC_INFO info;

        memset(&info, 0x00, sizeof(NIC_INFO));
        info.NIC_Type = NIC_TYPE_WWAN;
        ret = KING_HardwareNICRequest(info, &hIcaMqttNIC);
        KING_SysLog("icamqtt_getNicHandle ret=%d, hIcaMqttNIC=%d", ret, hIcaMqttNIC);
    }
    
    return ret;
}

static int icamqtt_getContextHandle(void)
{
    int ret = 0;

    if (hIcaMqttNIC == -1)
    {
        return -1;
    }

    if (hIcaMqttContext == -1)
    {
        NIC_CONTEXT ApnInfo;
        
        memset(&ApnInfo, 0x00, sizeof(NIC_CONTEXT));
        ApnInfo.IpType = IP_ADDR_TYPE_V4;
        ret = KING_ConnectionAcquire(hIcaMqttNIC, ApnInfo, &hIcaMqttContext);
        KING_SysLog("icamqtt_getContextHandle ret=%d, hIcaMqttContext=%d", ret, hIcaMqttContext);
    }
    
    return ret;
}

static icaMqttParaType icamqtt_imqttParaInputType(char *parameName)
{
    if(AT_StrCaselessCmp((char *)parameName, (char *)"TIMEOUT") == 0)
    {
        return ICAMQTT_TIMEOUT_TYPE;
    }
    else if(AT_StrCaselessCmp((char *)parameName, (char *)"CLEAN") == 0)
    {
        return ICAMQTT_CLEAN_TYPE;
    }
    else if(AT_StrCaselessCmp((char *)parameName, (char *)"KEEPALIVE") == 0)
    {
        return ICAMQTT_KEEPALIVE_TYPE;
    } 
    else if(AT_StrCaselessCmp((char *)parameName, (char *)"VERSION") == 0)
    {
        return ICAMQTT_VERSION_TYPE;
    } 
    else if(AT_StrCaselessCmp((char *)parameName, (char *)"AUTHMODE") == 0)
    {
        return ICAMQTT_AUTHMODE_TYPE;
    }
    else if(AT_StrCaselessCmp((char *)parameName, (char *)"USER") == 0)
    {
        return ICAMQTT_USER_TYPE;
    }
    else if(AT_StrCaselessCmp((char *)parameName, (char *)"PASSWORD") == 0)
    {
        return ICAMQTT_PASSWD_TYPE;
    }
    else if(AT_StrCaselessCmp((char *)parameName, (char *)"CLIENTID") == 0)
    {
        return ICAMQTT_CLIENTID_TYPE;
    }
    else
    {
        return ICAMQTT_NONE_TYPE;
    }
}

static void icamqtt_recvBinRsp(void)
{
    char *rspStr = NULL;
    int size = 0;
    
    rspStr = malloc(mqttRcvBinMsgInfo.rcvDataLen + 180);
    if (rspStr == NULL)
    {
        return;
    }
    memset(rspStr, 0x00, mqttRcvBinMsgInfo.rcvDataLen + 180);
    size = sprintf(rspStr, "\r\n+IMQTTRCVPUBIN: %u,\"%s\",%d,\"", mqttRcvBinMsgInfo.packetId, 
            mqttRcvBinMsgInfo.topic, mqttRcvBinMsgInfo.rcvDataLen);
    memcpy(rspStr + size, mqttRcvBinMsgInfo.msg, mqttRcvBinMsgInfo.rcvDataLen);
    size += mqttRcvBinMsgInfo.rcvDataLen;
    size += sprintf(rspStr + size, "\"\r\n");
    at_CmdRespOutputNText(mqttInfo.atEngine, (uint8 *)rspStr, size);

    if (rspStr != NULL)
    {
        free(rspStr);
        rspStr = NULL;
    }
}

static void icamqtt_recvRsp(AT_CMD_ENGINE_T *atEngine, bool isCmd)
{
    char *rspStr = NULL, *pMsg = NULL;
    uint8 frag_num = 0, frag_id = 0;
    bool isBreak = FALSE;
    uint16 templen = 0, totalLen = 0, length = 0, lastLen = 0;
    int size = 0;
    
    if (mqttInfo.rcvFormat == 1 
        && (mqttRcvMsgInfo.rcvDataLen > ICAMQTT_PUB_MSG_BUF_SIZE))
    {
        if (isCmd)
        {
            at_CmdRespCmeError(atEngine, ERR_AT_UNKNOWN);
        }
        return;
    }
    if (mqttInfo.rcvFormat == 1)
    {
        pMsg = malloc(mqttRcvMsgInfo.rcvDataLen * 2 + 1);
        if (pMsg == NULL)
        {
            if (isCmd)
            {
                at_CmdRespCmeError(atEngine, ERR_AT_CME_NO_MEMORY);
            }
            return;
        }
        memset(pMsg, 0x00, mqttRcvMsgInfo.rcvDataLen * 2 + 1);
        atUtils_ConvertBinToHex((uint8 *)mqttRcvMsgInfo.msg, 
                              (uint16)mqttRcvMsgInfo.rcvDataLen, 
                              (uint8 *)pMsg,
                              0);
        totalLen = mqttRcvMsgInfo.rcvDataLen * 2;
        rspStr = malloc(170 + mqttInfo.rcvFragLen * 2);
    }
    else
    {
        pMsg = malloc(mqttRcvMsgInfo.rcvDataLen + 1);
        if (pMsg == NULL)
        {
            if (isCmd)
            {
                at_CmdRespCmeError(atEngine, ERR_AT_CME_NO_MEMORY);
            }
            return;
        }
        memset(pMsg, 0x00, mqttRcvMsgInfo.rcvDataLen + 1);
        memcpy(pMsg, mqttRcvMsgInfo.msg, mqttRcvMsgInfo.rcvDataLen);
        totalLen = mqttRcvMsgInfo.rcvDataLen;
        rspStr = malloc(170 + mqttInfo.rcvFragLen);
    }
    if (rspStr == NULL)
    {
        if (isCmd)
        {
            at_CmdRespCmeError(atEngine, ERR_AT_CME_NO_MEMORY);
        }
        if (pMsg != NULL)
        {
            free(pMsg);
        }
        return;
    }
    
    frag_num = mqttRcvMsgInfo.rcvDataLen/mqttInfo.rcvFragLen;
    if (mqttRcvMsgInfo.rcvDataLen%mqttInfo.rcvFragLen == 0)
    {
       frag_num--;
    }
    if (frag_num == 0)
    {
        if (mqttInfo.rcvFormat == 1)
        {
            memset(rspStr, 0x00, 170 + mqttInfo.rcvFragLen * 2);
            length = mqttRcvMsgInfo.rcvDataLen * 2;
        }
        else
        {
            memset(rspStr, 0x00, 170 + mqttInfo.rcvFragLen);
            length = mqttRcvMsgInfo.rcvDataLen;
        }
        size = sprintf(rspStr, "\r\n+IMQTTRCVPUB: %d,\"%s\",%d,\"", 
                mqttRcvMsgInfo.packetId, mqttRcvMsgInfo.topic, 
                mqttRcvMsgInfo.rcvDataLen);
        memcpy(rspStr + size, pMsg, length);
        size += length;
        size += sprintf(rspStr + size, "\",%d,0\r\n", mqttInfo.rcvFormat);
        at_CmdRespOutputNText( atEngine, (uint8 *)rspStr, size);
    }
    else
    {
        frag_id = frag_num;
        if (mqttInfo.rcvFormat == 1)
        {
            length = mqttInfo.rcvFragLen * 2;
        }
        else
        {
            length = mqttInfo.rcvFragLen;
        }
        while(templen < totalLen)
        {
            memset(rspStr, 0x00, 170 + length);
            size = 0;
            if(totalLen - templen > length)
            {
                size = sprintf(rspStr, "\r\n+IMQTTRCVPUB: %d,\"%s\",%d,\"", 
                        mqttRcvMsgInfo.packetId, mqttRcvMsgInfo.topic, 
                        mqttInfo.rcvFragLen);
                memcpy((char*)rspStr + size, pMsg + templen,
                        length);
                size += length;
                if (isCmd)
                {
                    size += sprintf(rspStr + size, "\",%d,%d", 
                            mqttInfo.rcvFormat, frag_id);
                }
                else
                {
                    size += sprintf(rspStr + size, "\",%d,%d\r\n", 
                            mqttInfo.rcvFormat, frag_id);
                }
                templen += length;
                isBreak = FALSE;
            }
            else
            {
                if (mqttInfo.rcvFormat == 1)
                {
                    lastLen = (totalLen - templen)/2;
                }
                else
                {
                    lastLen = totalLen - templen;
                }
                size = sprintf(rspStr, "\r\n+IMQTTRCVPUB: %d,\"%s\",%d,\"", 
                      mqttRcvMsgInfo.packetId, mqttRcvMsgInfo.topic, 
                      lastLen);
                memcpy((char*)rspStr + size, (pMsg + templen), (totalLen - templen));
                size += (totalLen - templen);
                size += sprintf(rspStr + size, "\",%d,0\r\n", 
                        mqttInfo.rcvFormat);
                isBreak = TRUE;
            }
            at_CmdRespOutputNText( atEngine, (uint8 *)rspStr, size);
            frag_id--;
            if (isBreak)
            {
                break;
            }
        }
    }
    if (rspStr != NULL)
    {
        free(rspStr);
    }
    if (pMsg != NULL)
    {
        free(pMsg);
    }
    if (isCmd)
    {
        at_CmdRespOK(atEngine);
    }
}

static void icamqtt_reconn(void)
{
    KING_SysLog("icamqtt_reconn connState=%d", mqttInfo.connState);
    if (mqttInfo.connState != ICAMQTT_CONN_STATE_CONNECTED_TYPE)
    {
        icamqtt_connect(mqttInfo.atEngine, TRUE);
    }
}

static void icamqtt_recvThread(void* argv)
{
    KING_SysLog("icamqtt_recvThread start");
    while(1)
    {
        if (!icamqttRunning)
        {
            KING_SemPut(icamqttTlsSem);
            break;
        }
        if (pIcaMqttClient == NULL)
        {
            mqtt_utils_SleepMs(200);
            continue;
        }
        mqtt_client_yield(pIcaMqttClient, 200);
        if (mqttInfo.connState == ICAMQTT_CONN_STATE_CONNECTED_TYPE)
        {
            mqtt_utils_SleepMs(2000);
        }
        else
        {
            mqttInfo.reconnTryCnt++;
            KING_SysLog("reconnect count is %d", mqttInfo.reconnTryCnt);
            if (mqttInfo.reconnTryCnt > ICAMQTT_NORMAL_RECONN_MAX_COUNT)
            {
                icamqtt_disconnectNetwork(TRUE);
            }
            else
            {
                icamqtt_reconn();
                mqtt_utils_SleepMs(10000);
            }
        }
    }
    
    if (pIcaMqttThreadID != NULL)
    {
        pIcaMqttThreadID = NULL;
    } 
    KING_ThreadExit();
}

static void icamqtt_createThreadID(void)
{
    if (pIcaMqttThreadID == NULL)
    {
        int ret = 0;
        THREAD_ATTR_S threadAttr;
        
        KING_SysLog("icamqtt_createThreade Thread create!!");
        memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
        threadAttr.fun = icamqtt_recvThread;
        threadAttr.priority = THREAD_PRIORIT1;
        threadAttr.stackSize = 1024 * 5;
        threadAttr.queueNum = 64; 

        ret = KING_ThreadCreate("IcaMqttThread", &threadAttr, &pIcaMqttThreadID);
        if (ret != 0)
        {
            KING_SysLog("icamqtt_createThreade Thread Failed!!");
        }
    }
}

static void icamqtt_modeSet(uint8 mode)
{
    mqttInfo.mode = mode;
}

static uint8 icamqtt_modeGet(void)
{
    return mqttInfo.mode;
}

static void icamqtt_imqttOPENCmdSet
(
    AT_CMD_ENGINE_T *atEngine,
    char *hostname,
    uint16 port
)
{
    memset(mqttInfo.hostname, 0x00, DEV_SIGN_HOSTNAME_MAXLEN + 1);
    memcpy(mqttInfo.hostname, hostname, strlen(hostname));
    mqttInfo.port = port;
    mqttInfo.atEngine = atEngine;
    mqtt_utils_SetHostName(hostname);
    mqtt_utils_SetPort(port);
    memset(mqttConnInfo.hostname, 0x00, DEV_SIGN_HOSTNAME_MAXLEN + 1);
    if (strlen(hostname) > 0)
    {
        strcpy(mqttConnInfo.hostname, hostname);
    }
    mqttConnInfo.port = port;

    at_CmdRespOK(atEngine);
    return;
}

static void icamqtt_imqttOPENCmdRead(AT_CMD_ENGINE_T *atEngine)
{
    char rspString[140] = {0};

    if (mqttInfo.port == 0)
    {
        mqttInfo.port = 1883;
    }
    memset(rspString, 0x00, 140);
    sprintf (rspString, "+IMQTTOPEN: \"%s\",%d", mqttInfo.hostname, mqttInfo.port);
    at_CmdRespInfoText(atEngine, (uint8 *)rspString);
    at_CmdRespOK(atEngine);
    
    return;
}

static void icamqtt_imqttAUTHCmdSet
(
    AT_CMD_ENGINE_T *atEngine,
    char *prokey,
    char *devname,
    char *devsecret
)
{
    int rc = -1;
    iotx_dev_meta_info_t meta;

    mqtt_utils_SetProductKey(prokey);
    mqtt_utils_SetDeviceName(devname);
    mqtt_utils_SetDeviceSecret(devsecret);
    memset(&meta, 0x00, sizeof(iotx_dev_meta_info_t));
    mqtt_utils_GetProductKey(meta.product_key);
    mqtt_utils_GetDeviceName(meta.device_name);
    mqtt_utils_GetDeviceSecret(meta.device_secret);
    meta.is_tls = mqttInfo.mode;
    rc = IOT_Sign_MQTT(IOTX_CLOUD_REGION_SHANGHAI, &meta, &mqttConnInfo);
    if (rc < 0)
    {
        at_CmdRespOK(atEngine);
        if (mqttParaCmdInfo.authMode == 1)
        {
            at_CmdRespOutputText(atEngine, (uint8 *)"\r\n+IMQTTAUTH: FAIL\r\n");
        }
        return;
    }
    mqttInfo.atEngine = atEngine;
    memset(mqttInfo.productKey, 0x00, IOTX_PRODUCT_KEY_LEN + 1);
    memcpy(mqttInfo.productKey, prokey, strlen(prokey));
    memset(mqttInfo.deviceName, 0x00, IOTX_DEVICE_NAME_LEN + 1);
    memcpy(mqttInfo.deviceName, devname, strlen(devname));
    memset(mqttInfo.deviceSecret, 0x00, IOTX_DEVICE_SECRET_LEN + 1);
    memcpy(mqttInfo.deviceSecret, devsecret, strlen(devsecret));
    
    at_CmdRespOK(atEngine);
    if (mqttParaCmdInfo.authMode == 1)
    {
        mqttInfo.isMqttAuth = TRUE;
        at_CmdRespOutputText(atEngine, (uint8 *)"\r\n+IMQTTAUTH: OK\r\n");
    }
    return;
}

static void icamqtt_icamqttAUTHCmdRead(AT_CMD_ENGINE_T *atEngine)
{
    char rspString[140] = {0};

    memset(rspString, 0x00, 140);
    if (mqttInfo.isMqttAuth)
    {
        KING_SysLog("icamqtt_icamqttAUTHCmdRead %0x%0x%0x%0x%0x!!", mqttInfo.productKey[0], mqttInfo.productKey[1], mqttInfo.productKey[2],mqttInfo.productKey[3],mqttInfo.productKey[4]);
        sprintf (rspString, "+IMQTTAUTH: \"%s\",\"%s\",\"%s\"", 
                mqttInfo.productKey, mqttInfo.deviceName, 
                mqttInfo.deviceSecret);
    }
    else
    {
        sprintf (rspString, "+IMQTTAUTH: \"\",\"\",\"\"");
    }
    at_CmdRespInfoText(atEngine, (uint8 *)rspString);
    at_CmdRespOK(atEngine);
    return;
}

static void icamqtt_imqttPARASetAndSave
(
    AT_CMD_ENGINE_T *atEngine,
    icaMqttParaType paraType, 
    uint16 param,
    char *str
)
{
    KING_SysLog("icamqtt_imqttPARASetAndSave() paraType = %d", paraType);
    if (paraType == ICAMQTT_TIMEOUT_TYPE)
    {
        mqttParaCmdInfo.timeout = param;
        mqttInfo.connParaFlag.timeoutFlag = TRUE;
    }
    else if (paraType == ICAMQTT_CLEAN_TYPE)
    {
        mqttParaCmdInfo.clean = param;
        mqttInfo.connParaFlag.cleanFlag = TRUE; 
    }
    else if (paraType == ICAMQTT_KEEPALIVE_TYPE)
    {
        mqttParaCmdInfo.keepalive = param;
        mqttInfo.connParaFlag.keepaliveFlag = TRUE;
    }
    else if (paraType == ICAMQTT_VERSION_TYPE)
    {
        memset(mqttParaCmdInfo.vesion, 0x00, 6);
        strcpy(mqttParaCmdInfo.vesion, str);
        mqttInfo.connParaFlag.versionFlag = TRUE;
    }
    else if (paraType == ICAMQTT_AUTHMODE_TYPE)
    {
        mqttParaCmdInfo.authMode = param;
        mqttInfo.connParaFlag.authModeFlag = TRUE;
    }
    else if (paraType == ICAMQTT_USER_TYPE)
    {
        if (strlen(str) > 0)
        {
            memset(mqttConnInfo.username, 0x00, DEV_SIGN_USERNAME_MAXLEN + 1);
            memcpy(mqttConnInfo.username, str, strlen(str));
            mqtt_utils_SetUserName(str);
        }
    }
    else if (paraType == ICAMQTT_PASSWD_TYPE)
    {
        if (strlen(str) > 0)
        {
            memset(mqttConnInfo.password, 0x00, DEV_SIGN_PASSWORD_MAXLEN + 1);
            memcpy(mqttConnInfo.password, str, strlen(str));
            mqtt_utils_SetPassword(str);
        }
    }
    else if (paraType == ICAMQTT_CLIENTID_TYPE)
    {
        if (strlen(str) > 0)
        {
            memset(mqttConnInfo.clientid, 0x00, DEV_SIGN_CLIENT_ID_MAXLEN + 1);
            memcpy(mqttConnInfo.clientid, str, strlen(str));
            mqtt_utils_SetClientID(str);
        }
    }
    else
    {
        at_CmdRespCmeError(atEngine, ERR_AT_CME_PARAM_INVALID);
        return;
    }
    
    KING_SysLog("icamqtt_imqttPARASetAndSave() end");
    at_CmdRespOK(atEngine);
    return;
}

static void icamqtt_imqttPARACmdRead(AT_CMD_ENGINE_T *atEngine)
{
    uint16 value = 0;
    char buf[100] = {0};
    uint8 i = 0;
    typedef struct _MQTT_PARA
    {
        icaMqttParaType para_type;
        char para_name[20];
        uint8 value_type; // 0: int, 1: string
    } MQTT_PARA_T;
    MQTT_PARA_T params[] = {
        {ICAMQTT_TIMEOUT_TYPE, "TIMEOUT", 0},
        {ICAMQTT_CLEAN_TYPE, "CLEAN", 0},
        {ICAMQTT_KEEPALIVE_TYPE, "KEEPALIVE", 0},
        {ICAMQTT_VERSION_TYPE, "VERSION", 1},
        {ICAMQTT_AUTHMODE_TYPE, "AUTHMODE", 0},
        {ICAMQTT_USER_TYPE, "USER", 1},
        {ICAMQTT_PASSWD_TYPE, "PASSWORD", 1},
        {ICAMQTT_CLIENTID_TYPE, "CLIENTID", 1},
    };
    uint8 count;
    char *p = NULL;
    char username[DEV_SIGN_USERNAME_MAXLEN + 1] = {0};
    char password[DEV_SIGN_PASSWORD_MAXLEN + 1] = {0};
    char clientID[DEV_SIGN_CLIENT_ID_MAXLEN + 1] = {0};
    
    count = sizeof(params)/sizeof(params[0]);
    
    for (i = 0; i < count; i++)
    {
        switch(params[i].para_type)
        {
        case ICAMQTT_TIMEOUT_TYPE:
            value = mqttParaCmdInfo.timeout;
            break;

        case ICAMQTT_CLEAN_TYPE:
            value = mqttParaCmdInfo.clean;
            break;

        case ICAMQTT_KEEPALIVE_TYPE:
            value = mqttParaCmdInfo.keepalive;
            break;

        case ICAMQTT_VERSION_TYPE:
            p = mqttParaCmdInfo.vesion;
            break;

        case ICAMQTT_AUTHMODE_TYPE:
            value = mqttParaCmdInfo.authMode;
            break;

        case ICAMQTT_USER_TYPE:
            mqtt_utils_GetUserName(username);
            memset(mqttConnInfo.username, 0x00, DEV_SIGN_USERNAME_MAXLEN + 1);
            if (strlen(username) > 0)
            {
                strcpy(mqttConnInfo.username, username);
            }
            p = username;
            break;

        case ICAMQTT_PASSWD_TYPE:
            mqtt_utils_GetPassword(password);
            memset(mqttConnInfo.password, 0x00, DEV_SIGN_PASSWORD_MAXLEN + 1);
            if (strlen(password) > 0)
            {
                strcpy(mqttConnInfo.password, password);
            }
            p = password;
            break;
            
        case ICAMQTT_CLIENTID_TYPE:
            mqtt_utils_GetClientID(clientID);
            memset(mqttConnInfo.clientid, 0x00, DEV_SIGN_CLIENT_ID_MAXLEN + 1);
            if (strlen(clientID) > 0)
            {
                strcpy(mqttConnInfo.clientid, clientID);
            }
            p = clientID;
            break;
            
        default:
            break;
        }

        if (1 == params[i].value_type)
        {
            sprintf(buf, "+IMQTTPARA: \"%s\",\"%s\"\r\n", params[i].para_name, p);
        }
        else
        {
            if (i == 0)
            {
                at_CmdRespOutputText(atEngine, (uint8 *)"\r\n");
            }
            sprintf(buf, "+IMQTTPARA: \"%s\",%d\r\n", params[i].para_name, value);
        }
        at_CmdRespOutputText(atEngine, (uint8 *)buf);
    }
    
    at_CmdRespOK(atEngine);
    return;
}

static int icamqtt_connect(AT_CMD_ENGINE_T *atEngine, bool isReconn)
{
    int ret = 0;
    
    ret = icamqtt_getNicHandle();
    if (ret == -1)
    {
        KING_SysLog("icamqtt_connect,get nic handle failed!");
        if (!isReconn)
        {
            at_CmdRespCmeError(atEngine, ERR_AT_CME_OPTION_NOT_SURPORT);
        }
        return -1;
    }
    
    ret = icamqtt_getContextHandle();
    if (ret == -1)
    {
        KING_SysLog("icamqtt_connect,get context handle failed!");
        if (!isReconn)
        {
            at_CmdRespCmeError(atEngine, ERR_AT_CME_OPTION_NOT_SURPORT);
        }
        return -1;
    }
    
    mqttParaInfo.hContext = hIcaMqttContext;
    if (!isReconn)
    {
        pIcaMqttClient = mqtt_client_connect(&mqttParaInfo);
        if (pIcaMqttClient == NULL)
        {
            KING_SysLog("icamqtt_connect Failed!");
            at_CmdRespCmeError(atEngine, CME_ICA_MQTT_CONNECTED_FAIL);
            return -1;
        }
        
        mqttInfo.connState = ICAMQTT_CONN_STATE_CONNECTED_TYPE;
        icamqttRunning = TRUE;
        icamqtt_createThreadID();
    }
    return 0;
}

static void icamqtt_imqttCONNECTCmdExe(AT_CMD_ENGINE_T *atEngine)
{
    int ret = -1;
    
    if (!icamqtt_checkConnPara())
    {
        KING_SysLog("icamqtt connect is not set para!");
        at_CmdRespCmeError(atEngine, CME_ICA_MQTT_NOT_SET_CONN_PARA);
        return;
    }

    if ((!mqttInfo.isMqttAuth) && (1 == mqttParaCmdInfo.authMode))
    {
        KING_SysLog("icamqtt connect is no auth!");
        at_CmdRespCmeError(atEngine, CME_ICA_MQTT_NOT_AUTH);
        return;
    }

    if (mqtt_client_is_connected(pIcaMqttClient) == 1)
    {
        KING_SysLog("icamqtt connect ,is already connected!");
        at_CmdRespCmeError(atEngine, CME_ICA_MQTT_ALREADY_CONNECTED);
        return;
    }
    
    mqttInfo.isReconn = FALSE;
    mqttInfo.reconnTryCnt = 0;
    mqttInfo.atEngine = atEngine;

    icamqtt_paramsSet();
    if (mqttParaInfo.sslMode)
    {
        if (mqttParaInfo.ca_crt == NULL)
        {
            mqttParaInfo.ca_crt = icamqtt_ca_crt;
        }
    }
    else
    {
        mqttParaInfo.ca_crt = NULL;
    }
    ret = icamqtt_connect(atEngine, FALSE);
    if (ret == 0)
    {
        at_CmdRespOK(atEngine);
    }
    
    return;
}

static void icamqtt_disconnectNetwork(bool isReconn)
{
    KING_SysLog("icamqtt_DisconnectNetwork start");
    mqttInfo.connState = ICAMQTT_CONN_STATE_IDLE_TYPE;
    if (pIcaMqttClient != NULL)
    {
        mqtt_client_destroy(&pIcaMqttClient);
    }

    if (icamqttRunning)
    {
        if (NULL == icamqttTlsSem)
        {
            int ret = 0;
            
            ret = KING_SemCreate("ICAMQTT TLS SEM", 0, (SEM_HANDLE *)&icamqttTlsSem);
            KING_SysLog("%s: KING_SemCreate ret = %d\r\n", __FUNCTION__, ret);
            if (ret < 0 || icamqttTlsSem == NULL)
            {
                return;
            }
        }
        
        icamqttRunning = FALSE;
        KING_SemGet(icamqttTlsSem, WAIT_OPT_INFINITE); 
        if (icamqttTlsSem != NULL)
        {
            KING_SemDelete(icamqttTlsSem);
            icamqttTlsSem = NULL;
        }
    }
    
    memset(&(mqttSubMsgInfo), 0x00, sizeof(icaMqttSubMsgInfo));
    memset(&(mqttPubMsgInfo), 0x00, sizeof(icaMqttPubMsgInfo));
    memset(&(mqttRcvMsgInfo), 0x00, sizeof(icaMqttRecvMsgInfo));
    memset(&(mqttUnsubMsgInfo), 0x00, sizeof(icaMqttUnsubMsgInfo));
    memset(&(mqttPubTempInfo), 0x00, sizeof(icaMqttPubTempFragInfo));
    memset(&(mqttPubBinMsgInfo), 0x00, sizeof(icaMqttPubBinMsgInfo));
    memset(&(mqttRcvBinMsgInfo), 0x00, sizeof(icaMqttRecvMsgInfo));    
}

static void icamqtt_imqttDISCONNCmdExe(AT_CMD_ENGINE_T *atEngine)
{
    icamqtt_disconnectNetwork(FALSE);
    mqttInfo.atEngine = atEngine;
    at_CmdRespOK(atEngine);
    
    return;
}

static void icamqtt_messageArrive(void *pcontext, void *pclient, mqtt_event_msg_t *msg)
{
    KING_SysLog("icamqtt_messageArrive msg->event_type = %d.", msg->event_type);
    switch (msg->event_type) 
    {
        case MQTT_EVENT_PUBLISH_RECEIVED:
        {
            mqtt_client_topic_info_t *ptopic_info = (mqtt_client_topic_info_t *) msg->msg;
            
            if (ptopic_info->topic_len > ICAMQTT_PUB_MSG_BUF_SIZE)
            {
                break;
            }
            if (pubType == ICAMQTT_PUB_BIN_TYPE)
            {
                memset(&(mqttRcvBinMsgInfo), 0x00, sizeof(icaMqttRecvMsgInfo));
                mqttRcvBinMsgInfo.rcvDataLen = ptopic_info->payload_len;
                memcpy(mqttRcvBinMsgInfo.msg, ptopic_info->payload, ptopic_info->payload_len);
                memcpy(mqttRcvBinMsgInfo.topic, ptopic_info->ptopic, ptopic_info->topic_len);
                mqttRcvBinMsgInfo.packetId = ptopic_info->packet_id; 
                icamqtt_recvBinRsp();
            }
            else
            {
                memset(&(mqttRcvMsgInfo), 0x00, sizeof(icaMqttRecvMsgInfo));
                mqttRcvMsgInfo.rcvDataLen = ptopic_info->payload_len;
                memcpy(mqttRcvMsgInfo.msg, ptopic_info->payload, ptopic_info->payload_len);
                memcpy(mqttRcvMsgInfo.topic, ptopic_info->ptopic, ptopic_info->topic_len);
                mqttRcvMsgInfo.packetId = ptopic_info->packet_id; 
                icamqtt_recvRsp(mqttInfo.atEngine, FALSE);
            }
            break;
        }
        
        default:
            KING_SysLog("MQTT Should NOT arrive here.");
            break;
    }
}

static void icamqtt_imqttSUBCmdSet
(
    AT_CMD_ENGINE_T *atEngine,
    char *topic,
    uint8 qos
)
{
    int sub_ret = 0;
    char rspString[12] = {0};
    
    if (mqtt_client_is_connected(pIcaMqttClient) != 1)
    {
        KING_SysLog("icamqtt_imqttSUBCmdSet ,is not connect!");
        at_CmdRespCmeError(atEngine, ERR_AT_CME_EXE_NOT_SURPORT);
        return;
    }

    if (mqttSubMsgInfo.used == TRUE)
    {
        KING_SysLog("icamqtt_imqttSUBCmdSet ,is used!");
        at_CmdRespCmeError(atEngine, ERR_AT_CME_EXE_NOT_SURPORT);
        return;
    }
    
    sub_ret = mqtt_client_subscribe(pIcaMqttClient, topic, qos, icamqtt_messageArrive, NULL);
    if (sub_ret < 0)
    {
        KING_SysLog("icamqtt_imqttSUBCmdSet ,sub is fail!");
        at_CmdRespCmeError(atEngine, CME_ICA_MQTT_SUB_FAIL);
        return;
    }
    
    memset(&(mqttSubMsgInfo), 0x00, sizeof(icaMqttSubMsgInfo));
    mqttSubMsgInfo.qos = qos;
    memcpy(mqttSubMsgInfo.topic, topic, strlen(topic));
    mqttSubMsgInfo.packetId = sub_ret;
    mqttSubMsgInfo.used = TRUE;
    mqttInfo.atEngine = atEngine;

    memset(rspString, 0x00, 12);
    sprintf (rspString, "%u", mqttSubMsgInfo.packetId);
    at_CmdRespInfoText(atEngine, (uint8 *)rspString);
    at_CmdRespOK(atEngine);
}

static void icamqtt_imqttSUBCmdRead(AT_CMD_ENGINE_T *atEngine)
{
    if (strlen(mqttSubMsgInfo.topic) > 0)
    {
        char rspString[ICAMQTT_STR_RSP_LEN] = {0};
        
        memset(rspString, 0x00, ICAMQTT_STR_RSP_LEN);
        sprintf (rspString, "+IMQTTSUB: \"%s\",%d", 
                mqttSubMsgInfo.topic, mqttSubMsgInfo.qos);
        at_CmdRespInfoText(atEngine, (uint8 *)rspString);
        at_CmdRespOK(atEngine);
    }
    else
    {
        at_CmdRespOK(atEngine);
    }
    
    return;
}

static void icamqtt_imqttPUBCmdSet
(
    AT_CMD_ENGINE_T *atEngine,
    char *topic,
    uint8 qos,
    char *msg,
    uint16 msgLen,
    uint8 format,
    uint8 frag_id
)
{
    int pub_ret = 0;
    mqtt_client_topic_info_t topic_msg;
    char rspString[12] = {0};
    char tempMsg[ICAMQTT_PUB_MSG_BUF_SIZE + 1] = {0};
    char *actualMsg = NULL;
    UINT16 totalLen = 0;
    
    KING_SysLog("icamqtt_imqttPUBCmdSet() format = %d", format);
    if ((format == 1) && (msgLen % 2 != 0))
    {
        at_CmdRespCmeError(atEngine, ERR_AT_CME_PARAM_INVALID);
        return;
    }
    KING_SysLog("icamqtt_imqttPUBCmdSet() frag_id = %d", frag_id);
    if (frag_id >= ICAMQTT_FRAGMENT_ID_MAX)
    {
        KING_SysLog("icamqtt_imqttPUBCmdSet() frag_id = %d is error!", frag_id);
        at_CmdRespCmeError(atEngine, ERR_AT_CME_PARAM_INVALID);
        return;
    }
    
    if (mqtt_client_is_connected(pIcaMqttClient) != 1)
    {
        KING_SysLog("icamqtt_imqttPUBCmdSet ,is not connect!");
        at_CmdRespCmeError(atEngine, ERR_AT_CME_EXE_NOT_SURPORT);
        return;
    }
    
#if 0    
    KING_SysLog("icamqtt_imqttPUBCmdSet() used = %d", mqttPubMsgInfo.used);
    if (mqttPubMsgInfo.used == TRUE || mqttPubBinMsgInfo.used == TRUE)
    {
        KING_SysLog("icamqtt_imqttPUBCmdSet ,is used!");
        at_CmdRespCmeError(atEngine, ERR_AT_CME_EXE_NOT_SURPORT);
        return;
    }
#endif    
    KING_SysLog("icamqtt_imqttPUBCmdSet() msgLen = %d", msgLen);
    if (mqttPubTempInfo.isFirst == FALSE)
    {
        icamqtt_resetPubInfo();
        mqttPubTempInfo.fragNum = frag_id;
        mqttPubTempInfo.isFirst = TRUE;
        memset(mqttPubTempInfo.topic, 0x00, ICAMQTT_STR_LEN);
        memcpy(mqttPubTempInfo.topic, topic, strlen(topic));
        mqttPubTempInfo.qos = qos;
        mqttPubTempInfo.format = format;
    }
    
    KING_SysLog("icamqtt_imqttPUBCmdSet() frag_num = %d", mqttPubTempInfo.fragNum);
    if (mqttPubTempInfo.fragNum > 0)
    {
        uint8 frag_idx = mqttPubTempInfo.fragNum - frag_id;
        
        KING_SysLog("icamqtt_imqttPUBCmdSet() frag_idx = %d", frag_idx);
        if (((mqttPubTempInfo.fragMsgMaxLen + msgLen) > 1024) 
            || (frag_id > mqttPubTempInfo.fragNum)
            || ((mqttPubTempInfo.fragNum != frag_id) 
            && ((mqttPubFragInfo[frag_idx-1].isUsed == FALSE)
            || (mqttPubTempInfo.qos != qos) || (mqttPubTempInfo.format != format)
            || AT_StrCaselessCmp(mqttPubTempInfo.topic, topic) != 0)))
        {
            icamqtt_resetPubInfo();
            memset(&(mqttPubTempInfo), 0x00, sizeof(icaMqttPubTempFragInfo));
            KING_SysLog("icamqtt_imqttPUBCmdSet() input is invalid!");
            at_CmdRespCmeError(atEngine, ERR_AT_CME_PARAM_INVALID);
            return;
        }
        mqttPubFragInfo[frag_idx].fragId = frag_id;
        if (mqttPubFragInfo[frag_idx].fragMsg != NULL)
        {
            free(mqttPubFragInfo[frag_idx].fragMsg);
            mqttPubFragInfo[frag_idx].fragMsg = NULL;
        }
        mqttPubFragInfo[frag_idx].fragMsg = malloc(msgLen + 1);
        if (mqttPubFragInfo[frag_idx].fragMsg == NULL)
        {
            icamqtt_resetPubInfo();
            memset(&(mqttPubTempInfo), 0x00, sizeof(icaMqttPubTempFragInfo));
            KING_SysLog("icamqtt_imqttPUBCmdSet() malloc is fail!");
            at_CmdRespCmeError(atEngine, ERR_AT_CME_NO_MEMORY);
            return;
        }
        memset(mqttPubFragInfo[frag_idx].fragMsg, 0x00, msgLen + 1);
        memcpy(mqttPubFragInfo[frag_idx].fragMsg, msg, msgLen);
        if (mqttPubFragInfo[frag_idx].isUsed)
        {
            mqttPubTempInfo.fragMsgMaxLen -= mqttPubFragInfo[frag_idx].len;
        }

        mqttPubFragInfo[frag_idx].isUsed = TRUE;
        mqttPubFragInfo[frag_idx].len = msgLen;
        mqttPubTempInfo.fragMsgMaxLen += msgLen;
        if (frag_id > 0)
        {
            at_CmdRespOK(atEngine);
            return;
        }
        if (icamqtt_catPubInfo(tempMsg) == -1)
        {
            icamqtt_resetPubInfo();
            memset(&(mqttPubTempInfo), 0x00, sizeof(icaMqttPubTempFragInfo));
            KING_SysLog("icamqtt_imqttPUBCmdSet() cat pub info fail!");
            at_CmdRespCmeError(atEngine, ERR_AT_UNKNOWN);
            return;
        }
    }
    else
    {
        memcpy(tempMsg, msg, msgLen);
        mqttPubTempInfo.fragMsgMaxLen = msgLen;
    }

    memset(&topic_msg, 0x00, sizeof(mqtt_client_topic_info_t));
    topic_msg.qos = qos;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    if (format == 1)
    {
        totalLen = mqttPubTempInfo.fragMsgMaxLen/2;
    }
    else
    {
        totalLen = mqttPubTempInfo.fragMsgMaxLen;
    }
    actualMsg = malloc(totalLen + 1);
    if (actualMsg == NULL)
    {
        icamqtt_resetPubInfo();
        memset(&(mqttPubTempInfo), 0x00, sizeof(icaMqttPubTempFragInfo));
        KING_SysLog("icamqtt_imqttPUBCmdSet() actualMsg malloc is fail!");
        at_CmdRespCmeError(atEngine, ERR_AT_CME_NO_MEMORY);
        return;
    }
    memset(actualMsg, 0x00, totalLen + 1);
    if (format == 1)
    {
        atUtils_ConvertHexToBin((char *)tempMsg, 
                              (uint16)mqttPubTempInfo.fragMsgMaxLen, 
                              (char *)actualMsg);
    }
    else
    {
        memcpy(actualMsg, tempMsg, mqttPubTempInfo.fragMsgMaxLen);
    }
    topic_msg.payload = (void *)actualMsg;
    topic_msg.payload_len = totalLen;
    
    pub_ret = mqtt_client_publish(pIcaMqttClient, topic, &topic_msg);
    if (pub_ret < 0)
    {
        if (actualMsg != NULL)
        {
            free(actualMsg);
        }
        memset(&(mqttPubTempInfo), 0x00, sizeof(icaMqttPubTempFragInfo));
        icamqtt_resetPubInfo();
        KING_SysLog("icamqtt_imqttPUBCmdSet() pub is fail!");
        at_CmdRespCmeError(atEngine, CME_ICA_MQTT_PUB_FAIL);
        return;
    }
    
    if (actualMsg != NULL)
    {
        free(actualMsg);
    }
    pubType = ICAMQTT_PUB_TYPE;
    memset(&(mqttPubMsgInfo), 0x00, sizeof(icaMqttPubMsgInfo));
    icamqtt_getPubfragInfo();
    mqttPubMsgInfo.fragNum = mqttPubTempInfo.fragNum;
    mqttPubMsgInfo.qos = qos;
    mqttPubMsgInfo.format = format;
    memset(mqttPubMsgInfo.topic, 0x00, ICAMQTT_STR_LEN);
    memcpy(mqttPubMsgInfo.topic, topic, strlen(topic));
    memset(mqttPubMsgInfo.msg, 0x00, ICAMQTT_PUB_MSG_BUF_SIZE + 1);
    memcpy(mqttPubMsgInfo.msg, tempMsg, strlen(tempMsg));
    mqttPubMsgInfo.packetId = pub_ret;
    //mqttPubMsgInfo.used = TRUE;
    memset(&(mqttPubTempInfo), 0x00, sizeof(icaMqttPubTempFragInfo));
    icamqtt_resetPubInfo();
    mqttInfo.atEngine = atEngine;

    memset(rspString, 0x00, 12);
    sprintf (rspString, "%u", mqttPubMsgInfo.packetId);
    at_CmdRespInfoText(atEngine, (uint8 *)rspString);
    at_CmdRespOK(atEngine);
    if (qos == 0 || qos == 2)
    {
        icamqtt_urcRsp("+IMQTTPUB", mqttPubMsgInfo.packetId, 0);
        //mqttPubMsgInfo.used = FALSE;
    }
    return;
}

static void icamqtt_imqttPUBCmdRead(AT_CMD_ENGINE_T *atEngine)
{
    char tempMsg[ICAMQTT_PUB_MSG_BUF_SIZE + 1] = {0};

    if (mqttPubMsgInfo.fragNum > 0)
    {
        uint8 i = 0;
        uint16 tempLen = 0;
        
        for (i = 0; i <= mqttPubMsgInfo.fragNum; i++)
        {
            memset(g_rspStr, 0x00, sizeof((char *)g_rspStr));
            memset(tempMsg, 0x00, ICAMQTT_PUB_MSG_BUF_SIZE + 1);
            memcpy(tempMsg, mqttPubMsgInfo.msg + tempLen, mqttPubMsgInfo.fragInfo[i].fragLen);
            sprintf((char *)g_rspStr, "\r\n+IMQTTPUB: \"%s\",%d,\"%s\",%d,%d", 
                    mqttPubMsgInfo.topic, mqttPubMsgInfo.qos, 
                    tempMsg, 
                    mqttPubMsgInfo.format, 
                    mqttPubMsgInfo.fragInfo[i].fragId);
            at_CmdRespOutputText( atEngine, g_rspStr);
            tempLen += mqttPubMsgInfo.fragInfo[i].fragLen;
        }
        
        at_CmdRespOutputText( atEngine, (uint8 *)"\r\n");
    }
    else
    {
        memset(g_rspStr, 0x00, sizeof((char *)g_rspStr));
        sprintf ((char *)g_rspStr, "\r\n+IMQTTPUB: \"%s\",%d,\"%s\",%d,0\r\n", 
                mqttPubMsgInfo.topic, mqttPubMsgInfo.qos, 
                mqttPubMsgInfo.msg, mqttPubMsgInfo.format);
        at_CmdRespOutputText( atEngine, g_rspStr);
    }
    
    at_CmdRespOK(atEngine);
    return;
}

static void icamqtt_imqttUNSUBCmdSet(AT_CMD_ENGINE_T *atEngine,  char *topic)
{
    int unsub_ret = 0;
    char rspString[12] = {0};
    
    if (mqtt_client_is_connected(pIcaMqttClient) != 1)
    {
        KING_SysLog("icamqtt_imqttUNSUBCmdSet ,is not connect!");
        at_CmdRespCmeError(atEngine, ERR_AT_CME_EXE_NOT_SURPORT);
        return;
    }

    if (mqttUnsubMsgInfo.used == TRUE)
    {
        KING_SysLog("icamqtt_imqttUNSUBCmdSet ,is used!");
        at_CmdRespCmeError(atEngine, ERR_AT_CME_EXE_NOT_SURPORT);
        return;
    }
    
    unsub_ret = mqtt_client_unsubscribe(pIcaMqttClient, topic);
    if (unsub_ret < 0)
    {
        KING_SysLog("icamqtt_imqttUNSUBCmdSet() pub is fail!");
        at_CmdRespCmeError(atEngine, CME_ICA_MQTT_UNSUB_FAIL);
        return;
    }
    
    memset(&(mqttUnsubMsgInfo), 0x00, sizeof(icaMqttUnsubMsgInfo));
    memcpy(mqttUnsubMsgInfo.topic, topic, strlen(topic));
    mqttUnsubMsgInfo.packetId = unsub_ret;
    mqttUnsubMsgInfo.used = TRUE;
    mqttInfo.atEngine = atEngine;

    memset(rspString, 0x00, 12);
    sprintf (rspString, "%u", mqttUnsubMsgInfo.packetId);
    at_CmdRespInfoText(atEngine, (uint8 *)rspString);
    at_CmdRespOK(atEngine);
    return;
}

static void icamqtt_imqttUNSUBCmdRead(AT_CMD_ENGINE_T *atEngine)
{
    if (strlen(mqttUnsubMsgInfo.topic) > 0)
    {
        char rspString[ICAMQTT_STR_RSP_LEN] = {0};
        
        memset(rspString, 0x00, ICAMQTT_STR_RSP_LEN);
        sprintf (rspString, "+IMQTTUNSUB: \"%s\"", mqttUnsubMsgInfo.topic);
        at_CmdRespInfoText(atEngine, (uint8 *)rspString);
        at_CmdRespOK(atEngine);
    }
    else
    {
        at_CmdRespOK(atEngine);
    }
    return;
}

static void icamqtt_imqttSTATECmdRead(AT_CMD_ENGINE_T *atEngine)
{
    int state = 0;
    char rspString[25] = {0};
    
    if (mqttParaInfo.client_id == NULL)
    {
        state = 0;
    }
    else
    {
        state = mqtt_client_is_connected(pIcaMqttClient);
        if (state < 0)
        {
           state = 0; 
        }
    }
        
    memset(rspString, 0x00, 25);
    sprintf (rspString, "+IMQTTSTATE: %d", state);
    
    at_CmdRespInfoText(atEngine, (uint8 *)rspString);
    at_CmdRespOK(atEngine);
    
    return;
}

static void icamqtt_networkDownUrcReport(void)
{
    if (mqttInfo.connState == ICAMQTT_CONN_STATE_CONNECTED_TYPE)
    {
        if (hIcaMqttContext != -1)
        {
            KING_ConnectionRelease(hIcaMqttContext);
            hIcaMqttContext = -1;
        }
        icamqtt_urcStrRsp("+IMQTTDISCONN");
        mqttInfo.connState = ICAMQTT_CONN_STATE_IDLE_TYPE;
        mqttInfo.reconnTryCnt = 0;
        mqttInfo.isReconn = TRUE;
        mqtt_client_DisconnectByNetwork(pIcaMqttClient);
    }
}

static void icamqtt_netCallBack(uint32 eventID, void* pdata, uint32 len)
{
    //KH_CB_DATA_U *data = (KH_CB_DATA_U *)pdata;

    KING_SysLog("icamqtt_netCallBack() eventID=0x%x, len=%d, pdata=%p\r\n", eventID, len, pdata);
    switch(eventID)
    {
    case NW_EVENT_GSM_REGISTERED_IND:
        KING_SysLog("[mqtt]NET registered\r\n");
        break;

    case NW_EVENT_GSM_UNREGISTERED_IND:
        KING_SysLog("[mqtt]NET unregistered\r\n");
        break;

    case NW_EVENT_GPRS_ATTACH_IND:
        KING_SysLog("[mqtt]GPRS attach\r\n");
        break;

    case NW_EVENT_GPRS_DEATTACH_IND:
        KING_SysLog("[mqtt]GPRS deattached\r\n");
        break;

    case NW_EVENT_PS_ONOFF_RSP:
        KING_SysLog("[mqtt]GPRS attach rsp\r\n");
        break;

    case NW_EVENT_ATTACH_RSP:
        KING_SysLog("[mqtt]GPRS attach rsp\r\n");
        break;

    case NW_EVENT_PDP_ACT_RSP:
        KING_SysLog("[mqtt]pdp act rsp\r\n");
        break;

    case NW_EVENT_PDP_DEACT_IND:
    {
        KING_SysLog("[mqtt]PDP deactive ind\r\n");
        icamqtt_networkDownUrcReport();
    }
        break;

    default:
        break;
    }
}

static int icamqtt_sendPubBinData(uint8 *pData, uint16 dataLen)
{
    int pub_ret = 0;
    mqtt_client_topic_info_t topic_msg;

    pubType = ICAMQTT_PUB_BIN_TYPE;
    memset((void *)mqttPubBinMsgInfo.msg, 0x00, ICAMQTT_PUB_MSG_BUF_SIZE + 1);
    memcpy((void *)mqttPubBinMsgInfo.msg, pData, dataLen);
    mqttPubBinMsgInfo.len = dataLen;
    
    KING_SysLog("icamqtt_sendByPassData: len: %d", mqttPubBinMsgInfo.len);
    memset(&topic_msg, 0x00, sizeof(mqtt_client_topic_info_t));
    topic_msg.qos = mqttPubBinMsgInfo.qos;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload_len = mqttPubBinMsgInfo.len;
    topic_msg.payload = (void *)mqttPubBinMsgInfo.msg;
    pub_ret = mqtt_client_publish(pIcaMqttClient, mqttPubBinMsgInfo.topic, &topic_msg);
    if (pub_ret < 0)
    {
        at_CmdRespCmeError(mqttInfo.atEngine, ERR_AT_CME_EXE_FAIL);
    }
    else
    {
        //mqttPubBinMsgInfo.used = TRUE;
        mqttPubBinMsgInfo.packetId = pub_ret;
        memset(g_rspStr, 0x00, sizeof((char *)g_rspStr));
        sprintf((char *)g_rspStr, "%u", mqttPubBinMsgInfo.packetId);
        at_CmdRespInfoText(mqttInfo.atEngine, g_rspStr);
        at_CmdRespOK(mqttInfo.atEngine);
        if (mqttPubBinMsgInfo.qos == 0 || mqttPubBinMsgInfo.qos == 2)
        {
            icamqtt_urcRsp("+IMQTTPUBIN", mqttPubBinMsgInfo.packetId, 0);
            //mqttPubBinMsgInfo.used = FALSE;
        }
    }
    return 0;
}

static void icamqtt_getDataFromUart(const uint8_t *data, unsigned length, uint8 flag)
{ 
    if (length == 0)
    {
        return;
    }
    if(flag == 1)
    {
        at_CmdSetLineMode(mqttInfo.atEngine);
        icamqtt_sendPubBinData(icamqttBypassData, icamqttBypassLen);
        if (icamqttBypassData != NULL)
        {
            free(icamqttBypassData);
            icamqttBypassData = NULL;
        }
        icamqttBypassLen = 0;
    }
    else
    {
        if(icamqttBypassLen + length > 1024)
        {
            KING_SysLog("icamqttBypassLen beyond max len");
            icamqttBypassLen = 0;
            if (icamqttBypassData != NULL)
            {
                free(icamqttBypassData);
                icamqttBypassData = NULL;
            }
            at_CmdRespCmeError(mqttInfo.atEngine, ERR_AT_CME_PARAM_INVALID);
            return;
        }
        
        at_CmdRespOutputNText(mqttInfo.atEngine, data, length);
        
        if(icamqttBypassData == NULL)
        {
            icamqttBypassData = (uint8*)malloc(ICAMQTT_PUB_MSG_BUF_SIZE + 1);
            if(icamqttBypassData == NULL)
            {
                KING_SysLog("icamqttBypassData is NULL");
                at_CmdRespCmeError(mqttInfo.atEngine, ERR_AT_CME_PARAM_INVALID);
                icamqttBypassLen = 0;
                return;
            }
            memset(icamqttBypassData, 0x00, ICAMQTT_PUB_MSG_BUF_SIZE + 1);
        }

        memcpy(icamqttBypassData + icamqttBypassLen, data, length);
        icamqttBypassLen += length;
    }
}

static int icamqtt_BypassDataRecv(void *param, const uint8_t *data, unsigned length)
{
    int ret = -1;
    uint32 curTick, rxInterval;
    static uint32 lastRxTick;
    uint8 flag = 0;
    
    KING_CurrentTickGet(&curTick);
    rxInterval = (curTick > lastRxTick ? curTick - lastRxTick : 0xffffffff - lastRxTick + curTick);
    lastRxTick = curTick;
    if(rxInterval >= 1000)
    {
        if((3 == length) && (0 == strncmp((char*)data, "+++", 3)))
        {
            flag = 1;
        }
    }

    if (icamqttMutex == NULL)
    {
        ret = KING_MutexCreate("icamqttMutex", 0, &icamqttMutex);
        if (ret != 0 )
        {
            KING_SysLog("KING_MutexCreate fail");
            return -1;
        }
    }

    KING_MutexLock(icamqttMutex, WAIT_OPT_INFINITE);
    icamqtt_getDataFromUart(data, length, flag);
    KING_MutexUnLock(icamqttMutex);

    return length;    
}

/*******************************************************************************
 ** Function 
 ******************************************************************************/
void icamqtt_Init(void)
{
    icamqtt_setDefaultPara();
    icamqtt_resetPubInfo();
    memset(&(mqttSubMsgInfo), 0x00, sizeof(icaMqttSubMsgInfo));
    memset(&(mqttPubMsgInfo), 0x00, sizeof(icaMqttPubMsgInfo));
    memset(&(mqttRcvMsgInfo), 0x00, sizeof(icaMqttRecvMsgInfo));
    memset(&(mqttUnsubMsgInfo), 0x00, sizeof(icaMqttUnsubMsgInfo));
    memset(&(mqttPubTempInfo), 0x00, sizeof(icaMqttPubTempFragInfo));
    memset(&(mqttPubBinMsgInfo), 0x00, sizeof(icaMqttPubBinMsgInfo));
    memset(&(mqttRcvBinMsgInfo), 0x00, sizeof(icaMqttRecvMsgInfo));

    mqttInfo.connState = ICAMQTT_CONN_STATE_IDLE_TYPE;
    mqttInfo.isMqttAuth = FALSE;
    mqttInfo.connParaFlag.keepaliveFlag = FALSE;
    mqttInfo.connParaFlag.timeoutFlag = FALSE;
    mqttInfo.connParaFlag.cleanFlag = FALSE;
    mqttInfo.connParaFlag.versionFlag = FALSE;
    mqttInfo.connParaFlag.authModeFlag = FALSE;
    mqttInfo.isReconn = FALSE;
    mqttInfo.reconnTryCnt = 0;
    mqttInfo.mode = 0;
    mqttInfo.rcvFormat = 0;
    mqttInfo.rcvFragLen = 1024;
    
    KING_RegNotifyFun(DEV_CLASS_NW, 0, icamqtt_netCallBack);
}

void AT_CmdFunc_IMQTTMODE(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[ICAMQTT_STR_RSP_LEN] = {0};

            memset(strRsp, 0x00, ICAMQTT_STR_RSP_LEN);
            sprintf(strRsp, "+IMQTTMODE: \"mode\"");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;

        case AT_CMD_SET:
        {
            bool paramok = true;
            uint8 mode = 0;

            if (pParam->paramCount != 1)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            }
            
            mode = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            
            icamqtt_modeSet(mode);
            at_CmdRespOK(pParam->engine);
            break;
        }

        case AT_CMD_READ:
        {
            char rspString[20] = {0};
        
            memset(rspString, 0x00, 20);
            sprintf (rspString, "+IMQTTMODE: %d", icamqtt_modeGet());
            at_CmdRespInfoText(pParam->engine, (uint8 *)rspString);
            at_CmdRespOK(pParam->engine);
            break;
        }

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    } 

    return;
}

void AT_CmdFunc_IMQTTOPEN(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[ICAMQTT_STR_RSP_LEN] = {0};

            memset(strRsp, 0x00, ICAMQTT_STR_RSP_LEN);
            sprintf(strRsp, "+IMQTTOPEN: \"hostname\",\"port\"");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;

        case AT_CMD_SET:
        {
            bool paramok = true;
            uint16 port = 0;
            const uint8 *hostName = NULL;

            if (pParam->paramCount != 2)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            }
            
            hostName = at_ParamOptStr(pParam->params[0], &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            if (strlen((char *)hostName) == 0 ||  strlen((char *)hostName) > DEV_SIGN_HOSTNAME_MAXLEN)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            
            port = at_ParamUintInRange(pParam->params[1], 1, 65535, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            
            icamqtt_imqttOPENCmdSet(pParam->engine, (char *)hostName, port);
            break;
        }

        case AT_CMD_READ:
            icamqtt_imqttOPENCmdRead(pParam->engine);
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    } 

    return;
}

void AT_CmdFunc_IMQTTAUTH(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[ICAMQTT_STR_RSP_LEN] = {0};

            memset(strRsp, 0x00, ICAMQTT_STR_RSP_LEN);
            sprintf(strRsp, "+IMQTTAUTH: (\"ProductKey\"),(\"DeviceName\"),(\"DeviceSecret\")");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;

        case AT_CMD_SET:
        {
            bool paramok = true;
            const uint8 *productKey = NULL;
            const uint8 *deviceName = NULL;
            const uint8 *deviceSecret = NULL;

            if (pParam->paramCount != 3)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            }
            
            productKey = at_ParamOptStr(pParam->params[0], &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            if (strlen((char *)productKey) == 0 ||  strlen((char *)productKey) > IOTX_PRODUCT_KEY_LEN)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            
            deviceName = at_ParamOptStr(pParam->params[1], &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            if (strlen((char *)deviceName) == 0 ||  strlen((char *)deviceName) > IOTX_DEVICE_NAME_LEN)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            
            deviceSecret = at_ParamOptStr(pParam->params[2], &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            if (strlen((char *)deviceSecret) == 0 ||  strlen((char *)deviceSecret) > IOTX_DEVICE_SECRET_LEN)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }

            icamqtt_imqttAUTHCmdSet(pParam->engine, (char *)productKey, 
                                    (char *)deviceName, (char *)deviceSecret);
            break;
        }

        case AT_CMD_READ:
            icamqtt_icamqttAUTHCmdRead(pParam->engine);
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    } 

    return;
}

void AT_CmdFunc_IMQTTPARA(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[ICAMQTT_STR_RSP_LEN] = {0};

            memset(strRsp, 0x00, ICAMQTT_STR_RSP_LEN);
            sprintf(strRsp, "+IMQTTPARA: \"ParaTag\",\"ParaValue\"");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;

        case AT_CMD_SET:
        {
            bool paramok = true;
            int param = 0;
            uint16 minVal = 0, maxVal = 0;
            icaMqttParaType paraType = ICAMQTT_NONE_TYPE;
            const uint8_t *argStr = NULL;
            const uint8_t *arg1Str = NULL;

            if (pParam->paramCount != 2)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            }
            
            argStr = at_ParamOptStr(pParam->params[0], &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            if (strlen((char *)argStr) == 0 ||  strlen((char *)argStr) > 20)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            paraType = icamqtt_imqttParaInputType((char *)argStr);
            if (paraType == ICAMQTT_NONE_TYPE)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            KING_SysLog("AT_CmdFunc_IMQTTPARA paraType %d!!", paraType);
            if (paraType == ICAMQTT_TIMEOUT_TYPE ||
                paraType == ICAMQTT_CLEAN_TYPE ||
                paraType == ICAMQTT_KEEPALIVE_TYPE ||
                paraType == ICAMQTT_AUTHMODE_TYPE)
            {
                if (paraType == ICAMQTT_TIMEOUT_TYPE)
                {
                    minVal = 0;
                    maxVal = 65535;
                }
                else if (paraType == ICAMQTT_CLEAN_TYPE)
                {
                    minVal = 0;
                    maxVal = 1;
                }
                else if (paraType == ICAMQTT_KEEPALIVE_TYPE)
                {
                    minVal = 0;
                    maxVal = 65535;
                }
                else if (paraType == ICAMQTT_AUTHMODE_TYPE)
                {
                    minVal = 0;
                    maxVal = 1;
                }
                
                param = at_ParamUintInRange(pParam->params[1], minVal, maxVal, &paramok);
                KING_SysLog("AT_CmdFunc_IMQTTPARA param %d!!", param);
                if (!paramok)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    return;
                }
                if (paraType == ICAMQTT_KEEPALIVE_TYPE)
                {
                    if (param < 60 && param >= 0)
                    {
                        param = 60;
                    }
                    else if(param > 180 && param < 65535)
                    {
                        param = 180;
                    }
                }
            }
            else if (paraType == ICAMQTT_VERSION_TYPE)
            {
                arg1Str = at_ParamOptStr(pParam->params[1], &paramok);
                if (!paramok || strlen((char *)arg1Str) <= 0 || strlen((char *)arg1Str) > 64)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    return;
                }

                if (AT_StrCaselessCmp(arg1Str, "3.1.1") != 0
                    && (AT_StrCaselessCmp(arg1Str, "3.1") != 0))
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    break;
                }
            }
            else if (paraType == ICAMQTT_USER_TYPE)
            {
                arg1Str = at_ParamOptStr(pParam->params[1], &paramok);
                if (!paramok || strlen((char *)arg1Str) > DEV_SIGN_USERNAME_MAXLEN)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    return;
                }
            }
            else if (paraType == ICAMQTT_PASSWD_TYPE)
            {
                arg1Str = at_ParamOptStr(pParam->params[1], &paramok);
                if (!paramok || strlen((char *)arg1Str) > DEV_SIGN_PASSWORD_MAXLEN)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    return;
                }
            }
            else if (paraType == ICAMQTT_CLIENTID_TYPE)
            {
                arg1Str = at_ParamOptStr(pParam->params[1], &paramok);
                if (!paramok || strlen((char *)arg1Str) > DEV_SIGN_CLIENT_ID_MAXLEN)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    return;
                }
            }
            else
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            }
            icamqtt_imqttPARASetAndSave(pParam->engine, paraType, param, (char *)arg1Str);
            break;
        }

        case AT_CMD_READ:
            icamqtt_imqttPARACmdRead(pParam->engine);
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    } 

    return;
}

void AT_CmdFunc_IMQTTCONN(AT_CMD_PARA *pParam)
{
    if (pParam == NULL)
    {
        return;
    }
    
    mqttInfo.atEngine = pParam->engine;
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            at_CmdRespOK(pParam->engine);
            break;

        case AT_CMD_EXE:
            icamqtt_imqttCONNECTCmdExe(pParam->engine);
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }

    return;
}

void AT_CmdFunc_IMQTTDISCONN(AT_CMD_PARA *pParam)
{
    if (pParam == NULL)
    {
        return;
    }
    
    mqttInfo.atEngine = pParam->engine;
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            at_CmdRespOK(pParam->engine);
            break;

        case AT_CMD_EXE:
            icamqtt_imqttDISCONNCmdExe(pParam->engine);
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }

    return;
}

void AT_CmdFunc_IMQTTSUB(AT_CMD_PARA *pParam)
{
    if (pParam == NULL)
    {
        return;
    }
    
    mqttInfo.atEngine = pParam->engine;
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[ICAMQTT_STR_RSP_LEN] = {0};
        
            memset(strRsp, 0x00, ICAMQTT_STR_RSP_LEN);
            sprintf(strRsp, "+IMQTTSUB: \"topic\",\"qos\"");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;
            
        case AT_CMD_SET:
        {
            bool paramok = true;
            const uint8_t *topic = NULL;
            uint8 qos = 0;

            if (pParam->paramCount != 2)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            }
            
            topic = at_ParamOptStr(pParam->params[0], &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            if (strlen((char *)topic) <= 0 ||  strlen((char *)topic) >= ICAMQTT_STR_LEN)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            
            qos = at_ParamUintInRange(pParam->params[1], 0, 2, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            icamqtt_imqttSUBCmdSet(pParam->engine, (char *)topic, qos);                
        }
            break;
            
        case AT_CMD_READ:
            icamqtt_imqttSUBCmdRead(pParam->engine);
            break;
            
        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }
    
    return;
}

void AT_CmdFunc_IMQTTPUB(AT_CMD_PARA *pParam)
{
    if (pParam == NULL)
    {
        return;
    }
    
    mqttInfo.atEngine = pParam->engine;
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[ICAMQTT_STR_RSP_LEN] = {0};
        
            memset(strRsp, 0x00, ICAMQTT_STR_RSP_LEN);
            sprintf(strRsp, "+IMQTTPUB: \"topic\",\"qos\",\"message\",\"format\",\"fragment_id\"");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;
            
        case AT_CMD_SET:
        {
            bool paramok = true;
            const uint8_t *topic = NULL;
            const uint8_t *msg = NULL;
            uint8 qos = 0, format = 0, fragId = 0;
            uint16 msgLen = 0;

            if (pParam->paramCount < 3 || pParam->paramCount > 5)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            }
            
            topic = at_ParamOptStr(pParam->params[0], &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            if (strlen((char *)topic) <= 0 ||  strlen((char *)topic) >= ICAMQTT_STR_LEN)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            
            qos = at_ParamUintInRange(pParam->params[1], 0, 2, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            msg = at_ParamOptStr(pParam->params[2], &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            if (strlen((char *)msg) <= 0 ||  strlen((char *)msg) > ICAMQTT_PUB_MSG_BUF_SIZE)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            msgLen = pParam->params[2]->length;
            
            if (pParam->paramCount > 3)
            {
                format = at_ParamUintInRange(pParam->params[3], 0, 1, &paramok);
                if (!paramok)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    return;
                }
            }
            if (pParam->paramCount > 4)
            {
                fragId = at_ParamUintInRange(pParam->params[4], 0, 10, &paramok);
                if (!paramok)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    return;
                }
            }
            
            icamqtt_imqttPUBCmdSet(pParam->engine, (char *)topic, qos,
                                (char *)msg, msgLen, format, 
                                fragId);                
        }
            break;
            
        case AT_CMD_READ:
            icamqtt_imqttPUBCmdRead(pParam->engine);
            break;
            
        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }
    
    return;
}

void AT_CmdFunc_IMQTTUNSUB(AT_CMD_PARA *pParam)
{
    if (pParam == NULL)
    {
        return;
    }
    
    mqttInfo.atEngine = pParam->engine;
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[ICAMQTT_STR_RSP_LEN] = {0};
        
            memset(strRsp, 0x00, ICAMQTT_STR_RSP_LEN);
            sprintf(strRsp, "+IMQTTUNSUB: \"topic\"");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;
            
        case AT_CMD_SET:
        {
            bool paramok = true;
            const uint8_t *topic = NULL;

            if (pParam->paramCount != 1)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            }
            
            topic = at_ParamOptStr(pParam->params[0], &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            if (strlen((char *)topic) <= 0 ||  strlen((char *)topic) >= ICAMQTT_STR_LEN)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            
            icamqtt_imqttUNSUBCmdSet(pParam->engine, (char *)topic);                
        }
            break;
            
        case AT_CMD_READ:
            icamqtt_imqttUNSUBCmdRead(pParam->engine);
            break;
            
        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }
    
    return;
}

void AT_CmdFunc_IMQTTSTATE(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            at_CmdRespOK(pParam->engine);
            break;
            
        case AT_CMD_READ:
            icamqtt_imqttSTATECmdRead(pParam->engine);
            break;
            
        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }
    
    return;
}

void AT_CmdFunc_IMQTTRCVPUB(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            at_CmdRespOK(pParam->engine);
            break;
            
        case AT_CMD_READ:
            icamqtt_recvRsp(pParam->engine, TRUE);
            break;
            
        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }
    
    return;
}

void AT_CmdFunc_IMQTTRCVCFG(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[ICAMQTT_STR_RSP_LEN] = {0};
        
            memset(strRsp, 0x00, ICAMQTT_STR_RSP_LEN);
            sprintf(strRsp, "+IMQTTRCVCFG: \"format\",\"fragment_len\"");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;
            
        case AT_CMD_READ:
        {
            char strRsp[ICAMQTT_STR_RSP_LEN] = {0};
        
            memset(strRsp, 0x00, ICAMQTT_STR_RSP_LEN);
            sprintf(strRsp, "+IMQTTRCVCFG: %d,%d", mqttInfo.rcvFormat, mqttInfo.rcvFragLen);
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;

        case AT_CMD_SET:
        {
            bool paramok = true;
            uint16 format = 0, fragLen = 0;
            
            if (pParam->paramCount != 2)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            }
            
            format = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            
            fragLen = at_ParamUintInRange(pParam->params[1], 1, 1024, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            mqttInfo.rcvFormat = (uint8)format;
            mqttInfo.rcvFragLen = fragLen;

            at_CmdRespOK(pParam->engine);
        }
            break;
            
        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }
    
    return;
}

void AT_CmdFunc_IMQTTPUBIN(AT_CMD_PARA *pParam)
{
    if (pParam == NULL)
    {
        return;
    }
    
    mqttInfo.atEngine = pParam->engine;
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
        {
            char strRsp[ICAMQTT_STR_RSP_LEN] = {0};
        
            memset(strRsp, 0x00, ICAMQTT_STR_RSP_LEN);
            sprintf(strRsp, "+IMQTTPUBIN: \"topic\",\"qos\",\"msg_len\"");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
            memset(strRsp, 0x00, ICAMQTT_STR_RSP_LEN);
            sprintf(strRsp, "\r\n\"bin_message\"");
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
        }
            break;  
            
        case AT_CMD_READ:
        {
            memset(g_rspStr, 0x00, sizeof((char *)g_rspStr));
            sprintf((char *)g_rspStr, "+IMQTTPUBIN: \"%s\",%d,%d",
                    mqttPubBinMsgInfo.topic, mqttPubBinMsgInfo.qos, 
                    mqttPubBinMsgInfo.len);
            at_CmdRespInfoText(pParam->engine, g_rspStr);
            at_CmdRespOK(pParam->engine);
            if (mqttPubBinMsgInfo.len > 0)
            {
                memset(g_rspStr, 0x00, sizeof((char *)g_rspStr));
                g_rspStr[0] = '\"';
                memcpy(g_rspStr + 1, mqttRcvBinMsgInfo.msg, mqttPubBinMsgInfo.len);
                g_rspStr[mqttPubBinMsgInfo.len + 1] = '\"';
                at_CmdRespInfoNText(pParam->engine, g_rspStr, mqttPubBinMsgInfo.len + 2);
            }
        }
            break;

        case AT_CMD_SET:
        {
            bool paramok = true;
            int msgLen = 0, qos = 0;
            const uint8_t *topic = NULL;
            
            if (pParam->paramCount != 3)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            }
            if (mqtt_client_is_connected(pIcaMqttClient) != 1)
            {
                KING_SysLog("imqttPUBIN ,is not connect!");
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
                return;
            }
            #if 0
            if (mqttPubMsgInfo.used == TRUE || mqttPubBinMsgInfo.used == TRUE)
            {
                KING_SysLog("imqttPUBIN ,is used!");
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
                return;
            }
            #endif
            topic = at_ParamOptStr(pParam->params[0], &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            if (strlen((char *)topic) <= 0 ||  strlen((char *)topic) >= ICAMQTT_STR_LEN)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            memset(mqttPubBinMsgInfo.topic, 0x00, ICAMQTT_STR_LEN);
            memcpy(mqttPubBinMsgInfo.topic, topic, strlen((char *)topic));

            qos = at_ParamDefUintInRange(pParam->params[1], 1, 0, 2, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            mqttPubBinMsgInfo.qos = qos;

            msgLen = at_ParamUintInRange(pParam->params[2], 0, 1024, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }

            if (!pParam->iExDataLen)
            {
                if(msgLen == 0)
                {
                    at_CmdRespOutputPrompt(pParam->engine);
                    at_CmdSetBypassMode(pParam->engine, icamqtt_BypassDataRecv, NULL);
                }
                else
                {
                    at_CmdRespOutputText(pParam->engine, (uint8 *)"\r\nOK\r\n");
                    at_CmdSetRawMode(pParam->engine, msgLen);
                }
            }
            else
            {
                if (msgLen != pParam->iExDataLen) 
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CMS_INVALID_LEN);
                    break;
                }
                else
                {
                    icamqtt_sendPubBinData(pParam->pExData, (uint16)msgLen);
                    break;
                }
            }
        }
            break;
            
        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }
    
    return;            
}

void AT_CmdFunc_IMQTTRCVPUBIN(AT_CMD_PARA *pParam)
{
    if (pParam == NULL)
    {
        return;
    }
    
    mqttInfo.atEngine = pParam->engine;
    switch (pParam->iType)
    {
        case AT_CMD_READ:
        {
            memset(g_rspStr, 0x00, sizeof((char *)g_rspStr));
            sprintf((char *)g_rspStr, "+IMQTTRCVPUBIN: %u,\"%s\",%d", mqttRcvBinMsgInfo.packetId, 
                    mqttRcvBinMsgInfo.topic, mqttRcvBinMsgInfo.rcvDataLen);
            at_CmdRespInfoText(pParam->engine, g_rspStr);
            at_CmdRespOK(pParam->engine);
            if (mqttRcvBinMsgInfo.rcvDataLen > 0)
            {
                memset(g_rspStr, 0x00, sizeof((char *)g_rspStr));
                g_rspStr[0] = '\"';
                memcpy(g_rspStr + 1, mqttRcvBinMsgInfo.msg, mqttRcvBinMsgInfo.rcvDataLen);
                g_rspStr[mqttRcvBinMsgInfo.rcvDataLen + 1] ='\"';
                at_CmdRespInfoNText(pParam->engine, g_rspStr, mqttRcvBinMsgInfo.rcvDataLen + 2);
            }
        }
            break;  
            
        case AT_CMD_TEST:
            at_CmdRespOK(pParam->engine);
            break;
            
        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }
    
    return;            
}

