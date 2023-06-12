/*******************************************************************************
 ** File Name:   mqtt_client.c
 ** Author:      Allance.Chen
 ** Date:        2020-06-04
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains ds functions about MQTT client.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-06-04    Allance.Chen         Create.
 ******************************************************************************/
/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "mqtt_client.h"


/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define KEEP_ALIVE_INTERVAL_DEFAULT_MIN                 60
#define KEEP_ALIVE_INTERVAL_DEFAULT_MAX                 180
#define MQTT_CLIENT_RECONNECT_INTERVAL_DEFAULT_MS       (10000)
#define MQTT_CLIENT_RECONNECT_INTERVAL_MAX_MS           (60000)
#define MQTT_CONNECT_REQUIRED_BUFLEN                    (256)
#define MQTT_WAIT_CONNACK_MAX                           (10)
#define MQTT_PACKET_ID_MAX                              (65535)
#define MQTT_TOPIC_NAME_MAX_LEN                         (256)
#define MQTT_SUB_REQUEST_NUM_MAX                        (256)
#define MQTT_REPUB_NUM_MAX                              (20)
#define MQTT_MUTLI_SUBSCIRBE_MAX                        (5)
#define MQTT_CONFIG_SUBINFO_LIFE                        (10)

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef struct {
    int packet_id;
    uint8 ack_type;
    mqtt_event_handle_func_fpt sub_state_cb;
    struct list_head linked_list;
} mqtt_sub_node_t;


/*******************************************************************************
 ** Variables
 ******************************************************************************/
static struct list_head g_mqtt_sub_list = LIST_HEAD_INIT(g_mqtt_sub_list);
static SEM_HANDLE mqttSem = NULL;
static int isMqttDnsRunning = 0;
static int isMqttDnsSucc = 0;
static SOCK_IN_ADDR_T hostAddr;

/*******************************************************************************
 ** Function 
 ******************************************************************************/
static int mqtt_client_check_rule(char *iterm, mqtt_client_topic_type_t type)
{
    int i = 0;
    int len = 0;

    if (NULL == iterm) 
    {
        mqtt_printf("iterm is NULL");
        return FAIL_RETURN;
    }

    len = strlen(iterm);

    for (i = 0; i < len; i++) 
    {
        if (TOPIC_FILTER_TYPE == type) 
        {
            if ('+' == iterm[i] || '#' == iterm[i]) 
            {
                if (1 != len) 
                {
                    mqtt_printf("the character # and + is error");
                    return FAIL_RETURN;
                }
            }
        } 
        else 
        {
            if ('+' == iterm[i] || '#' == iterm[i]) 
            {
                mqtt_printf("has character # and + is error");
                return FAIL_RETURN;
            }
        }

        if (iterm[i] < 32 || iterm[i] >= 127) 
        {
            return FAIL_RETURN;
        }
    }
    return SUCCESS_RETURN;
}

static char mqtt_client_is_topic_matched(char *topicFilter, MQTTString *topicName)
{
    char *curf = topicFilter;
    char *curn = topicName->lenstring.data;
    char *curn_end = curn + topicName->lenstring.len;
    
    if (!topicFilter || !topicName) 
    {
        return 0;
    }
    while (*curf && curn < curn_end) 
    {
        if (*curn == '/' && *curf != '/') 
        {
            break;
        }

        if (*curf != '+' && *curf != '#' && *curf != *curn) 
        {
            break;
        }

        if (*curf == '+') 
        {
            /* skip until we meet the next separator, or end of string */
            char *nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/') 
            {
                nextpos = ++curn + 1;
            }
        } 
        else if (*curf == '#') 
        {
            curn = curn_end - 1;    /* skip until end of string */
        }
        curf++;
        curn++;
    }

    return (curn == curn_end) && (*curf == '\0');
}

static int mqtt_client_check_topic(const char *topicName, mqtt_client_topic_type_t type)
{
    int mask = 0;
    char *delim = "/";
    char *iterm = NULL;
    char topicString[MQTT_TOPIC_NAME_MAX_LEN];
    
    //if (NULL == topicName || '/' != topicName[0]) 
    //{
        //return FAIL_RETURN;
    //}

    if (strlen(topicName) > MQTT_TOPIC_NAME_MAX_LEN) 
    {
        mqtt_printf("len of topicName exceeds 64");
        return FAIL_RETURN;
    }

    memset(topicString, 0x0, MQTT_TOPIC_NAME_MAX_LEN);
    strncpy(topicString, topicName, MQTT_TOPIC_NAME_MAX_LEN - 1);

    iterm = strtok(topicString, delim);

    if (SUCCESS_RETURN != mqtt_client_check_rule(iterm, type)) 
    {
        mqtt_printf("run iotx_check_rule error");
        return FAIL_RETURN;
    }

    for (;;) 
    {
        iterm = strtok(NULL, delim);

        if (iterm == NULL) 
        {
            break;
        }

        /* The character '#' is not in the last */
        if (1 == mask) 
        {
            mqtt_printf("the character # is error");
            return FAIL_RETURN;
        }

        if (SUCCESS_RETURN != mqtt_client_check_rule(iterm, type)) 
        {
            mqtt_printf("run iotx_check_rule error");
            return FAIL_RETURN;
        }

        if (iterm[0] == '#') 
        {
            mask = 1;
        }
    }

    return SUCCESS_RETURN;
}

static int mqtt_client_set_connect_params(mqtt_client_t *pClient, MQTTPacket_connectData *pConnectParams)
{
    if (NULL == pClient || NULL == pConnectParams) 
    {
        return NULL_VALUE_ERROR;
    }
    
    memcpy(pClient->connect_data.struct_id, pConnectParams->struct_id, 4);
    pClient->connect_data.struct_version = pConnectParams->struct_version;
    pClient->connect_data.MQTTVersion = pConnectParams->MQTTVersion;
    pClient->connect_data.clientID = pConnectParams->clientID;
    pClient->connect_data.cleansession = pConnectParams->cleansession;
    pClient->connect_data.willFlag = pConnectParams->willFlag;
    pClient->connect_data.username = pConnectParams->username;
    pClient->connect_data.password = pConnectParams->password;
    memcpy(pClient->connect_data.will.struct_id, pConnectParams->will.struct_id, 4);
    pClient->connect_data.will.struct_version = pConnectParams->will.struct_version;
    pClient->connect_data.will.topicName = pConnectParams->will.topicName;
    pClient->connect_data.will.message = pConnectParams->will.message;
    pClient->connect_data.will.qos = pConnectParams->will.qos;
    pClient->connect_data.will.retained = pConnectParams->will.retained;

    if (pConnectParams->keepAliveInterval < KEEP_ALIVE_INTERVAL_DEFAULT_MIN) 
    {
        mqtt_printf("Input heartbeat interval(%d ms) < Allowed minimum(%d ms)",
                    (pConnectParams->keepAliveInterval * 1000),
                    (KEEP_ALIVE_INTERVAL_DEFAULT_MIN * 1000)
                    );
        mqtt_printf("Reset heartbeat interval => %d Millisecond",
                    (KEEP_ALIVE_INTERVAL_DEFAULT_MIN * 1000)
                    );
        pClient->connect_data.keepAliveInterval = KEEP_ALIVE_INTERVAL_DEFAULT_MIN;
    } 
    else if (pConnectParams->keepAliveInterval > KEEP_ALIVE_INTERVAL_DEFAULT_MAX) 
    {
        mqtt_printf("Input heartbeat interval(%d ms) > Allowed maximum(%d ms)",
                    (pConnectParams->keepAliveInterval * 1000),
                    (KEEP_ALIVE_INTERVAL_DEFAULT_MAX * 1000)
                    );
        mqtt_printf("Reset heartbeat interval => %d Millisecond",
                    (KEEP_ALIVE_INTERVAL_DEFAULT_MAX * 1000)
                    );
        pClient->connect_data.keepAliveInterval = KEEP_ALIVE_INTERVAL_DEFAULT_MAX;
    } 
    else 
    {
        pClient->connect_data.keepAliveInterval = pConnectParams->keepAliveInterval;
    }

    return SUCCESS_RETURN;
}

static void mqtt_client_set_client_state(mqtt_client_t *pClient, mqtt_client_state_t newState)
{
    mqtt_utils_MutexLock(pClient->lock_generic);
    pClient->client_state = newState;
    mqtt_utils_MutexUnlock(pClient->lock_generic);
}

static mqtt_client_state_t mqtt_client_get_client_state(mqtt_client_t *pClient)
{
    mqtt_client_state_t state;

    mqtt_utils_MutexLock(pClient->lock_generic);
    state = pClient->client_state;
    mqtt_utils_MutexUnlock(pClient->lock_generic);

    return state;
}

static int mqtt_client_get_next_packetid(mqtt_client_t *c)
{
    unsigned int id = 0;

    if (!c) 
    {
        return FAIL_RETURN;
    }

    mqtt_utils_MutexLock(c->lock_generic);
    c->packet_id = (c->packet_id == MQTT_PACKET_ID_MAX) ? 1 : c->packet_id + 1;
    id = c->packet_id;
    mqtt_utils_MutexUnlock(c->lock_generic);

    return id;
}

static int mqtt_client_send_packet(mqtt_client_t *c, char *buf, int length, mqtt_time_t *time)
{
    int rc = FAIL_RETURN;
    int sent = 0;
    unsigned int left_t = 0;

    if (!c || !buf || !time) 
    {
        return rc;
    }

    while (sent < length && !mqtt_utils_time_is_expired(time)) 
    {
        left_t = mqtt_utils_time_left(time);
        left_t = (left_t == 0) ? 1 : left_t;
        rc = c->network->write(c->network, &buf[sent], length, left_t);
        if (rc < 0) 
        { 
            break;
        }
        sent += rc;
    }

    if (sent == length) 
    {
        rc = SUCCESS_RETURN;
    } 
    else 
    {
        rc = MQTT_NETWORK_ERROR;
    }
    return rc;
}

static int mqtt_client_decode_packet(mqtt_client_t *c, int *value, int timeout)
{
    char i;
    int multiplier = 1;
    int len = 0;

    if (!c || !value) 
    {
        return FAIL_RETURN;
    }

    *value = 0;
    do {
        int rc = MQTTPACKET_READ_ERROR;

        if (++len > 4) 
        {
            return MQTTPACKET_READ_ERROR; /* bad data */
        }

        rc = c->network->read(c->network, &i, 1, timeout == 0 ? 1 : timeout);
        if (rc != 1) 
        {
            return MQTT_NETWORK_ERROR;
        }

        *value += (i & 127) * multiplier;
        multiplier *= 128;
    } while ((i & 128) != 0);

    return len;
}

static int mqtt_client_handle_event(mqtt_event_handle_t *handle, mqtt_client_t *c, mqtt_event_msg_t *msg)
{
    if (handle == NULL || handle->h_fp == NULL)
    {
        return FAIL_RETURN;
    }
    
    handle->h_fp(handle->pcontext, c, msg);
    return 0;
}

