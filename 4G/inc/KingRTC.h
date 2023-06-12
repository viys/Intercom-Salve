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
 * ��ȡϵͳ��ǰtick.
 * @note Tickֵ�Ǵ�CPU�ϵ翪ʼ���������ҿ��ܷ�����ת��
 * @note Tickֵ��Ӳ����صļ����������ms��Ҫ�ײ�֧�֡�
 *
 * @param[out] tick   1tick=1ms (asr1802Ϊ5ms)
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
 * ����ϵͳ��ǰʱ��
 *
 * @param[in]  pSystemTime
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LocalTimeSet(const CALENDAR_TIME* pSystemTime);

/**
 * ��ȡϵͳ��ǰʱ��
 *
 * @param[out]  pSystemTime
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LocalTimeGet(CALENDAR_TIME* pSystemTime);

/**
 * ����RTC���ӡ�
 * APP����ͨ��RegNotifyFun()ע��ALARM����ʱ�Ļص�������
 * ������ӷ���ʱϵͳ���ڹػ�״̬�������¼�������ϵͳ����,
 * ��֮ǰע��Ļص����������ᱻ���á�
 *
 * @param[in] alarm ��Ҫ���õ�����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_RtcAlarmSet(ALARM_CFG_S alarm);

/**
 * ��ȡĳ�����ӵ���Ϣ.
 *
 * @param[in] alarmId    ��Ҫ�鿴������ID.
 * @param[out] alarm     ���ص�������Ϣ
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_RtcAlarmGet(uint32 alarmId, ALARM_CFG_S* alarm);

/**
 * ȡ����ɾ��ĳ�����ӵ���Ϣ.
 * Delete�󣬵���RtcAlarmGet(alarmId)������-1.
 *
 * @param[in] alarmId    ��Ҫɾ��������ID.
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_RtcAlarmDelete(uint32 alarmId);
#endif

