/*******************************************************************************
 ** File Name:  mbedtls_test.c
 ** Author:      Allance.Chen
 ** Date:        2020-07-02
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains functions about mbedtls test.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-07-02    Allance.Chen         Create.
 ******************************************************************************/
/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "kingnet.h"
#include "kingsocket.h"
#include "kingcstd.h"
#include "kingrtos.h"
#include "kingplat.h"
#include "kingCbData.h"
#include "net_sockets.h"
#include "entropy.h"
#include "ctr_drbg.h"
#include "debug.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("mbedtls: "fmt, ##args); } while(0)

#define CHEERZING_SSL_TEST_SERVER   "test.cheerzing.com"
#define CHEERZING_SSL_TEST_PORT     9443

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef struct _MebTlsDataParams {
    mbedtls_ssl_context ssl;          /**< mbed TLS control context. */
    mbedtls_net_context fd;           /**< mbed TLS network context. */
    mbedtls_ssl_config conf;          /**< mbed TLS configuration context. */
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_x509_crt cacertl;         /**< mbed TLS CA certification. */
    mbedtls_x509_crt clicert;         /**< mbed TLS Client certification. */
    mbedtls_pk_context pkey;          /**< mbed TLS Client key. */

    uint8 isHandshake;
} MebTlsDataParams_t, *MebTlsDataParams_pt;

/*******************************************************************************
 ** External Variables 
 ******************************************************************************/
extern const char mbedtls_cas_pem[];
extern const size_t mbedtls_cas_pem_len;

extern const char mbedtls_cheerzing_test_pem[];
extern const size_t mbedtls_cheerzing_test_pem_len;

extern const char mbedtls_cheerzing_test_cli_crt[];
extern const size_t mbedtls_cheerzing_test_cli_crt_len;

extern const char mbedtls_cheerzing_test_cli_key[];
extern const size_t mbedtls_cheerzing_test_cli_key_len;

/*******************************************************************************
 ** Variables
 ******************************************************************************/
const mbedtls_x509_crt_profile mbedtls_test_x509_crt_profile =
{
    /* Hashes from SHA-1 and above */
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA1 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_RIPEMD160 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA224 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA256 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA384 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA512 ),
    0xFFFFFFF, /* Any PK alg    */
    0xFFFFFFF, /* Any curve     */
    1024,
};

const char *mbedtls_pers = "mbedtls_test";

static MebTlsDataParams_pt pTlsData = NULL;
static THREAD_HANDLE tlsThreadID = NULL;
static MSG_HANDLE tlsMsgHandle = NULL;
static int isTlsSucc = 0;
static SEM_HANDLE tlsSem = NULL;
static int isTlsRunning = 0;
static hardwareHandle hTlsNIC = -1;
static softwareHandle hTlsContext = -1;
static int authMode = 0;
static SOCK_IN_ADDR_T tlsHostAddr;

/*******************************************************************************
 ** Local Function Delcarations
 ******************************************************************************/
static void tls_network_disconnect(void);

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
static void tls_debug(void *ctx, int level, const char *file, int line, const char *str)
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
    LogPrintf("mbed TLS: %s:%04d: %s",  p_file_name, line, str );
}

static int tls_getNicHandle(void)
{
    int ret = 0;
    NIC_INFO info;

    memset(&info, 0x00, sizeof(NIC_INFO));
    info.NIC_Type = NIC_TYPE_WWAN;
    ret = KING_HardwareNICRequest(info, &hTlsNIC);
    LogPrintf("tls_getNicHandle ret=%d, hTlsNIC=%d", ret, hTlsNIC);

    return ret;
}

static int tls_getContextHandle(int isIpv6)
{
    int ret = 0;
    NIC_CONTEXT ApnInfo;

    if (hTlsNIC == -1)
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
    ret = KING_ConnectionAcquire(hTlsNIC, ApnInfo, &hTlsContext);
    LogPrintf("socket_getContextHandle ret=%d, hTlsContext=%d", ret, hTlsContext);

    return ret;
}

