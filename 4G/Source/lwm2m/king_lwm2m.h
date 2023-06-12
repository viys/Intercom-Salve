/**
 * Copyright (c) 2019 Xiamen CHEERZING IOT Technology Co., Ltd.
 * All rights reserved.
 *
 * Created by Nestor.Zhang
**/
#ifndef __KING_LWM2M_H__
#define __KING_LWM2M_H__


#include "KingCSTD.h"
#include "KingRtos.h"
#include "KingRTC.h"
#include "KingSocket.h"

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
#define INT64_MAX	    0x7fffffffffffffffL
#endif

#ifndef UINT64_MAX
#define UINT64_MAX 	    0xffffffffffffffffUL
#endif

// TODO: 这些宏定义应该是编译器相关的宏定义�?
#if 0

#ifndef DBL_MAX
#define DBL_MAX         __DBL_MAX__
#endif

#ifndef FLT_MAX
#define FLT_MAX         __FLT_MAX__
#endif

#ifndef FLT_EPSILON
#define FLT_EPSILON     __FLT_EPSILON__
#endif

#else

/* maximum representable finite floating-point number. */
#define FLT_MAX  3.40282347e+38F
#define DBL_MAX  1.79769313486231571e+308
#define LDBL_MAX 1.79769313486231571e+308L

/* minimum positive floating point number x such that 1.0 + x != 1.0 */
#define FLT_EPSILON         1.19209290e-7F
#define DBL_EPSILON         2.2204460492503131e-16
#define LDBL_EPSILON        2.2204460492503131e-16L

#endif

extern void LogPrintf(const char *fmt, ...);

// standard function supported by KingSDK.
#define time            KING_time
//#define memcpy          KING_memcpy
//#define memset          KING_memset
//#define memmove         KING_memmove
//#define memcmp          KING_memcmp
//#define sprintf         KING_sprintf
//#define strlen          KING_strlen
//#define strncmp         KING_strncmp
//#define lwm2m_strncmp   KING_strncmp
//#define strchr          KING_strchr
//#define strrchr         KING_strrchr
//#define rand            KING_rand
//#define srand           KING_srand
//#define isspace         KING_isspace

#define LWM2M_CLIENT_MODE   (1)
#define LWM2M_LITTLE_ENDIAN (1)
#define LWM2M_BOOTSTRAP     (1)
//Enable LWM2M LOGS as necessary
#define LWM2M_WITH_LOGS     (1)

void * lwm2m_malloc(size_t s);


#define LWM2M_EVENT_BASE 0

#define LWM2M_EVENT_BOOTSTRAP_START LWM2M_EVENT_BASE + 1
#define LWM2M_EVENT_BOOTSTRAP_SUCCESS LWM2M_EVENT_BASE + 2
#define LWM2M_EVENT_BOOTSTRAP_FAILED LWM2M_EVENT_BASE + 3
#define LWM2M_EVENT_CONNECT_SUCCESS LWM2M_EVENT_BASE + 4
#define LWM2M_EVENT_CONNECT_FAILED LWM2M_EVENT_BASE + 5

#define LWM2M_EVENT_REG_SUCCESS LWM2M_EVENT_BASE + 6
#define LWM2M_EVENT_REG_FAILED LWM2M_EVENT_BASE + 7
#define LWM2M_EVENT_REG_TIMEOUT LWM2M_EVENT_BASE + 8

#define LWM2M_EVENT_LIFETIME_TIMEOUT LWM2M_EVENT_BASE + 9
#define LWM2M_EVENT_UPDATE_SUCCESS LWM2M_EVENT_BASE + 11
#define LWM2M_EVENT_UPDATE_FAILED LWM2M_EVENT_BASE + 12
#define LWM2M_EVENT_UPDATE_TIMEOUT LWM2M_EVENT_BASE + 13

#define LWM2M_EVENT_RESPONSE_FAILED LWM2M_EVENT_BASE + 20
#define LWM2M_EVENT_NOTIFY_FAILED LWM2M_EVENT_BASE + 21
#define LWM2M_EVENT_NOTIFY_SEND_SUCCESS LWM2M_EVENT_BASE + 22
#define LWM2M_EVENT_NOTIFY_SENT_SUCCESS LWM2M_EVENT_BASE + 23
#define LWM2M_EVENT_NOTIFY_SENT_TIMEOUT LWM2M_EVENT_BASE + 24

#define LWM2M_EVENT_UPDATE_BINDING_SEND LWM2M_EVENT_BASE + 25
#define LWM2M_EVENT_UPDATE_BINDING_SUCCESS LWM2M_EVENT_BASE + 26
#define LWM2M_EVENT_UPDATE_BINDING_FAILED LWM2M_EVENT_BASE + 27
#define LWM2M_EVENT_UPDATE_BINDING_TIMEOUT LWM2M_EVENT_BASE + 28

#define LWM2M_EVENT_DEREG_SUCCESS LWM2M_EVENT_BASE + 29
#define LWM2M_EVENT_DEREG_FAILED LWM2M_EVENT_BASE + 30
#define LWM2M_EVENT_DEREG_TIMEOUT LWM2M_EVENT_BASE + 31

typedef enum
{
    EVENT_IND,
    OBJ_READ_IND,
    OBJ_WRITE_IND,
    OBJ_EXE_IND,
    OBJ_OBSERVE_IND,
    OBJ_SETPARAM_IND,
    OBJ_OPERATE_RSP_IND,
    SERVER_REGCMD_RESULT_IND,
    SERVER_QUIT_IND,
    FOTA_DOWNLOADING_IND,
    FOTA_DOWNLOAD_FAILED_IND,
    FOTA_DOWNLOAD_SUCCESS_IND,
    FOTA_PACKAGE_CHECK_IND,
    FOTA_UPGRADE_OK_IND,
    DATA_SEND_IND,
} LWM2M_EVENT_IND;

typedef struct
{
    LWM2M_EVENT_IND id;     ///< event identifier
    uint32_t param1; ///< 1st parameter
    uint32_t param2; ///< 2nd parameter
    uint32_t param3; ///< 3rd parameter
} LWM2M_EVENT_CONTEXT;

typedef void (*lwm2m_callback)(LWM2M_EVENT_CONTEXT* event);

uint8_t *lwm2m_eventName(uint8_t nEvId);
void lwm2m_sethandler(lwm2m_callback cb);
void lwm2m_postevent(LWM2M_EVENT_IND eventId, uint32_t result, uint32_t nParam1, uint32_t nParam2);


#endif
