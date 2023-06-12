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

extern int sockaddr_cmp(struct sockaddr *x, struct sockaddr *y);

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
            KING_strncpy(uriBuffer, dataP->value.asBuffer.buffer, dataP->value.asBuffer.length);
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
            KING_memcpy(buff, dataP->value.asBuffer.buffer, dataP->value.asBuffer.length);
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
            KING_memcpy(buff, dataP->value.asBuffer.buffer, dataP->value.asBuffer.length);
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


#if 0
static int net_would_block(const mbedtls_net_context *ctx)
{
    /*
     * Never return 'WOULD BLOCK' on a non-blocking socket
     */
    if ((fcntl(ctx->fd, F_GETFL, 0) & O_NONBLOCK) != O_NONBLOCK)
        return (0);

    switch (errno)
    {
#if defined EAGAIN
    case EAGAIN:
#endif
#if defined EWOULDBLOCK && EWOULDBLOCK != EAGAIN
    case EWOULDBLOCK:
#endif
        return (1);
    }
    return (0);
}

/*
 * Write at most 'len' characters
 */
int mbeddtls_lwm2m_send(void *connP, const unsigned char *buf, size_t len)
{
    LogPrintf("mbeddtls_lwm2m_send enter");
    dtls_connection_t *ctx = (dtls_connection_t *)connP;
    int fd = ctx->sock;
    LogPrintf("mbeddtls_lwm2m_send enter,fd=%d", fd);
    mbedtls_net_context *mbed_ctx = ctx->server_fd;

    if (fd < 0)
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);

    int nbSent = -1;
    size_t offset;

//char s[INET6_ADDRSTRLEN];add LWIP_IPV4_ON/LWIP_IPV6_ON
#if defined(LWIP_IPV4_ON) && defined(LWIP_IPV6_ON)
    char s[INET6_ADDRSTRLEN];
#else
#ifdef LWIP_IPV4_ON
    char s[INET_ADDRSTRLEN];
#else
    char s[INET6_ADDRSTRLEN];
#endif
#endif
    uint8 port;

    s[0] = 0;
#ifdef LWIP_IPV4_ON

    if (AF_INET == ctx->addr.family)
    {
        struct sockaddr *saddr = (struct sockaddr *)&ctx->addr;
        inet_ntop(saddr->family, &saddr->sin_addr, s, INET_ADDRSTRLEN);
        port = saddr->port;
    }
#endif
#ifdef LWIP_IPV6_ON
    if (AF_INET6 == ctx->addr6.sin6_family)
    {
        struct sockaddr6 *saddr = (struct sockaddr6 *)&ctx->addr;
        inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
        port = saddr->sin6_port;
    }
#endif
    LogPrintf("Sending %d bytes to ip:%s\r\n", len, s);

    offset = 0;
    while (offset != len)
    {
        nbSent = sendto(fd, buf + offset, len - offset, 0, (struct sockaddr *)&(ctx->addr), ctx->addrLen);
        if (nbSent < 0)
        {

            if (net_would_block(mbed_ctx) != 0)
                return (MBEDTLS_ERR_SSL_WANT_WRITE);

#if (defined(_WIN32) || defined(_WIN32_WCE)) && !defined(EFIX64) && \
    !defined(EFI32)
            if (WSAGetLastError() == WSAECONNRESET)
                return (MBEDTLS_ERR_NET_CONN_RESET);
#else
            if (errno == EPIPE || errno == ECONNRESET)
                return (MBEDTLS_ERR_NET_CONN_RESET);

            if (errno == EINTR)
                return (MBEDTLS_ERR_SSL_WANT_WRITE);
#endif
            LogPrintf("++++Send error errno:%d\r\n", errno);
            return (MBEDTLS_ERR_NET_SEND_FAILED);
        }
        offset += nbSent;
    }
    LogPrintf("return len = %d", offset);
    return offset;
}

/*
 * Read at most 'len' characters
 */
