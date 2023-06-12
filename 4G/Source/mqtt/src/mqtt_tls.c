/*******************************************************************************
 ** File Name:   mqtt_tls.c
 ** Author:      Allance.Chen
 ** Date:        2020-06-05
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains ds functions about mqtt tls.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-06-05     Allance.Chen         Create.
 ******************************************************************************/
 /*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "net.h"
#include "debug.h"
#include "ssl.h"
#include "entropy.h"
#include "ctr_drbg.h"
#include "error.h"
#include "certs.h"
#include "platform.h"

#include "kingsocket.h"
#include "kingCbData.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define SEND_TIMEOUT_SECONDS                (10)
#define GUIDER_ONLINE_HOSTNAME              ("iot-auth.cn-shanghai.aliyuncs.com")
#define GUIDER_PRE_ADDRESS                  ("100.67.80.107")

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef struct _TLSDataParams {
    mbedtls_ssl_context ssl;          /**< mbed TLS control context. */
    mbedtls_net_context fd;           /**< mbed TLS network context. */
    mbedtls_ssl_config conf;          /**< mbed TLS configuration context. */
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_x509_crt cacertl;         /**< mbed TLS CA certification. */
    mbedtls_x509_crt clicert;         /**< mbed TLS Client certification. */
    mbedtls_pk_context pkey;          /**< mbed TLS Client key. */
} TLSDataParams_t, *TLSDataParams_pt;

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static int isMqttTlsSucc = 0;
static SEM_HANDLE mqttTlsSem = NULL;
static int isMqttTlsRunning = 0;

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
static int mqtt_ssl_dummy_rng(void *ctx, unsigned char *buffer , size_t len)
{
    static size_t ctr;
    size_t i;

    for (i = 0; i < len; i++) 
    {
        buffer[i] = (unsigned char)++ctr;
    }
    return 0;
}

static void mqtt_ssl_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    char *p_file_name = NULL;
    ((void) level);

    p_file_name = strrchr(file,'/'); 
    if(p_file_name == NULL)
    {
        p_file_name = (char *)file;
    }
    else
    {
        p_file_name++;
    }
    KING_SysLog( "mbed TLS: %s:%04d: %s",  p_file_name, line, str );
}