static int mqtt_client_read_packet(mqtt_client_t *c, mqtt_time_t *timer, unsigned int *packet_type)
{
    MQTTHeader header = {0};
    int len = 0;
    int rem_len = 0;
    int rc = 0;
    unsigned int left_t = 0;

    if (!c || !timer || !packet_type) 
    {
        return FAIL_RETURN;
    }
    
    mqtt_utils_MutexLock(c->lock_read_buf);
    /* 1. read the header byte.  This has the packet type in it */
    left_t = mqtt_utils_time_left(timer);
    left_t = (left_t == 0) ? 1 : left_t;
    rc = c->network->read(c->network, c->buf_read, 1, left_t);
    if (0 == rc) 
    { /* timeout */
        *packet_type = 0;
        mqtt_utils_MutexUnlock(c->lock_read_buf);
        return SUCCESS_RETURN;
    } 
    else if (1 != rc) 
    {
        mqtt_printf("mqtt read error, rc=%d", rc);
        mqtt_utils_MutexUnlock(c->lock_read_buf);
        return FAIL_RETURN;
    }

    len = 1;


    /* 2. read the remaining length.  This is variable in itself */
    left_t = mqtt_utils_time_left(timer);
    left_t = (left_t == 0) ? 1 : left_t;
    if ((rc = mqtt_client_decode_packet(c, &rem_len, left_t)) < 0) 
    {
        mqtt_printf("decodePacket error,rc = %d", rc);
        mqtt_utils_MutexUnlock(c->lock_read_buf);
        return rc;
    }

    len += MQTTPacket_encode((unsigned char *)c->buf_read + 1,
                            rem_len); /* put the original remaining length back into the buffer */

    /* Check if the received data length exceeds mqtt read buffer length */
    if ((rem_len > 0) && ((rem_len + len) > c->buf_size_read)) 
    {
        int needReadLen = c->buf_size_read - len;
        int remainDataLen = 0;
        char *remainDataBuf = NULL;
        
        mqtt_printf("mqtt read buffer is too short, mqttReadBufLen : %u, remainDataLen : %d", c->buf_size_read, rem_len);
        left_t = mqtt_utils_time_left(timer);
        left_t = (left_t == 0) ? 1 : left_t;
        if (c->network->read(c->network, c->buf_read + len, needReadLen, left_t) != needReadLen) 
        {
            mqtt_printf("mqtt read error");
            mqtt_utils_MutexUnlock(c->lock_read_buf);
            return FAIL_RETURN;
        }

        /* drop data whitch over the length of mqtt buffer */
        remainDataLen = rem_len - needReadLen;
        remainDataBuf = malloc(remainDataLen + 1);
        if (!remainDataBuf) 
        {
            mqtt_printf("allocate remain buffer failed");
            mqtt_utils_MutexUnlock(c->lock_read_buf);
            return FAIL_RETURN;
        }

        left_t = mqtt_utils_time_left(timer);
        left_t = (left_t == 0) ? 1 : left_t;
        if (c->network->read(c->network, remainDataBuf, remainDataLen, left_t) != remainDataLen) 
        {
            mqtt_printf("mqtt read error");
            free(remainDataBuf);
            remainDataBuf = NULL;
            mqtt_utils_MutexUnlock(c->lock_read_buf);
            return FAIL_RETURN;
        }

        free(remainDataBuf);
        remainDataBuf = NULL;
        mqtt_utils_MutexUnlock(c->lock_read_buf);
        if (NULL != c->handle_event.h_fp) 
        {
            mqtt_event_msg_t msg;

            msg.event_type = MQTT_EVENT_BUFFER_OVERFLOW;
            msg.msg = "mqtt read buffer is too short";
            mqtt_client_handle_event(&c->handle_event, c, &msg);
        }

        return SUCCESS_RETURN;
    }

    /* 3. read the rest of the buffer using a callback to supply the rest of the data */
    left_t = mqtt_utils_time_left(timer);
    left_t = (left_t == 0) ? 1 : left_t;
    if (rem_len > 0 && (c->network->read(c->network, c->buf_read + len, rem_len, left_t) != rem_len)) 
    {
        mqtt_printf("mqtt read error");
        mqtt_utils_MutexUnlock(c->lock_read_buf);
        return FAIL_RETURN;
    }

    header.byte = c->buf_read[0];
    *packet_type = header.bits.type;
    if ((len + rem_len) < c->buf_size_read) 
    {
        c->buf_read[len + rem_len] = '\0';
    }
    mqtt_utils_MutexUnlock(c->lock_read_buf);
    return SUCCESS_RETURN;
}

static int mqtt_client_puback(mqtt_client_t *c, unsigned int msgId, enum msgTypes type)
{
    int rc = 0;
    int len = 0;
    mqtt_time_t timer;

    if (!c) 
    {
        return FAIL_RETURN;
    }

    mqtt_utils_time_init(&timer);
    mqtt_utils_time_countdown_ms(&timer, c->request_timeout_ms);

    mqtt_utils_MutexLock(c->lock_write_buf);
    if (type == PUBACK) 
    {
        len = MQTTSerialize_ack((unsigned char *)c->buf_send, c->buf_size_send, PUBACK, 0, msgId);
    } 
    else if (type == PUBREC)
    {
        len = MQTTSerialize_ack((unsigned char *)c->buf_send, c->buf_size_send, PUBREC, 0, msgId);
    }
    else if (type == PUBREL)
    {
        len = MQTTSerialize_ack((unsigned char *)c->buf_send, c->buf_size_send, PUBREL, 0, msgId);
    }
    else 
    {
        mqtt_utils_MutexUnlock(c->lock_write_buf);
        return MQTT_PUBLISH_ACK_TYPE_ERROR;
    }

    if (len <= 0) 
    {
        mqtt_utils_MutexUnlock(c->lock_write_buf);
        return MQTT_PUBLISH_ACK_PACKET_ERROR;
    }

    rc = mqtt_client_send_packet(c, c->buf_send, len, &timer);
    if (rc != SUCCESS_RETURN) 
    {
        mqtt_utils_MutexUnlock(c->lock_write_buf);
        return MQTT_NETWORK_ERROR;
    }

    mqtt_utils_MutexUnlock(c->lock_write_buf);
    return SUCCESS_RETURN;
}

static int mqtt_client_mask_pubInfo_from(mqtt_client_t *c, uint16 msgId)
{
    mqtt_client_pub_info_t *node = NULL;

    if (!c) 
    {
        return FAIL_RETURN;
    }

    mqtt_utils_MutexLock(c->lock_list_pub);
    list_for_each_entry(node, &c->list_pub_wait_ack, linked_list, mqtt_client_pub_info_t) 
    {
        if (node->msg_id == msgId) 
        {
            node->node_state = MQTT_CLIENT_NODE_STATE_INVALID; 
        }
    }
    mqtt_utils_MutexUnlock(c->lock_list_pub);

    return SUCCESS_RETURN;
}

static int mqtt_client_pubInfo_clear(mqtt_client_t *pClient)
{
    mqtt_client_pub_info_t *node = NULL;
    
    if (!pClient) 
    {
        return FAIL_RETURN;
    }
    mqtt_printf("mqtt_client_pubInfo_clear start");

    mqtt_utils_MutexLock(pClient->lock_list_pub);
    list_for_each_entry(node, &pClient->list_pub_wait_ack, linked_list, mqtt_client_pub_info_t) 
    {
        mqtt_printf("mqtt_client_pubInfo_clear node->msg_id=%d", node->msg_id);
        if (node->msg_id != 0)
        {
            node->node_state = MQTT_CLIENT_NODE_STATE_INVALID;
        }
    }
    mqtt_utils_MutexUnlock(pClient->lock_list_pub);

    return SUCCESS_RETURN;
}

static int mqtt_client_mask_subInfo_from(mqtt_client_t *c, unsigned int msgId, mqtt_client_topic_handle_t **messageHandler)
{
    mqtt_client_subsribe_info_t *node = NULL;

    if (!c) 
    {
        return FAIL_RETURN;
    }

    list_for_each_entry(node, &c->list_sub_wait_ack, linked_list, mqtt_client_subsribe_info_t) 
    {
        if (node->msg_id == msgId) 
        {
            *messageHandler = node->handler;
            node->handler = NULL;
            node->node_state = MQTT_CLIENT_NODE_STATE_INVALID; 
            break;
        }
    }

    return SUCCESS_RETURN;
}

static int mqtt_client_push_subInfo_to(mqtt_client_t *c, int len, 
                                            unsigned short msgId, enum msgTypes type,
                                            mqtt_client_topic_handle_t *handler,
                                            mqtt_client_subsribe_info_t **node)
{
    int list_number = 0;

    if (!c || !handler || !node) 
    {
        return FAIL_RETURN;
    }

    mqtt_utils_MutexLock(c->lock_list_sub);
    list_number = list_entry_number(&c->list_sub_wait_ack);

    mqtt_printf("c->list_sub_wait_ack->len:MQTT_SUB_REQUEST_NUM_MAX = %d:%d",
                list_number,
                MQTT_SUB_REQUEST_NUM_MAX);

    if (list_number >= MQTT_SUB_REQUEST_NUM_MAX) 
    {
        mqtt_utils_MutexUnlock(c->lock_list_sub);
        mqtt_printf("number of subInfo more than max!,size = %d", list_number);
        return FAIL_RETURN;
    }

    mqtt_client_subsribe_info_t *subInfo = (mqtt_client_subsribe_info_t *)mqtt_utils_Malloc(sizeof(
                                        mqtt_client_subsribe_info_t) + len);
    if (NULL == subInfo) 
    {
        mqtt_utils_MutexUnlock(c->lock_list_sub);
        mqtt_printf("run iotx_memory_malloc is error!");
        return FAIL_RETURN;
    }

    subInfo->node_state = MQTT_CLIENT_NODE_STATE_NORMANL;
    subInfo->msg_id = msgId;
    subInfo->len = len;
    mqtt_utils_time_start(&subInfo->sub_start_time);
    subInfo->type = type;
    subInfo->handler = handler;
    INIT_LIST_HEAD(&subInfo->linked_list);

    list_add_tail(&subInfo->linked_list, &c->list_sub_wait_ack);
    *node = subInfo;

    mqtt_utils_MutexUnlock(c->lock_list_sub);

    return SUCCESS_RETURN;
}

static int mqtt_client_push_pubInfo_to(mqtt_client_t *c, int len, unsigned short msgId, mqtt_client_pub_info_t **node)
{
    int list_number;

    if (!c || !node) 
    {
        mqtt_printf("the param of c is error!");
        return FAIL_RETURN;
    }

    list_number = list_entry_number(&c->list_pub_wait_ack);
    if ((len < 0) || (len > c->buf_size_send)) 
    {
        mqtt_printf("the param of len is error!");
        return FAIL_RETURN;
    }

    if (list_number >= MQTT_REPUB_NUM_MAX) 
    {
        mqtt_printf("more than %u elements in republish list. List overflow!", list_number);
        return FAIL_RETURN;
    }

    mqtt_client_pub_info_t *repubInfo = (mqtt_client_pub_info_t *)mqtt_utils_Malloc(sizeof(mqtt_client_pub_info_t) + len);
    if (NULL == repubInfo) 
    {
        mqtt_printf("run iotx_memory_malloc is error!");
        return FAIL_RETURN;
    }

    repubInfo->node_state = MQTT_CLIENT_NODE_STATE_NORMANL;
    repubInfo->msg_id = msgId;
    repubInfo->len = len;
    mqtt_utils_time_start(&repubInfo->pub_start_time);
    repubInfo->buf = (unsigned char *)repubInfo + sizeof(mqtt_client_pub_info_t);

    memcpy(repubInfo->buf, c->buf_send, len);
    INIT_LIST_HEAD(&repubInfo->linked_list);

    list_add_tail(&repubInfo->linked_list, &c->list_pub_wait_ack);

    *node = repubInfo;

    return SUCCESS_RETURN;
}

static int mqtt_client_check_handle_is_identical
(
    mqtt_client_topic_handle_t *messageHandlers1,
    mqtt_client_topic_handle_t *messageHandler2
)
{
    int topicNameLen = 0;
    
    if (!messageHandlers1 || !messageHandler2) 
    {
        return 1;
    }

    if (!(messageHandlers1->topic_filter) || !(messageHandler2->topic_filter)) 
    {
        return 1;
    }

    topicNameLen = strlen(messageHandlers1->topic_filter);
    if (topicNameLen != strlen(messageHandler2->topic_filter)) 
    {
        return 1;
    }

    if (0 != strncmp(messageHandlers1->topic_filter, messageHandler2->topic_filter, topicNameLen)) 
    {
        return 1;
    }
    if (messageHandlers1->handle.h_fp != messageHandler2->handle.h_fp) 
    {
        return 1;
    }

    /* context must be identical also */
    if (messageHandlers1->handle.pcontext != messageHandler2->handle.pcontext) 
    {
        return 1;
    }

    return 0;
}