int mbeddtls_lwm2m_recv(void *ctx, unsigned char *buffer, size_t len)
{

    int fd = ((dtls_connection_t *)ctx)->sock;
    LogPrintf("mbeddtls_lwm2m_recv enter,fd=%d", fd);
    mbedtls_net_context *mbed_ctx = ((dtls_connection_t *)ctx)->server_fd;
    if (fd < 0)
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);

    /*
     * This part will set up an interruption until an event happen on SDTIN or the socket until "tv" timed out (set
     * with the precedent function)
     */
    int numBytes;

    sockaddr_storage addr;
    socklen_t addrLen;
    addrLen = sizeof(addr);

    /*
             * We retrieve the data received
             */
    numBytes = recvfrom(fd, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&addr, &addrLen);

    if (0 > numBytes)
    {

        if (net_would_block(mbed_ctx) != 0)
            return (MBEDTLS_ERR_SSL_WANT_READ);

#if (defined(_WIN32) || defined(_WIN32_WCE)) && !defined(EFIX64) && \
    !defined(EFI32)
        if (WSAGetLastError() == WSAECONNRESET)
            return (MBEDTLS_ERR_NET_CONN_RESET);
#else
        if (errno == EPIPE || errno == ECONNRESET)
            return (MBEDTLS_ERR_NET_CONN_RESET);

        if (errno == EINTR)
            return (MBEDTLS_ERR_SSL_WANT_READ);
#endif

        return (MBEDTLS_ERR_NET_RECV_FAILED);
    }
    else if (0 < numBytes)
    {
        LogPrintf("mbeddtls_lwm2m_recv ,numBytes=%d", numBytes);
//char s[INET6_ADDRSTRLEN];add LWIP_IPV4_ON/LWIP_IPV6_ON
#if defined(LWIP_IPV4_ON) && defined(LWIP_IPV6_ON)
        char s[INET6_ADDRSTRLEN];
#else
#ifdef LWIP_IPV4_ON
        char s[INET_ADDRSTRLEN];
#else
        char s[INET6_ADDRSTRLEN];
#endif
#endif
        uint8 port = 0;
#ifdef LWIP_IPV4_ON
        if (AF_INET == addr.ss_family)
        {
            struct sockaddr *saddr = (struct sockaddr *)&addr;
            inet_ntop(saddr->family, &saddr->sin_addr, s, INET_ADDRSTRLEN);
            port = saddr->port;
        }
#endif
#ifdef LWIP_IPV6_ON
        if (AF_INET6 == addr.ss_family)
        {
            struct sockaddr6 *saddr = (struct sockaddr6 *)&addr;
            inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
            port = saddr->sin6_port;
        }
#endif
        LogPrintf("mbeddtls_lwm2m_recv:%d bytes received from [%s]:%hu\r\n", numBytes, s, ntohs(port));
    }
    return numBytes;
}

int mbeddtls_lwm2m_recv_timeout(void *ctx, unsigned char *buf, size_t len,
                                uint32 timeout)
{
    int ret;
    struct timeval tv;
    fd_set read_fds;
    uint32 selectms = 500;
    //int fd = ((mbedtls_net_context *) ctx)->fd;
    int fd = ((dtls_connection_t *)ctx)->sock;
    //mbedtls_net_context *mbed_ctx = ctx->server_fd;
    LogPrintf("mbedtls_lwm2m_recv_timeout entering");
    if (fd < 0)
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);

    //tv.tv_sec = timeout / 1000;
    //tv.tv_usec = (timeout % 1000) * 1000;

    //ret = select(fd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv);
    uint32 trytimes = timeout/selectms;
    uint32 leftms = timeout%selectms;
    tv.tv_sec = selectms / 1000;
    tv.tv_usec = (tv.tv_sec == 0)? (selectms*1000):(selectms%1000 * 1000);
    for(int i = 0; i< trytimes ; i++){
        if(((dtls_connection_t *)ctx)->ssl->isquit == 1)
            return MBEDTLS_ERR_SSL_QUIT_FORCED;
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);
        ret = select(fd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv);
        if(ret != 0)
            break;
    }
    if(ret == 0 && leftms > 0){
        if(((dtls_connection_t *)ctx)->ssl->isquit == 1)
            return MBEDTLS_ERR_SSL_QUIT_FORCED;
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);
        tv.tv_sec = leftms/1000;
        tv.tv_usec = (tv.tv_sec == 0)?(leftms*1000):(leftms%1000 * 1000);
        ret = select(fd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv);
    }
    /* Zero fds ready means we timed out */
    if (ret == 0)
        return (MBEDTLS_ERR_SSL_TIMEOUT);

    if (ret < 0)
    {
        if (errno == EINTR)
            return (MBEDTLS_ERR_SSL_WANT_READ);

        return (MBEDTLS_ERR_NET_RECV_FAILED);
    }

    /* This call will not block */
    return (mbeddtls_lwm2m_recv(ctx, buf, len));
}

