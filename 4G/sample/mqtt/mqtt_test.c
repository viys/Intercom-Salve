#include "kingnet.h"
#include "kingsocket.h"
#include "mqtt_client.h"
#include "dev_sign_api.h"
#include "infra_defs.h"
#include "mqtt_utils.h"
#include "kingplat.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("mqtt: "fmt, ##args); } while(0)


/*******************************************************************************
 ** Variables
 ******************************************************************************/
static hardwareHandle hMqttNIC = -1;
static softwareHandle hMqttContext = -1;
static mqtt_client_param_t mqttParam;
static iotx_sign_mqtt_t mqttConnInfoTest;
static mqtt_client_t *pMqttClient = NULL;
static THREAD_HANDLE mqttConnThreadID = NULL;
static THREAD_HANDLE mqttThreadID = NULL;
static MSG_HANDLE mqttMsgHandle = NULL;

static int isConnected = 0;
static int isTlsMode = 1;// 0---not tls, 1---TLS

static const char *mqtt_ca_crt = \
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
static int mqtt_disconnect(void);
static int mqtt_unsub(void);

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
static int mqtt_getNicHandle(void)
{
    int ret = 0;
    NIC_INFO info;

    memset(&info, 0x00, sizeof(NIC_INFO));
    info.NIC_Type = NIC_TYPE_WWAN;
    ret = KING_HardwareNICRequest(info, &hMqttNIC);
    LogPrintf("Mqtt_getNicHandle ret=%d, hNIC=%d", ret, hMqttNIC);

    return ret;
}

static int mqtt_getContextHandle(int isIpv6)
{
    int ret = 0;
    NIC_CONTEXT ApnInfo;

    if (hMqttNIC == -1)
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
    ret = KING_ConnectionAcquire(hMqttNIC, ApnInfo, &hMqttContext);
    LogPrintf("Mqtt_getContextHandle ret=%d, hMqttContext=%d", ret, hMqttContext);

    return ret;
}

static int mqtt_releaseContextHandle(void)
{
    int ret = 0;

    if (hMqttContext == -1)
    {
        return -1;
    }
    
    ret = KING_ConnectionRelease(hMqttContext);
    LogPrintf("mqtt_releaseContextHandle ret=%d", ret);

    return ret;
}

static void mqtt_authSet(char *prokey, char *devname, char *devsecret)
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
    meta.is_tls = isTlsMode;
    rc = IOT_Sign_MQTT(IOTX_CLOUD_REGION_SHANGHAI, &meta, &mqttConnInfoTest);
    if (rc < 0)
    {
        return;
    }

    return;
}

static void mqtt_reconn(void)
{
    LogPrintf("mqtt_reconn isConnected=%d", isConnected);
    if (!isConnected)
    {
        int ret = 0;
        
        ret = mqtt_getNicHandle();
        if (ret == -1)
        {
            LogPrintf("mqtt_reconn, get nic handle failed!");
            return;
        }
        
        ret = mqtt_getContextHandle(0);
        if (ret == -1)
        {
            LogPrintf("mqtt_reconn,get context handle failed!");
            return;
        }
        
        mqttParam.hContext = hMqttContext;
    }
}

static void mqtt_conn_thread(void *param)
{
    LogPrintf("mqtt_conn_thread start");
    while(1)
    {
        if (pMqttClient == NULL)
        {
            mqtt_utils_SleepMs(200);
            continue;
        }
        mqtt_client_yield(pMqttClient, 200);
        if (isConnected)
        {
            mqtt_utils_SleepMs(2000);
        }
        else
        {
            mqtt_reconn();
            mqtt_utils_SleepMs(5000);
        }
    }
    LogPrintf("MQTT mqtt_unsub start.");
    mqtt_unsub();
    KING_Sleep(4000);
    
    LogPrintf("MQTT mqtt_disconnect start.");
    mqtt_disconnect();
    KING_Sleep(1000);
    if (mqttConnThreadID != NULL)
    {
        mqttConnThreadID = NULL;
    } 
    KING_ThreadExit();
}

