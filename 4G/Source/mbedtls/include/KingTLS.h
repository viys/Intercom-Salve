/**
 * Copyright (c) 2019 Xiamen CHEERZING IOT Technology Co., Ltd.
 * All rights reserved.
 *
 * Created by Nestor.Zhang
**/
#ifndef __KING_TLS_H__
#define __KING_TLS_H__


#include "KingCSTD.h"
#include "KingRtos.h"
#include "KingRTC.h"
#include "KingPlat.h"
#include "KingDef.h"
#include "Kingsocket.h"

#ifndef NULL
#define NULL    0
#endif

#define true     1
#define false    0


// TODO should check if long int is size of int64.
#ifndef __UINT64_T__
typedef unsigned long long int       uint64_t;
#define __UINT64_T__
#endif

#ifndef __INT64_T__
typedef long long int                int64_t;
#define __INT64_T__
#endif

#ifndef __UINT32_T__
typedef unsigned int    uint32_t;
#define __UINT32_T__
#endif

#ifndef __INT32_T__
typedef signed int      int32_t;
#define __INT32_T__
#endif

#ifndef __UINT16_T__
typedef unsigned short  uint16_t;
#define __UINT16_T__
#endif

#ifndef __INT16_T__
typedef signed short    int16_t;
#define __INT16_T__
#endif

#ifndef __UINT8_T__
typedef unsigned char   uint8_t;
#define __UINT8_T__
#endif

#ifndef __INT8_T__
typedef signed char     int8_t;
#define __INT8_T__
#endif

typedef unsigned long   DWORD;  /* actually long, but this should be okay */


// Socket Error Codes.
#define EINPROGRESS     SOCKET_ERR_INPROGRESS
#define ETIMEDOUT       SOCKET_ERR_TIMED_OUT
#define EAGAIN          SOCKET_ERR_AGAIN  /* Try again */
#define EWOULDBLOCK     EAGAIN  /* Operation would block */


/* maximum values of exact-width unsigned integer types */

#ifndef INT8_MIN
#define INT8_MIN        (-0x7f-1)
#endif

#ifndef INT8_MAX
#define INT8_MAX        0x7f
#endif

#ifndef UINT8_MAX
#define UINT8_MAX       0xff
#endif

#ifndef INT16_MIN
#define INT16_MIN       (-0x7fff-1)
#endif

#ifndef INT16_MAX
#define INT16_MAX       0x7fff
#endif

#ifndef UINT16_MAX
#define UINT16_MAX      0xffff
#endif

#ifndef INT32_MIN
#define INT32_MIN       (-0x7fffffff-1)
#endif

#ifndef INT32_MAX
#define INT32_MAX       0x7fffffff
#endif

#ifndef UINT32_MAX
#define UINT32_MAX      0xffffffff
#endif

#ifndef INT64_MIN
#define INT64_MIN       (-0x7fffffffffffffffLL-1)
#endif

#ifndef INT64_MAX
#define INT64_MAX       0x7fffffffffffffffL
#endif

#ifndef UINT64_MAX
#define UINT64_MAX      0xffffffffffffffffUL
#endif

#ifndef SCHAR_MIN
#define SCHAR_MIN              (-128)
#endif
#ifndef SCHAR_MAX
#define SCHAR_MAX              (127)
#endif
#ifndef UCHAR_MAX
#define UCHAR_MAX              (255U)
#endif

#define INT_MAX                 0x7fffffff
#define UINT_MAX                0xffffffffU
#define LONG_MAX                0x7fffffff
#define LONG_MIN                -(0x7fffffff-1)
#define ULONG_MAX               0xffffffffU

#ifdef __SIZE_MAX__
#define SIZE_MAX __SIZE_MAX__
#else
#define SIZE_MAX UINT_MAX
#endif

/* Standard random functions, work as the libc ones. */
#define RAND_MAX        32767

//Config
#ifndef NO_PERSISTENCE
#define NO_PERSISTENCE
#endif

#ifndef NOSTACKTRACE
#define NOSTACKTRACE
#endif

#ifndef HIGH_PERFORMANCE
#define HIGH_PERFORMANCE
#endif

#ifdef HEAP_H
#undef HEAP_H
#endif

#ifdef OPENSSL
#undef OPENSSL
#endif

#ifndef MBEDTLS
#define MBEDTLS
#endif


// standard function supported by KingSDK.
#define gettimeofday    KING_GetTimeOfDay
#define time            KING_time
#define htonl           KING_htonl
#define htons           KING_htons
#define ntohl           KING_ntohl
#define ntohs           KING_ntohs


extern void LogPrintf(const char *fmt, ...);

#define MBEDTLS_NO_TRACE
#define MBEDTLS_KING_SOCKETS

#endif