static int tls_releaseContextHandle(void)
{
    int ret = 0;

    if (hTlsContext == -1)
    {
        return -1;
    }
    
    ret = KING_ConnectionRelease(hTlsContext);
    LogPrintf("tls_releaseContextHandle ret=%d", ret);

    return ret;
}

static int tls_client_init(MebTlsDataParams_t *pTlsData)
{
    int ret = -1;
    
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold((int)1);
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
    LogPrintf("mbed TLS: p_handle_ptr->entropy.source_count:%d", pTlsData->entropy.source_count);
    if((ret = mbedtls_ctr_drbg_seed( &(pTlsData->ctr_drbg), mbedtls_entropy_func,
        &(pTlsData->entropy), (const unsigned char *)mbedtls_pers, strlen(mbedtls_pers))) != 0)
    {
        LogPrintf("mbed TLS: failed --- mbedtls_ctr_drbg_seed returned %d", ret);
        ret = -1;
        return ret;
    }


    /* Setup default config */
    LogPrintf("mbed TLS: Setting up the SSL/TLS structure...");
    if(( ret = mbedtls_ssl_config_defaults( &(pTlsData->conf),
                                            MBEDTLS_SSL_IS_CLIENT,
                                            MBEDTLS_SSL_TRANSPORT_STREAM,
                                            MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        LogPrintf("mbed TLS: failed --- mbedtls_ssl_config_defaults returned %d", ret);
        ret = -1;
        return ret;
    }

    pTlsData->conf.ciphersuite_list[MBEDTLS_SSL_MINOR_VERSION_0] = mbedtls_ssl_list_ciphersuites();
    pTlsData->conf.ciphersuite_list[MBEDTLS_SSL_MINOR_VERSION_1] = mbedtls_ssl_list_ciphersuites();
    pTlsData->conf.ciphersuite_list[MBEDTLS_SSL_MINOR_VERSION_2] = mbedtls_ssl_list_ciphersuites();
    pTlsData->conf.ciphersuite_list[MBEDTLS_SSL_MINOR_VERSION_3] = mbedtls_ssl_list_ciphersuites();            
    mbedtls_ssl_conf_min_version(&(pTlsData->conf),
                                MBEDTLS_SSL_MAJOR_VERSION_3,
                                MBEDTLS_SSL_MINOR_VERSION_0);
    
    mbedtls_ssl_conf_max_version(&(pTlsData->conf),
                                MBEDTLS_SSL_MAJOR_VERSION_3,
                                MBEDTLS_SSL_MINOR_VERSION_3);

    mbedtls_ssl_conf_rng(&(pTlsData->conf), mbedtls_ctr_drbg_random, &pTlsData->ctr_drbg);
    mbedtls_ssl_conf_dbg(&(pTlsData->conf), tls_debug, NULL);
    
#if defined(MBEDTLS_X509_CRT_PARSE_C) 
    LogPrintf("mbed TLS: Loading the CA root certificate ..." );
    ret = mbedtls_x509_crt_parse(&(pTlsData->cacertl),
                                (const unsigned char *)mbedtls_cas_pem,
                                mbedtls_cas_pem_len);
    if(ret < 0)
    {
        LogPrintf("mbed TLS: SSL Cfg failed --- mbedtls_x509_crt_parse returned -0x%x", -ret);
    }
    ret = mbedtls_x509_crt_parse(&(pTlsData->cacertl),
                                (const unsigned char *)mbedtls_cheerzing_test_pem,
                                mbedtls_cheerzing_test_pem_len);
    if(ret < 0)
    {
        LogPrintf("mbed TLS: SSL Cfg failed --- mbedtls_x509_crt_parse returned -0x%x", -ret);
    }
    ret = mbedtls_x509_crt_parse(&(pTlsData->clicert),
                                (const unsigned char *)mbedtls_cheerzing_test_cli_crt,
                                mbedtls_cheerzing_test_cli_crt_len);
    if(ret < 0)
    {
        LogPrintf("mbed TLS: SSL Cfg failed --- mbedtls_x509_crt_parse returned -0x%x", -ret);
    }
    
    ret = mbedtls_pk_parse_key(&(pTlsData->pkey),
                                (const unsigned char *)mbedtls_cheerzing_test_cli_key,
                                mbedtls_cheerzing_test_cli_key_len+1, NULL, 0);
    if(ret < 0)
    {
        LogPrintf("mbed TLS: SSL Cfg failed --- mbedtls_pk_parse_key returned -0x%x", -ret);
    }
            
    mbedtls_ssl_conf_cert_profile(&(pTlsData->conf), &mbedtls_test_x509_crt_profile);
    
#if defined(MBEDTLS_ARC4_C)
    /* Enable ARC4 */
    mbedtls_ssl_conf_arc4_support(&(pTlsData->conf), MBEDTLS_SSL_ARC4_ENABLED);
#endif
    mbedtls_ssl_conf_authmode(&(pTlsData->conf), MBEDTLS_SSL_VERIFY_NONE);

    return 0;
}

static int tls_testInit(void)
{
    int ret = -1;
    
    pTlsData = malloc(sizeof(MebTlsDataParams_t));
    if (NULL == pTlsData) 
    {
        return -1;
    }
    memset(pTlsData, 0x0, sizeof(MebTlsDataParams_t));
    
    ret = tls_client_init(pTlsData);
    return ret;
}

static void tls_SocketProcessCb(uint32 eventID, void* pdata, uint32 len)
{
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pdata;
    
    LogPrintf("tls_SocketProcessCb() eventID=%d, dataLen=%d\r\n", eventID, len);
    switch (eventID)
    {
        case SOCKET_CONNECT_EVENT_RSP:
        {
            LogPrintf("mbedtls SOCKET_CONNECT_EVENT_RSP\r\n");
            isTlsSucc = 1;
            KING_SemPut(tlsSem);
        }
            break;

        case SOCKET_ACCEPT_EVENT_IND:
            LogPrintf("mbedtls test SOCKET_ACCEPT_EVENT_IND\r\n");
            break;

        case SOCKET_CLOSE_EVENT_RSP:
        {
            LogPrintf("mbedtls test SOCKET_CLOSE_EVENT_RSP\r\n");
        }
            break;

        case SOCKET_ERROR_EVENT_IND:
            LogPrintf("mbedtls test SOCKET_ERROR_EVENT_IND\r\n");
            if (isTlsRunning)
            {
                isTlsSucc = 0;
                KING_SemPut(tlsSem);
            }
            break;

        case SOCKET_WRITE_EVENT_IND:
            LogPrintf("mbedtls test SOCKET_WRITE_EVENT_IND ack by peer uSendDataSize=%d\r\n", data->socket.availSize);
            break;

        case SOCKET_READ_EVENT_IND:
        {
            LogPrintf("mbedtls test SOCKET_READ_EVENT_IND ack by peer uSendDataSize=%d\r\n", data->socket.availSize);
        }
            break;
            
        case SOCKET_FULL_CLOSED_EVENT_IND:
            LogPrintf("mbedtls test SOCKET_FULL_CLOSED_EVENT_IND\r\n");
            break;
            
        default:
            LogPrintf("mbedtls test unexpect event/response %d\r\n", eventID);
            break;
    }
}

static int tls_connect(mbedtls_net_context *ctx, SOCK_IN_ADDR_T *pHostAddr, uint16 port, int proto)
{
    int fd = 0;
    union sockaddr_aligned sock_addr;
    uint32 sockaddr_size = 0;
    int ret = 0;
    int protocol = 0;
    int addr_family;
    int sock_type = 0;
    
    if (isTlsRunning == 1)
    {
        return -1;
    }

    LogPrintf("%s: start ", __FUNCTION__);
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
        LogPrintf("%s: create socket error", __FUNCTION__);
        return -1;
    }
    ctx->fd = fd;
    
    mbedtls_net_set_block(ctx);
    KING_RegNotifyFun(DEV_CLASS_SOCKET, fd, tls_SocketProcessCb);
    
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
        LogPrintf("%s: ip_addr=%u\r\n", __FUNCTION__, sock_addr4->sin_addr.addr);

        sockaddr_size = sizeof(SOCK_ADDR_IN_T);
    }

    if (NULL == tlsSem)
    {
        ret = KING_SemCreate("MbedTLS SEM", 0, (SEM_HANDLE *)&tlsSem);
        LogPrintf("%s: KING_SemCreate() ret = %d\r\n", __FUNCTION__, ret);
        if (ret < 0 || tlsSem == NULL)
        {
            return -1;
        }
    }

    ret = KING_SocketConnect(ctx->fd, &sock_addr, sockaddr_size);
    if (ret < 0)
    {
        LogPrintf("%s: connect fail\r\n", __FUNCTION__);
        KING_SocketClose(ctx->fd);
        ctx->fd = -1;
        return -1;
    }
    
    isTlsRunning = 1;
    KING_SemGet(tlsSem, WAIT_OPT_INFINITE);
    isTlsRunning = 0;
    KING_SemDelete(tlsSem);
    tlsSem = NULL;
    
    LogPrintf("%s: isTlsSucc=%d", __FUNCTION__, isTlsSucc);
    if (isTlsSucc == 0)
    {
        LogPrintf("%s: connect fail\r\n", __FUNCTION__);
        KING_SocketClose(ctx->fd);
        ctx->fd = -1;
        return -1;
    }
    isTlsSucc = 0;

    return 0;
}