static void mqtt_CreateThreade(void)
{
    if (mqttConnThreadID == NULL)
    {
        int ret = 0;
        THREAD_ATTR_S threadAttr;
        
        memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
        threadAttr.fun = mqtt_conn_thread;
        threadAttr.priority = THREAD_PRIORIT1;
        threadAttr.stackSize = 1024 * 5;
        threadAttr.queueNum = 64; 

        ret = KING_ThreadCreate("MqttConnThread", &threadAttr, &mqttConnThreadID);
        if (ret != 0)
        {
            LogPrintf("mqtt_CreateThreade Thread Failed!!");
        }
    }
}

static void mqtt_EventHandle(void *pcontext, void *pclient, mqtt_event_msg_t *msg)
{
    uint32 idx = (uint32)pcontext;
    uint32 packetId = (uint32)msg->msg;
    mqtt_client_topic_info_t *topic_info = (mqtt_client_topic_info_t *)msg->msg;

    LogPrintf("mqtt_EventHandle,begin.msg->event_type=%d",msg->event_type);
    LogPrintf("mqtt_EventHandle,idx=%d",idx);
    switch (msg->event_type) 
    {
        case MQTT_EVENT_UNDEF:
            LogPrintf("MQTT undefined event occur.");
            break;

        case MQTT_EVENT_DISCONNECT:
            LogPrintf("MQTT disconnect.");
            isConnected = 0;
            break;

        case MQTT_EVENT_RECONNECT:
            LogPrintf("MQTT reconnect.");
            isConnected = 1;
            break;

        case MQTT_EVENT_SUBCRIBE_SUCCESS:
            LogPrintf("MQTT subscribe success, packetId-id=%u", (unsigned int)packetId);
            break;

        case MQTT_EVENT_SUBCRIBE_TIMEOUT:
            LogPrintf("MQTT subscribe wait ack timeout, packetId=%u", (unsigned int)packetId);
            break;

        case MQTT_EVENT_SUBCRIBE_NACK:
            LogPrintf("MQTT subscribe nack, packetId=%u", (unsigned int)packetId);
            break;

        case MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            LogPrintf("MQTT unsubscribe success, packetId=%u", (unsigned int)packetId);
            break;

        case MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            LogPrintf("MQTT unsubscribe timeout, packetId=%u", (unsigned int)packetId);
            break;

        case MQTT_EVENT_UNSUBCRIBE_NACK:
            LogPrintf("MQTT unsubscribe nack, packetId=%u", (unsigned int)packetId);
            break;

        case MQTT_EVENT_PUBLISH_SUCCESS:
            LogPrintf("MQTT publish success, packetId=%u", (unsigned int)packetId);
            break;

        case MQTT_EVENT_PUBLISH_TIMEOUT:
            LogPrintf("MQTT publish timeout, packetId=%u", (unsigned int)packetId);
            break;

        case MQTT_EVENT_PUBLISH_NACK:
            LogPrintf("MQTT publish nack, packetId=%u", (unsigned int)packetId);
            break;

        case MQTT_EVENT_PUBLISH_RECEIVED:
            LogPrintf("MQTT topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
                            topic_info->topic_len,
                            topic_info->ptopic,
                            topic_info->payload_len,
                            topic_info->payload);
            break;

        default:
            LogPrintf("MQTT Should NOT arrive here.");
            break;
    }
}