static int mqtt_client_check_handle_is_identical_ex
(
    mqtt_client_topic_handle_t *messageHandlers1,
    mqtt_client_topic_handle_t *messageHandler2
)
{
    int topicNameLen = 0;
    
    if (!messageHandlers1 || !messageHandler2) 
    {
        return 1;
    }

    if (!(messageHandlers1 ->topic_filter) || !(messageHandler2->topic_filter)) 
    {
        return 1;
    }

    topicNameLen = strlen(messageHandlers1->topic_filter);
    if (topicNameLen != strlen(messageHandler2->topic_filter)) 
    {
        return 1;
    }

    if (0 != strncmp(messageHandlers1->topic_filter, messageHandler2->topic_filter, topicNameLen)) 
    {
        return 1;
    }

    return 0;
}

static int mqtt_client_remove_handle_from_list(mqtt_client_t *c, mqtt_client_topic_handle_t *h)
{
    mqtt_client_topic_handle_t **hp, *h1;

    hp = &c->first_sub_handle;
    while ((*hp) != NULL) 
    {
        h1 = *hp;
        if (h1 == h) 
        {
            *hp = h->next;
        } 
        else 
        {
            hp = &h1->next;
        }
    }

    return 0;
}

static int mqtt_client_disconnect_send_packet(mqtt_client_t *c)
{
    int rc = FAIL_RETURN;
    int len = 0;
    mqtt_time_t timer;    

    if (!c) 
    {
        return FAIL_RETURN;
    }

    mqtt_utils_MutexLock(c->lock_write_buf);

    len = MQTTSerialize_disconnect((unsigned char *)c->buf_send, c->buf_size_send);

    mqtt_utils_time_init(&timer);
    mqtt_utils_time_countdown_ms(&timer, c->request_timeout_ms);

    if (len > 0) 
    {
        rc = mqtt_client_send_packet(c, c->buf_send, len, &timer); 
    }
    
    mqtt_utils_MutexUnlock(c->lock_write_buf);
    return rc;
}

static void mqtt_client_deliver_message(mqtt_client_t *c, MQTTString *topicName, mqtt_client_topic_info_t *topic_msg)
{
    int flag_matched = 0;

    if (!c || !topicName || !topic_msg) 
    {
        return;
    }

    topic_msg->ptopic = topicName->lenstring.data;
    topic_msg->topic_len = topicName->lenstring.len;

    /* we have to find the right message handler - indexed by topic */
    mqtt_utils_MutexLock(c->lock_generic);
    mqtt_client_topic_handle_t *h;
    for (h = c->first_sub_handle; h != NULL; h = h->next) 
    {
        if (MQTTPacket_equals(topicName, (char *)h->topic_filter)
            || mqtt_client_is_topic_matched((char *)h->topic_filter, topicName)) 
        {
            mqtt_client_topic_handle_t *msg_handle = h;

            mqtt_printf("topic be matched");
            mqtt_utils_MutexUnlock(c->lock_generic);
            if (NULL != msg_handle->handle.h_fp) 
            {
                mqtt_event_msg_t msg;
                
                msg.event_type = MQTT_EVENT_PUBLISH_RECEIVED;
                msg.msg = (void *)topic_msg;
                mqtt_client_handle_event(&msg_handle->handle, c, &msg);
                flag_matched = 1;
            }

            mqtt_utils_MutexLock(c->lock_generic);
        }
    }

    mqtt_utils_MutexUnlock(c->lock_generic);
    mqtt_printf("mqtt_client_deliver_message flag_matched:%d", flag_matched);
    if (0 == flag_matched) 
    {
        mqtt_printf("NO matching any topic, call default handle function");
        if (NULL != c->handle_event.h_fp) 
        {
            mqtt_event_msg_t msg;

            msg.event_type = MQTT_EVENT_PUBLISH_RECEIVED;
            msg.msg = topic_msg;
            mqtt_client_handle_event(&c->handle_event, c, &msg);
        }
    }
}

static void mqtt_client_event_handle_sub(void *pcontext, void *pclient, mqtt_event_msg_t *msg)
{
    mqtt_client_t *client = (mqtt_client_t *)pclient;
    int packet_id = (int) msg->msg;
    mqtt_sub_node_t *node = NULL;
    mqtt_sub_node_t *next = NULL;
    
    if (pclient == NULL || msg == NULL) 
    {
        return;
    }
    
    mqtt_printf("packet_id = %d, event_type=%d", packet_id, msg->event_type);
    mqtt_utils_MutexLock(client->lock_generic);
    list_for_each_entry_safe(node, next, &g_mqtt_sub_list, linked_list, mqtt_sub_node_t) 
    {
        if (node->packet_id == packet_id) 
        {
            node->ack_type = msg->event_type;
        }
    }
    mqtt_utils_MutexUnlock(client->lock_generic);
}

static int mqtt_client_handle_recv_CONNACK(mqtt_client_t *c)
{
    int rc = SUCCESS_RETURN;
    unsigned char connack_rc = 255;
    char sessionPresent = 0;

    if (!c) 
    {
        return FAIL_RETURN;
    }

    if (MQTTDeserialize_connack((unsigned char *)&sessionPresent, &connack_rc, (unsigned char *)c->buf_read,
                                c->buf_size_read) != 1) 
    {
        mqtt_printf("connect ack is error");
        return MQTT_CONNECT_ACK_PACKET_ERROR;
    }
    mqtt_printf("mqtt_client_handle_recv_CONNACK connack_rc=%d", connack_rc);

    switch (connack_rc) 
    {
        case MQTT_CONNECTION_ACCEPTED:
            rc = SUCCESS_RETURN;
            break;
            
        case MQTT_UNNACCEPTABLE_PROTOCOL:
            rc = MQTT_CONANCK_UNACCEPTABLE_PROTOCOL_VERSION_ERROR;
            break;
            
        case MQTT_CLIENTID_REJECTED:
            rc = MQTT_CONNACK_IDENTIFIER_REJECTED_ERROR;
            break;
            
        case MQTT_SERVER_UNAVAILABLE:
            rc = MQTT_CONNACK_SERVER_UNAVAILABLE_ERROR;
            break;
            
        case MQTT_BAD_USERNAME_OR_PASSWORD:
            rc = MQTT_CONNACK_BAD_USERDATA_ERROR;
            break;
            
        case MQTT_NOT_AUTHORIZED:
            rc = MQTT_CONNACK_NOT_AUTHORIZED_ERROR;
            break;
            
        default:
            rc = MQTT_CONNACK_UNKNOWN_ERROR;
            break;
    }

    return rc;
}

static int mqtt_client_handle_recv_PUBACK(mqtt_client_t *c)
{
    unsigned short mypacketid;
    unsigned char dup = 0;
    unsigned char type = 0;

    if (!c) 
    {
        return FAIL_RETURN;
    }

    if (MQTTDeserialize_ack(&type, &dup, &mypacketid, (unsigned char *)c->buf_read, c->buf_size_read) != 1) 
    {
        return MQTT_PUBLISH_ACK_PACKET_ERROR;
    }

    (void)mqtt_client_mask_pubInfo_from(c, (uint16)mypacketid);

    /* call callback function to notify that PUBLISH is successful */
    if (NULL != c->handle_event.h_fp) 
    {
        mqtt_event_msg_t msg;
        
        msg.event_type = MQTT_EVENT_PUBLISH_SUCCESS;
        msg.msg = (void *)(int)mypacketid;
        mqtt_client_handle_event(&c->handle_event, c, &msg);
    }

    return SUCCESS_RETURN;
}

static int mqtt_client_handle_recv_SUBACK(mqtt_client_t *c)
{
    unsigned short mypacketid;
    int i = 0, count = 0, fail_flag = -1, j = 0;
    int grantedQoS[MQTT_MUTLI_SUBSCIRBE_MAX];
    int rc;
    mqtt_client_topic_handle_t *messagehandler = NULL;
    int flag_dup = 0;
    mqtt_event_msg_t msg;

    if (!c) 
    {
        return FAIL_RETURN;
    }

    rc = MQTTDeserialize_suback(&mypacketid, MQTT_MUTLI_SUBSCIRBE_MAX, &count, grantedQoS, (unsigned char *)c->buf_read,
                                c->buf_size_read);
    if (rc < 0) 
    {
        mqtt_printf("Sub ack packet error, rc = MQTTDeserialize_suback() = %d", rc);
        return MQTT_SUBSCRIBE_ACK_PACKET_ERROR;
    }

    mqtt_printf("%20s : %d", "Return Value", rc);
    mqtt_printf("%20s : %d", "Packet ID", mypacketid);
    mqtt_printf("%20s : %d", "Count", count);
    for (i = 0; i < count; ++i) 
    {
        mqtt_printf("%16s[%02d] : %d", "Granted QoS", i, grantedQoS[i]);
    }

    mqtt_utils_MutexLock(c->lock_list_sub);
    (void)mqtt_client_mask_subInfo_from(c, mypacketid, &messagehandler);
    mqtt_utils_MutexUnlock(c->lock_list_sub);
    if ((NULL == messagehandler)) 
    {
        return MQTT_SUB_INFO_NOT_FOUND_ERROR;
    }

    if (NULL == messagehandler->topic_filter) 
    {
        mqtt_utils_Free(messagehandler);
        return MQTT_SUB_INFO_NOT_FOUND_ERROR;
    }

    if ((NULL == messagehandler->handle.h_fp)) 
    {
        mqtt_utils_Free((void *)messagehandler->topic_filter);
        mqtt_utils_Free(messagehandler);
        return MQTT_SUB_INFO_NOT_FOUND_ERROR;
    }

    for (j = 0; j <  count; j++) 
    {
        mqtt_client_topic_handle_t *h;
        
        fail_flag = 0;
        /* In negative case, grantedQoS will be 0xFFFF FF80, which means -128 */
        if ((uint8)grantedQoS[j] == 0x80) 
        {
            fail_flag = 1;
            mqtt_printf("MQTT SUBSCRIBE failed, ack code is 0x80");
        }

        flag_dup = 0;
        mqtt_utils_MutexLock(c->lock_generic);
        for (h = c->first_sub_handle; h; h = h->next) 
        {
            /* If subscribe the same topic and callback function, then ignore */
            if (0 == mqtt_client_check_handle_is_identical(h, messagehandler)) 
            {
                /* if subscribe a identical topic and relate callback function, then ignore this subscribe */
                flag_dup = 1;
                mqtt_printf("There exists duplicate topic and related handle in list");

                if (fail_flag == 1) 
                {
                    mqtt_client_remove_handle_from_list(c, h);
                    mqtt_utils_Free((void *)h->topic_filter);
                    mqtt_utils_Free(h);
                }
                break;
            }
        }
        mqtt_utils_MutexUnlock(c->lock_generic);

        if (fail_flag == 0 && flag_dup == 0) 
        {
            mqtt_client_topic_handle_t *handle = mqtt_utils_Malloc(sizeof(mqtt_client_topic_handle_t));

            if (!handle) 
            {
                mqtt_utils_Free((void *)messagehandler[j].topic_filter);
                mqtt_utils_Free(messagehandler);
                return FAIL_RETURN;
            }

            memset(handle, 0, sizeof(mqtt_client_topic_handle_t));

            handle->topic_filter = messagehandler[j].topic_filter;
            handle->handle.h_fp = messagehandler[j].handle.h_fp;
            handle->handle.pcontext = messagehandler[j].handle.pcontext;
            handle->topic_type =  messagehandler[j].topic_type;

            mqtt_utils_MutexLock(c->lock_generic);
            handle->next = c->first_sub_handle;
            c->first_sub_handle = handle;
            mqtt_utils_MutexUnlock(c->lock_generic);
        } 
        else 
        {
            mqtt_utils_Free((void *)messagehandler[j].topic_filter);
        }
    }
    mqtt_utils_Free(messagehandler);

    msg.msg = (void *)(int)mypacketid;
    if (fail_flag == 1)
    {
        msg.event_type = MQTT_EVENT_SUBCRIBE_NACK;
    } 
    else
    {
        msg.event_type = MQTT_EVENT_SUBCRIBE_SUCCESS;
    }

    mqtt_client_event_handle_sub(c->handle_event.pcontext, c, &msg);

    if (NULL != c->handle_event.h_fp)
    {
        mqtt_client_handle_event(&c->handle_event, c, &msg);
    }

    return SUCCESS_RETURN;
}