static int tls_network_read(MebTlsDataParams_t *pTlsData, char *buffer, int len, int timeout_ms)
{
    uint32_t        readLen = 0;
    static int      net_status = 0;
    int             ret = -1;

    mbedtls_ssl_conf_read_timeout(&(pTlsData->conf), timeout_ms);
    //while (readLen < len)
    //{
        ret = mbedtls_ssl_read(&(pTlsData->ssl), (unsigned char *)(buffer + readLen), (len - readLen));
        LogPrintf("tls_network_read: ret = %d", ret);
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
                //mbedtls_strerror(ret, err_str, sizeof(err_str));
                LogPrintf("ssl recv error: code = 0x%x", ret);
                net_status = -2; /* connection is closed */
                //break;
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
                //mbedtls_strerror(ret, err_str, sizeof(err_str));
                LogPrintf("ssl recv error: ret = 0x%x", ret);
                net_status = -1;
                return -1; /* Connection error */
            }
        }
    //}
    return (readLen > 0) ? readLen : net_status;
}

static int tls_network_write(MebTlsDataParams_t *pTlsData, const char *buffer, int len, int timeout_ms)
{
    uint32_t writtenLen = 0;
    int ret = -1;

    while (writtenLen < len) 
    {
        ret = mbedtls_ssl_write(&(pTlsData->ssl), (unsigned char *)(buffer + writtenLen), (len - writtenLen));
        LogPrintf("tls_network_write: ret = %d", ret);
        if (ret > 0) 
        {
            writtenLen += ret;
            continue;
        } 
        else if (ret == 0) 
        {
            LogPrintf("ssl write timeout");
            return 0;
        } 
        else 
        {
            //char err_str[33];
            //mbedtls_strerror(ret, err_str, sizeof(err_str));
            LogPrintf("ssl write fail, code=0x%x", ret);
            return -1; /* Connnection error */
        }
    }

    return writtenLen;
}