static int mqtt_ssl_client_init(TLSDataParams_t *pTlsData)
{
    int ret = -1;
    
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold((int)2);
#endif

    mbedtls_net_init(&(pTlsData->fd));
    mbedtls_ssl_init(&(pTlsData->ssl));
    mbedtls_ssl_config_init(&(pTlsData->conf));
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_init(&(pTlsData->cacertl));
    mbedtls_x509_crt_init(&(pTlsData->clicert));
    mbedtls_pk_init(&(pTlsData->pkey));
#endif    

    mbedtls_entropy_init( &(pTlsData->entropy) );
    KING_SysLog("mbed TLS: p_handle_ptr->entropy.source_count:%d", pTlsData->entropy.source_count);
    if((ret = mbedtls_ctr_drbg_seed( &(pTlsData->ctr_drbg), mqtt_ssl_dummy_rng,
        &(pTlsData->entropy), NULL, 0)) != 0)
    {
        KING_SysLog("mbed TLS: failed --- mbedtls_ctr_drbg_seed returned %d", ret);
        ret = -1;
        return ret;
    }

    /* Setup default config */
    KING_SysLog("mbed TLS: Setting up the SSL/TLS structure...");
    if(( ret = mbedtls_ssl_config_defaults( &(pTlsData->conf),
                                            MBEDTLS_SSL_IS_CLIENT,
                                            MBEDTLS_SSL_TRANSPORT_STREAM,
                                            MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        KING_SysLog("mbed TLS: failed --- mbedtls_ssl_config_defaults returned %d", ret);
        ret = -1;
        return ret;
    }

    pTlsData->conf.ciphersuite_list[MBEDTLS_SSL_MINOR_VERSION_0] = mbedtls_ssl_list_ciphersuites();
    pTlsData->conf.ciphersuite_list[MBEDTLS_SSL_MINOR_VERSION_1] = mbedtls_ssl_list_ciphersuites();
    pTlsData->conf.ciphersuite_list[MBEDTLS_SSL_MINOR_VERSION_2] = mbedtls_ssl_list_ciphersuites();
    pTlsData->conf.ciphersuite_list[MBEDTLS_SSL_MINOR_VERSION_3] = mbedtls_ssl_list_ciphersuites();            

    return 0;
}

static void mqtt_ssl_SocketProcessCb(uint32 eventID, void* pdata, uint32 len)
{
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pdata;
    
    KING_SysLog("mqtt_ssl_SocketProcessCb() eventID=%d, dataLen=%d\r\n", eventID, len);

    switch (eventID)
    {
        case SOCKET_CONNECT_EVENT_RSP:
        {
            KING_SysLog("mqtt ssl SOCKET_CONNECT_EVENT_RSP\r\n");
            isMqttTlsSucc = 1;
            KING_SemPut(mqttTlsSem);
        }
            break;

        case SOCKET_ACCEPT_EVENT_IND:
            KING_SysLog("mqtt ssl SOCKET_ACCEPT_EVENT_IND\r\n");
            break;

        case SOCKET_CLOSE_EVENT_RSP:
        {
            KING_SysLog("mqtt ssl SOCKET_CLOSE_EVENT_RSP\r\n");
        }
            break;

        case SOCKET_ERROR_EVENT_IND:
            KING_SysLog("mqtt ssl SOCKET_ERROR_EVENT_IND\r\n");
            if (isMqttTlsRunning)
            {
                isMqttTlsSucc = 0;
                KING_SemPut(mqttTlsSem);
            }
            break;

        case SOCKET_WRITE_EVENT_IND:
            KING_SysLog("mqtt ssl SOCKET_WRITE_EVENT_IND ack by peer uSendDataSize=%d\r\n", data->socket.availSize);
            break;

        case SOCKET_READ_EVENT_IND:
        {
            KING_SysLog("mqtt ssl SOCKET_READ_EVENT_IND ack by peer uSendDataSize=%d\r\n", data->socket.availSize);
        }
            break;
            
        case SOCKET_FULL_CLOSED_EVENT_IND:
            KING_SysLog("mqtt ssl SOCKET_FULL_CLOSED_EVENT_IND\r\n");            
            if (isMqttTlsRunning)
            {
                isMqttTlsSucc = 0;
                KING_SemPut(mqttTlsSem);
            }
            break;
            
        default:
            KING_SysLog("mqtt ssl unexpect event/response %d\r\n", eventID);
            break;
    }
}

static int mqtt_ssl_connect(mbedtls_net_context *ctx, SOCK_IN_ADDR_T *pHostAddr, uint16 port, int proto)
{
    int fd = 0;
    union sockaddr_aligned sock_addr;
    uint32 sockaddr_size = 0;
    int ret = 0;
    int protocol = 0;
    int addr_family;
    int sock_type = 0;
    
    if (isMqttTlsRunning == 1)
    {
        return -1;
    }

    KING_SysLog("mqtt_ssl_connect start ");
    if (pHostAddr->type == IP_ADDR_TYPE_V6)
    {
        addr_family = AF_INET6;
    }
    else
    {
        addr_family = AF_INET;
    }
    if (proto == MBEDTLS_NET_PROTO_UDP)
    {
        sock_type = SOCK_DGRAM;
        protocol = IPPROTO_UDP;
    }
    else
    {
        sock_type = SOCK_STREAM;
        protocol = IPPROTO_TCP;
    }

    ret = KING_SocketCreate(addr_family, sock_type, protocol, &fd);
    if (ret < 0 || fd < 0) 
    {
        KING_SysLog("mqtt_ssl_connect create socket error");
        return -1;
    }
    ctx->fd = fd;
    
    mbedtls_net_set_block(ctx);
    KING_RegNotifyFun(DEV_CLASS_SOCKET, fd, mqtt_ssl_SocketProcessCb);
    
    if (pHostAddr->type == IP_ADDR_TYPE_V6)
    {
        struct sockaddr_in6 *sock_addr6 = (struct sockaddr_in6 *)&(sock_addr);
        
        sock_addr6->sin6_len = sizeof(struct sockaddr_in6);
        sock_addr6->sin6_family = AF_INET6;
        sock_addr6->sin6_port = KING_htons(port);
        sock_addr6->sin6_addr.addr[0] = pHostAddr->u_addr.ip6.addr[0];
        sock_addr6->sin6_addr.addr[1] = pHostAddr->u_addr.ip6.addr[1];
        sock_addr6->sin6_addr.addr[2] = pHostAddr->u_addr.ip6.addr[2];
        sock_addr6->sin6_addr.addr[3] = pHostAddr->u_addr.ip6.addr[3];
        
        sockaddr_size = sizeof(struct sockaddr_in6);
    }
    else
    {
        struct sockaddr_in *sock_addr4 = (struct sockaddr_in *)&(sock_addr);
        
        sock_addr4->sin_len = sizeof(struct sockaddr_in);
        sock_addr4->sin_family = AF_INET;
        sock_addr4->sin_port = KING_htons(port);
        sock_addr4->sin_addr.addr = pHostAddr->u_addr.ip4.addr;
        KING_SysLog("mqtt_ssl_connect ip_addr=%u\r\n", sock_addr4->sin_addr.addr);

        sockaddr_size = sizeof(SOCK_ADDR_IN_T);
    }

    if (NULL == mqttTlsSem)
    {
        ret = KING_SemCreate("MQTT TLS SEM", 0, (SEM_HANDLE *)&mqttTlsSem);
        KING_SysLog("%s: KING_SemCreate() ret = %d\r\n", __FUNCTION__, ret);
        if (ret < 0 || mqttTlsSem == NULL)
        {
            return -1;
        }
        
        //KING_SemGet(mqttTlsSem, 0);
    }

    ret = KING_SocketConnect(ctx->fd, &sock_addr, sockaddr_size);
    if (ret < 0)
    {
        KING_SysLog("mqtt_ssl_connect connect fail\r\n");
        KING_SocketClose(ctx->fd);
        ctx->fd = -1;
        return -1;
    }
    
    isMqttTlsRunning = 1;
    KING_SemGet(mqttTlsSem, WAIT_OPT_INFINITE);
    isMqttTlsRunning = 0;
    KING_SemDelete(mqttTlsSem);
    mqttTlsSem = NULL;
    
    KING_SysLog("mqtt_ssl_connect isMqttTlsSucc=%d", isMqttTlsSucc);
    if (isMqttTlsSucc == 0)
    {
        KING_SysLog("mqtt_ssl_connect connect fail\r\n");
        KING_SocketClose(ctx->fd);
        ctx->fd = -1;
        return -1;
    }
    isMqttTlsSucc = 0;

    return 0;
}

static int mqtt_ssl_network_connect(TLSDataParams_t *pTlsData, 
                                SOCK_IN_ADDR_T *pHostAddr, uint16 port,
                                const char *ca_crt, size_t ca_crt_len,
                                const char *client_crt,   size_t client_crt_len,
                                const char *client_key,   size_t client_key_len,
                                const char *client_pwd, size_t client_pwd_len)
{
    int ret = -1;
    
    /*
        * 0. Init
        */
    if (0 != (ret = mqtt_ssl_client_init(pTlsData)))
    {
        KING_SysLog(" failed ! ssl_client_init returned -0x%04x", ret);
        return ret;
    }

    /*
        * 1. Start the connection
        */
    KING_SysLog("Connecting to %d...", port);
    if (0 != (ret = mqtt_ssl_connect(&(pTlsData->fd), pHostAddr, port, MBEDTLS_NET_PROTO_TCP))) 
    {
        KING_SysLog(" failed ! net_connect returned -0x%04x", ret);
        return ret;
    }

    /* OPTIONAL is not optimal for security, but makes interop easier in this simplified example */
    if (ca_crt != NULL) 
    {
        mbedtls_ssl_conf_authmode(&(pTlsData->conf), MBEDTLS_SSL_VERIFY_OPTIONAL);
    } 
    else 
    {
        mbedtls_ssl_conf_authmode(&(pTlsData->conf), MBEDTLS_SSL_VERIFY_NONE);
    }
    mbedtls_ssl_conf_session_tickets(&(pTlsData->conf),MBEDTLS_SSL_SESSION_TICKETS_DISABLED);

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (ca_crt != NULL)
    {
        KING_SysLog("ca_crt ca_crt_len %d", ca_crt_len);
        ret = mbedtls_x509_crt_parse(&(pTlsData->cacertl),
                                    (const unsigned char *)ca_crt,
                                    strlen(ca_crt) + 1);
        if(ret < 0)
        {
            KING_SysLog("ca_crt mbedtls_x509_crt_parse returned -0x%x", -ret);
            return ret;
        }
    }
    if (client_crt != NULL)
    {
        ret = mbedtls_x509_crt_parse(&(pTlsData->clicert),
                                    (const unsigned char *)client_crt,
                                    strlen(client_crt)+1);
        if(ret < 0)
        {
            KING_SysLog("client_crt mbedtls_x509_crt_parse returned -0x%x", -ret);
            return ret;
        }
    }
    if (client_key != NULL)
    {
        ret = mbedtls_pk_parse_key(&(pTlsData->pkey),
                                    (const unsigned char *)client_key,
                                    strlen(client_key)+1, 
                                    (const unsigned char *)client_pwd, 
                                    client_pwd_len);
        if(ret < 0)
        {
            KING_SysLog("mbedtls_pk_parse_key returned -0x%x", -ret);
            return ret;
        }
    }
    mbedtls_ssl_conf_ca_chain(&(pTlsData->conf), &(pTlsData->cacertl), NULL);

    if (client_crt != NULL && client_key != NULL)
    {
        if ((ret = mbedtls_ssl_conf_own_cert(&(pTlsData->conf), &(pTlsData->clicert), &(pTlsData->pkey))) != 0) 
        {
            KING_SysLog(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n", ret);
            return ret;
        }
    }
#endif

    mbedtls_ssl_conf_rng(&(pTlsData->conf), mbedtls_ctr_drbg_random, &pTlsData->ctr_drbg);
    mbedtls_ssl_conf_dbg(&(pTlsData->conf), mqtt_ssl_debug, NULL);
    //mbedtls_ssl_conf_dbg(&(pTlsData->conf), mqtt_ssl_debug, stdout);
    //mbedtls_ssl_conf_read_timeout(&(pTlsData->conf), 20000);

    if ((ret = mbedtls_ssl_setup(&(pTlsData->ssl), &(pTlsData->conf))) != 0) 
    {
        KING_SysLog("failed! mbedtls_ssl_setup returned %d", ret);
        return ret;
    }

    //mbedtls_ssl_set_hostname(&(pTlsData->ssl), addr);
    mbedtls_ssl_set_bio(&(pTlsData->ssl), &(pTlsData->fd), 
                        mbedtls_net_send, mbedtls_net_recv, 
                        mbedtls_net_recv_timeout);

    /*
    * 4. Handshake
    */
    KING_SysLog("Performing the SSL/TLS handshake...");
    while ((ret = mbedtls_ssl_handshake(&(pTlsData->ssl))) != 0) 
    {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) 
        {
            KING_SysLog("failed  ! mbedtls_ssl_handshake returned -0x%04x", -ret);
            return ret;
        }
    }
    KING_SysLog("mbedtls_ssl_handshake ok");
    /*
    * 5. Verify the server certificate
    */
    if (pTlsData->conf.authmode != MBEDTLS_SSL_VERIFY_NONE)
    {
        KING_SysLog("  . Verifying peer X.509 certificate..");
        if (0 != (ret = mbedtls_ssl_get_verify_result(&(pTlsData->ssl)))) 
        {
            KING_SysLog(" failed  ! verify result not confirmed.");
            return ret;
        }
    }
    //mbedtls_ssl_conf_read_timeout(&(pTlsData->conf), 100);
    return 0;
}

static int mqtt_ssl_network_read(TLSDataParams_t *pTlsData, char *buffer, int len, int timeout_ms)
{
    uint32_t        readLen = 0;
    static int      net_status = 0;
    int             ret = -1;
    char            err_str[33];

    mbedtls_ssl_conf_read_timeout(&(pTlsData->conf), timeout_ms);
    while (readLen < len) 
    {
        ret = mbedtls_ssl_read(&(pTlsData->ssl), (unsigned char *)(buffer + readLen), (len - readLen));
        if (ret > 0) 
        {
            readLen += ret;
            net_status = 0;
        } 
        else if (ret == 0) 
        {
            /* if ret is 0 and net_status is -2, indicate the connection is closed during last call */
            return (net_status == -2) ? net_status : readLen;
        } 
        else 
        {
            if (MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY == ret) 
            {
                mbedtls_strerror(ret, err_str, sizeof(err_str));
                KING_SysLog("ssl recv error: code = %d, err_str = '%s'", ret, err_str);
                net_status = -2; /* connection is closed */
                break;
            } 
            else if ((MBEDTLS_ERR_SSL_TIMEOUT == ret)
                || (MBEDTLS_ERR_SSL_CONN_EOF == ret)
                || (MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED == ret)
                || (MBEDTLS_ERR_SSL_NON_FATAL == ret)) 
            {
                /* read already complete */
                /* if call mbedtls_ssl_read again, it will return 0 (means EOF) */

                return readLen;
            } 
            else 
            {
                mbedtls_strerror(ret, err_str, sizeof(err_str));
                KING_SysLog("ssl recv error: code = %d, err_str = '%s'", ret, err_str);
                net_status = -1;
                return -1; /* Connection error */
            }
        }
    }

    return (readLen > 0) ? readLen : net_status;
}

static int mqtt_ssl_network_write(TLSDataParams_t *pTlsData, const char *buffer, int len, int timeout_ms)
{
    uint32_t writtenLen = 0;
    int ret = -1;

    while (writtenLen < len) 
    {
        ret = mbedtls_ssl_write(&(pTlsData->ssl), (unsigned char *)(buffer + writtenLen), (len - writtenLen));
        if (ret > 0) 
        {
            writtenLen += ret;
            continue;
        } 
        else if (ret == 0) 
        {
            KING_SysLog("ssl write timeout");
            return 0;
        } 
        else 
        {
            char err_str[33];
            mbedtls_strerror(ret, err_str, sizeof(err_str));
            KING_SysLog("ssl write fail, code=%d, str=%s", ret, err_str);
            return -1; /* Connnection error */
        }
    }

    return writtenLen;
}

static void mqtt_ssl_network_disconnect(TLSDataParams_t *pTlsData)
{
    mbedtls_ssl_close_notify(&(pTlsData->ssl));
    KING_SocketShutdown( pTlsData->fd.fd, 2 );
    KING_SocketClose( pTlsData->fd.fd );
    KING_UnRegNotifyFun(DEV_CLASS_SOCKET, pTlsData->fd.fd, NULL);
    pTlsData->fd.fd = -1;
    mbedtls_net_free(&(pTlsData->fd));
#if defined(MBEDTLS_X509_CRT_PARSE_C)    
    mbedtls_x509_crt_free(&(pTlsData->cacertl));
    mbedtls_x509_crt_free(&(pTlsData->clicert));
    mbedtls_pk_free(&(pTlsData->pkey));
#endif
    mbedtls_ssl_free(&(pTlsData->ssl));
    mbedtls_ssl_config_free(&(pTlsData->conf));
    mbedtls_ctr_drbg_free(&(pTlsData->ctr_drbg));
    mbedtls_entropy_free(&(pTlsData->entropy));   
    KING_SysLog("ssl_disconnect");
}

/*******************************************************************************
 ** Function 
 ******************************************************************************/
int mqtt_ssl_read(int handle, char *buf, int len, int timeout_ms)
{
    return mqtt_ssl_network_read((TLSDataParams_t *)handle, buf, len, timeout_ms);;
}

int mqtt_ssl_write(int handle, const char *buf, int len, int timeout_ms)
{
    return mqtt_ssl_network_write((TLSDataParams_t *)handle, buf, len, timeout_ms);
}

int32_t mqtt_ssl_destroy(int handle)
{
    if ((int)NULL == handle) 
    {
        KING_SysLog("handle is NULL");
        return 0;
    }

    mqtt_ssl_network_disconnect((TLSDataParams_t *)handle);
    free((void *)handle);
    handle = NULL;
    return 0;
}

int mqtt_ssl_establish(SOCK_IN_ADDR_T *pHostAddr,
                            uint16 port,
                            const char *ca_crt,
                            size_t ca_crt_len,
                            const char *client_crt,
                            size_t client_crt_len,
                            const char *client_key,
                            size_t client_key_len,
                            const char *client_pwd,
                            size_t client_pwd_len
                            )
{
    TLSDataParams_pt    pTlsData;

    if (pHostAddr == NULL) 
    {
        KING_SysLog("input params are NULL, abort");
        return 0;
    }

    pTlsData = malloc(sizeof(TLSDataParams_t));
    if (NULL == pTlsData) 
    {
        return (int)NULL;
    }
    memset(pTlsData, 0x0, sizeof(TLSDataParams_t));

    if (0 != mqtt_ssl_network_connect(pTlsData, pHostAddr, port, 
                                        ca_crt, ca_crt_len, 
                                        client_crt, client_crt_len, 
                                        client_key, client_key_len, 
                                        client_pwd, client_pwd_len)) 
    {
        mqtt_ssl_network_disconnect(pTlsData);
        free((void *)pTlsData);
        pTlsData = NULL;
        return (int)NULL;
    }

    return (int)pTlsData;
}
