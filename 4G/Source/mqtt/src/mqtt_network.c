/*******************************************************************************
 ** File Name:   mqtt_network.c
 ** Author:      Allance.Chen
 ** Date:        2020-06-04
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains ds functions about MQTT network.
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
 #include "mqtt_network.h"

/*******************************************************************************
  ** TCP connection
  ******************************************************************************/
int mqtt_network_read_tcp(MQTT_NETWORK_S *mqtt_network, char *buffer, uint32 len, uint32 timeout_ms)
{
    return mqtt_tcp_read(mqtt_network->handle, buffer, len, timeout_ms);
}
 
static int mqtt_network_write_tcp(MQTT_NETWORK_S *mqtt_network, const char *buffer, uint32 len, uint32 timeout_ms)
{
    return mqtt_tcp_write(mqtt_network->handle, buffer, len, timeout_ms);
}
 
static int mqtt_network_disconnect_tcp(MQTT_NETWORK_S *mqtt_network)
{
    if (mqtt_network->handle == (int)(-1)) 
    {
        return -1;
    }

    mqtt_tcp_destroy(mqtt_network->handle);
    mqtt_network->handle = -1;
    return 0;
}
 
static int mqtt_network_connect_tcp(MQTT_NETWORK_S *mqtt_network)
{
    if (NULL == mqtt_network) 
    {
        network_printf("network is null");
        return -1;
    }

    mqtt_network->handle = mqtt_tcp_establish(mqtt_network->pHostAddr, mqtt_network->port);
    if (mqtt_network->handle == (int)(-1)) 
    {
        return -1;
    }

    return 0;
}
 
/*******************************************************************************
   ** SSL connection
   ******************************************************************************/
#ifdef KING_SDK_MBEDTLS_SUPPORT
static int mqtt_network_read_ssl(MQTT_NETWORK_S *mqtt_network, char *buffer, uint32 len, uint32 timeout_ms)
{
    if (NULL == mqtt_network) 
    {
        network_printf("network is null");
        return -1;
    }

    return mqtt_ssl_read((int)mqtt_network->handle, buffer, len, timeout_ms);
}
 
static int mqtt_network_write_ssl(MQTT_NETWORK_S *mqtt_network, const char *buffer, uint32 len, uint32 timeout_ms)
{
    if (NULL == mqtt_network) 
    {
        network_printf("network is null");
        return -1;
    }

    return mqtt_ssl_write((int)mqtt_network->handle, buffer, len, timeout_ms);
}
 
static int mqtt_network_disconnect_ssl(MQTT_NETWORK_S *mqtt_network)
{
    if (NULL == mqtt_network) 
    {
        network_printf("network is null");
        return -1;
    }

    mqtt_ssl_destroy((int)mqtt_network->handle);
    mqtt_network->handle = 0;

    return 0;
}
 