static void mqtt_SetParam(void)
{
    memset(&mqttParam, 0x00, sizeof(mqtt_client_param_t));
    
    mqttParam.hContext = hMqttContext;
    mqttParam.port = mqttConnInfoTest.port;
    mqttParam.host = mqttConnInfoTest.hostname;
    mqttParam.client_id = mqttConnInfoTest.clientid;
    mqttParam.username = mqttConnInfoTest.username;
    mqttParam.password = mqttConnInfoTest.password;
    
    mqttParam.request_timeout_ms = 60 * 1000;
    mqttParam.keepalive_interval_ms = 60 * 1000;
    mqttParam.read_buf_size = 1500;
    mqttParam.write_buf_size = 1500;
    mqttParam.clean_session = 1;
    mqttParam.sslMode = isTlsMode;// 0---TCP, 1---TLS
    mqttParam.reconn = 1;//开启重连
    
    mqttParam.handle_event.h_fp = mqtt_EventHandle;
    mqttParam.handle_event.pcontext = NULL;    
    if (mqttParam.sslMode == 1)
    {
        if (mqttParam.ca_crt == NULL)
        {
            mqttParam.ca_crt = mqtt_ca_crt;
            if (mqttParam.ca_crt != NULL)
            {
                LogPrintf("mqtt_SetParam, mqtt_test_ca_len=%u", strlen(mqttParam.ca_crt));
            }
            
        }
        //mqttParam.ca_crt=NULL;  //不需要CA证书可以把ca_crt设置为NULL
        mqttParam.client_crt = NULL;
        mqttParam.client_key = NULL;
        mqttParam.client_pwd = NULL;
    }
}

static void mqtt_MessageArrive(void *pcontext, void *pclient, mqtt_event_msg_t *msg)
{
    LogPrintf("mqtt_MessageArrive event_type = %d.", msg->event_type);
    switch (msg->event_type) 
    {
        case MQTT_EVENT_PUBLISH_RECEIVED:
        {
            mqtt_client_topic_info_t *ptopic_info = (mqtt_client_topic_info_t *) msg->msg;
            
            LogPrintf("mqtt_MessageArrive payload_len=%lu", ptopic_info->payload_len);
            LogPrintf("mqtt_MessageArrive payload=%s", ptopic_info->payload);
            LogPrintf("mqtt_MessageArrive ptopic=%s", ptopic_info->ptopic);
            LogPrintf("mqtt_MessageArrive packet_id=%d", ptopic_info->packet_id);
            break;
        }
        
        default:
            LogPrintf("mqtt Should NOT arrive here.");
            break;
    }
}

static int mqtt_connect(void)
{
    if (mqtt_client_is_connected(pMqttClient) == 1)
    {
        LogPrintf("MQTT ALREADY CONNECT.");
        return -1;
    }
    mqtt_SetParam();
    pMqttClient = mqtt_client_connect(&mqttParam);
    if (pMqttClient == NULL)
    {
        LogPrintf("MQTT CONNECT FAIL!");
        return -1;
    }
    isConnected = 1;
    mqtt_CreateThreade();
    return 0;
}

static int mqtt_sub(void)
{
    int subRet = -1;
    
    if (mqtt_client_is_connected(pMqttClient) == 0)
    {
        LogPrintf("mqtt_sub is not connected.");
        return -1;
    }
    subRet = mqtt_client_subscribe(pMqttClient, 
                                    "/MfgmvrPzJA7/device_3/get1", 1, 
                                    mqtt_MessageArrive, 
                                    NULL);
    if (subRet < 0)
    {
        LogPrintf("mqtt_sub subscribe is fail.");
        return -1;
    }
    return 0;
}

static int mqtt_pub(void)
{
    int pubRet = -1;
    mqtt_client_topic_info_t topic_msg;
    
    if (mqtt_client_is_connected(pMqttClient) == 0)
    {
        LogPrintf("mqtt_pub is not connected.");
        return -1;
    }
    memset(&topic_msg, 0x00, sizeof(mqtt_client_topic_info_t));
    
    topic_msg.qos = 1;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)"1234567890";
    topic_msg.payload_len = strlen("1234567890");
    pubRet = mqtt_client_publish(pMqttClient, "/MfgmvrPzJA7/device_3/get1", &topic_msg);
    if (pubRet < 0)
    {
        LogPrintf("mqtt_pub publish is fail.");
        return -1;
    }

    return 0;
}

