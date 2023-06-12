#include "kingDef.h"
#include "infra_defs.h"
#include "infra_string.h"
#include "infra_sha256.h"
#include "dev_sign_api.h"
#include "mqtt_utils.h"
#include "mqtt_client.h"

#define SIGN_MQTT_PORT (1883)

/* all secure mode define */
#define MODE_TLS_DIRECT             "2"
#define MODE_TCP_DIRECT_PLAIN       "3"

#define SECURE_TLS_MODE             MODE_TLS_DIRECT

/* use fixed timestamp */
#define TIMESTAMP_VALUE             "2524608000000"

/* clientid key value pair define */
const char *clientid_kv[][2] = {
    {
        "timestamp", TIMESTAMP_VALUE
    },
    {
        "securemode", MODE_TCP_DIRECT_PLAIN
    },
    {
        "signmethod", "hmacsha256"
    },
#ifdef DEVICE_MODEL_ENABLE
    {
        "v", IOTX_ALINK_VERSION
    },
#else
    {
        "gw", "0"
    },
    {
        "ext", "0"
    },
#endif
    {
        "ver", "c-sdk-"IOTX_SDK_VERSION
    },
};

static int _sign_get_clientid(char *clientid_string, char *device_id, uint8 is_tls)
{
    uint8 i;

    if (clientid_string == NULL || clientid_kv == NULL) {
        return FAIL_RETURN;
    }
    if (is_tls == 1)
    {
       clientid_kv[1][1] =  MODE_TLS_DIRECT;
    }
    else
    {
       clientid_kv[1][1] = MODE_TCP_DIRECT_PLAIN;  
    }
    memset(clientid_string, 0, DEV_SIGN_CLIENT_ID_MAXLEN + 1);
    memcpy(clientid_string, device_id, strlen(device_id));
    memcpy(clientid_string + strlen(clientid_string), "|", 1);

    for (i = 0; i < (sizeof(clientid_kv) / (sizeof(clientid_kv[0]))); i++) {
        if ((strlen(clientid_string) + strlen(clientid_kv[i][0]) + strlen(clientid_kv[i][1]) + 2) >
            DEV_SIGN_CLIENT_ID_MAXLEN) {
            return FAIL_RETURN;
        }

        memcpy(clientid_string + strlen(clientid_string), clientid_kv[i][0], strlen(clientid_kv[i][0]));
        memcpy(clientid_string + strlen(clientid_string), "=", 1);
        memcpy(clientid_string + strlen(clientid_string), clientid_kv[i][1], strlen(clientid_kv[i][1]));
        memcpy(clientid_string + strlen(clientid_string), ",", 1);
    }

    memcpy(clientid_string + strlen(clientid_string) - 1, "|", 1);
    mqtt_info("mqtt clientid=%s", clientid_string);

    return SUCCESS_RETURN;
}

