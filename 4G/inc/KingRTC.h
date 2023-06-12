#ifndef __KING_RTC_H__
#define __KING_RTC_H__

#include "KingDef.h"

// RTC ALARM events
#define RTC_EVENT_ALARM_EXPIRED     0x0001

#define ALARM_ID_0      0
#define ALARM_ID_1      1
#define ALARM_ID_2      2

#define IS_LEAP_YEAR(year) ((!(year % 4) && ((year % 100) || !(year % 400))) ? 1 : 0 )
#define IS_VALID_YEAR(X) (((0 <= X) && (X <= 99)) ? 1 : 0)
#define IS_VALID_MONTH(X) (((1 <= X) && (X <= 12)) ? 1 : 0)
#define IS_VALID_DAY(X) (((1 <= X) && (X <= 31)) ? 1 : 0)
#define IS_VALID_HOUR(X) (((0 <= X) && (X < 24)) ? 1 : 0)
#define IS_VALID_MINUTE(X) (((0 <= X) && (X < 60)) ? 1 : 0)
#define IS_VALID_SECOND(X) (((0 <= X) && (X < 60)) ? 1 : 0)

typedef long  time_t;

struct timeval {
    long tv_sec;         /* seconds */
    long tv_usec;        /* and microseconds */
};

struct timezone {
    int tz_minuteswest;     /* minutes west of Greenwich */
    int tz_dsttime;         /* type of DST correction */
};

typedef struct
{
    CALENDAR_TIME alarmTime;    ///< time to trigger ALARM.
    uint32      alarmId;        ///< ID of this timer. Starts from 0.
    bool        bActive;        ///< Active or InActive
} ALARM_CFG_S;


/**
 * 获取系统当前tick.
 * @note Tick值是从CPU上电开始计数，并且可能发生翻转。
 * @note Tick值是硬件相关的计数，换算成ms需要底层支持。
 *
 * @param[out] tick   1tick=1ms (asr1802为5ms)
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_CurrentTickGet(uint32* tick);

/**
 * @brief Get the time as well as a timezone.
 *
 * @note The tv argument is a struct timeval and gives the number of seconds
 * and microseconds since the Epoch.
 *
 * @note The tz argument is a struct timezone.
 * The use of the timezone structure is obsolete;
 * the tz argument should normally be specified as NULL.
 *
 * @note If either tv or tz is NULL, the corresponding structure is not set or returned.
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_GetTimeOfDay(struct timeval *tv, struct timezone *tz);

/**
 * time() returns the time as the number of seconds since the Epoch,
 * 1970-01-01 00:00:00 +0000 (UTC).
 *
 * If tloc is non-NULL, the return value is also stored in the memory
 * pointed to by tloc.
 *
 * On success, the value of time in seconds since the Epoch is returned.
 * On error, ((time_t) -1) is returned.
 */
time_t KING_time(time_t *tloc);


/**
 * 设置系统当前时间
 *
 * @param[in]  pSystemTime
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LocalTimeSet(const CALENDAR_TIME* pSystemTime);

/**
 * 获取系统当前时间
 *
 * @param[out]  pSystemTime
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LocalTimeGet(CALENDAR_TIME* pSystemTime);

/**
 * 设置RTC闹钟。
 * APP可以通过RegNotifyFun()注册ALARM发生时的回调函数。
 * 如果闹钟发生时系统处于关机状态，闹钟事件将触发系统开机,
 * 但之前注册的回调函数将不会被调用。
 *
 * @param[in] alarm 需要设置的闹钟
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_RtcAlarmSet(ALARM_CFG_S alarm);

/**
 * 获取某个闹钟的信息.
 *
 * @param[in] alarmId    需要查看的闹钟ID.
 * @param[out] alarm     返回的闹钟信息
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_RtcAlarmGet(uint32 alarmId, ALARM_CFG_S* alarm);

/**
 * 取消并删除某个闹钟的信息.
 * Delete后，调用RtcAlarmGet(alarmId)将返回-1.
 *
 * @param[in] alarmId    需要删除的闹钟ID.
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_RtcAlarmDelete(uint32 alarmId);
#endif

