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
 *    Simon Bernard - initial API and implementation
 *    Christian Renz - Please refer to git log
 *
 *******************************************************************************/

#ifndef DTLS_CONNECTION_H_
#define DTLS_CONNECTION_H_

#include "KingDef.h"
#include "KingSocket.h"

#include "liblwm2m.h"
#include "mbedtls/include/ssl.h"
#include "mbedtls/include/net_sockets.h"
#include "mbedtls/include/entropy.h"
#include "mbedtls/include/ctr_drbg.h"
#include "mbedtls/include/timing.h"

typedef struct sockaddr     sockaddr_storage;
typedef uint32 socklen_t;

#define LWM2M_STANDARD_PORT_STR "5683"
#define LWM2M_STANDARD_PORT      5683
#define LWM2M_DTLS_PORT_STR     "5684"
#define LWM2M_DTLS_PORT          5684
#define LWM2M_BSSERVER_PORT_STR "5685"
#define LWM2M_BSSERVER_PORT      5685

// after 40sec of inactivity we rehandshake
#define DTLS_NAT_TIMEOUT 40

struct addrinfo {
    int               ai_flags;      /* Input flags. */
    int               ai_family;     /* Address family of socket. */
    int               ai_socktype;   /* Socket type. */
    int               ai_protocol;   /* Protocol of socket. */
    socklen_t         ai_addrlen;    /* Length of socket address. */
    struct sockaddr  *ai_addr;       /* Socket address of socket. */
    char             *ai_canonname;  /* Canonical name of service location. */
    struct addrinfo  *ai_next;       /* Pointer to next in list. */
};

typedef struct _dtls_connection_t
{
    struct _dtls_connection_t *  next;
    int                     sock;
     struct sockaddr     addr;
#ifdef LWIP_IPV6_ON
    struct sockaddr_in6     addr6;
#endif
    size_t                  addrLen;
    lwm2m_object_t * securityObj;
    int securityInstId;
    lwm2m_context_t * lwm2mH;
    time_t lastSend; // last time a data was sent to the server (used for NAT timeouts)
    mbedtls_ssl_context* ssl;
    mbedtls_net_context* server_fd;
    mbedtls_entropy_context* entropy;
    mbedtls_ctr_drbg_context* ctr_drbg;
    mbedtls_ssl_config* conf;
    mbedtls_x509_crt* cacert;
    int sock_id;
    int issecure;
} dtls_connection_t;

int create_socket(const char * portStr, int ai_family);
dtls_connection_t * connection_find(dtls_connection_t * connList, const sockaddr_storage * addr, size_t addrLen);
dtls_connection_t * connection_new_incoming(dtls_connection_t * connList, int sock, const struct sockaddr * addr, size_t addrLen);

#if (defined WITH_TINYDTLS) || (defined WITH_MBEDTLS)
dtls_connection_t * connection_create(dtls_connection_t * connList, int sock, lwm2m_object_t * securityObj, int instanceId, lwm2m_context_t * lwm2mH, int addressFamily);
#else
dtls_connection_t * connection_create(dtls_connection_t * connList,
                                 int sock,
                                 char * host,
                                 char * port,
                                 int addressFamily);
#endif

void connection_free(dtls_connection_t * connList);

int connection_send(dtls_connection_t *connP, uint8_t * buffer, size_t length);
int connection_handle_packet(dtls_connection_t *connP, uint8_t * buffer, size_t length);

//rehandshake a connection, useful when your NAT timed out and your client has a new IP/PORT
//int connection_rehandshake(dtls_connection_t *connP, bool sendCloseNotify);


#endif