static int mqtt_unsub(void)
{
    int unsubRet = -1;
    
    if (mqtt_client_is_connected(pMqttClient) == 0)
    {
        LogPrintf("mqtt_unsub is not connected.");
        return -1;
    }
    unsubRet = mqtt_client_unsubscribe(pMqttClient, 
                                        "/MfgmvrPzJA7/device_3/get1");
    if (unsubRet < 0)
    {
        LogPrintf("mqtt_unsub unsubscribe is fail.");
        return -1;
    }
    return 0;
}

static int mqtt_disconnect(void)
{
    LogPrintf("mqtt_disconnect start");
    if (pMqttClient != NULL)
    {
        mqtt_client_destroy(&pMqttClient);
    }
    mqtt_releaseContextHandle();
    isConnected = 0;
    return 0;
}

static void mqtt_startTest(void)
{
    int ret = -1;
    
    mqtt_authSet("MfgmvrPzJA7","device_3","lZmH5rxgXpkk5UrAh6R7vhyjnWdJlAR7");
    ret = mqtt_connect();
    if (ret != 0)
    {
        LogPrintf("mqtt_startTest mqtt_connect fail!\r\n");
    }
    else
    {
        KING_Sleep(3000);
        LogPrintf("MQTT mqtt_sub start.");
        mqtt_sub();
        KING_Sleep(3000);
        
        LogPrintf("MQTT mqtt_pub start.");
        mqtt_pub();
        KING_Sleep(4000);
    }
}

static void mqtt_netTest(void)
{
    NET_STATE_E state = NET_NO_SERVICE;
    int ret = 0;

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
            break;
        }
        KING_Sleep(1000);
    }

    mqtt_getNicHandle();
    mqtt_getContextHandle(0);
}

static void mqtt_test_thread(void *param)
{
    MSG_S msg;
    int ret = -1;

    LogPrintf("mqtt_test_thread is running. mqttThreadID = 0x%X\r\n", mqttThreadID);
    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        LogPrintf("mqtt_test_thread!\r\n");
        if (mqttMsgHandle != NULL)
        {
            LogPrintf("mqtt_test_thread KING_MsgRcv!\r\n");
            ret = KING_MsgRcv(&msg, mqttMsgHandle, WAIT_OPT_INFINITE);
            if (ret == -1)
            {
                LogPrintf("mqtt_test_thread ret=%d\r\n", ret);
                continue;
            }
            LogPrintf("mqtt_test_thread receive msg. MsgId=%d\r\n", msg.messageID);
            if (msg.messageID == 120)
            {
                mqtt_netTest();
                mqtt_startTest();
                break;
            }
        }
    }
    
    if (mqttMsgHandle)
    {
        KING_MsgDelete(mqttMsgHandle);
        mqttMsgHandle = NULL;
    }
    if (mqttThreadID != NULL)
    {
        mqttThreadID = NULL;
    } 
    KING_ThreadExit();
}

void mqtt_test(void)
{
    MSG_S msg;
    int ret = -1;
    THREAD_ATTR_S threadAttr;
    
    ret = KING_MsgCreate(&mqttMsgHandle);
    LogPrintf("mqtt_test is KING_MsgCreate ret %d", ret);
    if(-1 == ret)
    {
        LogPrintf("KING_MsgCreate, errcode=0x%x\r\n", KING_GetLastErrCode());
    }

    KING_Sleep(1000);

    LogPrintf("mqtt_test is KING_ThreadCreate ");
    memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
    threadAttr.fun = mqtt_test_thread;
    threadAttr.priority = THREAD_PRIORIT1;
    threadAttr.stackSize = 1024*5;
    threadAttr.queueNum = 64; 
    
    LogPrintf("mqtt_test is create thread 0x%08x", &mqttThreadID);
    ret = KING_ThreadCreate("mqttTestThread", &threadAttr, &mqttThreadID);
    if (-1 == ret)
    {
        LogPrintf("mqtt_test Thread Failed!! Errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    KING_Sleep(1000);
    
    LogPrintf("mqtt_test is KING_MsgSend ");
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = 120;
    ret = KING_MsgSend(&msg, mqttMsgHandle);
    if(-1 == ret)
    {
        LogPrintf("KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
}