static int mqtt_client_handle_recv_PUBLISH(mqtt_client_t *c)
{
    int result = 0;
    MQTTString topicName;
    mqtt_client_topic_info_t topic_msg;
    int qos = 0;
    uint32 payload_len = 0;

    if (!c) 
    {
        return FAIL_RETURN;
    }
    mqtt_printf("mqtt_client_handle_recv_PUBLISH start");

    memset(&topic_msg, 0x0, sizeof(mqtt_client_topic_info_t));
    memset(&topicName, 0x0, sizeof(MQTTString));

    if (1 != MQTTDeserialize_publish((unsigned char *)&topic_msg.dup,
                                    (int *)&qos,
                                    (unsigned char *)&topic_msg.retain,
                                    (unsigned short *)&topic_msg.packet_id,
                                    &topicName,
                                    (unsigned char **)&topic_msg.payload,
                                    (int *)&payload_len,
                                    (unsigned char *)c->buf_read,
                                    c->buf_size_read)) 
    {
        return MQTT_PUBLISH_PACKET_ERROR;
    }
    topic_msg.qos = (unsigned char)qos;
    topic_msg.payload_len = payload_len;

    mqtt_printf("%20s : %08d", "Packet Ident", topic_msg.packet_id);
    mqtt_printf("%20s : %d", "Topic Length", topicName.lenstring.len);
    mqtt_printf("%20s : %.*s",
                    "Topic Name",
                    topicName.lenstring.len,
                    topicName.lenstring.data);
    mqtt_printf("%20s : %d / %d", "Payload Len/Room",
                    topic_msg.payload_len,
                    c->buf_read + c->buf_size_read - topic_msg.payload);
    mqtt_printf("%20s : %d", "Receive Buflen", c->buf_size_read);

    topic_msg.ptopic = NULL;
    topic_msg.topic_len = 0;

    mqtt_printf("delivering msg ...");

    mqtt_printf("mqtt_client_handle_recv_PUBLISH topic_msg.qos=%d", topic_msg.qos);
    mqtt_client_deliver_message(c, &topicName, &topic_msg);

    if (topic_msg.qos == MQTT_CLIENT_QOS0) 
    {
        return SUCCESS_RETURN;
    } 
    else if (topic_msg.qos == MQTT_CLIENT_QOS1) 
    {
        result = mqtt_client_puback(c, topic_msg.packet_id, PUBACK);
    } 
    else if (topic_msg.qos == MQTT_CLIENT_QOS2) 
    {
        result = mqtt_client_puback(c, topic_msg.packet_id, PUBREC);
    } 
    else 
    {
        mqtt_printf("Invalid QOS, QOSvalue = %d", topic_msg.qos);
        return MQTT_PUBLISH_QOS_ERROR;
    }

    return result;
}

static int mqtt_client_handle_recv_PUBRECRELACK(mqtt_client_t *c, unsigned int packetType)
{
    int rc = 0;
    unsigned short mypacketid;
    unsigned char dup = 0;
    unsigned char type = 0;
    int len = 0;
    mqtt_time_t timer;
    
    if (!c) 
    {
        return FAIL_RETURN;
    }
    mqtt_printf("mqtt_client_handle_recv_PUBRECREL start");
    
    if (MQTTDeserialize_ack(&type, &dup, 
                            (unsigned short *)&mypacketid, 
                            (unsigned char *)c->buf_read, 
                            c->buf_size_read) != 1)
    {
        return  MQTT_PUBLISH_REC_PACKET_ERROR;
    }
    
    mqtt_utils_time_init(&timer);
    mqtt_utils_time_countdown_ms(&timer, c->request_timeout_ms);
    
    mqtt_utils_MutexLock(c->lock_write_buf);
    len = MQTTSerialize_ack((unsigned char *)c->buf_send, 
                            c->buf_size_send,
                            (packetType == PUBREC) ? PUBREL : PUBCOMP, 
                            0, 
                            mypacketid);
    if (len <= 0) 
    {
        mqtt_utils_MutexUnlock(c->lock_write_buf);
        return MQTT_PUBLISH_ACK_PACKET_ERROR;
    }

    rc = mqtt_client_send_packet(c, c->buf_send, len, &timer);
    if (rc != SUCCESS_RETURN) 
    {
        mqtt_utils_MutexUnlock(c->lock_write_buf);
        return MQTT_NETWORK_ERROR;
    }

    mqtt_utils_MutexUnlock(c->lock_write_buf);
    return SUCCESS_RETURN;
}

static int mqtt_client_handle_recv_UNSUBACK(mqtt_client_t *c)
{
    unsigned short mypacketid = 0;  
    mqtt_client_topic_handle_t *messageHandler = NULL;
    mqtt_client_topic_handle_t *h, *h_next;
    
    if (!c) 
    {
        return FAIL_RETURN;
    }

    if (MQTTDeserialize_unsuback(&mypacketid, (unsigned char *)c->buf_read, c->buf_size_read) != 1) 
    {
        return MQTT_UNSUBSCRIBE_ACK_PACKET_ERROR;
    }

    mqtt_utils_MutexLock(c->lock_list_sub);
    (void)mqtt_client_mask_subInfo_from(c, mypacketid, &messageHandler);
    mqtt_utils_MutexUnlock(c->lock_list_sub);

    if (NULL == messageHandler) 
    {
        return MQTT_SUB_INFO_NOT_FOUND_ERROR;
    }

    if (NULL == messageHandler->topic_filter) 
    {
        mqtt_utils_Free(messageHandler);
        return MQTT_SUB_INFO_NOT_FOUND_ERROR;
    }
    
    mqtt_utils_MutexLock(c->lock_generic);
    for (h = c->first_sub_handle; h != NULL; h = h_next) 
    {
        h_next = h->next;

        if (0 == mqtt_client_check_handle_is_identical_ex(h, messageHandler)) 
        {
            mqtt_client_remove_handle_from_list(c, h);
            if (h->topic_filter != NULL) 
            {
                mqtt_utils_Free((void *)h->topic_filter);
                h->topic_filter = NULL;
            }
            mqtt_utils_Free(h);
        }
    }

    if (NULL != c->handle_event.h_fp) 
    {
        mqtt_event_msg_t msg;
        
        msg.event_type = MQTT_EVENT_UNSUBCRIBE_SUCCESS;
        msg.msg = (void *)(int)mypacketid;
        mqtt_client_handle_event(&c->handle_event, c, &msg);
    }
    mqtt_utils_Free((void *)messageHandler->topic_filter);
    mqtt_utils_Free(messageHandler);

    mqtt_utils_MutexUnlock(c->lock_generic);
    return SUCCESS_RETURN;
}

static int mqtt_client_wait_CONNACK(mqtt_client_t *c)
{
    unsigned char wait_connack = 0;
    unsigned int packetType = 0;
    int rc = 0;
    mqtt_time_t timer;

    if (!c) 
    {
        return FAIL_RETURN;
    }

    mqtt_utils_time_init(&timer);
    mqtt_utils_time_countdown_ms(&timer, c->request_timeout_ms);

    do {
         /* read the socket, see what work is due */
        rc = mqtt_client_read_packet(c, &timer, &packetType);
        mqtt_printf("mqtt_client_wait_CONNACK,packetType = %d", packetType); 
        if (rc != SUCCESS_RETURN) 
        {
            mqtt_printf("readPacket error,result = %d", rc);
            return MQTT_NETWORK_ERROR;
        }

        if (++wait_connack > MQTT_WAIT_CONNACK_MAX) 
        {
            mqtt_printf("wait connack timeout");
            return MQTT_NETWORK_ERROR;
        }
    } while (packetType != CONNACK);
    
    mqtt_utils_MutexLock(c->lock_read_buf);
    rc = mqtt_client_handle_recv_CONNACK(c);
    mqtt_utils_MutexUnlock(c->lock_read_buf);
    if (SUCCESS_RETURN != rc) 
    {
        mqtt_printf("recvConnackProc error,result = %d", rc);
    }

    return rc;
}

static int mqtt_client_cycle(mqtt_client_t *c, mqtt_time_t *timer)
{
    unsigned int packetType;
    int rc = SUCCESS_RETURN;
    mqtt_client_state_t state = MQTT_CLIENT_STATE_INVALID;

    if (!c) 
    {
        return FAIL_RETURN;
    }
    mqtt_printf("mqtt_client_cycle start");

    state = mqtt_client_get_client_state(c);
    mqtt_printf("mqtt_client_cycle state=%d",state);
    if (state != MQTT_CLIENT_STATE_CONNECTED) 
    {
        mqtt_printf("state = %d", state);
        return MQTT_STATE_ERROR;
    }

    /* read the socket, see what work is due */
    rc = mqtt_client_read_packet(c, timer, &packetType);
    mqtt_printf("mqtt_client_cycle mqtt_client_read_packet rc=%d",rc);
    if (rc != SUCCESS_RETURN) 
    {
        mqtt_client_set_client_state(c, MQTT_CLIENT_STATE_DISCONNECTED);
        mqtt_printf("readPacket error,result = %d", rc);
        (void)mqtt_client_pubInfo_clear(c);
        return MQTT_NETWORK_ERROR;
    }
    mqtt_printf("mqtt_client_cycle  packetType=%d",packetType);
    if (0 == packetType) 
    {
        return SUCCESS_RETURN;
    }
    /* clear ping mark when any data received from MQTT broker */
    mqtt_utils_MutexLock(c->lock_generic);
    c->ping_mark = 0;
    c->keepalive_probes = 0;
    mqtt_utils_MutexUnlock(c->lock_generic);
    mqtt_utils_MutexLock(c->lock_read_buf);

    mqtt_printf("mqtt_client_cycle 11 packetType=%d",packetType);
    switch (packetType) 
    {
        case CONNACK: 
        {
            mqtt_printf("CONNACK");
            break;
        }

        case PUBACK:
        case PUBCOMP:    
        {
            mqtt_printf("PUBACK");
            rc = mqtt_client_handle_recv_PUBACK(c);
            if (SUCCESS_RETURN != rc) 
            {
                mqtt_printf("recvPubackProc error,result = %d", rc);
            }

            break;
        }

        case SUBACK: 
        {
            mqtt_printf("SUBACK");
            rc = mqtt_client_handle_recv_SUBACK(c);
            if (SUCCESS_RETURN != rc) 
            {
                mqtt_printf("recvSubAckProc error,result = %d", rc);
            }
            break;
        }

        case PUBLISH:
        {
            mqtt_printf("PUBLISH");
            rc = mqtt_client_handle_recv_PUBLISH(c);
            if (SUCCESS_RETURN != rc) 
            {
                mqtt_printf("recvPublishProc error,result = %d", rc);
            }
            break;
        }
        
        case UNSUBACK: 
        {
            mqtt_printf("UNSUBACK");
            rc = mqtt_client_handle_recv_UNSUBACK(c);
            if (SUCCESS_RETURN != rc) 
            {
                mqtt_printf("recvUnsubAckProc error,result = %d", rc);
            }
            break;
        }
        
        case PUBREC:
        case PUBREL:
        {
            mqtt_printf("PUBREC or PUBREL");
            rc = mqtt_client_handle_recv_PUBRECRELACK(c, packetType);
            if (SUCCESS_RETURN != rc) 
            {
                mqtt_printf("recvPublishProc error,result = %d", rc);
            }
            break;
        }
            
        case PINGRESP: 
        {
            rc = SUCCESS_RETURN;
            mqtt_printf("receive ping response!");
            break;
        }

        default:
            mqtt_printf("INVALID TYPE");
            mqtt_utils_MutexUnlock(c->lock_read_buf);
            return FAIL_RETURN;
    }
    
    mqtt_utils_MutexUnlock(c->lock_read_buf);
    return rc;
}

static int mqtt_client_disconnect(mqtt_client_t *pClient)
{
    int rc = -1;

    if (NULL == pClient) 
    {
        return NULL_VALUE_ERROR;
    }

    if (mqtt_client_is_connected(pClient)) 
    {
        rc = mqtt_client_disconnect_send_packet(pClient);
        mqtt_printf("rc = mqtt_client_disconnect_send_packet() = %d", rc);
    }

    /* close tcp/ip socket or free tls resources */
    pClient->network->disconnect(pClient->network);
    mqtt_client_set_client_state(pClient, MQTT_CLIENT_STATE_INITIALIZED);

    mqtt_printf("mqtt disconnect!");
    return SUCCESS_RETURN;
}

