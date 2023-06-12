#ifndef _INFRA_DEFS_H_
#define _INFRA_DEFS_H_

#include "kingdef.h"

#define IOTX_SDK_VERSION                "3.0.0"
#define IOTX_ALINK_VERSION              "20"
#define IOTX_FIRMWARE_VERSION_LEN       (32)
#define IOTX_PRODUCT_KEY_LEN            (20)
#define IOTX_DEVICE_NAME_LEN            (32)
#define IOTX_DEVICE_SECRET_LEN          (64)
#define IOTX_PRODUCT_SECRET_LEN         (64)
#define IOTX_PARTNER_ID_LEN             (64)
#define IOTX_MODULE_ID_LEN              (64)
#define IOTX_NETWORK_IF_LEN             (160)
#define IOTX_FIRMWARE_VER_LEN           (32)
#define IOTX_URI_MAX_LEN                (135)

typedef struct _iotx_dev_meta_info {
    uint8 is_tls;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1];
    char product_secret[IOTX_PRODUCT_SECRET_LEN + 1];
    char device_name[IOTX_DEVICE_NAME_LEN + 1];
    char device_secret[IOTX_DEVICE_SECRET_LEN + 1];
} iotx_dev_meta_info_t;

typedef struct {
    const char *region;
    uint16 port;
} iotx_region_item_t;

typedef enum {
    IOTX_CLOUD_REGION_SHANGHAI,   /* Shanghai */
    IOTX_CLOUD_REGION_SINGAPORE,  /* Singapore */
    IOTX_CLOUD_REGION_JAPAN,      /* Japan */
    IOTX_CLOUD_REGION_USA_WEST,   /* America */
    IOTX_CLOUD_REGION_GERMANY,    /* Germany */
    IOTX_CLOUD_REGION_CUSTOM,     /* Custom setting */
    IOTX_CLOUD_DOMAIN_MAX         /* Maximum number of domain */
} iotx_mqtt_region_types_t;

#define IOTX_MQTT_DOMAIN_NUMBER (5)
extern const char * g_infra_mqtt_domain[IOTX_MQTT_DOMAIN_NUMBER];

typedef enum {
    IOTX_HTTP_REGION_SHANGHAI,   /* Shanghai */
    IOTX_HTTP_REGION_SINGAPORE,  /* Singapore */
    IOTX_HTTP_REGION_JAPAN,      /* Japan */
    IOTX_HTTP_REGION_AMERICA,    /* America */
    IOTX_HTTP_REGION_GERMANY,    /* Germany */
    IOTX_HTTP_REGION_MAX         /* Maximum number of domain */
} iotx_http_region_types_t;

#define IOTX_HTTP_DOMAIN_NUMBER (5)
extern const char *g_infra_http_domain[IOTX_HTTP_DOMAIN_NUMBER];

#endif

