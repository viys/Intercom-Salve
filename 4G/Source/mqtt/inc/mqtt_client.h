/*******************************************************************************
 ** File Name:   mqtt_client.h
 ** Author:      Allance.Chen
 ** Date:        2020-06-04
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains ds functions about mqtt client.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-06-04     Allance.Chen         Create.
 ******************************************************************************/
#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "lite-list.h"
#include "compat_aos_list.h"

#include "mqtt_utils.h"
#include "mqtt_network.h"
#include "MQTTPacket.h"
#include "MQTTConnect.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define mqtt_printf(fmt, args...)    do { KING_SysLog("mqtt: "fmt, ##args); } while(0)

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef enum MQTT_RETURN_CODES {
    ERROR_DEVICE_NOT_EXSIT = -311,
    ERROR_NET_TIMEOUT = -310,
    ERROR_CERT_VERIFY_FAIL  = -309,
    ERROR_NET_SETOPT_TIMEOUT = -308,
    ERROR_NET_SOCKET = -307,
    ERROR_NET_CONNECT = -306,
    ERROR_NET_BIND = -305,
    ERROR_NET_LISTEN = -304,
    ERROR_NET_RECV = -303,
    ERROR_NET_SEND = -302,
    ERROR_NET_CONN = -301,
    ERROR_NET_UNKNOWN_HOST = -300,

    MQTT_SUB_INFO_NOT_FOUND_ERROR = -43,
    MQTT_PUSH_TO_LIST_ERROR = -42,
    MQTT_TOPIC_FORMAT_ERROR = -41,
    NETWORK_RECONNECT_TIMED_OUT_ERROR = -40,/** Returned when the Network is disconnected and the reconnect attempt has timed out */
    MQTT_CONNACK_UNKNOWN_ERROR = -39,/** Connect request failed with the server returning an unknown error */
    MQTT_CONANCK_UNACCEPTABLE_PROTOCOL_VERSION_ERROR = -38,/** Connect request failed with the server returning an unacceptable protocol version error */
    MQTT_CONNACK_IDENTIFIER_REJECTED_ERROR = -37,/** Connect request failed with the server returning an identifier rejected error */
    MQTT_CONNACK_SERVER_UNAVAILABLE_ERROR = -36,/** Connect request failed with the server returning an unavailable error */
    MQTT_CONNACK_BAD_USERDATA_ERROR = -35,/** Connect request failed with the server returning a bad userdata error */
    MQTT_CONNACK_NOT_AUTHORIZED_ERROR = -34,/** Connect request failed with the server failing to authenticate the request */
    MQTT_CONNECT_ERROR = -33,
    MQTT_CREATE_THREAD_ERROR = -32,
    MQTT_PING_PACKET_ERROR = -31,
    MQTT_CONNECT_PACKET_ERROR = -30,
    MQTT_CONNECT_ACK_PACKET_ERROR = -29,
    MQTT_NETWORK_CONNECT_ERROR = -28,
    MQTT_STATE_ERROR = -27,
    MQTT_SUBSCRIBE_PACKET_ERROR = -26,
    MQTT_SUBSCRIBE_ACK_PACKET_ERROR = -25,
    MQTT_SUBSCRIBE_ACK_FAILURE = -24,
    MQTT_SUBSCRIBE_QOS_ERROR = -23,
    MQTT_UNSUBSCRIBE_PACKET_ERROR = -22,
    MQTT_PUBLISH_PACKET_ERROR = -21,
    MQTT_PUBLISH_QOS_ERROR = -20,
    MQTT_PUBLISH_ACK_PACKET_ERROR = -19,
    MQTT_PUBLISH_COMP_PACKET_ERROR = -18,
    MQTT_PUBLISH_REC_PACKET_ERROR = -17,
    MQTT_PUBLISH_REL_PACKET_ERROR = -16,
    MQTT_UNSUBSCRIBE_ACK_PACKET_ERROR = -15,
    MQTT_NETWORK_ERROR = -14,
    MQTT_PUBLISH_ACK_TYPE_ERROR = -13,

    ERROR_NO_MEM = -1016,
    ERROR_CERTIFICATE_EXPIRED = -1015,
    ERROR_MALLOC = -1014,
    ERROR_NO_ENOUGH_MEM = -1013,               /**< Writes more than size value. */
    /**
         * @brief dev_sign
         *
         * -1100 ~ -1200
         *
         */
    ERROR_DEV_SIGN_SOURCE_TOO_SHORT = -1104,
    ERROR_DEV_SIGN_PASSWORD_TOO_SHORT = -1103,
    ERROR_DEV_SIGN_USERNAME_TOO_SHORT = -1102,
    ERROR_DEV_SIGN_CLIENT_ID_TOO_SHORT = -1101,
    ERROR_DEV_SIGN_HOST_NAME_TOO_SHORT = -1100,
    
    NULL_VALUE_ERROR = -2,

    FAIL_RETURN = -1,                        /**< generic error. */
    SUCCESS_RETURN = 0,


    /* @value > 0, reserved for other usage */

} mqtt_err_t;