static void mqtt_client_sub_wait_ack_list_destroy(mqtt_client_t *pClient)
{
    mqtt_client_subsribe_info_t *node = NULL, *next_node = NULL;

    list_for_each_entry_safe(node, next_node, &pClient->list_sub_wait_ack, linked_list, mqtt_client_subsribe_info_t) 
    {
        list_del(&node->linked_list);
        if (node->handler != NULL) 
        {
            mqtt_utils_Free((void *)node->handler->topic_filter);
            mqtt_utils_Free((void *)node->handler);
        }
        mqtt_utils_Free((void *)node);
    }
}

static int mqtt_client_release(mqtt_client_t *pClient)
{
    if (NULL == pClient) 
    {
        return NULL_VALUE_ERROR;
    }
    /* iotx_delete_thread(pClient); */
    mqtt_utils_SleepMs(100);

    mqtt_client_disconnect(pClient);
    mqtt_client_set_client_state(pClient, MQTT_CLIENT_STATE_INVALID);
    mqtt_utils_SleepMs(100);

    if (pClient->first_sub_handle != NULL) 
    {
        mqtt_client_topic_handle_t *handler = pClient->first_sub_handle;
        mqtt_client_topic_handle_t *next_handler = pClient->first_sub_handle;

        while (handler) 
        {
            next_handler = handler->next;
            if (handler->topic_filter != NULL) 
            {
                mqtt_utils_Free((void *)handler->topic_filter);
                handler->topic_filter = NULL;
            }

            mqtt_utils_Free((void *)handler);
            handler = next_handler;
        }
    }
    
    mqtt_utils_MutexDestroy(pClient->lock_generic);
    mqtt_utils_MutexDestroy(pClient->lock_list_sub);
    mqtt_utils_MutexDestroy(pClient->lock_list_pub);
    mqtt_utils_MutexDestroy(pClient->lock_write_buf);
    mqtt_utils_MutexDestroy(pClient->lock_yield);
    mqtt_utils_MutexDestroy(pClient->lock_read_buf);

    mqtt_client_sub_wait_ack_list_destroy(pClient);
    mqtt_client_sub_wait_ack_list_destroy(pClient);
    if (pClient->buf_send != NULL) 
    {
        mqtt_utils_Free((void *)pClient->buf_send);
        pClient->buf_send = NULL;
    }
    if (pClient->buf_read != NULL) 
    {
        mqtt_utils_Free((void *)pClient->buf_read);
        pClient->buf_read = NULL;
    }
    if (NULL != pClient->network) 
    {
        mqtt_utils_Free((void *)pClient->network);
        pClient->network = NULL;
    }
    
    mqtt_printf("mqtt release!");
    return SUCCESS_RETURN;
}

static void mqtt_client_dnsReqCb(void* pData, uint32 len)
{
    ASYN_DNS_PARSE_T *dnsParse = (ASYN_DNS_PARSE_T *)pData;
    
    KING_SysLog("mqtt_client_dnsReqCb hostname=%s\r\n", dnsParse->hostname);
    KING_SysLog("mqtt_client_dnsReqCb dnsParse->result=%d\n", dnsParse->result);
    if (dnsParse->result == DNS_PARSE_SUCCESS)
    {
        hostAddr = dnsParse->addr;
        isMqttDnsSucc = 1;
    }
    else
    {
        KING_SysLog("mqtt_client_dnsReqCb fail!\n");
        isMqttDnsSucc = 0;
    }
    KING_SemPut(mqttSem);
}


static int mqtt_client_hostByName(softwareHandle hContext, const char *hostName)
{
    int ret = 0;

    if (isMqttDnsRunning == 1)
    {
        return -1;
    }
    if (NULL == mqttSem)
    {
        ret = KING_SemCreate("MQTT SEM", 0, (SEM_HANDLE *)&mqttSem);
        mqtt_printf("%s: KING_SemCreate() ret = %d\r\n", __FUNCTION__, ret);
        if (ret < 0 || mqttSem == NULL)
        {
            return -1;
        }
    }
    memset(&hostAddr, 0x00, sizeof(SOCK_IN_ADDR_T));
    ret = KING_NetGetHostByName(hContext, (char *)hostName, &hostAddr, 10000, mqtt_client_dnsReqCb);
    mqtt_printf("mqtt_client_hostByName() ret=%d", ret);
    if (ret != 0)
    {
        return -1;
    }
    
    isMqttDnsRunning = 1;
    KING_SemGet(mqttSem, WAIT_OPT_INFINITE);
    isMqttDnsRunning = 0;
    if (isMqttDnsSucc == 0)
    {
        return -1;
    }
    isMqttDnsSucc = 0;
    
    return 0;
}

static int mqtt_client_connect_init(mqtt_client_t *pClient, mqtt_client_param_t *pInitParams)
{
    int rc = FAIL_RETURN;
    mqtt_client_state_t mc_state = MQTT_CLIENT_STATE_INVALID;
    MQTTPacket_connectData connectdata = MQTTPacket_connectData_initializer;
    
    mqtt_printf("mqtt_client_connect_init start\n");

    if (pClient == NULL || pInitParams == NULL 
        || pInitParams->write_buf_size == 0 
        || pInitParams->read_buf_size == 0)
    {
        return NULL_VALUE_ERROR;
    }

    memset(pClient, 0x0, sizeof(mqtt_client_t));

    mqtt_utils_MutexCreate(&pClient->lock_generic);
    mqtt_printf("mqtt_client_connect_init pClient->lock_generic\n");
    if (!pClient->lock_generic) 
    {
        return FAIL_RETURN;
    }

    mqtt_utils_MutexCreate(&pClient->lock_list_sub);
    mqtt_printf("mqtt_client_connect_init pClient->lock_list_sub\n");
    if (!pClient->lock_list_sub) 
    {
        goto RETURN;
    }

    mqtt_utils_MutexCreate(&pClient->lock_list_pub);
    if (!pClient->lock_list_pub) 
    {
        goto RETURN;
    }

    mqtt_utils_MutexCreate(&pClient->lock_yield);
    if (!pClient->lock_yield) 
    {
        goto RETURN;
    }

    mqtt_utils_MutexCreate(&pClient->lock_write_buf);
    if (!pClient->lock_write_buf) 
    {
        goto RETURN;
    }

    mqtt_utils_MutexCreate(&pClient->lock_read_buf);
    if (!pClient->lock_read_buf) 
    {
        goto RETURN;
    }
    
    connectdata.MQTTVersion = 4;
    connectdata.keepAliveInterval = pInitParams->keepalive_interval_ms / 1000;

    connectdata.clientID.cstring = (char *)pInitParams->client_id;
    connectdata.username.cstring = (char *)pInitParams->username;
    connectdata.password.cstring = (char *)pInitParams->password;
    connectdata.cleansession = pInitParams->clean_session;
    connectdata.willFlag = pInitParams->willflag;
    connectdata.will.topicName.cstring = (char *)pInitParams->willtopic;
    connectdata.will.message.cstring = (char *)pInitParams->willmsg;
    connectdata.will.retained = pInitParams->willretain;
    
    pClient->reconn = pInitParams->reconn;
    pClient->sslMode = pInitParams->sslMode;
    mqtt_printf("mqtt_client_connect_init sslMode=%d\n", pInitParams->sslMode);
    pClient->request_timeout_ms = pInitParams->request_timeout_ms;
    mqtt_printf("mqtt_client_connect_init request_timeout_ms=%ld\n", pInitParams->request_timeout_ms);
    
    mqtt_printf("mqtt_client_connect_init write_buf_size=%d\n", pInitParams->write_buf_size);
    pClient->buf_send = mqtt_utils_Malloc(pInitParams->write_buf_size);
    if (pClient->buf_send == NULL) 
    {
        goto RETURN;
    }
    pClient->buf_size_send = pInitParams->write_buf_size;
    mqtt_printf("mqtt_client_connect_init buf_size_send=%d\n", pClient->buf_size_send);

    pClient->buf_read = mqtt_utils_Malloc(pInitParams->read_buf_size);
    if (pClient->buf_read == NULL) 
    {
        goto RETURN;
    }
    pClient->buf_size_read = pInitParams->read_buf_size;
    mqtt_printf("mqtt_client_connect_init buf_size_read=%d\n", pClient->buf_size_read);
    pClient->keepalive_probes = 0;

    pClient->handle_event.h_fp = pInitParams->handle_event.h_fp;
    pClient->handle_event.pcontext = pInitParams->handle_event.pcontext;

    /* Initialize reconnect parameter */
    pClient->reconnect_param.reconnect_time_interval_ms = MQTT_CLIENT_RECONNECT_INTERVAL_DEFAULT_MS;
    INIT_LIST_HEAD(&pClient->list_pub_wait_ack);
    INIT_LIST_HEAD(&pClient->list_sub_wait_ack);

    /* Initialize MQTT connect parameter */
    rc = mqtt_client_set_connect_params(pClient, &connectdata);
    if (SUCCESS_RETURN != rc) 
    {
        mc_state = MQTT_CLIENT_STATE_INVALID;
        goto RETURN;
    }

    mqtt_utils_time_init(&pClient->next_ping_time);
    mqtt_utils_time_init(&pClient->reconnect_param.reconnect_next_time);
    
    mqtt_printf("mqtt_client_connect_init hContext=%d", pInitParams->hContext);
    if (mqtt_client_hostByName(pInitParams->hContext, pInitParams->host) != 0)
    {
        mc_state = MQTT_CLIENT_STATE_INVALID;
        rc = FAIL_RETURN;
        goto RETURN;
    }
    pClient->network = (MQTT_NETWORK_S *)mqtt_utils_Malloc(sizeof(MQTT_NETWORK_S));
    if (NULL == pClient->network) 
    {
        mqtt_printf("allocate Network struct failed");
        rc = FAIL_RETURN;
        goto RETURN;
    }
    memset(pClient->network, 0x0, sizeof(MQTT_NETWORK_S));

    rc = mqtt_network_init(pClient->network, &hostAddr, pInitParams->port,
                            pInitParams->sslMode, pInitParams->ca_crt, pInitParams->client_crt,
                            pInitParams->client_key, pInitParams->client_pwd);
    mqtt_printf("mqtt_network_init rc = %d\n",rc);
    if (SUCCESS_RETURN != rc) 
    {
        mc_state = MQTT_CLIENT_STATE_INVALID;
        goto RETURN;
    }

    mc_state = MQTT_CLIENT_STATE_INITIALIZED;
    rc = SUCCESS_RETURN;
    mqtt_printf("MQTT init success!");

RETURN :
    mqtt_client_set_client_state(pClient, mc_state);
    if (rc != SUCCESS_RETURN) 
    {
        if (pClient->buf_send != NULL) 
        {
            mqtt_utils_Free(pClient->buf_send);
            pClient->buf_send = NULL;
        }
        if (pClient->buf_read != NULL) 
        {
            mqtt_utils_Free(pClient->buf_read);
            pClient->buf_read = NULL;
        }
        if (pClient->network) 
        {
            mqtt_utils_Free(pClient->network);
            pClient->network= NULL;
        }
        if (pClient->lock_list_sub) 
        {
            mqtt_utils_MutexDestroy(pClient->lock_list_sub);
            pClient->lock_list_sub = NULL;
        }
        if (pClient->lock_list_pub) 
        {
            mqtt_utils_MutexDestroy(pClient->lock_list_pub);
            pClient->lock_list_pub = NULL;
        }
        if (pClient->lock_write_buf) 
        {
            mqtt_utils_MutexDestroy(pClient->lock_write_buf);
            pClient->lock_write_buf = NULL;
        }
        if (pClient->lock_read_buf) 
        {
            mqtt_utils_MutexDestroy(pClient->lock_read_buf);
            pClient->lock_read_buf = NULL;
        }
        if (pClient->lock_yield) 
        {
            mqtt_utils_MutexDestroy(pClient->lock_yield);
            pClient->lock_yield = NULL;
        }
    }
    mqtt_printf("mqtt_client_connect_init end rc = %d\n",rc);

    return rc;
}

