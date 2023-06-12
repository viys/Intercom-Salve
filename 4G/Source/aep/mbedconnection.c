/*******************************************************************************
 *
 * Copyright (c) 2015 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    Christian Renz - Please refer to git log
 *
 *******************************************************************************/

#include "mbedconnection.h"
#include "lwm2m/internals.h"

#include "mbedtls/include/net_sockets.h"
#include "mbedtls/include/timing.h"
#include "mbedtls/include/debug.h"

#define COAP_PORT "5683"
#define COAPS_PORT "5684"
#define URI_LENGTH 256

#define MAX_PACKET_SIZE 1024

#if (defined WITH_TINYDTLS) || (defined WITH_MBEDTLS)

/********************* Security Obj Helpers **********************/
char *security_get_uri(lwm2m_object_t *obj, int instanceId, char *uriBuffer, int bufferSize)
{
    int size = 1;
    lwm2m_data_t *dataP = lwm2m_data_new(size);
    dataP->id = 0; // security server uri

    obj->readFunc(instanceId, &size, &dataP, obj);
    if (dataP != NULL &&
        dataP->type == LWM2M_TYPE_STRING &&
        dataP->value.asBuffer.length > 0)
    {
        if (bufferSize > dataP->value.asBuffer.length)
        {
            memset(uriBuffer, 0, dataP->value.asBuffer.length + 1);
            strncpy(uriBuffer, dataP->value.asBuffer.buffer, dataP->value.asBuffer.length);
            lwm2m_data_free(size, dataP);
            return uriBuffer;
        }
    }
    lwm2m_data_free(size, dataP);
    return NULL;
}

int64_t security_get_mode(lwm2m_object_t *obj, int instanceId)
{
    int64_t mode;
    int size = 1;
    lwm2m_data_t *dataP = lwm2m_data_new(size);
    dataP->id = 2; // security mode

    obj->readFunc(instanceId, &size, &dataP, obj);
    if (0 != lwm2m_data_decode_int(dataP, &mode))
    {
        lwm2m_data_free(size, dataP);
        return mode;
    }

    lwm2m_data_free(size, dataP);
    LogPrintf("Unable to get security mode : use not secure mode");
    return LWM2M_SECURITY_MODE_NONE;
}

char *security_get_public_id(lwm2m_object_t *obj, int instanceId, int *length)
{
    int size = 1;
    lwm2m_data_t *dataP = lwm2m_data_new(size);
    if (dataP == NULL)
        return NULL;
    dataP->id = 3; // public key or id

    obj->readFunc(instanceId, &size, &dataP, obj);
    if (dataP->type == LWM2M_TYPE_OPAQUE)
    {
        char *buff;

        buff = (char *)lwm2m_malloc(dataP->value.asBuffer.length);
        if (buff != 0)
        {
            memcpy(buff, dataP->value.asBuffer.buffer, dataP->value.asBuffer.length);
            *length = dataP->value.asBuffer.length;
        }
        lwm2m_data_free(size, dataP);

        return buff;
    }
    else
    {
        lwm2m_data_free(size, dataP);
        return NULL;
    }
}

char *security_get_secret_key(lwm2m_object_t *obj, int instanceId, int *length)
{
    int size = 1;
    lwm2m_data_t *dataP = lwm2m_data_new(size);
    if (dataP == NULL)
        return NULL;
    dataP->id = 5; // secret key
    LogPrintf("security_get_secret_key");
    obj->readFunc(instanceId, &size, &dataP, obj);
    if (dataP->type == LWM2M_TYPE_OPAQUE)
    {
        char *buff;
        buff = (char *)lwm2m_malloc(dataP->value.asBuffer.length);
        if (buff != 0)
        {
            memcpy(buff, dataP->value.asBuffer.buffer, dataP->value.asBuffer.length);
            *length = dataP->value.asBuffer.length;
        }
        lwm2m_data_free(size, dataP);
        return buff;
    }
    else
    {
        lwm2m_data_free(size, dataP);
        return NULL;
    }
}

/********************* Security Obj Helpers Ends **********************/
#endif

int send_data(dtls_connection_t *connP,
              uint8 *buffer,
              size_t length)
{
    int ret;
    int offset = 0;
    
    LOG_ARG("send_data sock:%d,addr:0x%x,addrLen:%d,length:%d",connP->sock,&(connP->addr),connP->addrLen,length);

    // LWIP low level just malloc a send buffer, and copy the data into the buffer.
    ret = KING_SocketSendto(connP->sock, buffer + offset, length - offset, 0, &(connP->addr), connP->addrLen);
    if (ret == -1)
    {
        LOG_ARG("errno: 0x%x",KING_GetLastErrCode());
        return -1;
    }

    connP->lastSend = lwm2m_gettime();
    return 0;
}