int32 IOT_Sign_MQTT(iotx_mqtt_region_types_t region, iotx_dev_meta_info_t *meta, iotx_sign_mqtt_t *signout)
{
    uint16 length = 0;
    char device_id[IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 1] = {0};
    char signsource[DEV_SIGN_SOURCE_MAXLEN] = {0};
    uint16 signsource_len = 0, port = 0;
    uint8 sign[32] = {0};
    const char *sign_fmt = "clientId%sdeviceName%sproductKey%stimestamp%s";
    char hostname[DEV_SIGN_HOSTNAME_MAXLEN + 1] = {0};
    char username[DEV_SIGN_USERNAME_MAXLEN + 1] = {0};
    char password[DEV_SIGN_PASSWORD_MAXLEN + 1] = {0};
    char clientid[DEV_SIGN_CLIENT_ID_MAXLEN + 1] = {0};
    
    if (region >= IOTX_MQTT_DOMAIN_NUMBER || meta == NULL) 
    {
        return -1;
    }

    memset(signout, 0, sizeof(iotx_sign_mqtt_t));

    memcpy(device_id, meta->product_key, strlen(meta->product_key));
    memcpy(device_id + strlen(device_id), ".", strlen("."));
    memcpy(device_id + strlen(device_id), meta->device_name, strlen(meta->device_name));

    signsource_len = strlen(sign_fmt) + strlen(device_id) + strlen(meta->device_name) + strlen(meta->product_key) + strlen(
                                 TIMESTAMP_VALUE) + 1;
    if (signsource_len >= DEV_SIGN_SOURCE_MAXLEN) 
    {
        return ERROR_DEV_SIGN_SOURCE_TOO_SHORT;
    }
    memset(signsource, 0, DEV_SIGN_SOURCE_MAXLEN);
    memcpy(signsource, "clientId", strlen("clientId"));
    memcpy(signsource + strlen(signsource), device_id, strlen(device_id));
    memcpy(signsource + strlen(signsource), "deviceName", strlen("deviceName"));
    memcpy(signsource + strlen(signsource), meta->device_name, strlen(meta->device_name));
    memcpy(signsource + strlen(signsource), "productKey", strlen("productKey"));
    memcpy(signsource + strlen(signsource), meta->product_key, strlen(meta->product_key));
    memcpy(signsource + strlen(signsource), "timestamp", strlen("timestamp"));
    memcpy(signsource + strlen(signsource), TIMESTAMP_VALUE, strlen(TIMESTAMP_VALUE));

    utils_hmac_sha256((uint8 *)signsource, strlen(signsource), (uint8 *)meta->device_secret,
                      strlen(meta->device_secret), sign);

    /* Get Sign Information For MQTT */
    mqtt_utils_GetHostName(hostname);
    if (strlen(hostname) > 0)
    {
        length = strlen(hostname);
    }
    else
    {
        length = strlen(meta->product_key) + strlen(g_infra_mqtt_domain[region]) + 2;
    }
    if (length > DEV_SIGN_HOSTNAME_MAXLEN) 
    {
        return ERROR_DEV_SIGN_HOST_NAME_TOO_SHORT;
    }

    memset(signout->hostname, 0, DEV_SIGN_HOSTNAME_MAXLEN + 1);
    if (strlen(hostname) > 0)
    {
        memcpy(signout->hostname, hostname, strlen(hostname));
    }
    else
    {
        memcpy(signout->hostname, meta->product_key, strlen(meta->product_key));
        memcpy(signout->hostname + strlen(signout->hostname), ".", strlen("."));
        memcpy(signout->hostname + strlen(signout->hostname), g_infra_mqtt_domain[region],
               strlen(g_infra_mqtt_domain[region]));
    }

    mqtt_utils_GetUserName(username);
    if (strlen(username) > 0)
    {
        length = strlen(username);
    }
    else
    {
        length = strlen(meta->device_name) + strlen(meta->product_key) + 2;
    }
    if (length > DEV_SIGN_USERNAME_MAXLEN) 
    {
        return ERROR_DEV_SIGN_USERNAME_TOO_SHORT;
    }

    //check if username has been configured
    memset(signout->username, 0x00, DEV_SIGN_USERNAME_MAXLEN + 1);
    if (strlen(username) > 0)
    {
        memcpy(signout->username, username, strlen(username));
    }
    else
    {
        memcpy(signout->username, meta->device_name, strlen(meta->device_name));
        memcpy(signout->username + strlen(signout->username), "&", strlen("&"));
        memcpy(signout->username + strlen(signout->username), meta->product_key, strlen(meta->product_key));
    }

    //check if password has been configured
    memset(signout->password, 0x00, DEV_SIGN_PASSWORD_MAXLEN + 1);
    mqtt_utils_GetPassword(password);
    if (strlen(password) > 0)
    {
        memcpy(signout->password, password, strlen(password));
    }
    else
    {
        infra_hex2str(sign, 32, signout->password);
    }
    
    //check if clientid has been configured
    memset(signout->clientid, 0x00, DEV_SIGN_CLIENT_ID_MAXLEN + 1);
    mqtt_utils_GetClientID(clientid);
    if (strlen(clientid) > 0)
    {
        memcpy(signout->clientid, clientid, strlen(clientid));
    }
    else
    {
        if (_sign_get_clientid(signout->clientid, device_id, meta->is_tls) != SUCCESS_RETURN) 
        {
            return ERROR_DEV_SIGN_CLIENT_ID_TOO_SHORT;
        }
    }
    
    port = mqtt_utils_GetPort();
    if (port == 0)
    {
        if (meta->is_tls == 1)
        {
            signout->port = 443;
        }
        else
        {
            signout->port = 1883;
        }
    }
    else
    {
        signout->port = port;
    }
    
    return SUCCESS_RETURN;
}