static int mqtt_client_connect_send_packet(mqtt_client_t *pClient)
{
    MQTTPacket_connectData *pConnectParams;
    mqtt_time_t connectTimer;
    int len = 0;

    if (!pClient) 
    {
        return FAIL_RETURN;
    }

    pConnectParams = &pClient->connect_data;
    mqtt_utils_MutexLock(pClient->lock_write_buf);

    if ((len = MQTTSerialize_connect((unsigned char *)pClient->buf_send, pClient->buf_size_send, pConnectParams)) <= 0) 
    {
        mqtt_printf("Serialize connect packet failed,len = %d", len);
        mqtt_utils_MutexUnlock(pClient->lock_write_buf);
        return MQTT_CONNECT_PACKET_ERROR;
    }

    /* send the connect packet */
    mqtt_utils_time_init(&connectTimer);
    mqtt_utils_time_countdown_ms(&connectTimer, pClient->request_timeout_ms);
    if ((mqtt_client_send_packet(pClient, pClient->buf_send, len, &connectTimer)) != SUCCESS_RETURN) 
    {
        mqtt_printf("send connect packet failed");
        mqtt_utils_MutexUnlock(pClient->lock_write_buf);
        return MQTT_NETWORK_ERROR;
    }
    mqtt_utils_MutexUnlock(pClient->lock_write_buf);
    
    return SUCCESS_RETURN;
}

static int mqtt_client_subscribe_send_packet
(
    mqtt_client_t *c, const char *topicFilter, 
    mqtt_client_qos_t qos, unsigned int msgId,
    mqtt_event_handle_func_fpt messageHandler, void *pcontext
)
{
    int len = 0;
    mqtt_time_t timer;
    MQTTString topic = MQTTString_initializer;
    mqtt_client_topic_handle_t *handler = NULL;
    mqtt_client_subsribe_info_t    *node = NULL;

    if (!c || !topicFilter || !messageHandler) 
    {
        return FAIL_RETURN;
    }
    
    if (!c->buf_send) 
    {
        return FAIL_RETURN;
    }

    topic.cstring = (char *)topicFilter;
    mqtt_utils_time_init(&timer);
    mqtt_utils_time_countdown_ms(&timer, c->request_timeout_ms);

    handler = mqtt_utils_Malloc(sizeof(mqtt_client_topic_handle_t));
    if (NULL == handler) 
    {
        return FAIL_RETURN;
    }

    handler->topic_filter = mqtt_utils_Malloc(strlen(topicFilter) + 1);
    if (NULL == handler->topic_filter) 
    {
        mqtt_utils_Free(handler);
        return FAIL_RETURN;
    }
    memcpy((char *)handler->topic_filter, topicFilter, strlen(topicFilter) + 1);
    handler->handle.h_fp = messageHandler;
    handler->handle.pcontext = pcontext;

    mqtt_utils_MutexLock(c->lock_write_buf);

    len = MQTTSerialize_subscribe((unsigned char *)c->buf_send, c->buf_size_send, 0, (unsigned short)msgId, 1, &topic,
                                    (int *)&qos);
    if (len <= 0) 
    {
        mqtt_utils_Free((void *)handler->topic_filter);
        mqtt_utils_Free(handler);
        mqtt_utils_MutexUnlock(c->lock_write_buf);
        return MQTT_SUBSCRIBE_PACKET_ERROR;
    }

    /* push the element to list of wait subscribe ACK */
    if (SUCCESS_RETURN != mqtt_client_push_subInfo_to(c, len, msgId, SUBSCRIBE, handler, &node)) 
    {
        mqtt_printf("push publish into to pubInfolist failed!");
        mqtt_utils_Free((void *)handler->topic_filter);
        mqtt_utils_Free(handler);
        mqtt_utils_MutexUnlock(c->lock_write_buf);
        return MQTT_PUSH_TO_LIST_ERROR;
    }

    mqtt_printf("%20s : %08d", "Packet Ident", msgId);
    mqtt_printf("%20s : %s", "Topic", topicFilter);
    mqtt_printf("%20s : %d", "QoS", (int)qos);
    mqtt_printf("%20s : %d", "Packet Length", len);

    if ((mqtt_client_send_packet(c, c->buf_send, len, &timer)) != SUCCESS_RETURN) 
    {
        mqtt_utils_MutexLock(c->lock_list_sub);
        list_del(&node->linked_list);
        mqtt_utils_Free(node);
        mqtt_utils_MutexUnlock(c->lock_list_sub);
        mqtt_printf("run sendPacket error!");
        mqtt_utils_Free((void *)handler->topic_filter);
        mqtt_utils_Free(handler);
        mqtt_utils_MutexUnlock(c->lock_write_buf);
        return MQTT_NETWORK_ERROR;
    }
    mqtt_utils_MutexUnlock(c->lock_write_buf);

    return SUCCESS_RETURN;
}

static int mqtt_client_unsubscribe_send_packet(mqtt_client_t *c, const char *topicFilter, unsigned int msgId)
{
    mqtt_time_t timer;
    MQTTString topic = MQTTString_initializer;
    int len = 0;
    mqtt_client_topic_handle_t *handler = NULL;
    mqtt_client_subsribe_info_t *node = NULL;
    MQTTString cur_topic;
    mqtt_client_topic_handle_t *h, *h2;

    if (!c || !topicFilter) 
    {
        return FAIL_RETURN;
    }

    topic.cstring = (char *)topicFilter;
    mqtt_utils_time_init(&timer);
    mqtt_utils_time_countdown_ms(&timer, c->request_timeout_ms);

    handler = mqtt_utils_Malloc(sizeof(mqtt_client_topic_handle_t));
    if (NULL == handler) 
    {
        return FAIL_RETURN;
    }

    handler->topic_filter = mqtt_utils_Malloc(strlen(topicFilter) + 1);
    if (NULL == handler->topic_filter) 
    {
        mqtt_utils_Free(handler);
        return FAIL_RETURN;
    }
    memcpy((char *)handler->topic_filter, topicFilter, strlen(topicFilter) + 1);

    mqtt_utils_MutexLock(c->lock_write_buf);

    if ((len = MQTTSerialize_unsubscribe((unsigned char *)c->buf_send, c->buf_size_send, 0, (unsigned short)msgId, 1,
                                        &topic)) <= 0) 
    {
        mqtt_utils_Free((void *)handler->topic_filter);
        mqtt_utils_Free(handler);
        mqtt_utils_MutexUnlock(c->lock_write_buf);
        return MQTT_UNSUBSCRIBE_PACKET_ERROR;
    }

    if (SUCCESS_RETURN != mqtt_client_push_subInfo_to(c, len, msgId, UNSUBSCRIBE, handler, &node)) 
    {
        mqtt_printf("push publish into to pubInfolist failed!");
        mqtt_utils_Free((void *)handler->topic_filter);
        mqtt_utils_Free(handler);
        mqtt_utils_MutexUnlock(c->lock_write_buf);
        return MQTT_PUSH_TO_LIST_ERROR;
    }

    if ((mqtt_client_send_packet(c, c->buf_send, len, &timer)) != SUCCESS_RETURN) 
    { 
        mqtt_utils_MutexLock(c->lock_list_sub);
        list_del(&node->linked_list);
        mqtt_utils_Free(node);
        mqtt_utils_MutexUnlock(c->lock_list_sub);
        mqtt_utils_Free((void *)handler->topic_filter);
        mqtt_utils_Free(handler);
        mqtt_utils_MutexUnlock(c->lock_write_buf);
        return MQTT_NETWORK_ERROR;
    }

    cur_topic.cstring = NULL;
    cur_topic.lenstring.data = (char *)handler->topic_filter;

    cur_topic.lenstring.len = strlen(handler->topic_filter) + 1;

    /* we have to find the right message handler - indexed by topic */
    mqtt_utils_MutexLock(c->lock_generic);
    for (h = c->first_sub_handle; h != NULL; h = h2) 
    {
        h2 = h->next;
        if (MQTTPacket_equals(&cur_topic, (char *)h->topic_filter)
            || mqtt_client_is_topic_matched((char *)h->topic_filter, &cur_topic)) 
        {
            mqtt_printf("topic be matched");
            mqtt_client_remove_handle_from_list(c, h);
            mqtt_utils_Free((void *)h->topic_filter);
            mqtt_utils_Free(h);
        }

    }
    mqtt_utils_MutexUnlock(c->lock_generic);
    mqtt_utils_MutexUnlock(c->lock_write_buf);
    return SUCCESS_RETURN;
}

static int mqtt_client_publish_send_packet(mqtt_client_t *c, const char *topicName, mqtt_client_topic_info_t *topic_msg)

{
    mqtt_time_t timer;
    MQTTString topic = MQTTString_initializer;
    int len = 0;
    mqtt_client_pub_info_t  *node = NULL;

    if (!c || !topicName || !topic_msg) 
    {
        return FAIL_RETURN;
    }

    topic.cstring = (char *)topicName;
    mqtt_utils_time_init(&timer);
    mqtt_utils_time_countdown_ms(&timer, c->request_timeout_ms);

    mqtt_utils_MutexLock(c->lock_list_pub);
    mqtt_utils_MutexLock(c->lock_write_buf);

    len = MQTTSerialize_publish((unsigned char *)c->buf_send,
                                c->buf_size_send,
                                0,
                                topic_msg->qos,
                                topic_msg->retain,
                                topic_msg->packet_id,
                                topic,
                                (unsigned char *)topic_msg->payload,
                                topic_msg->payload_len);
    if (len <= 0) 
    {
        mqtt_printf("MQTTSerialize_publish is error, len=%d, buf_size_send=%u, payloadlen=%u",
                        len,
                        c->buf_size_send,
                        topic_msg->payload_len);
        mqtt_utils_MutexUnlock(c->lock_write_buf);
        mqtt_utils_MutexUnlock(c->lock_list_pub);
        return MQTT_PUBLISH_PACKET_ERROR;
    }

    /* If the QOS >1, push the information into list of wait publish ACK */
    if (topic_msg->qos > MQTT_CLIENT_QOS0) 
    {
        if (SUCCESS_RETURN != mqtt_client_push_pubInfo_to(c, len, topic_msg->packet_id, &node)) 
        {
            mqtt_printf("push publish into to pubInfolist failed!");
            mqtt_utils_MutexUnlock(c->lock_write_buf);
            mqtt_utils_MutexUnlock(c->lock_list_pub);
            return MQTT_PUSH_TO_LIST_ERROR;
        }
    }
    /* send the publish packet */
    if (mqtt_client_send_packet(c, c->buf_send, len, &timer) != SUCCESS_RETURN) 
    {
        if (topic_msg->qos > MQTT_CLIENT_QOS0) 
        {
            /* If not even successfully sent to IP stack, meaningless to wait QOS1 ack, give up waiting */
            list_del(&node->linked_list);
            mqtt_utils_Free(node);
        }
        mqtt_utils_MutexUnlock(c->lock_write_buf);
        mqtt_utils_MutexUnlock(c->lock_list_pub);
        return MQTT_NETWORK_ERROR;
    }

    mqtt_utils_MutexUnlock(c->lock_write_buf);
    mqtt_utils_MutexUnlock(c->lock_list_pub);
    return SUCCESS_RETURN;
}

static int mqtt_client_keepalive_send_packet(mqtt_client_t *pClient)
{
    int len = 0;
    int rc = 0;
    mqtt_time_t timer;

    if (!pClient) 
    {
        return FAIL_RETURN;
    }

    mqtt_utils_time_init(&timer);
    mqtt_utils_time_countdown_ms(&timer, 1000);

    mqtt_utils_MutexLock(pClient->lock_write_buf);

    len = MQTTSerialize_pingreq((unsigned char *)pClient->buf_send, pClient->buf_size_send);
    mqtt_printf("len = MQTTSerialize_pingreq() = %d", len);
    if (len <= 0) 
    {
        mqtt_printf("Serialize ping request is error");
        mqtt_utils_MutexUnlock(pClient->lock_write_buf);
        return MQTT_PING_PACKET_ERROR;
    }

    rc = mqtt_client_send_packet(pClient, pClient->buf_send, len, &timer);
    if (SUCCESS_RETURN != rc) 
    {
        /* ping outstanding, then close socket unsubscribe topic and handle callback function */
        mqtt_printf("ping outstanding is error,result = %d", rc);

        mqtt_utils_MutexUnlock(pClient->lock_write_buf);
        return MQTT_NETWORK_ERROR;
    }
    mqtt_utils_MutexUnlock(pClient->lock_write_buf);
    return SUCCESS_RETURN;
}