static void lwm2m_debug(void *ctx, int level,
                        const char *file, int line,
                        const char *param)
{
    ((void)level);
    LogPrintf("%s", param);
}


INT32 get_dtls_context(dtls_connection_t *connList)
{
    int ret;
    const char *pers = "lwm2mclient";
    LogPrintf("Enterring get_dtls_context");

    connList->server_fd = lwm2m_malloc(sizeof(mbedtls_net_context));
    if (connList->server_fd == NULL)
    {
        LogPrintf("connList->server_fd MALLOC FAILED:%d!\n", sizeof(mbedtls_net_context));
        return -1;
    }
    mbedtls_net_init(connList->server_fd);

    connList->ssl = lwm2m_malloc(sizeof(mbedtls_ssl_context));
    if (connList->ssl == NULL)
    {
        LogPrintf("connList->ssl MALLOC FAILED:%d!\n", sizeof(mbedtls_ssl_context));
        return -1;
    }
    mbedtls_ssl_init(connList->ssl);
    connList->conf = lwm2m_malloc(sizeof(mbedtls_ssl_config));
    if (connList->conf == NULL)
    {
        LogPrintf("connList->conf MALLOC FAILED:%d!\n", sizeof(mbedtls_ssl_config));
        return -1;
    }
    mbedtls_ssl_config_init(connList->conf);

    connList->cacert = lwm2m_malloc(sizeof(mbedtls_x509_crt));
    if (connList->cacert == NULL)
    {
        LogPrintf("connList->cacert MALLOC FAILED:%d!\n", sizeof(mbedtls_x509_crt));
        return -1;
    }
    mbedtls_x509_crt_init(connList->cacert);

    connList->ctr_drbg = lwm2m_malloc(sizeof(mbedtls_ctr_drbg_context));
    if (connList->ctr_drbg == NULL)
    {
        LogPrintf("connList->ctr_drbg MALLOC FAILED:%d!\n", sizeof(mbedtls_ctr_drbg_context));
        return -1;
    }
    mbedtls_ctr_drbg_init(connList->ctr_drbg);

    connList->entropy = lwm2m_malloc(sizeof(mbedtls_entropy_context));
    if (connList->entropy == NULL)
    {
        LogPrintf("connList->entropy MALLOC FAILED:%d!\n", sizeof(mbedtls_entropy_context));
        return -1;
    }
    mbedtls_entropy_init(connList->entropy);

    if ((ret = mbedtls_ctr_drbg_seed(connList->ctr_drbg, mbedtls_entropy_func, connList->entropy,
                                     (const unsigned char *)pers, KING_strlen(pers))) != 0)
    {
        LogPrintf("mbedtls_ctr_drbg_seed failed...,ret=%d\n", ret);
        return ret;
    }

    connList->server_fd->fd = connList->sock;
    LogPrintf("mbedtls_use the sock=%d\n", connList->sock);
    if ((ret = mbedtls_ssl_config_defaults(connList->conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        LogPrintf("mbedtls_ssl_config_defaults failed ret = %d\n", ret);
        return ret;
    }
    ret = mbedtls_net_set_block(connList->server_fd);
    mbedtls_timing_delay_context *timer = lwm2m_malloc(sizeof(mbedtls_timing_delay_context));
    if (timer == NULL)
        LogPrintf("get_dtls_context timer malloc failed:%d\n", sizeof(mbedtls_timing_delay_context));
    mbedtls_ssl_set_timer_cb(connList->ssl, timer, mbedtls_timing_set_delay,
                             mbedtls_timing_get_delay);

    int length = 0;
    int id_len = 0;
    char *psk = security_get_secret_key(connList->securityObj, connList->securityInstId, &length);

    char *psk_id = security_get_public_id(connList->securityObj, connList->securityInstId, &id_len);

    if ((ret = mbedtls_ssl_conf_psk(connList->conf, psk, length,
                                    psk_id,
                                    id_len)) != 0)
    {
        LogPrintf(" failed! mbedtls_ssl_conf_psk returned %d\n\n", ret);
        return ret;
    }
    LogPrintf(" mbedtls_ssl_conf_psk returned %d,psk=%s,psk_id=%s\n\n", ret, psk, psk_id);
    lwm2m_free(psk);
    lwm2m_free(psk_id);
    /* OPTIONAL is not optimal for security,
    * but makes interop easier in this simplified example */
    mbedtls_ssl_conf_authmode(connList->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(connList->conf, connList->cacert, NULL);
    mbedtls_ssl_conf_rng(connList->conf, mbedtls_ctr_drbg_random, connList->ctr_drbg);
    mbedtls_ssl_conf_dbg(connList->conf, lwm2m_debug, NULL);
    mbedtls_debug_set_threshold(3);

    if ((ret = mbedtls_ssl_setup(connList->ssl, connList->conf)) != 0)
    {
        LogPrintf("mbedtls_ssl_setup failed ret = %d\n", ret);
        return ret;
    }
    mbedtls_ssl_conf_read_timeout(connList->conf, 60000);
    mbedtls_ssl_set_bio(connList->ssl, connList, mbeddtls_lwm2m_send, mbeddtls_lwm2m_recv, mbeddtls_lwm2m_recv_timeout);
    /*
    * 4. Handshake
    */

    while ((ret = mbedtls_ssl_handshake(connList->ssl)) != 0)
    {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            LogPrintf(" failed ! mbedtls_ssl_handshake returned %x\n\n", -ret);
            return ret;
        }
    }
    LogPrintf(" success ! mbedtls_ssl_handshake returned %x\n\n", -ret);
    LogPrintf(" ok\n");
    return 0;
}
#endif

int send_data(dtls_connection_t *connP,
              uint8 *buffer,
              size_t length)
{
    int ret;
    int offset = 0;

    // LWIP low level just malloc a send buffer, and copy the data into the buffer.
    ret = KING_SocketSendto(connP->sock, buffer + offset, length - offset, 0, (struct sockaddr *)&(connP->addr), connP->addrLen);
    if (ret == -1)
        return -1;

    connP->lastSend = lwm2m_gettime();
    return 0;
}


int create_socket(const char *portStr, int ai_family)
{
    int s = -1;
    int ret = -1;
    int port_nr = KING_atoi(portStr);
    //struct sockaddr bindAddr = {0};

    LogPrintf("Enterring create_socket( )\r\n");

    if ((port_nr <= 0) || (port_nr > 0xffff)) {
      return -1;
    }

    ret = KING_SocketCreate(ai_family, SOCK_DGRAM, 0, 0, &s);
    if (ret == 0)
    {
        LogPrintf("Create Socket successfully!\r\n");
#if 0 // TODO do we really need to bind()? -Nestor
        LogPrintf("Try to bind socket to local port [%d]... \r\n", port_nr);
        bindAddr.family = AF_INET;
        bindAddr.port = KING_htons((uint16)port_nr);
        bindAddr.ip_addr = 0; // IP_ADDR_ANY
        if(-1 == KING_SocketBind(s, (const struct sockaddr *)&bindAddr, sizeof(struct sockaddr)))
        {
            LogPrintf("Fail to bind socket to local port[%d], errorCode[0x%X]\r\n",
                    port_nr, KING_GetLastErrCode());
            KING_SocketClose(s);
            s = -1;
        }
#endif
    }
    return s;
}

dtls_connection_t *connection_new_incoming(dtls_connection_t *connList,
                                           int sock,
                                           const struct sockaddr *addr,
                                           size_t addrLen)
{
    dtls_connection_t *connP = NULL;

    connP = (dtls_connection_t *)lwm2m_malloc(sizeof(dtls_connection_t));
    if (connP != NULL)
    {
        memset(connP, 0, sizeof(dtls_connection_t));
        connP->sock = sock;
        KING_memcpy(&(connP->addr), addr, addrLen);
        connP->addrLen = addrLen;
        connP->next = connList;
        LogPrintf("new_incoming");

        /*      connP->dtlsSession = (session_t *)lwm2m_malloc(sizeof(session_t));

#ifdef LWIP_IPV6_ON
				connP->dtlsSession->addr.sin6 = connP->addr6;
#else
				connP->dtlsSession->addr.sin = connP->addr;
#endif

        connP->dtlsSession->size = connP->addrLen;
        connP->lastSend = lwm2m_gettime();*/
    }
    else
    {
        LogPrintf("new_incoming,malloc failed!");
    }

    return connP;
}

int sockaddr_cmp(struct sockaddr *x, struct sockaddr *y)
{
    if (x->family != y->family) return FALSE;
    if (x->port != y->port) return FALSE;

    return (x->ip_addr == y->ip_addr);
}

dtls_connection_t *connection_find(dtls_connection_t *connList,
                                   const sockaddr_storage *addr,
                                   size_t addrLen)
{
    dtls_connection_t *connP;

    connP = connList;
/* TODO we have only one connection! -Nestor
    while (connP != NULL)
    {

        if (sockaddr_cmp((struct sockaddr *)(&connP->addr), (struct sockaddr *)addr))
        {
            return connP;
        }

        connP = connP->next;
    }
*/
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
    struct sockaddr *sa = NULL;
    socklen_t sl = 0;
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
    if (0 == strncmp(uri, "coaps://", KING_strlen("coaps://")))
    {
        host = uri + KING_strlen("coaps://");
        defaultport = COAPS_PORT;
    }
    else if (0 == strncmp(uri, "coap://", KING_strlen("coap://")))
    {
        host = uri + KING_strlen("coap://");
        defaultport = COAP_PORT;
    }
    else
    {
        return NULL;
    }
    port = KING_strrchr(host, ':');
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

    //域名解析 -Nestor
    //if (0 != getaddrinfo(host, port, &hints, &servinfo) || servinfo == NULL)
    //    return NULL;
    // ret = KING_NetGetHostByName(0, host, pAddr, 5000, dw_get_host_by_name_handler);
    // Host is IP address, skip DNS step
    if (-1 == KING_ipaton(host, &ipAddr))
    {
        LogPrintf("convert host [%s] fail!\r\n", host);
    }
    sockAddr.family = AF_INET;
    sockAddr.ip_addr = ipAddr.u_addr.ip4.addr;
    sockAddr.port = KING_htons(KING_atoi(port));

    // Create socket
    KING_SocketCreate(AF_INET, SOCK_DGRAM, 0, 0, &sock);
    // Connect
    //
    //

    if (sock >= 0)
    {
        connP = connection_new_incoming(connList, sock, servinfo->ai_addr, servinfo->ai_addrlen);
        //close(s);
        //connP->sock_id = s;
        // do we need to start tinydtls?
        if (connP != NULL)
        {
            LogPrintf(" connP != NULL");
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
                    LogPrintf(" lwm2m get_dtls_context ret=%d", ret);
                    if (NULL != servinfo)
                        freeaddrinfo(servinfo);
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

    if (NULL != servinfo)
        freeaddrinfo(servinfo);
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
    SOCK_ADDR_T     sockAddr;
    //int ret = -1;
    //int sockid = -1;
    dtls_connection_t *connP = NULL;

    if (-1 == KING_ipaton(host, &ipAddr))
    {
        LogPrintf("convert host [%s] fail!\r\n", host);
    }
    sockAddr.family = addressFamily;
    sockAddr.ip_addr = ipAddr.u_addr.ip4.addr;
    sockAddr.port = KING_htons(KING_atoi(port));

#if 0 // create and connect only for verify the IP address? -Nestor
    // Create socket
    KING_SocketCreate(AF_INET, SOCK_DGRAM, 0, 0, &sockid);
    // Connect
    //
    //

    if (sockid >= 0)
    {
        connP = connection_new_incoming(connList, sock, &sockAddr, sizeof(SOCK_ADDR_T));
        close(sockid);
    }
#endif
    connP = connection_new_incoming(connList, sock, &sockAddr, sizeof(SOCK_ADDR_T));

    return connP;
}
#endif

void connection_free(dtls_connection_t *connList)
{
    LogPrintf("Enter connection_free");
    //dtls_connection_t *nextQ;

    //nextQ = connList->next;
    while (connList)
    {
        LogPrintf("connList != NULL");

        dtls_connection_t *nextP;

        nextP = connList->next;

        if (connList->server_fd)
        {

            mbedtls_net_free(connList->server_fd);
            lwm2m_free(connList->server_fd);
            connList->server_fd = NULL;
        }

        if (connList->entropy)
        {
            mbedtls_entropy_free(connList->entropy);
            lwm2m_free(connList->entropy);
            connList->entropy = NULL;
        }
        if (connList->conf)
        {
            mbedtls_ssl_config_free(connList->conf);
            lwm2m_free(connList->conf);
            connList->conf = NULL;
        }
        if (connList->cacert)
        {
            mbedtls_x509_crt_free(connList->cacert);
            lwm2m_free(connList->cacert);
            connList->cacert = NULL;
        }

        if (connList->ssl)
        {
            if (connList->ssl->p_timer)
                lwm2m_free(connList->ssl->p_timer);
            mbedtls_ssl_free(connList->ssl);
            lwm2m_free(connList->ssl);
            connList->ssl = NULL;
        }
        if (connList->ctr_drbg)
        {
            mbedtls_ctr_drbg_free(connList->ctr_drbg);
            lwm2m_free(connList->ctr_drbg);
            connList->ctr_drbg = NULL;
        }

        LogPrintf("M2M# connection_free connList = %p", connList);

        lwm2m_free(connList);
        connList = nextP;
    }
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
    LogPrintf("connection_send success\r\n");
    return 0;
}

int connection_handle_packet(dtls_connection_t *connP, uint8 *buffer, size_t numBytes)
{

    if (connP->issecure != 0) //(connP->dtlsSession != NULL)
    {
        LogPrintf("security mode");
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
    int i = 0;
    dtls_connection_t *connP = (dtls_connection_t *)sessionH;

    if (connP == NULL)
    {
        LogPrintf("#> failed sending %lu bytes, missing connection\r\n", length);
        return COAP_500_INTERNAL_SERVER_ERROR;
    }

    for (i = 0; i < length; i++)
    {
        LogPrintf("lwm2m_buffer_send: payload[%d]=[0x%02X]\r\n", i, buffer[i]);
    }

    if (-1 == connection_send(connP, buffer, length))
    {
        LogPrintf("#> failed sending %lu bytes\r\n", length);
        return COAP_500_INTERNAL_SERVER_ERROR;
    }
    LogPrintf("lwm2m_buffer_send success\r\n");
    return COAP_NO_ERROR;
}

bool lwm2m_session_is_equal(void *session1,
                            void *session2,
                            void *userData)
{
    return (session1 == session2);
}