typedef enum {
    /* Undefined event */
    MQTT_EVENT_UNDEF = 0,

    /* MQTT disconnect event */
    MQTT_EVENT_DISCONNECT = 1,

    /* MQTT reconnect event */
    MQTT_EVENT_RECONNECT = 2,

    /* A ACK to the specific subscribe which specify by packet-id be received */
    MQTT_EVENT_SUBCRIBE_SUCCESS = 3,

    /* No ACK to the specific subscribe which specify by packet-id be received in timeout period */
    MQTT_EVENT_SUBCRIBE_TIMEOUT = 4,

    /* A failed ACK to the specific subscribe which specify by packet-id be received*/
    MQTT_EVENT_SUBCRIBE_NACK = 5,

    /* A ACK to the specific unsubscribe which specify by packet-id be received */
    MQTT_EVENT_UNSUBCRIBE_SUCCESS = 6,

    /* No ACK to the specific unsubscribe which specify by packet-id be received in timeout period */
    MQTT_EVENT_UNSUBCRIBE_TIMEOUT = 7,

    /* A failed ACK to the specific unsubscribe which specify by packet-id be received*/
    MQTT_EVENT_UNSUBCRIBE_NACK = 8,

    /* A ACK to the specific publish which specify by packet-id be received */
    MQTT_EVENT_PUBLISH_SUCCESS = 9,

    /* No ACK to the specific publish which specify by packet-id be received in timeout period */
    MQTT_EVENT_PUBLISH_TIMEOUT = 10,

    /* A failed ACK to the specific publish which specify by packet-id be received*/
    MQTT_EVENT_PUBLISH_NACK = 11,

    /* MQTT packet published from MQTT remote broker be received */
    MQTT_EVENT_PUBLISH_RECEIVED = 12,

    /* MQTT packet buffer overflow which the remaining space less than to receive byte */
    MQTT_EVENT_BUFFER_OVERFLOW = 13,
} mqtt_event_type_t;

typedef enum {
    TOPIC_NAME_TYPE = 0,
    TOPIC_FILTER_TYPE
} mqtt_client_topic_type_t;

/* State of MQTT client */
typedef enum {
    MQTT_CLIENT_STATE_INVALID = 0,                    /* MQTT in invalid state */
    MQTT_CLIENT_STATE_INITIALIZED = 1,                /* MQTT in initializing state */
    MQTT_CLIENT_STATE_CONNECTED = 2,                  /* MQTT in connected state */
    MQTT_CLIENT_STATE_DISCONNECTED = 3,               /* MQTT in disconnected state */
    MQTT_CLIENT_STATE_DISCONNECTED_RECONNECTING = 4,  /* MQTT in reconnecting state */
} mqtt_client_state_t;

typedef enum MQTT_NODE_STATE {
    MQTT_CLIENT_NODE_STATE_NORMANL = 0,
    MQTT_CLIENT_NODE_STATE_INVALID,
} mqtt_client_node_t;

typedef enum {
    MQTT_CLIENT_QOS0 = 0,
    MQTT_CLIENT_QOS1,
    MQTT_CLIENT_QOS2
} mqtt_client_qos_t;

/* Reconnected parameter of MQTT client */
typedef struct {
    mqtt_time_t         reconnect_next_time;        /* the next time point of reconnect */
    uint32            reconnect_time_interval_ms; /* time interval of this reconnect */
} mqtt_client_reconnect_param_t;

typedef struct {
    mqtt_event_type_t  event_type;
    void *msg;
} mqtt_event_msg_t;

typedef void (*mqtt_event_handle_func_fpt)(void *pcontext, void *pclient, mqtt_event_msg_t *msg);

/* The structure of MQTT event handle */
typedef struct {
    mqtt_event_handle_func_fpt h_fp;
    void *pcontext;
} mqtt_event_handle_t;

typedef struct {
    uint16        packet_id;
    uint8         qos;
    uint8         dup;
    uint8         retain;
    uint16        topic_len;
    uint32        payload_len;
    const char    *ptopic;
    const char    *payload;
} mqtt_client_topic_info_t;

typedef struct REPUBLISH_INFO {
    mqtt_time_t               pub_start_time;     /* start time of publish request */
    mqtt_client_node_t        node_state;         /* state of this node */
    uint16                    msg_id;             /* packet id of publish */
    uint32                    len;                /* length of publish message */
    unsigned char             *buf;                /* publish message */
    struct list_head          linked_list;
} mqtt_client_pub_info_t;

