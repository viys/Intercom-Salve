/**
 * Copyright (c) 2019 Xiamen CHEERZING IOT Technology Co., Ltd.
 * All rights reserved.
 *
 * Created by Nestor.Zhang
**/
#ifndef __DTLS_KING_DEF_H__
#define __DTLS_KING_DEF_H__


#include "KingCSTD.h"
#include "KingRtos.h"
#include "KingRTC.h"
#include "KingSocket.h"

#ifndef NULL
#define NULL    0
#endif

#define true     1
#define false    0

#ifndef inline
#define inline __inline
#endif

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


typedef unsigned int clock_t;    /* cpu time type */

/* maximum values of exact-width unsigned integer types */
#define UINT8_MAX            255
#define UINT16_MAX           65535
#define UINT32_MAX           4294967295u

/** Defined to 1 if tinydtls is built with support for ECC */
#define DTLS_ENABLE_ECC 1

/** Defined to 1 if tinydtls is built with support for PSK */
#define DTLS_ENABLE_PSK 1

#define DTLS_LITTLE_ENDIAN 1

#define KING_DTLS_LOG    1
extern void LogPrintf(const char *fmt, ...);

#endif
