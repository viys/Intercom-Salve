/*******************************************************************************
 ** File Name:   httptls_api.c
 ** Author:      Allance.Chen
 ** Date:        2020-06-30
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains functions about http tls api.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-06-30     Allance.Chen         Create.
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
#include "httpTls_api.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define HTTP_TLS_READ_TIMEOUT         10000

/*******************************************************************************
**  Type definition
********************************************************************************/
typedef struct httpTlsDataParams {
    HTTP_TLS_STATE_E state;

    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

#if defined(MBEDTLS_X509_CRT_PARSE_C)  
    mbedtls_x509_crt cacert;
    mbedtls_x509_crt client_cert;
    mbedtls_pk_context client_key;

    int ignoretime;
#endif
    int force_ciphersuite[2];   /* protocol/ciphersuite to use, or all      */

    mbedtls_net_context httpFd;
} HTTP_TLS_DATA_PARAMS_T, *HTTP_TLS_DATA_PARAMS_PT;

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static HTTP_TLS_DATA_PARAMS_PT pHttpTlsData = NULL;

const mbedtls_x509_crt_profile s_httptls_x509_crt_profile =
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

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
static void httpTls_debug( void *ctx, int level, const char *file, int line, const char *str )
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

static uint32 httpTls_real_confirm(int ignoretime, uint32 verify_result)
{
    //KING_SysLog("certificate verification result: 0x%02x\n", verify_result);
    if(ignoretime == 1)
    {
        if ((verify_result & MBEDTLS_X509_BADCERT_EXPIRED)
            || (verify_result & MBEDTLS_X509_BADCRL_EXPIRED)
            || (verify_result & MBEDTLS_X509_BADCERT_NOT_TRUSTED)
            || (verify_result & MBEDTLS_X509_BADCRL_NOT_TRUSTED)
            || (verify_result & MBEDTLS_X509_BADCERT_FUTURE)
            || (verify_result & MBEDTLS_X509_BADCRL_FUTURE)
            )
        {
            return 0;
        }
    }

    return verify_result;
}

/*******************************************************************************
 ** Function 
 ******************************************************************************/
