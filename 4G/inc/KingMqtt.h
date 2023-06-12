#ifndef __KING_MQTT_H__
#define __KING_MQTT_H__

#include "KingDef.h"

typedef void* MQTT_HANDLE;

/**
 * @ingroup mqtt
 * qos value */
typedef enum
{
  MQTT_QOS_0,
  MQTT_QOS_1,
  MQTT_QOS_2,
  MQTT_QOS_NUM
} MQTT_QOS;


typedef enum
{
  MQTT_TYPE_TCP,
  MQTT_TYPE_TLS,
  MQTT_TYPE_NUM
} MQTT_TYPE;

/**
 * MQTT client connection states
 */
typedef enum {
  MQTT_DISCONNECT,
  MQTT_CONNECTED,
  MQTT_ERROR     //this state means CZ_MqttState param err or not support this operation
}MQTT_CONN_STATE;


/**
 * @ingroup mqtt
 * Connection status codes
 * async from server while connecting*/
typedef enum
{
  /** Accepted */
  MQTT_CONNECT_ACCEPTED                 = 0,
  /** Refused protocol version */
  MQTT_CONNECT_REFUSED_PROTOCOL_VERSION = 1,
  /** Refused identifier */
  MQTT_CONNECT_REFUSED_IDENTIFIER       = 2,
  /** Refused server */
  MQTT_CONNECT_REFUSED_SERVER           = 3,
  /** Refused user credentials */
  MQTT_CONNECT_REFUSED_USERNAME_PASS    = 4,
  /** Refused not authorized */
  MQTT_CONNECT_REFUSED_NOT_AUTHORIZED_  = 5,
  /** Disconnected */
  MQTT_CONNECT_DISCONNECTED             = 256,
  /** Timeout */
  MQTT_CONNECT_TIMEOUT                  = 257,

  /**Receive data format err*/
  MQTT_REC_DATA_FORMAT_ERR               = 258
} MQTT_NORMAL_STATUS;


/**
 * @ingroup mqtt
 * Function prototype for mqtt request callback. Called when a subscribe, unsubscribe
 * or publish request has completed
 * @param arg Pointer to user data supplied when invoking request
 * @param err ERR_OK on success
 *            ERR_TIMEOUT if no response was received within timeout,
 *            ERR_ABRT if (un)subscribe was denied
 */
typedef void (*MQTT_ERR_CB)(void *arg, int err);


/**
 * @ingroup mqtt
 * Function prototype for mqtt connection status callback. Called when
 * client has connected to the server after initiating a mqtt connection attempt by
 * calling mqtt_connect() or when connection is closed by server or an error
 *
 * @param client MQTT client itself
 * @param arg Additional argument to pass to the callback function
 * @param status Connect result code or disconnection notification @see MQTT_NORMAL_STATUS
 *
 */
typedef void (*MQTT_CONN_CB)(MQTT_HANDLE mqtt_handle, void *arg, MQTT_NORMAL_STATUS status);


/**
 * @ingroup mqtt
 * Data callback flags */

/** Flag set when last fragment of data arrives in data callback */
#define MQTT_DATA_FLAG_LAST     1

/**
 * @ingroup mqtt
 * Function prototype for MQTT incoming publish data callback function. Called when data
 * arrives to a subscribed topic @see mqtt_subscribe
 *
 * @param arg Additional argument to pass to the callback function
 * @param data User data, pointed object, data may not be referenced after callback return,
          NULL is passed when all publish data are delivered
 * @param len Length of publish data fragment
 * @param flags MQTT_DATA_FLAG_LAST set when this call contains the last part of data from publish message
 *
 */
typedef void (*MQTT_IN_DATA_CB)(void *arg, const uint8 *data, uint16 len, uint8 flags);


/**
 * @ingroup mqtt
 * Function prototype for MQTT incoming publish function. Called when an incoming publish
 * arrives to a subscribed topic @see mqtt_subscribe
 *
 * @param arg Additional argument to pass to the callback function
 * @param topic Zero terminated Topic text string, topic may not be referenced after callback return
 * @param tot_len Total length of publish data, if set to 0 (no publish payload) data callback will not be invoked
 */
typedef void (*MQTT_IN_PUB_CB)(void *arg, const char *topic, uint32 tot_len);


/**
 * @ingroup mqtt
 * Function prototype for mqtt request callback. Called when a subscribe, unsubscribe
 * or publish request has completed
 * @param arg Pointer to user data supplied when invoking request
 * @param err ERR_OK on success
 *            ERR_TIMEOUT if no response was received within timeout,
 *            ERR_ABRT if (un)subscribe was denied
 */
typedef void (*MQTT_REQ_CB)(void *arg, int err);

typedef struct mqtt_conn_param_t
{
    const char *hostname;
    uint16 port;
    /** Client identifier, must be set by caller */
    const char *client_id;
    /** User name and password, set to NULL if not used */
    const char *client_user;
    const char *client_pass;
    /** keep alive time in seconds, 0 to disable keep alive functionality*/
    uint16 keep_alive;
    /** will topic, set to NULL if will is not to be used,
        will_msg, will_qos and will retain are then ignored */
    const char* will_topic;
    const char* will_msg;
    uint8 will_qos;
    uint8 will_retain;
    uint8 clean_session;
    uint16 connect_timeout;
    MQTT_TYPE type;
} MQTT_CONN_PARAM;

/**
 * MQTT Init
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_MqttInit(void);

int KING_MqttClientNew(MQTT_HANDLE* mqtt_handle);

int KING_MqttSetParam(MQTT_HANDLE mqtt_handle, MQTT_CONN_PARAM *param);

int KING_MqttSetCert(MQTT_HANDLE mqtt_handle, const char *ca_pem, const char *cert_pem, const char *pk_key, const char* pwd);

int KING_MqttConnect(MQTT_HANDLE mqtt_handle, MQTT_CONN_CB cb, void *arg);

int KING_MqttDisonnect(MQTT_HANDLE mqtt_handle);

int KING_MqttPublish(MQTT_HANDLE mqtt_handle, const char *topic, const void *payload, uint16 payload_length,
                     uint8 dup, MQTT_QOS qos, uint8 retain, MQTT_REQ_CB cb, void *arg);

int KING_MqttSub(MQTT_HANDLE mqtt_handle, const char *topic, MQTT_QOS qos, MQTT_REQ_CB cb, void *arg);

int KING_MqttUnsub(MQTT_HANDLE mqtt_handle, const char *topic, MQTT_REQ_CB cb, void *arg);

int KING_MqttState(MQTT_HANDLE mqtt_handle, MQTT_CONN_STATE *state);

int KING_MqttSetInpubCb(MQTT_HANDLE mqtt_handle, MQTT_IN_PUB_CB pub_cb, MQTT_IN_DATA_CB data_cb, void *arg);

int KING_MqttClientDestroy(MQTT_HANDLE mqtt_handle);

/**
 * MQTT DeInit
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_MqttDeinit(void);

#endif // __KING_MQTT_H__