static int tls_network_connect(MebTlsDataParams_t *pTlsData, 
                                SOCK_IN_ADDR_T *pHostAddr, uint16 port)
{
    int ret = -1;
    
    if (0 != (ret = tls_client_init(pTlsData)))
    {
        LogPrintf(" failed ! ssl_client_init returned -0x%04x", ret);
        return ret;
    }

    LogPrintf("Connecting to %d...", port);
    if (0 != (ret = tls_connect(&(pTlsData->fd), pHostAddr, port, MBEDTLS_NET_PROTO_TCP))) 
    {
        LogPrintf(" failed ! net_connect returned -0x%04x", ret);
        return ret;
    }

    mbedtls_ssl_conf_session_tickets(&(pTlsData->conf), MBEDTLS_SSL_SESSION_TICKETS_DISABLED);
#if 0
    if (pTlsData->conf.authmode == MBEDTLS_SSL_VERIFY_REQUIRED) 
    {
    #if defined(MBEDTLS_X509_CRT_PARSE_C) 
        ret = mbedtls_x509_crt_parse(&(pTlsData->cacertl),
                                    (const unsigned char *)ssl_def_cas_pem,
                                    ssl_def_cas_pem_len);
        if(ret < 0)
        {
            LogPrintf("[KING] mbed TLS: SSL Cfg failed --- mbedtls_x509_crt_parse returned -0x%x", -ret);
        }
        ret = mbedtls_x509_crt_parse(&(pTlsData->cacertl),
                                    (const unsigned char *)ssl_cheerzing_test_pem,
                                    ssl_cheerzing_test_pem_len);
        if(ret < 0)
        {
            LogPrintf("mbed TLS: SSL Cfg failed --- mbedtls_x509_crt_parse returned -0x%x", -ret);
        }
        ret = mbedtls_x509_crt_parse(&(pTlsData->clicert),
                                    (const unsigned char *)ssl_test_cli_crt,
                                    ssl_test_cli_crt_len);
        if(ret < 0)
        {
            LogPrintf("mbed TLS: SSL Cfg failed --- mbedtls_x509_crt_parse returned -0x%x", -ret);
        }
        ret = mbedtls_pk_parse_key(&(pTlsData->pkey),
                                    (const unsigned char *)ssl_test_cli_key,
                                    ssl_test_cli_key_len, NULL, 0);
        if(ret < 0)
        {
            LogPrintf("mbed TLS: SSL Cfg failed --- mbedtls_pk_parse_key returned -0x%x", -ret);
        }
    #endif
    }
#endif

    mbedtls_ssl_conf_ca_chain(&(pTlsData->conf), &(pTlsData->cacertl), NULL);
    if (pTlsData->conf.authmode == MBEDTLS_SSL_VERIFY_REQUIRED) 
    {
        if(( ret = mbedtls_ssl_conf_own_cert(&(pTlsData->conf), &(pTlsData->clicert), &(pTlsData->pkey) ) ) != 0 )
        {
            LogPrintf( "failed! mbedtls_ssl_conf_own_cert returned %d\n\n", ret);
            return ret;
        } 
    } 
#endif
    if ((ret = mbedtls_ssl_setup(&(pTlsData->ssl), &(pTlsData->conf))) != 0) 
    {
        LogPrintf("failed! mbedtls_ssl_setup returned %d", ret);
        return ret;
    }

    mbedtls_ssl_set_bio(&(pTlsData->ssl), &(pTlsData->fd), 
                        mbedtls_net_send, mbedtls_net_recv, 
                        mbedtls_net_recv_timeout);

    LogPrintf("Performing the SSL/TLS handshake...");
    while ((ret = mbedtls_ssl_handshake(&(pTlsData->ssl))) != 0) 
    {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) 
        {
            LogPrintf("failed  ! mbedtls_ssl_handshake returned -0x%04x", -ret);
            return ret;
        }
    }
    LogPrintf("mbedtls_ssl_handshake ok");

    if (pTlsData->conf.authmode != MBEDTLS_SSL_VERIFY_NONE)
    {
        LogPrintf("  . Verifying peer X.509 certificate..");
        if (0 != (ret = mbedtls_ssl_get_verify_result(&(pTlsData->ssl)))) 
        {
            LogPrintf(" failed  ! verify result not confirmed.");
            return ret;
        }
    }
    
    pTlsData->isHandshake = 1;
    return 0;
}