int httpTls_client_init(void)
{
    int ret = -1;

    //KING_SysLog("mbed TLS: httpTls_client_init.");
    if (pHttpTlsData == NULL)
    {
        pHttpTlsData = malloc(sizeof(HTTP_TLS_DATA_PARAMS_T));
        if (NULL == pHttpTlsData) 
        {
            KING_SysLog("mbed TLS: pHttpTlsData malloc failed!");
            return -1;
        }
        memset(pHttpTlsData, 0x0, sizeof(HTTP_TLS_DATA_PARAMS_T));
    }

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold((int)1);
#endif

    mbedtls_net_init(&(pHttpTlsData->httpFd));
    mbedtls_ssl_init(&(pHttpTlsData->ssl));
    mbedtls_ssl_config_init(&(pHttpTlsData->conf));
    mbedtls_ctr_drbg_init(&(pHttpTlsData->ctr_drbg));
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_init(&(pHttpTlsData->cacert));
    mbedtls_x509_crt_init(&(pHttpTlsData->client_cert));
    mbedtls_pk_init(&(pHttpTlsData->client_key));
#endif    
    mbedtls_ssl_conf_dbg(&(pHttpTlsData->conf), httpTls_debug, NULL);
    mbedtls_entropy_init( &(pHttpTlsData->entropy) );
    
    //KING_SysLog("mbed TLS: p_handle_ptr->entropy.source_count:%d", pHttpTlsData->entropy.source_count);
    if((ret = mbedtls_ctr_drbg_seed( &(pHttpTlsData->ctr_drbg), mbedtls_entropy_func,
        &(pHttpTlsData->entropy), (const unsigned char *)"mbedtls_client", strlen("mbedtls_client"))) != 0)
    {
        KING_SysLog("mbed TLS: failed --- mbedtls_ctr_drbg_seed returned %d", ret);
        ret = -1;
        return ret;
    }

    /* Setup default config */
    //KING_SysLog("mbed TLS: Setting up the SSL/TLS structure...");
    if(( ret = mbedtls_ssl_config_defaults( &(pHttpTlsData->conf),
                                            MBEDTLS_SSL_IS_CLIENT,
                                            MBEDTLS_SSL_TRANSPORT_STREAM,
                                            MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        KING_SysLog("mbed TLS: failed --- mbedtls_ssl_config_defaults returned %d", ret);
        ret = -1;
        return ret;
    }

    pHttpTlsData->conf.ciphersuite_list[MBEDTLS_SSL_MINOR_VERSION_0] = mbedtls_ssl_list_ciphersuites();
    pHttpTlsData->conf.ciphersuite_list[MBEDTLS_SSL_MINOR_VERSION_1] = mbedtls_ssl_list_ciphersuites();
    pHttpTlsData->conf.ciphersuite_list[MBEDTLS_SSL_MINOR_VERSION_2] = mbedtls_ssl_list_ciphersuites();
    pHttpTlsData->conf.ciphersuite_list[MBEDTLS_SSL_MINOR_VERSION_3] = mbedtls_ssl_list_ciphersuites();            
    mbedtls_ssl_conf_min_version(&(pHttpTlsData->conf),
                                MBEDTLS_SSL_MAJOR_VERSION_3,
                                MBEDTLS_SSL_MINOR_VERSION_0);

    mbedtls_ssl_conf_max_version(&(pHttpTlsData->conf),
                                MBEDTLS_SSL_MAJOR_VERSION_3,
                                MBEDTLS_SSL_MINOR_VERSION_3);
    
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_ssl_conf_cert_profile(&(pHttpTlsData->conf), &s_httptls_x509_crt_profile);
#endif
    
#if defined(MBEDTLS_ARC4_C)
    /* Enable ARC4 */
    mbedtls_ssl_conf_arc4_support(&(pHttpTlsData->conf), MBEDTLS_SSL_ARC4_ENABLED);
#endif
    mbedtls_ssl_conf_authmode(&(pHttpTlsData->conf), MBEDTLS_SSL_VERIFY_NONE);

    pHttpTlsData->state = HTTP_TLS_STATE_CREATED;

    return 0;
}

int32 httpTls_setVersion(uint16 sslVersion)
{
    int32 ret = 0;
    
    if (pHttpTlsData == NULL)
    {
        KING_SysLog( "%s: http tls is not init!", __FUNCTION__);
        ret = -1;
        return ret;
    }    
    if(HTTP_TLS_STATE_CREATED != pHttpTlsData->state)
    {
        KING_SysLog("%s: socket is not connect!", __FUNCTION__);
        ret = -1;
        return ret;
    }
     
    switch(sslVersion)
    {
        case 0:
            mbedtls_ssl_conf_min_version(&(pHttpTlsData->conf), MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_0);
            mbedtls_ssl_conf_max_version(&(pHttpTlsData->conf), MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_0);
            break;
            
        case 1:
            mbedtls_ssl_conf_min_version(&(pHttpTlsData->conf), MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_1);
            mbedtls_ssl_conf_max_version(&(pHttpTlsData->conf), MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_1);  
            break;
            
        case 2:
            mbedtls_ssl_conf_min_version(&(pHttpTlsData->conf), MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_2);
            mbedtls_ssl_conf_max_version(&(pHttpTlsData->conf), MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_2);
            break;
            
        case 3:
            mbedtls_ssl_conf_min_version(&(pHttpTlsData->conf), MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
            mbedtls_ssl_conf_max_version(&(pHttpTlsData->conf), MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
            break;
            
        case 4:
            mbedtls_ssl_conf_min_version(&(pHttpTlsData->conf), MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_0);
            mbedtls_ssl_conf_max_version(&(pHttpTlsData->conf), MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
            break;
        
        default:
            ret = -1;
            break;
    }

    return ret;
}

int32 httpTls_setChpherSuites(uint16 cipherSuites)
{
    int32 ret = 0;
    const mbedtls_ssl_ciphersuite_t *ciphersuite_info = NULL;
    
    if (pHttpTlsData == NULL)
    {
        KING_SysLog( "%s: http tls is not init!", __FUNCTION__);
        ret = -1;
        return ret;
    }    
    if(HTTP_TLS_STATE_CREATED != pHttpTlsData->state)
    {
        KING_SysLog("%s: socket is not connect!", __FUNCTION__);
        ret = -1;
        return ret;
    }  
    
    switch(cipherSuites)
    {
        case 0X0035:
        case 0X002F:
        case 0X0005: 
        case 0X0004:
        case 0X000A:
        case 0X003D:
        case 0XC002:
        case 0XC003:
        case 0XC004:
        case 0XC005:
        case 0XC007:
        case 0XC008: 
        case 0XC009:
        case 0XC00A:
        case 0XC011:
        case 0XC012: 
        case 0XC013:
        case 0XC014: 
        case 0XC00C:
        case 0XC00D:
        case 0XC00E:
        case 0XC00F:
        case 0XC023:
        case 0XC024:
        case 0XC025:
        case 0XC026:
        case 0XC027:  
        case 0XC028: 
        case 0XC029:
        case 0XC02A: 
        case 0XC02F:    
            ciphersuite_info = mbedtls_ssl_ciphersuite_from_id(cipherSuites);
            break;
            
        case 0XFFFF:
            pHttpTlsData->conf.ciphersuite_list[MBEDTLS_SSL_MINOR_VERSION_0] = mbedtls_ssl_list_ciphersuites();
            pHttpTlsData->conf.ciphersuite_list[MBEDTLS_SSL_MINOR_VERSION_1] = mbedtls_ssl_list_ciphersuites();
            pHttpTlsData->conf.ciphersuite_list[MBEDTLS_SSL_MINOR_VERSION_2] = mbedtls_ssl_list_ciphersuites();
            pHttpTlsData->conf.ciphersuite_list[MBEDTLS_SSL_MINOR_VERSION_3] = mbedtls_ssl_list_ciphersuites();            
            break;
            
        default:
            ret = -1;
            break;
    }
    if(NULL != ciphersuite_info)
    {
        pHttpTlsData->force_ciphersuite[0] = ciphersuite_info->id;
        pHttpTlsData->force_ciphersuite[1] = 0;
        mbedtls_ssl_conf_ciphersuites(&(pHttpTlsData->conf),(const int *)(pHttpTlsData->force_ciphersuite));
    }

    return ret;
}

int32 httpTls_setCert(HTTPTLS_CRT_T* pCrt)
{
    int32 ret = 0;
    
    if (pHttpTlsData == NULL)
    {
        KING_SysLog( "%s: http tls is not init!", __FUNCTION__);
        ret = -1;
        return ret;
    }    
    if(HTTP_TLS_STATE_CREATED != pHttpTlsData->state)
    {
        KING_SysLog("%s: socket is not connect!", __FUNCTION__);
        ret = -1;
        return ret;
    } 
    
    ret = mbedtls_x509_crt_parse(&(pHttpTlsData->cacert),
                                (const unsigned char *)pCrt->crt,
                                pCrt->len);
    if(ret < 0)
    {
        KING_SysLog("mbed TLS: SSL Cfg failed --- mbedtls_x509_crt_parse returned -0x%x", -ret);
    }

    return ret;
}

int32 httpTls_setClientCert(HTTPTLS_CRT_T* pCrt)
{
    int32 ret = 0;
    
    if (pHttpTlsData == NULL)
    {
        KING_SysLog( "%s: http tls is not init!", __FUNCTION__);
        ret = -1;
        return ret;
    }    
    if(HTTP_TLS_STATE_CREATED != pHttpTlsData->state)
    {
        KING_SysLog("%s: socket is not connect!", __FUNCTION__);
        ret = -1;
        return ret;
    } 
    
    ret = mbedtls_x509_crt_parse(&(pHttpTlsData->client_cert),
                                (const unsigned char *)pCrt->crt,
                                pCrt->len);
    if(ret < 0)
    {
        KING_SysLog("mbed TLS: SSL Cfg failed --- mbedtls_x509_crt_parse returned -0x%x", -ret);
    }

    return ret;
}

int32 httpTls_setClientKey(HTTPTLS_CRT_T* pCrt)
{
    int32 ret = 0;
    
    if (pHttpTlsData == NULL)
    {
        KING_SysLog( "%s: http tls is not init!", __FUNCTION__);
        ret = -1;
        return ret;
    }    
    if(HTTP_TLS_STATE_CREATED != pHttpTlsData->state)
    {
        KING_SysLog("%s: socket is not connect!", __FUNCTION__);
        ret = -1;
        return ret;
    }

    ret = mbedtls_pk_parse_key(&(pHttpTlsData->client_key),
                                (const unsigned char *)pCrt->crt,
                                pCrt->len, NULL, 0);
    if(ret < 0)
    {
        KING_SysLog("mbed TLS: SSL Cfg failed --- mbedtls_pk_parse_key returned -0x%x", -ret);
    }

    return ret;
}

int32 httpTls_setAuthmode(uint16 authmode)
{
    int32 ret = 0;
    
    if (pHttpTlsData == NULL)
    {
        KING_SysLog( "%s: http tls is not init!", __FUNCTION__);
        ret = -1;
        return ret;
    }    
    if(HTTP_TLS_STATE_CREATED != pHttpTlsData->state)
    {
        KING_SysLog("%s: socket is not connect!", __FUNCTION__);
        ret = -1;
        return ret;
    }

    if(authmode >= 3)
    {
        ret = -1;
        return ret;
    }
    if(authmode == 1)
    {
        mbedtls_ssl_conf_authmode(&(pHttpTlsData->conf), MBEDTLS_SSL_VERIFY_OPTIONAL);
    }
    else if(authmode == 2)
    {
        mbedtls_ssl_conf_authmode(&(pHttpTlsData->conf), MBEDTLS_SSL_VERIFY_REQUIRED);
    }
    else
    {
        mbedtls_ssl_conf_authmode(&(pHttpTlsData->conf), MBEDTLS_SSL_VERIFY_NONE);
    }
    
    return ret;
}

int32 httpTls_setIgnoreTime(int ignoretime)
{
    if (pHttpTlsData == NULL)
    {
        KING_SysLog( "%s: http tls is not init!", __FUNCTION__);
        return -1;
    }

    if(HTTP_TLS_STATE_CREATED != pHttpTlsData->state)
    {
        KING_SysLog("%s: socket is not connect!", __FUNCTION__);
        return -1;
    }
   
    if (ignoretime > 1 || ignoretime < 0)
    {
        return -1;
    }
    pHttpTlsData->ignoretime = ignoretime;

    return 0;
}

int32 httpTls_connect(int socketFd)
{
    int32 ret = 0;
    uint32_t flags;

    //KING_SysLog( "%s start.", __FUNCTION__);
    if(socketFd == -1)
    {
        KING_SysLog("%s: socket is not create!", __FUNCTION__);
        ret = -1;
        return ret;
    }
    if (pHttpTlsData == NULL)
    {
        KING_SysLog( "%s: http tls is not init!", __FUNCTION__);
        ret = -1;
        return ret;
    }    
    if(HTTP_TLS_STATE_CREATED != pHttpTlsData->state)
    {
        KING_SysLog("%s: socket is not connect!", __FUNCTION__);
        ret = -1;
        return ret;
    }
    pHttpTlsData->httpFd.fd = socketFd;

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_ssl_conf_ca_chain(&(pHttpTlsData->conf), &(pHttpTlsData->cacert), NULL);
    if (pHttpTlsData->conf.authmode == MBEDTLS_SSL_VERIFY_REQUIRED)
    {
        if(( ret = mbedtls_ssl_conf_own_cert(&(pHttpTlsData->conf), &(pHttpTlsData->client_cert), &(pHttpTlsData->client_key) ) ) != 0 )
        {
            KING_SysLog( "%s: mbedtls_ssl_conf_own_cert failed! returned %d", __FUNCTION__, ret);
            ret = -1;
            return ret;
        } 
    }
#endif

    mbedtls_ssl_conf_rng(&(pHttpTlsData->conf), mbedtls_ctr_drbg_random, &(pHttpTlsData->ctr_drbg));
    mbedtls_ssl_conf_read_timeout(&(pHttpTlsData->conf), HTTP_TLS_READ_TIMEOUT);

    if((ret = mbedtls_ssl_setup( &(pHttpTlsData->ssl), &(pHttpTlsData->conf))) != 0)
    {
        KING_SysLog( "%s: failed --- mbedtls_ssl_setup failed! returned %d", __FUNCTION__, ret);
        ret = -1;
        return ret;
    }

    mbedtls_ssl_set_bio(&(pHttpTlsData->ssl), &(pHttpTlsData->httpFd), mbedtls_net_send,
                        mbedtls_net_recv, mbedtls_net_recv_timeout);

    /** 3. Handshake **/
    KING_SysLog( "%s: Performing the SSL/TLS handshake...", __FUNCTION__);
    while(( ret = mbedtls_ssl_handshake( &(pHttpTlsData->ssl))) != 0)
    {
        if(ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            KING_SysLog( "%s: mbedtls_ssl_handshake failed! returned -0x%x", __FUNCTION__, -ret);
            ret = -1;
            return ret;
        }
    }
    KING_SysLog( "%s: Handshake ok", __FUNCTION__);
    
    /** 4. Verify the server certificate **/
    if(MBEDTLS_SSL_VERIFY_NONE != pHttpTlsData->conf.authmode)
    {
        //KING_SysLog( "%s: Verifying peer X.509 certificate...", __FUNCTION__);

        /* In real life, we probably want to bail out when ret != 0 */
        if((flags = httpTls_real_confirm(pHttpTlsData->ignoretime, mbedtls_ssl_get_verify_result( &(pHttpTlsData->ssl)))) != 0)
        {
            char vrfy_buf[512];

            KING_SysLog( "%s: Verifying peer X.509 certificate failed", __FUNCTION__);
        #if defined(MBEDTLS_X509_CRT_PARSE_C)   
            mbedtls_x509_crt_verify_info(vrfy_buf, sizeof( vrfy_buf ), "  ! ", flags);
        #endif
            KING_SysLog( "%s: %s\n", __FUNCTION__, vrfy_buf);
            ret = -1;
            return ret;
        }
        else
        {
            KING_SysLog( "%s: Verifying peer X.509 certificate ok", __FUNCTION__);
        }
    }

    mbedtls_ssl_conf_read_timeout(&(pHttpTlsData->conf), 0);
    pHttpTlsData->state = HTTP_TLS_STATE_CONNECTED;

    return ret;
}

int32 httpTls_read(int socketFd, char* pData, uint32 len, uint32 timeout_ms)
{
    int32 ret = -1;
    uint32 read_len = 0;

    //KING_SysLog( "%s: len=%d!", __FUNCTION__, len);
    if (pHttpTlsData == NULL)
    {
        KING_SysLog( "%s: http tls is not init!", __FUNCTION__);
        ret = -1;
        return ret;
    }  
    if(HTTP_TLS_STATE_CONNECTED != pHttpTlsData->state)
    {
        KING_SysLog( "%s: socket is not connect!", __FUNCTION__);
        ret = -1;
        return ret;
    }
  
    if(socketFd <= 0 || pHttpTlsData->httpFd.fd < 0)
    {
        KING_SysLog( "%s: socket fd has free!", __FUNCTION__);
        ret = -1;
        return ret;
    }
    
    if (socketFd != pHttpTlsData->httpFd.fd) 
    {
        KING_SysLog( "%s: socket fd is error!", __FUNCTION__);
        ret = -1;
        return ret;
    }
    
    if(pData == NULL || len == 0)
    {
        KING_SysLog( "%s: read data buf is NULL or len is 0!", __FUNCTION__);
        ret = -1;
        return ret;
    }

    if (timeout_ms > 0)
    {
        mbedtls_ssl_conf_read_timeout(&(pHttpTlsData->conf), timeout_ms);
    }

    do
    {
        ret = mbedtls_ssl_read(&(pHttpTlsData->ssl), (unsigned char *)(pData + read_len), len-read_len );
        if( ret < 0 )
        {
            KING_SysLog( "%s: mbedtls_ssl_read returned 0x%x", __FUNCTION__, ret );
            break;
        }
        else if(ret == 0)
        { 
            KING_SysLog( "%s: mbedtls_ssl_read return 0!The connection indicated an EOF!", __FUNCTION__);
            break;
        }
        else
        {
            //KING_SysLog( "%s: mbedtls_ssl_read len %d", __FUNCTION__, ret);
            read_len = read_len + ret;
        }
    }while(0);
    
    if(read_len > 0)
    {
        ret = read_len;
        KING_SysLog( "%s: mbedtls_ssl_read want len %d,act len %d.", __FUNCTION__, len, read_len );
    }
    if(ret  < 0)
    {
        ret = -1;
    }
  
    return ret;
}
 
int32 httpTls_write(int socketFd, char* pData, uint32 len)
{
    int32 ret = -1;
    
    //KING_SysLog( "%s: len=%d!", __FUNCTION__, len);
    if (pHttpTlsData == NULL)
    {
        KING_SysLog( "%s: http tls is not init!", __FUNCTION__);
        ret = -1;
        return ret;
    }
    if(HTTP_TLS_STATE_CONNECTED != pHttpTlsData->state)
    {
        KING_SysLog( "%s: socket is not connect!", __FUNCTION__);
        ret = -1;
        return ret;
    }
  
    if(socketFd <= 0 || pHttpTlsData->httpFd.fd < 0)
    {
        KING_SysLog( "%s: socket fd has free!", __FUNCTION__);
        ret = -1;
        return ret;
    }

    if (socketFd != pHttpTlsData->httpFd.fd) 
    {
        KING_SysLog( "%s: socket fd is error!", __FUNCTION__);
        ret = -1;
        return ret;
    }

    if(pData == NULL || len == 0)
    {
        KING_SysLog( "%s: read data buf is NULL or len is 0!", __FUNCTION__);
        ret = -1;
        return ret;
    }    
    ret = mbedtls_ssl_write( &(pHttpTlsData->ssl), (unsigned char *)pData, len );
    if(ret < 0)
    {
        KING_SysLog( "%s: mbedtls_ssl_write fail!", __FUNCTION__);
        ret = -1;
    }
    
    return ret;
}

int32 httpTls_close(void)
{
    //KING_SysLog( "%s start!", __FUNCTION__);
    if (pHttpTlsData != NULL)
    {
        mbedtls_ssl_close_notify(&(pHttpTlsData->ssl));
        //mbedtls_net_free(&(pHttpTlsData->httpFd));
        pHttpTlsData->httpFd.fd = -1;
    #if defined(MBEDTLS_X509_CRT_PARSE_C)    
        mbedtls_x509_crt_free(&(pHttpTlsData->cacert));
        mbedtls_x509_crt_free(&(pHttpTlsData->client_cert));
        mbedtls_pk_free(&(pHttpTlsData->client_key));
    #endif    
        mbedtls_ssl_free(&(pHttpTlsData->ssl) );
        mbedtls_ssl_config_free(&(pHttpTlsData->conf));
        mbedtls_ctr_drbg_free(&(pHttpTlsData->ctr_drbg));
        mbedtls_entropy_free(&(pHttpTlsData->entropy));
        free(pHttpTlsData);
        pHttpTlsData = NULL;
    }
    //KING_SysLog( "%s end!", __FUNCTION__);
    return 0;
}

uint8 httTls_getState(void)
{
    if (pHttpTlsData == NULL)
    {
        return HTTP_TLS_STATE_NONE;
    }

    return pHttpTlsData->state;
}