static int mqtt_network_connect_ssl(MQTT_NETWORK_S *mqtt_network)
{
    if (NULL == mqtt_network) 
    {
        network_printf("network is null");
        return -1;
    }
    
    network_printf("mqtt_network_connect_ssl port=%d!", mqtt_network->port);
    network_printf("mqtt_network_connect_ssl ca_crt_len=%d!", mqtt_network->ca_crt_len);
    if (0 != (mqtt_network->handle = (int)mqtt_ssl_establish(
                                            mqtt_network->pHostAddr,
                                            mqtt_network->port,
                                            mqtt_network->ca_crt,
                                            mqtt_network->ca_crt_len + 1,
                                            mqtt_network->client_crt,
                                            mqtt_network->client_crt_len + 1,
                                            mqtt_network->client_key,
                                            mqtt_network->client_key_len + 1,
                                            mqtt_network->client_pwd,
                                            mqtt_network->client_pwd_len + 1
                                            ))) 
    {
        return 0;
    } 
    return -1;
}
#endif
/*******************************************************************************
 ** network interface
 ******************************************************************************/
 int mqtt_network_read(MQTT_NETWORK_S *mqtt_network, char *buffer, uint32 len, uint32 timeout_ms)
 {
    int ret = 0;

    if (mqtt_network->sslMode == 1) 
    {
#ifdef KING_SDK_MBEDTLS_SUPPORT
        ret = mqtt_network_read_ssl(mqtt_network, buffer, len, timeout_ms);
#else
        network_printf("not support tls");
        ret = -1;
#endif
    }
    else if (mqtt_network->sslMode == 0)
    {
        ret = mqtt_network_read_tcp(mqtt_network, buffer, len, timeout_ms);
    }
    else 
    {
        ret = -1;
        network_printf("no method match!");
    }

    return ret;
 }
 
 int mqtt_network_write(MQTT_NETWORK_S *mqtt_network, const char *buffer, uint32 len, uint32 timeout_ms)
 {
    int ret = 0;

    if (mqtt_network->sslMode == 1) 
    {
#ifdef KING_SDK_MBEDTLS_SUPPORT
        ret = mqtt_network_write_ssl(mqtt_network, buffer, len, timeout_ms);
#else
        network_printf("not support tls");
        ret = -1;
#endif
    }
    else if (mqtt_network->sslMode == 0)
    {
        ret = mqtt_network_write_tcp(mqtt_network, buffer, len, timeout_ms);
    }
    else 
    {
        ret = -1;
        network_printf("no method match!");
    }
 
    return ret;
 }
 
 int mqtt_network_disconnect(MQTT_NETWORK_S *mqtt_network)
 {
     int ret = 0;
     
     if (mqtt_network->sslMode == 1) 
     {
#ifdef KING_SDK_MBEDTLS_SUPPORT
         ret = mqtt_network_disconnect_ssl(mqtt_network);
#else
         network_printf("not support tls");
         ret = -1;
#endif
     }
     else if (mqtt_network->sslMode == 0)
     {
         ret = mqtt_network_disconnect_tcp(mqtt_network);
     }
     else 
     {
         ret = -1;
         network_printf("no method match!");
     }
     
     return ret;
 }
 
 int mqtt_network_connect(MQTT_NETWORK_S *mqtt_network)
 {
     int ret = 0;
     
     if (mqtt_network->sslMode == 1) 
     {
#ifdef KING_SDK_MBEDTLS_SUPPORT
         ret = mqtt_network_connect_ssl(mqtt_network);
#else
          network_printf("not support tls");
          ret = -1;
#endif

     }
     else if (mqtt_network->sslMode == 0)
     {
         ret = mqtt_network_connect_tcp(mqtt_network);
     }
     else 
     {
         ret = -1;
         network_printf("no method match!");
     }
     
     return ret;
 }
 
int mqtt_network_init
(
    MQTT_NETWORK_S *mqtt_network, 
    SOCK_IN_ADDR_T *pHostAddr, 
    uint16 port, 
    uint8 sslMode,
    const char *ca_crt, 
    const char *client_crt, 
    const char *client_key, 
    const char *client_pwd
)
{
    if (mqtt_network == NULL || pHostAddr == NULL) 
    {
        network_printf("parameter error!");
        return -1;
    }
    network_printf("mqtt_network_init port=%d!", port);
    network_printf("mqtt_network_init sslMode=%d!", sslMode);
    mqtt_network->pHostAddr = pHostAddr;
    mqtt_network->port = port;
    mqtt_network->sslMode = sslMode;
    mqtt_network->ca_crt = ca_crt;
    mqtt_network->client_crt = client_crt;
    mqtt_network->client_key = client_key;
    mqtt_network->client_pwd = client_pwd;

    if (NULL == ca_crt) 
    {
        mqtt_network->ca_crt_len = 0;
    } 
    else 
    {
        mqtt_network->ca_crt_len = strlen(ca_crt);
    }
    
    if (NULL == client_crt) 
    {
        mqtt_network->client_crt_len = 0;
    } 
    else 
    {
        mqtt_network->client_crt_len = strlen(client_crt);
    }
    if (NULL == client_key) 
    {
        mqtt_network->client_key_len = 0;
    } 
    else 
    {
        mqtt_network->client_key_len = strlen(client_key);
    }  
    if (NULL == client_pwd) 
    {
        mqtt_network->client_pwd_len = 0;
    } 
    else 
    {
        mqtt_network->client_pwd_len = strlen(client_pwd);
    }
    
    mqtt_network->handle = 0;
    mqtt_network->read = mqtt_network_read;
    mqtt_network->write = mqtt_network_write;
    mqtt_network->disconnect = mqtt_network_disconnect;
    mqtt_network->connect = mqtt_network_connect;

    return 0;
}
 