static void tls_network_disconnect(void)
{
    if (pTlsData != NULL)
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
        pTlsData->isHandshake = 0;
        free(pTlsData);
        pTlsData = NULL;
        LogPrintf("tls_network_disconnect");
    }
}

static void tls_dnsReqCb(void* pData, uint32 len)
{
    ASYN_DNS_PARSE_T *dnsParse = (ASYN_DNS_PARSE_T *)pData;
    
    LogPrintf("tls_dnsReqCb hostname=%s\r\n", dnsParse->hostname);
    LogPrintf("tls_dnsReqCb dnsParse->result=%d\n", dnsParse->result);
    if (dnsParse->result == DNS_PARSE_SUCCESS)
    {
        int ret = -1;
        MSG_S msg;
        
        tlsHostAddr = dnsParse->addr;
        memset(&msg, 0x00, sizeof(MSG_S));
        msg.messageID = 131;
        ret = KING_MsgSend(&msg, tlsMsgHandle);
        if(-1 == ret)
        {
            LogPrintf("tls_dnsReqCb KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
            return;
        }        
    }
    else if (dnsParse->result == DNS_PARSE_ERROR)
    {
        LogPrintf("tls_dnsReqCb fail!\n");
    }
}


static void tls_getIPAddrByHostName(int pdpType, char *hostName)
{
    int ret = -1;
    SOCK_IN_ADDR_T destAddr;
    
    LogPrintf("tls_getIPAddrByHostName ... ...");
    if (hTlsContext == -1)
    {
        return;
    }
    memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
    ret = KING_NetGetHostByName(hTlsContext, hostName, &destAddr, 10000, (DNS_CALLBACK)tls_dnsReqCb);
    if (ret == 0)
    {
        if (((destAddr.type == IP_ADDR_TYPE_V6) && (0 == destAddr.u_addr.ip6.addr[0]) 
            && (0 == destAddr.u_addr.ip6.addr[1]) && (0 == destAddr.u_addr.ip6.addr[2])
            && (0 == destAddr.u_addr.ip6.addr[3])) || ((destAddr.type == IP_ADDR_TYPE_V4) 
            && (0 == destAddr.u_addr.ip4.addr)) )
        {
            LogPrintf("tls_getIPAddrByHostName ipv6 query QUEUED\r\n");
        }
        else
        {
            if (0 == destAddr.u_addr.ip4.addr)
            {
                LogPrintf("tls_getIPAddrByHostName query QUEUED\r\n");
            }
        }
    }
    else
    {
        LogPrintf("tls_getIPAddrByHostName fail\r\n");
    }
}

static void tls_startTest(int pdpType)
{
    int ret;
    NET_STATE_E state;

    LogPrintf("\r\n======== tls_startTest start... ========\r\n");
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

            if (pdpType == 1)
            {
                ret = KING_NetApnSet(1, "", NULL, NULL, 0, IP_ADDR_TYPE_V6);
            }
            else
            {
                ret = KING_NetApnSet(1, "", NULL, NULL, 0, IP_ADDR_TYPE_V4);
            }
            LogPrintf("KING_NetApnSet ret : %d\r\n", ret);
            if (ret != 0)
            {
                LogPrintf("KING_NetApnSet fail\r\n");
                break;
            }

            tls_getNicHandle();
            tls_getContextHandle(0);
            KING_Sleep(1000);
            if (pdpType == 1)
            {
            }
            else
            {
                tls_getIPAddrByHostName(0, CHEERZING_SSL_TEST_SERVER);
            }
            LogPrintf("tls_startTest complete\r\n");
            break;
        }
        
        KING_Sleep(1000);
    }

    LogPrintf("\r\n[KING]======== tls_startTest complete ========\r\n");
}

