#ifndef __KING_DEF_H__
#define __KING_DEF_H__

/*
 * stdarg.h中的va_start等定义是编译器相关的
 */
#ifdef CPL_CSDTK    // RDA平台CSDTK编译器
#include "stdarg_csdtk.h"
#elif defined (CPL_GCC)
#include "stdarg_csdtk.h"
#else
#include "stdarg_rvct.h"
#endif

#define TRUE     1
#define FALSE    0

#define SUCCESS  0
#define FAIL     -1

#ifndef NULL
  #define NULL  0
#endif

typedef unsigned int    bool;

typedef unsigned int    uint32;
typedef signed int      int32;

typedef unsigned short  uint16;
typedef signed short    int16;

typedef unsigned char   uint8;
typedef signed char     int8;

typedef unsigned short  wchar;

typedef unsigned int size_t;
typedef unsigned long long int uint64;

typedef unsigned long   DWORD;

/*
#define INVALID_HANDLE 0xFFFFFFFF

typedef uint32 THREAD_HANDLE;
typedef uint32 MUTEX_HANDLE;
typedef uint32 SEM_HANDLE;
typedef uint32 EVENT_HANDLE;
typedef uint32 MSG_HANDLE;
typedef uint32 TIMER_HANDLE;
*/

typedef void* THREAD_HANDLE;
typedef void* MUTEX_HANDLE;
typedef void* SEM_HANDLE;
typedef void* EVENT_HANDLE;
typedef void* MSG_HANDLE;
typedef void* TIMER_HANDLE;

#define BIT0 (0x1<<0)
#define BIT1 (0x1<<1)
#define BIT2 (0x1<<2)
#define BIT3 (0x1<<3)
#define BIT4 (0x1<<4)
#define BIT5 (0x1<<5)
#define BIT6 (0x1<<6)
#define BIT7 (0x1<<7)
#define BIT8 (0x1<<8)
#define BIT9 (0x1<<9)
#define BIT10 (0x1<<10)
#define BIT11 (0x1<<11)
#define BIT12 (0x1<<12)
#define BIT13 (0x1<<13)
#define BIT14 (0x1<<14)
#define BIT15 (0x1<<15)
#define BIT16 (0x1<<16)
#define BIT17 (0x1<<17)
#define BIT18 (0x1<<18)
#define BIT19 (0x1<<19)
#define BIT20 (0x1<<20)
#define BIT21 (0x1<<21)
#define BIT22 (0x1<<22)
#define BIT23 (0x1<<23)
#define BIT24 (0x1<<24)
#define BIT25 (0x1<<25)
#define BIT26 (0x1<<26)
#define BIT27 (0x1<<27)
#define BIT28 (0x1<<28)
#define BIT29 (0x1<<29)
#define BIT30 (0x1<<30)
#define BIT31 (0x1<<31)

#define BCD2BIN(x)    ((((x) >> 4) & 0x0f) * 10 + ((x) & 0x0f))
#define BIN2BCD(x)    ((((x) / 10) << 4 ) | ((x) % 10))

//luxiao, according to 3GPP 03.40
typedef struct _tagCalendarTime {
    uint8 yy;//bcd coding [0, 99]
    uint8 MM;//bcd coding [1, 12]
    uint8 dd;//bcd coding [1, 31]
    uint8 hh;//bcd coding [0, 23]
    uint8 mm;//bcd coding [0, 59]
    uint8 ss_l:4;//bcd coding [0, 9]
    uint8 ss_h:3;//bcd coding [0, 5]
    uint8 ew:1; //0: east 1: west
    uint16 tz:6; //binary coding
    uint16 ms:10;  //binary coding
}CALENDAR_TIME;

typedef enum {
    POWER_REASON_UNKNOWN = 0,           ///< placeholder for unknown reason
    POWER_REASON_PWRKEY = (1 << 0),     ///< boot by power key
    POWER_REASON_PIN_RESET = (1 << 1),  ///< boot by pin reset
    POWER_REASON_ALARM = (1 << 2),      ///< boot by alarm
    POWER_REASON_CHARGE = (1 << 3),     ///< boot by charge in
    POWER_REASON_WDG = (1 << 4),        ///< boot by watchdog
    POWER_REASON_PIN_WAKEUP = (1 << 5), ///< boot by wakeup
    POWER_REASON_PSM_WAKEUP = (1 << 6), ///< boot from PSM wakeup
    POWER_REASON_PANIC = (1 << 9),      ///< boot by panic reset
    POWER_REASON_MAX = 0x7FFFFFFF
} POWER_REASON_E;
#endif