static int mqtt_client_connection(mqtt_client_t *pClient)
{
    int rc = FAIL_RETURN;

    if (NULL == pClient) 
    {
        return NULL_VALUE_ERROR;
    }

    /* Establish TCP or TLS connection */
    rc = pClient->network->connect(pClient->network);
    mqtt_printf("mqtt_client_connection rc=%ld\n", rc);
    if (SUCCESS_RETURN != rc) 
    {
        pClient->network->disconnect(pClient->network);
        mqtt_printf("TCP or TLS Connection failed");

        if (ERROR_CERTIFICATE_EXPIRED == rc) 
        {
            mqtt_printf("certificate is expired!");
            return ERROR_CERT_VERIFY_FAIL;
        } 
        else 
        {
            return MQTT_NETWORK_CONNECT_ERROR;
        }
    }

    rc = mqtt_client_connect_send_packet(pClient);
    if (rc  != SUCCESS_RETURN) 
    {
        pClient->network->disconnect(pClient->network);
        mqtt_printf("send connect packet failed");
        return  rc;
    }

    if (SUCCESS_RETURN != mqtt_client_wait_CONNACK(pClient)) 
    {
        (void)mqtt_client_disconnect_send_packet(pClient);
        pClient->network->disconnect(pClient->network);
        mqtt_printf("wait connect ACK timeout, or receive a ACK indicating error!");
        return MQTT_CONNECT_ERROR;
    }

    mqtt_client_set_client_state(pClient, MQTT_CLIENT_STATE_CONNECTED);

    mqtt_utils_time_countdown_ms(&pClient->next_ping_time, pClient->connect_data.keepAliveInterval * 1000);

    mqtt_printf("mqtt connect success!");
    return SUCCESS_RETURN;
}

static int mqtt_client_sub(mqtt_client_t *c,
                                const char *topicFilter,
                                mqtt_client_qos_t qos,
                                mqtt_event_handle_func_fpt topic_handle_func,
                                void *pcontext)
{
    int rc = FAIL_RETURN;
    unsigned int msgId;

    if (NULL == c || NULL == topicFilter || !topic_handle_func) 
    {
        return NULL_VALUE_ERROR;
    }
    
    msgId = mqtt_client_get_next_packetid(c);
    if (!mqtt_client_is_connected(c)) 
    {
        mqtt_printf("mqtt client state is error,state = %d", mqtt_client_get_client_state(c));
        return MQTT_STATE_ERROR;
    }

    if (0 != mqtt_client_check_topic(topicFilter, TOPIC_FILTER_TYPE)) 
    {
        mqtt_printf("topic format is error,topicFilter = %s", topicFilter);
        return MQTT_TOPIC_FORMAT_ERROR;
    }

    mqtt_printf("PERFORM subscribe to '%s' (msgId=%d)", topicFilter, msgId);
    rc = mqtt_client_subscribe_send_packet(c, topicFilter, qos, msgId, topic_handle_func, pcontext);
    if (rc != SUCCESS_RETURN) 
    {
        if (rc == MQTT_NETWORK_ERROR) 
        {
            mqtt_client_set_client_state(c, MQTT_CLIENT_STATE_DISCONNECTED);
        }

        mqtt_printf("run MQTTSubscribe error, rc = %d", rc);
        return rc;
    }

    mqtt_printf("mqtt subscribe packet sent,topic = %s!", topicFilter);
    return msgId;
}

static int mqtt_client_unsub(mqtt_client_t *c, const char *topicFilter)
{
    int rc = FAIL_RETURN;
    unsigned int msgId = mqtt_client_get_next_packetid(c);

    if (NULL == c || NULL == topicFilter) 
    {
        return NULL_VALUE_ERROR;
    }

    if (0 != mqtt_client_check_topic(topicFilter, TOPIC_FILTER_TYPE)) 
    {
        mqtt_printf("topic format is error,topicFilter = %s", topicFilter);
        return MQTT_TOPIC_FORMAT_ERROR;
    }

    if (!mqtt_client_is_connected(c)) 
    {
        mqtt_printf("mqtt client state is error,state = %d", mqtt_client_get_client_state(c));
        return MQTT_STATE_ERROR;
    }

    rc = mqtt_client_unsubscribe_send_packet(c, topicFilter, msgId);
    if (rc != SUCCESS_RETURN) 
    {
        if (rc == MQTT_NETWORK_ERROR) 
        { 
            mqtt_client_set_client_state(c, MQTT_CLIENT_STATE_DISCONNECTED);
        }

        mqtt_printf("run MQTTUnsubscribe error!");
        return rc;
    }

    mqtt_printf("mqtt unsubscribe packet sent,topic = %s!", topicFilter);
    return (int)msgId;
}

static int mqtt_client_pub(mqtt_client_t *c, const char *topicName, mqtt_client_topic_info_t *topic_msg)
{
    uint16 msg_id = 0;
    int rc = FAIL_RETURN;

    if (NULL == c || NULL == topicName || topic_msg == NULL || topic_msg->payload == NULL)
    {
        return NULL_VALUE_ERROR;
    }

    if (0 != mqtt_client_check_topic(topicName, TOPIC_NAME_TYPE)) 
    {
        mqtt_printf("topic format is error,topicFilter = %s", topicName);
        return MQTT_TOPIC_FORMAT_ERROR;
    }

    if (!mqtt_client_is_connected(c)) 
    {
        mqtt_printf("mqtt client state is error,state = %d", mqtt_client_get_client_state(c));
        return MQTT_STATE_ERROR;
    }

    if (topic_msg->qos == MQTT_CLIENT_QOS1 || topic_msg->qos == MQTT_CLIENT_QOS2) 
    {
        msg_id = mqtt_client_get_next_packetid(c);
        topic_msg->packet_id = msg_id;
    }
    //if (topic_msg->qos == MQTT_CLIENT_QOS2) 
    //{
        //mqtt_printf("MQTTPublish return error,MQTT_QOS2 is now not supported.");
        //return MQTT_PUBLISH_QOS_ERROR;
    //}
    rc = mqtt_client_publish_send_packet(c, topicName, topic_msg);
    if (rc != SUCCESS_RETURN) 
    { 
        if (rc == MQTT_NETWORK_ERROR) 
        {
            mqtt_client_set_client_state(c, MQTT_CLIENT_STATE_DISCONNECTED);
        }
        mqtt_printf("MQTTPublish is error, rc = %d", rc);
        return rc;
    }

    return (int)msg_id;
}

static void mqtt_client_reconnect_callback(mqtt_client_t *pClient)
{
    if (NULL != pClient->handle_event.h_fp) 
    {
        mqtt_event_msg_t msg;

        msg.event_type = MQTT_EVENT_RECONNECT;
        msg.msg = NULL;

        pClient->handle_event.h_fp(pClient->handle_event.pcontext,
                                    pClient,
                                    &msg);
    }
}

static void mqtt_client_disconnect_callback(mqtt_client_t *pClient)
{
    if (NULL != pClient->handle_event.h_fp) 
    {
        mqtt_event_msg_t msg;
        msg.event_type = MQTT_EVENT_DISCONNECT;
        msg.msg = NULL;

        pClient->handle_event.h_fp(pClient->handle_event.pcontext,
                                    pClient,
                                    &msg);
    }
}

static int mqtt_client_keepalive_sub(mqtt_client_t *pClient)
{
    int rc = SUCCESS_RETURN;

    if (NULL == pClient) 
    {
        return NULL_VALUE_ERROR;
    }

    /* if in disabled state, without having to send ping packets */
    if (!mqtt_client_is_connected(pClient)) 
    {
        return SUCCESS_RETURN;
    }

    /* if there is no ping_timer timeout, then return success */
    if (!mqtt_utils_time_is_expired(&pClient->next_ping_time)) 
    {
        return SUCCESS_RETURN;
    }

    /* update to next time sending MQTT keep-alive */
    mqtt_utils_time_countdown_ms(&pClient->next_ping_time, pClient->connect_data.keepAliveInterval * 1000);

    rc = mqtt_client_keepalive_send_packet(pClient);
    if (SUCCESS_RETURN != rc) 
    {
        if (rc == MQTT_NETWORK_ERROR) 
        {
            mqtt_client_set_client_state(pClient, MQTT_CLIENT_STATE_DISCONNECTED);
        }
        mqtt_printf("ping outstanding is error,result = %d", rc);
        return rc;
    }

    mqtt_printf("send MQTT ping...");

    mqtt_utils_MutexLock(pClient->lock_generic);
    pClient->ping_mark = 1;
    pClient->keepalive_probes++;
    mqtt_utils_MutexUnlock(pClient->lock_generic);

    return SUCCESS_RETURN;
}

#if 0
static int mqtt_client_republish(mqtt_client_t *c, char *buf, int len)
{
    mqtt_time_t timer;

    mqtt_utils_time_init(&timer);
    mqtt_utils_time_countdown_ms(&timer, c->request_timeout_ms);

    mqtt_utils_MutexLock(c->lock_write_buf);

    if (mqtt_client_send_packet(c, buf, len, &timer) != SUCCESS_RETURN) 
    {
        mqtt_utils_MutexUnlock(c->lock_write_buf);
        return MQTT_NETWORK_ERROR;
    }

    mqtt_utils_MutexUnlock(c->lock_write_buf);
    return SUCCESS_RETURN;
}
#endif

static int mqtt_client_pub_info_proc(mqtt_client_t *pClient)
{
    //int rc = 0;
    mqtt_client_state_t state = MQTT_CLIENT_STATE_INVALID;
    mqtt_client_pub_info_t *node = NULL, *next_node = NULL;
    uint16 packet_id = 0;
    mqtt_event_msg_t msg;
    
    if (!pClient) 
    {
        return FAIL_RETURN;
    }
    mqtt_printf("mqtt_client_pub_info_proc start");

    mqtt_utils_MutexLock(pClient->lock_list_pub);
    list_for_each_entry_safe(node, next_node, &pClient->list_pub_wait_ack, linked_list, mqtt_client_pub_info_t) 
    {
        /* remove invalid node */
        if (MQTT_CLIENT_NODE_STATE_INVALID == node->node_state) 
        {
            list_del(&node->linked_list);
            mqtt_utils_Free(node);
            continue;
        }

        state = mqtt_client_get_client_state(pClient);
        if (state != MQTT_CLIENT_STATE_CONNECTED) 
        {
            continue;
        }

        /* check the request if timeout or not */
        if (mqtt_utils_time_spend(&node->pub_start_time) <= (pClient->request_timeout_ms * 2)) 
        {
            continue;
        }
        
        packet_id = node->msg_id;
        msg.event_type = MQTT_EVENT_PUBLISH_TIMEOUT;
        msg.msg = (void *)(int)packet_id;
        if (NULL != pClient->handle_event.h_fp) 
        {
            pClient->handle_event.h_fp(pClient->handle_event.pcontext, pClient, &msg);
        }

        list_del(&node->linked_list);
        mqtt_utils_Free(node);
    #if 0
        /* If wait ACK timeout, republish */
        rc = mqtt_client_republish(pClient, (char *)node->buf, node->len);
        mqtt_utils_time_start(&node->pub_start_time);

        if (MQTT_NETWORK_ERROR == rc) 
        {
            mqtt_client_set_client_state(pClient, MQTT_CLIENT_STATE_DISCONNECTED);
            break;
        }
    #endif
    }

    mqtt_utils_MutexUnlock(pClient->lock_list_pub);

    return SUCCESS_RETURN;
}