int create_socket(const char *portStr, int ai_family)
{
    int s = -1;
    int ret = -1;
    int port_nr = atoi(portStr);

    LOG("Enterring create_socket( )\r\n");

    if ((port_nr <= 0) || (port_nr > 0xffff)) {
      return -1;
    }

    ret = KING_SocketCreate(ai_family, SOCK_DGRAM, 0, &s);
    if (ret == 0)
    {
        LOG_ARG("Create Socket successfully! %d\r\n",s);
    }
    return s;
}

dtls_connection_t *connection_new_incoming(dtls_connection_t *connList,
                                           int sock,
                                           const SOCK_ADDR_ALIGNED_U *addr,
                                           size_t addrLen)
{
    dtls_connection_t *connP = NULL;

    connP = (dtls_connection_t *)lwm2m_malloc(sizeof(dtls_connection_t));
    if (connP != NULL)
    {
        memset(connP, 0, sizeof(dtls_connection_t));
        connP->sock = sock;
        memcpy(&(connP->addr), addr, addrLen);
        connP->addrLen = addrLen;
        connP->next = connList;
        LOG("new_incoming");
    }
    else
    {
        LOG("new_incoming,malloc failed!");
    }

    return connP;
}

dtls_connection_t *connection_find(dtls_connection_t *connList,
                                   const sockaddr_storage *addr,
                                   size_t addrLen)
{
    dtls_connection_t *connP;

    connP = connList;
    return connP;
}

#if (defined WITH_TINYDTLS) || (defined WITH_MBEDTLS)
dtls_connection_t *connection_create(dtls_connection_t *connList,
                                     int sock,
                                     lwm2m_object_t *securityObj,
                                     int instanceId,
                                     lwm2m_context_t *lwm2mH,
                                     int addressFamily)
{
    SOCK_IN_ADDR_T  ipAddr;
    SOCK_ADDR_T     sockAddr;
    int ret = -1;
    int s;
    dtls_connection_t *connP = NULL;
    char uriBuf[URI_LENGTH];
    char *uri;
    char *host;
    char *port;

    uri = security_get_uri(securityObj, instanceId, uriBuf, URI_LENGTH);
    if (uri == NULL)
        return NULL;

    // parse uri in the form "coaps://[host]:[port]"
    char *defaultport;
    if (0 == strncmp(uri, "coaps://", strlen("coaps://")))
    {
        host = uri + strlen("coaps://");
        defaultport = COAPS_PORT;
    }
    else if (0 == strncmp(uri, "coap://", strlen("coap://")))
    {
        host = uri + strlen("coap://");
        defaultport = COAP_PORT;
    }
    else
    {
        return NULL;
    }
    port = strrchr(host, ':');
    if (port == NULL)
    {
        port = defaultport;
    }
    else
    {
        // remove brackets
        if (host[0] == '[')
        {
            host++;
            if (*(port - 1) == ']')
            {
                *(port - 1) = 0;
            }
            else
            {
                return NULL;
            }
        }
        // split strings
        *port = 0;
        port++;
    }
    // Host is IP address, skip DNS step
    if (-1 == KING_ipaton(host, &ipAddr))
    {
        LOG_ARG("convert host [%s] fail!\r\n", host);
    }
    sockAddr.sa_family = AF_INET;
    sockAddr.ip_addr = ipAddr.u_addr.ip4.addr;
    sockAddr.port = KING_htons(atoi(port));

    // Create socket
    KING_SocketCreate(AF_INET, SOCK_DGRAM, 0, 0, &sock);
    if (sock >= 0)
    {
        connP = connection_new_incoming(connList, sock, &sockAddr, servinfo->ai_addrlen);
        //close(s);
        //connP->sock_id = s;
        // do we need to start tinydtls?
        if (connP != NULL)
        {
            LOG(" connP != NULL");
            connP->securityObj = securityObj;
            connP->securityInstId = instanceId;
            connP->lwm2mH = lwm2mH;

            if (security_get_mode(connP->securityObj, connP->securityInstId) != LWM2M_SECURITY_MODE_NONE)
            {
                connP->issecure = 1;
                //int ret = get_dtls_context(connP);
                ret = -1; // TODO
                if (ret != 0)
                {
                    LOG_ARG(" lwm2m get_dtls_context ret=%d", ret);
                    dtls_connection_t *targetP = connP;
                    connP = targetP->next;
                    targetP->next = NULL;
                    connection_free(targetP);
                    return NULL;
                }
            }
            else
            {
                // no dtls session
                connP->issecure = 0;
            }
        }
    }

    return connP;
}
#else
//newConnP = connection_create(dataP->connList, dataP->sock, host, port, dataP->addressFamily);
dtls_connection_t * connection_create(dtls_connection_t * connList,
                                 int sock,
                                 char * host,
                                 char * port,
                                 int addressFamily)
{
    SOCK_IN_ADDR_T  ipAddr;
    dtls_connection_t *connP = NULL;
    union sockaddr_aligned sock_addr;
    memset(&ipAddr, 0x00, sizeof(SOCK_IN_ADDR_T));
    LOG_ARG("host [%s]  port [%s] sock %d addressFamily %d\r\n", host,port,sock,addressFamily);
    if (-1 == KING_ipaton(host, &ipAddr))
    {
        LOG_ARG("convert host [%s] fail!\r\n", host);
    }
    if(ipAddr.type == IP_ADDR_TYPE_V4)
    {
        struct sockaddr_in *sock_addr4 = (struct sockaddr_in *)&(sock_addr);
        memset(&sock_addr, 0, sizeof(struct sockaddr));
        sock_addr4->sin_family = AF_INET;
        sock_addr4->sin_port = KING_htons(atoi(port));
        sock_addr4->sin_addr.addr = ipAddr.u_addr.ip4.addr;
        sock_addr4->sin_len = sizeof(struct sockaddr_in);
        connP = connection_new_incoming(connList, sock, &sock_addr, sizeof(struct sockaddr_in));
    }
    return connP;
}
#endif