typedef struct mqtt_client_topic_handle_s {
    const char *topic_filter;
    mqtt_client_topic_type_t topic_type;
    mqtt_event_handle_t handle;
    struct mqtt_client_topic_handle_s *next;
} mqtt_client_topic_handle_t;

typedef struct SUBSCRIBE_INFO {
    enum msgTypes               type;               /* type, (sub or unsub) */
    uint16                    msg_id;             /* packet id of subscribe(unsubcribe) */
    mqtt_time_t                 sub_start_time;     /* start time of subscribe request */
    mqtt_client_node_t              node_state;         /* state of this node */
    mqtt_client_topic_handle_t     *handler;            /* handle of topic subscribed(unsubcribed) */
    uint16                    len;                /* length of subscribe message */
    unsigned char              *buf;                /* subscribe message */
    struct list_head            linked_list;
} mqtt_client_subsribe_info_t;

/* The structure of MQTT initial parameter */
typedef struct {
    softwareHandle hContext;
    uint8 sslMode;
    uint8 reconn;
    uint16 port;                   /* Specify MQTT broker port */
    const char *host;                   /* Specify MQTT broker host */
    const char *client_id;
    const char *username;               /* Specify MQTT user name */
    const char *password;               /* Specify MQTT password */
    const char *ca_crt;
    const char *client_crt;
    const char *client_key;
    const char *client_pwd;
    const char *pub_key;
    const char *willtopic;
    const char *willmsg;
    uint8 willretain;
    uint8 willflag;
    uint8  clean_session;
    uint32  keepalive_interval_ms;
    uint32 request_timeout_ms;       /* Specify timeout of a MQTT request in millisecond */
    uint32 write_buf_size;           /* Specify size of write-buffer in byte */
    uint32 read_buf_size;            /* Specify size of read-buffer in byte */
    mqtt_event_handle_t handle_event;             /* Specify MQTT event handle */
} mqtt_client_param_t;

typedef struct {
    uint8 sslMode;
    uint8 reconn;
    void *lock_generic;                               /* generic lock */
    uint32 packet_id;                                  /* packet id */
    uint32 request_timeout_ms;                         /* request timeout in millisecond */
    uint32 buf_size_send;                              /* send buffer size in byte */
    uint32 buf_size_read;                              /* read buffer size in byte */
    uint8 keepalive_probes;                           /* keepalive probes */
    char *buf_send;                                   /* pointer of send buffer */
    char *buf_read;                                   /* pointer of read buffer */
    mqtt_client_topic_handle_t *first_sub_handle;                           /* list of subscribe handle */
    MQTT_NETWORK_S *network;                                    /* network parameter */
    mqtt_time_t next_ping_time;                             /* next ping time */
    int ping_mark;                                  /* flag of ping */
    mqtt_client_state_t client_state;                               /* state of MQTT client */
    mqtt_client_reconnect_param_t reconnect_param;                            /* reconnect parameter */
    MQTTPacket_connectData connect_data;                               /* connection parameter */
    struct list_head list_pub_wait_ack;                          /* list of wait publish ack */
    struct list_head list_sub_wait_ack;                          /* list of subscribe or unsubscribe ack */
    void *lock_list_pub;                              /* lock for list of QoS1 pub */
    void *lock_list_sub;                              /* lock for list of sub/unsub */
    void *lock_write_buf;                             /* lock of write */
    void *lock_read_buf;                             /* lock of write */
    void *lock_yield;
    mqtt_event_handle_t handle_event;                               /* event handle */
} mqtt_client_t;

/*******************************************************************************
 ** Function 
 ******************************************************************************/
void mqtt_client_DisconnectByNetwork(mqtt_client_t *pClient);
int mqtt_client_is_connected(mqtt_client_t *pClient);
mqtt_client_t *mqtt_client_connect(mqtt_client_param_t *pInitParams);
int mqtt_client_yield(mqtt_client_t *pClient, int timeout_ms);
int mqtt_client_destroy(mqtt_client_t **ppClient);
int mqtt_client_subscribe(mqtt_client_t *pClient,
                                const char *topic_filter,
                                mqtt_client_qos_t qos,
                                mqtt_event_handle_func_fpt topic_handle_func,
                                void *pcontext);
int mqtt_client_unsubscribe(mqtt_client_t *pClient, const char *topic_filter);
int mqtt_client_publish(mqtt_client_t *pClient, const char *topic_name, mqtt_client_topic_info_t *topic_msg);
#endif/*_MQTT_CLIENT_H__*/