static int mqtt_client_sub_info_proc(mqtt_client_t *pClient)
{
    int rc = SUCCESS_RETURN;
    uint16 packet_id = 0;
    enum msgTypes msg_type;
    mqtt_client_topic_handle_t *messageHandler = NULL;
    mqtt_event_msg_t msg;
    mqtt_client_subsribe_info_t *node = NULL, *next_node = NULL;

    if (!pClient) 
    {
        return FAIL_RETURN;
    }

    mqtt_utils_MutexLock(pClient->lock_list_sub);
    list_for_each_entry_safe(node, next_node, &pClient->list_sub_wait_ack, linked_list, mqtt_client_subsribe_info_t) 
    {
        messageHandler = NULL;
        memset(&msg, 0, sizeof(mqtt_event_msg_t));

        /* remove invalid node */
        if (MQTT_CLIENT_NODE_STATE_INVALID == node->node_state) 
        {
            list_del(&node->linked_list);
            mqtt_utils_Free(node);
            continue;
        }

        if (mqtt_client_get_client_state(pClient) != MQTT_CLIENT_STATE_CONNECTED) 
        {
            continue;
        }

        /* check the request if timeout or not */
        if (mqtt_utils_time_spend(&node->sub_start_time) <= (pClient->request_timeout_ms * MQTT_CONFIG_SUBINFO_LIFE)) 
        {
            /* continue to check the next node */
            continue;
        }

        /* When arrive here, it means timeout to wait ACK */
        packet_id = node->msg_id;
        msg_type = node->type;

        (void)mqtt_client_mask_subInfo_from(pClient, packet_id, &messageHandler);

        if (SUBSCRIBE == msg_type) 
        {
            msg.event_type = MQTT_EVENT_SUBCRIBE_TIMEOUT;
            msg.msg = (void *)(int)packet_id;
            mqtt_client_event_handle_sub(pClient->handle_event.pcontext, pClient, &msg);
        } 
        else 
        {
            msg.event_type = MQTT_EVENT_UNSUBCRIBE_TIMEOUT;
            msg.msg = (void *)(int)packet_id;
        }

        if (NULL != pClient->handle_event.h_fp) 
        {
            pClient->handle_event.h_fp(pClient->handle_event.pcontext, pClient, &msg);
        }

        if (messageHandler) 
        {
            mqtt_utils_Free((void *)messageHandler->topic_filter);
            mqtt_utils_Free(messageHandler);
        }

        list_del(&node->linked_list);
        mqtt_utils_Free(node);
    }

    mqtt_utils_MutexUnlock(pClient->lock_list_sub);
    return rc;
}

int mqtt_client_attempt_reconnect(mqtt_client_t *pClient)
{
    int rc;

    if (pClient == NULL)
    {
        return NULL_VALUE_ERROR;
    }

    pClient->network->disconnect(pClient->network);

    mqtt_printf("reconnect params: MQTTVersion=%d, clientID=%s, keepAliveInterval=%d, username=%s",
                    pClient->connect_data.MQTTVersion,
                    pClient->connect_data.clientID.cstring,
                    pClient->connect_data.keepAliveInterval,
                    pClient->connect_data.username.cstring);

    /* Ignoring return code. failures expected if network is disconnected */
    rc = mqtt_client_connection(pClient);
    if (SUCCESS_RETURN != rc) 
    {
        mqtt_printf("run mqtt_client_connection() error!");
        return rc;
    }

    return SUCCESS_RETURN;
}

int mqtt_client_handle_reconnect(mqtt_client_t *pClient)
{
    int             rc = FAIL_RETURN;
    uint32        interval_ms = 0;

    if (NULL == pClient) 
    {
        return NULL_VALUE_ERROR;
    }
    mqtt_printf("Waiting to reconnect...");
    if (!mqtt_utils_time_is_expired(&(pClient->reconnect_param.reconnect_next_time))) 
    {
        mqtt_utils_SleepMs(100);
        return FAIL_RETURN;
    }

    mqtt_printf("start to reconnect");
    rc = mqtt_client_attempt_reconnect(pClient);
    if (SUCCESS_RETURN == rc) 
    {
        mqtt_client_set_client_state(pClient, MQTT_CLIENT_STATE_CONNECTED);
        return SUCCESS_RETURN;
    } 
    else 
    {
        pClient->reconnect_param.reconnect_time_interval_ms = MQTT_CLIENT_RECONNECT_INTERVAL_DEFAULT_MS;
    }

    interval_ms = pClient->reconnect_param.reconnect_time_interval_ms;
    interval_ms += mqtt_utils_Random(pClient->reconnect_param.reconnect_time_interval_ms);
    if (MQTT_CLIENT_RECONNECT_INTERVAL_MAX_MS < interval_ms) 
    {
        interval_ms = MQTT_CLIENT_RECONNECT_INTERVAL_MAX_MS;
    }
    mqtt_utils_time_countdown_ms(&(pClient->reconnect_param.reconnect_next_time), interval_ms);

    mqtt_printf("mqtt reconnect failed rc = %d", rc);

    return rc;
}

static void mqtt_client_keepalive(mqtt_client_t *pClient)
{
    int rc = 0;
    mqtt_client_state_t currentState = MQTT_CLIENT_STATE_INVALID;

    if (!pClient) 
    {
        return;
    }

    mqtt_client_keepalive_sub(pClient);

    currentState = mqtt_client_get_client_state(pClient);
    do {
        if (MQTT_CLIENT_STATE_DISCONNECTED_RECONNECTING == currentState) 
        {
            /* Reconnection is successful, Resume regularly ping packets */
            mqtt_utils_MutexLock(pClient->lock_generic);
            pClient->ping_mark = 0;
            mqtt_utils_MutexUnlock(pClient->lock_generic);
            rc = mqtt_client_handle_reconnect(pClient);
            if (SUCCESS_RETURN != rc) 
            {
                mqtt_printf("reconnect network fail, rc = %d", rc);
            } 
            else 
            {
                mqtt_printf("network is reconnected!");
                mqtt_client_reconnect_callback(pClient);
                pClient->reconnect_param.reconnect_time_interval_ms = MQTT_CLIENT_RECONNECT_INTERVAL_DEFAULT_MS;
            }

            break;
        }

        if (MQTT_CLIENT_STATE_DISCONNECTED == currentState) 
        {
            mqtt_printf("network is disconnected!");
            mqtt_client_disconnect_callback(pClient);

            pClient->reconnect_param.reconnect_time_interval_ms = MQTT_CLIENT_RECONNECT_INTERVAL_DEFAULT_MS;
            mqtt_utils_time_countdown_ms(&(pClient->reconnect_param.reconnect_next_time),
                                        pClient->reconnect_param.reconnect_time_interval_ms);

            pClient->network->disconnect(pClient->network);
            mqtt_client_set_client_state(pClient, MQTT_CLIENT_STATE_DISCONNECTED_RECONNECTING);
            break;
        }
    } while (0);
}

void mqtt_client_DisconnectByNetwork(mqtt_client_t *pClient)
{
    mqtt_client_set_client_state(pClient, MQTT_CLIENT_STATE_DISCONNECTED);
}

int mqtt_client_is_connected(mqtt_client_t *pClient)
{
    if (pClient == NULL)
    {
        return 0;
    }
    if (mqtt_client_get_client_state(pClient) == MQTT_CLIENT_STATE_CONNECTED)
    {
        return 1;
    }

    return 0;
}

mqtt_client_t *mqtt_client_connect(mqtt_client_param_t *pInitParams)
{
    int err;
    mqtt_client_t *pclient = NULL;
    
    mqtt_printf("mqtt_client_connect start\n");
    if (pInitParams->host == NULL || pInitParams->port == 0 
        || !strlen(pInitParams->host)) 
    {
        mqtt_printf("init params is not complete");
        return NULL;
    }

    pclient = (mqtt_client_t *)malloc(sizeof(mqtt_client_t));
    if (NULL == pclient) 
    {
        mqtt_printf("not enough memory.");
        return NULL;
    }

    err = mqtt_client_connect_init(pclient, pInitParams);
    mqtt_printf("mqtt_client_connect mqtt_client_connect_init err=%d\n", err);
    if (SUCCESS_RETURN != err) 
    {
        mqtt_utils_Free(pclient);
        return NULL;
    }

    err = mqtt_client_connection(pclient);
    mqtt_printf("mqtt_client_connect mqtt_client_connection err=%d\n", err);
    if (SUCCESS_RETURN != err) 
    {
        mqtt_printf("mqtt_client_connect failed");
        mqtt_client_release(pclient);
        mqtt_utils_Free(pclient);
        return NULL;
    }

    mqtt_printf("mqtt_client_connect end err=%d\n", err);
    return pclient;
}

int mqtt_client_yield(mqtt_client_t *pClient, int timeout_ms)
{
    int                 rc = SUCCESS_RETURN;
    mqtt_time_t         time;

    if (pClient == NULL)
    {
        return -1;
    }
    if (timeout_ms < 0) 
    {
        mqtt_printf("Invalid argument, timeout_ms = %d", timeout_ms);
        return -1;
    }
    if (timeout_ms == 0) 
    {
        timeout_ms = 10;
    }

    mqtt_utils_time_init(&time);
    mqtt_utils_time_countdown_ms(&time, timeout_ms);
    do {
        if (SUCCESS_RETURN != rc) 
        {
            mqtt_printf("error occur rc=%d", rc);
        }

        mqtt_utils_MutexLock(pClient->lock_yield);

        rc = mqtt_client_cycle(pClient, &time);
        mqtt_printf("mqtt_client_yield mqtt_client_cycle rc=%d", rc);
        if (SUCCESS_RETURN == rc) 
        {
            mqtt_client_pub_info_proc(pClient);
            mqtt_client_sub_info_proc(pClient);
        }
        
        mqtt_client_keepalive(pClient);
        mqtt_utils_MutexUnlock(pClient->lock_yield);
        {
            unsigned int left_t = mqtt_utils_time_left(&time);

            if (left_t < 10) 
            {
                mqtt_utils_SleepMs(left_t);
            } 
            else 
            {
                mqtt_utils_SleepMs(10);
            }
        }
    } while (!mqtt_utils_time_is_expired(&time)&& (SUCCESS_RETURN == rc));

    return 0;
}

int mqtt_client_destroy(mqtt_client_t **ppClient)
{
    mqtt_client_t *client = NULL;
    
    if (ppClient != NULL) 
    {
        client = *ppClient;
        *ppClient = NULL;
    } 

    if (client == NULL)
    {
        return NULL_VALUE_ERROR;
    }
    mqtt_client_release(client);
    mqtt_utils_Free(client);

    return SUCCESS_RETURN;
}

int mqtt_client_subscribe(mqtt_client_t *pClient,
                                const char *topic_filter,
                                mqtt_client_qos_t qos,
                                mqtt_event_handle_func_fpt topic_handle_func,
                                void *pcontext)
{
    if (pClient == NULL) 
    { 
        return -1;
    }
    if (topic_handle_func == NULL)
    {
        return -1;
    }
    
    if (topic_filter == NULL || strlen(topic_filter) == 0)
    {
        return -1;
    }

    if (qos > MQTT_CLIENT_QOS2) 
    {
        return -1;
    }

    return mqtt_client_sub(pClient, topic_filter, qos, topic_handle_func, pcontext);
}

int mqtt_client_unsubscribe(mqtt_client_t *pClient, const char *topic_filter)
{
    if (pClient == NULL) 
    { 
        return -1;
    }
    
    if (topic_filter == NULL || strlen(topic_filter) == 0)
    {
        return -1;
    }
    return mqtt_client_unsub(pClient, topic_filter);
}

int mqtt_client_publish(mqtt_client_t *pClient, const char *topic_name, mqtt_client_topic_info_t *topic_msg)
{
    int rc = -1;

    if (pClient == NULL) 
    { 
        return -1;
    }

    if (topic_name == NULL || strlen(topic_name) == 0 || topic_msg == NULL || topic_msg->payload == NULL)
    {
        return -1;
    }

    rc = mqtt_client_pub(pClient, topic_name, topic_msg);
    return rc;
}