static void tls_test_thread(void *param)
{
    MSG_S msg;
    int ret = -1;

    LogPrintf("tls_test_thread is running. tlsThreadID = 0x%X\r\n", tlsThreadID);
    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        LogPrintf("tls_test_thread!\r\n");
        if (tlsMsgHandle != NULL)
        {
            LogPrintf("tls_test_thread KING_MsgRcv!\r\n");
            ret = KING_MsgRcv(&msg, tlsMsgHandle, WAIT_OPT_INFINITE);
            if (ret == -1)
            {
                LogPrintf("tls_test_thread ret=%d\r\n", ret);
                continue;
            }
            LogPrintf("tls_test_thread receive msg. MsgId=%d\r\n", msg.messageID);
            if (msg.messageID == 130)
            {
                tls_startTest(0);
            }
            else if (msg.messageID == 131)
            {
                MSG_S sendMsg;
                
                ret = tls_testInit();
                if (ret == -1)
                {
                    tls_network_disconnect();
                    break;
                }
                authMode = 0;
                mbedtls_ssl_conf_authmode(&(pTlsData->conf), authMode);
                ret = tls_network_connect(pTlsData, &tlsHostAddr, CHEERZING_SSL_TEST_PORT);
                if (ret == -1)
                {
                    tls_network_disconnect();
                    break;
                }
                memset(&sendMsg, 0x00, sizeof(MSG_S));
                sendMsg.messageID = 132;
                ret = KING_MsgSend(&sendMsg, tlsMsgHandle);
                if(-1 == ret)
                {
                    LogPrintf("msg.messageID is 132 KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
                    tls_network_disconnect();
                    break;
                }
            }
            else if (msg.messageID == 132)
            {
                uint8 buf[128] = {0};
                
                LogPrintf("tls_test_thread send data\r\n");
                memset(buf, 0x00, sizeof(buf));
                sprintf((char*)buf, "%s", "socket send and recv test");
                ret = tls_network_write(pTlsData, (char*)buf, strlen((char*)buf), 10000);
                if (-1 == ret)
                {
                    LogPrintf("tls_test_thread send fail\r\n");
                    tls_network_disconnect();
                    break;
                }
                else
                {
                    LogPrintf("tls_test_thread send succ\r\n");
                }
                if (pTlsData != NULL && pTlsData->isHandshake)
                {
                    MSG_S recvMsg;
                    int ret = -1;
                    
                    memset(&recvMsg, 0x00, sizeof(MSG_S));
                    recvMsg.messageID = 133;
                    recvMsg.DataLen = 100;
                    ret = KING_MsgSend(&recvMsg, tlsMsgHandle);
                    if(-1 == ret)
                    {
                        LogPrintf("msg.messageID is 133 KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
                        tls_network_disconnect();
                        break;
                    }
                }
            }
            else if (msg.messageID == 133)
            {
                uint16 uDataSize = 0;
                uint8 *pRecv     = NULL;
                
                LogPrintf("mbedtls_test_thread recv data\r\n");
                uDataSize = msg.DataLen;
                pRecv = malloc(uDataSize + 1);
                if(pRecv == NULL)
                {
                    LogPrintf("socket_test_thread 122 malloc fail\r\n");
                    break;
                }
                memset((void *)pRecv, 0x00, uDataSize + 1);
                ret = tls_network_read(pTlsData, (char*)pRecv, uDataSize, 10000);

                if(ret == -1)
                {
                    LogPrintf("[KING]KING_SocketRecv recv fail\r\n");
                }

                free(pRecv);
                pRecv = NULL;
                
                
                tls_network_disconnect();
                tls_releaseContextHandle();
                break;
            }
        }
    }
    if (tlsMsgHandle != NULL)
    {
        KING_MsgDelete(tlsMsgHandle);
        tlsMsgHandle = NULL;
    }
    if (tlsThreadID != NULL)
    {
        tlsThreadID = NULL; 
    }
    KING_ThreadExit();
}

void mbedtls_test(void)
{
    int ret = -1;
    MSG_S msg;
    THREAD_ATTR_S threadAttr;
    
    LogPrintf("mbedtls_test is KING_MsgCreate ");
    ret = KING_MsgCreate(&tlsMsgHandle);
    LogPrintf("mbedtls_test is KING_MsgCreate ret %d", ret);
    if(-1 == ret)
    {
        LogPrintf("mbedtls_test KING_MsgCreate, errcode=0x%x\r\n", KING_GetLastErrCode());
    }

    KING_Sleep(1000);

    LogPrintf("mbedtls_test is KING_ThreadCreate ");
    memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
    threadAttr.fun = tls_test_thread;
    threadAttr.priority = THREAD_PRIORIT1;
    threadAttr.stackSize = 1024 * 5;
    threadAttr.queueNum = 64; 
    
    LogPrintf("mbedtls_test is create tlsThreadID 0x%08x", &tlsThreadID);
    ret = KING_ThreadCreate("mbedtlsTestThread", &threadAttr, &tlsThreadID);
    if (-1 == ret)
    {
        LogPrintf("Create Test Thread Failed!! Errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    KING_Sleep(1000);
    
    LogPrintf("mbedtls_test is KING_MsgSend ");
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = 130;
    ret = KING_MsgSend(&msg, tlsMsgHandle);
    if(-1 == ret)
    {
        LogPrintf("mbedtls_test KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
}