void connection_free(dtls_connection_t *connList)
{
//    LOG("Enter connection_free");
//    while (connList)
//    {
//        LOG("connList != NULL");

//        dtls_connection_t *nextP;

//        nextP = connList->next;

//        if (connList->server_fd)
//        {

//            mbedtls_net_free(connList->server_fd);
//            lwm2m_free(connList->server_fd);
//            connList->server_fd = NULL;
//        }

//        if (connList->entropy)
//        {
//            mbedtls_entropy_free(connList->entropy);
//            lwm2m_free(connList->entropy);
//            connList->entropy = NULL;
//        }
//        if (connList->conf)
//        {
//            mbedtls_ssl_config_free(connList->conf);
//            lwm2m_free(connList->conf);
//            connList->conf = NULL;
//        }
//        if (connList->cacert)
//        {
//            mbedtls_x509_crt_free(connList->cacert);
//            lwm2m_free(connList->cacert);
//            connList->cacert = NULL;
//        }

//        if (connList->ssl)
//        {
//            if (connList->ssl->p_timer)
//                lwm2m_free(connList->ssl->p_timer);
//            mbedtls_ssl_free(connList->ssl);
//            lwm2m_free(connList->ssl);
//            connList->ssl = NULL;
//        }
//        if (connList->ctr_drbg)
//        {
//            mbedtls_ctr_drbg_free(connList->ctr_drbg);
//            lwm2m_free(connList->ctr_drbg);
//            connList->ctr_drbg = NULL;
//        }

//        LOG_ARG("M2M# connection_free connList = %p", connList);

//        lwm2m_free(connList);
//        connList = nextP;
//    }
}

int connection_send(dtls_connection_t *connP, uint8 *buffer, size_t length)
{

    if (connP->issecure == 0)
    {
        if (0 != send_data(connP, buffer, length))
        {
            return -1;
        }
    }
    else
    {
        // TODO if (-1 == mbedtls_ssl_write(connP->ssl, buffer, length))
        {
            return -1;
        }
    }
    LOG("connection_send success\r\n");
    return 0;
}

int connection_handle_packet(dtls_connection_t *connP, uint8 *buffer, size_t numBytes)
{

    if (connP->issecure != 0) //(connP->dtlsSession != NULL)
    {
        LOG("security mode");
#if 0
        mbedtls_net_set_nonblock(connP->server_fd);
        //mbedtls_ssl_set_bio( connList->ssl, connList, mbeddtls_lwm2m_send, mbeddtls_lwm2m_recv, mbeddtls_lwm2m_recv_timeout);
        mbedtls_ssl_set_bio(connP->ssl, connP, mbeddtls_lwm2m_send, mbeddtls_lwm2m_recv, NULL);
        int result = mbedtls_ssl_read(connP->ssl, buffer, MAX_PACKET_SIZE);
        if (result <= 0)
        {
            LogPrintf("error dtls handling message %d\n", result);
            return result;
        }
        LogPrintf("after mbedtls_ssl_read,relsult=%d", result);
        lwm2m_handle_packet(connP->lwm2mH, buffer, result, (void *)connP);
#endif
        return 0;
    }
    else
    {
        // no security, just give the plaintext buffer to liblwm2m
        lwm2m_handle_packet(connP->lwm2mH, buffer, numBytes, (void *)connP);
        return 0;
    }
}

uint8 lwm2m_buffer_send(void *sessionH,
                          uint8 *buffer,
                          size_t length,
                          void *userdata)
{
    dtls_connection_t *connP = (dtls_connection_t *)sessionH;

    if (connP == NULL)
    {
        LOG_ARG("#> failed sending %lu bytes, missing connection\r\n", length);
        return COAP_500_INTERNAL_SERVER_ERROR;
    }

    if (-1 == connection_send(connP, buffer, length))
    {
        LOG_ARG("#> failed sending %lu bytes\r\n", length);
        return COAP_500_INTERNAL_SERVER_ERROR;
    }
    LOG("lwm2m_buffer_send success\r\n");
    return COAP_NO_ERROR;
}

bool lwm2m_session_is_equal(void *session1,
                            void *session2,
                            void *userData)
{
    return (session1 == session2);
}


