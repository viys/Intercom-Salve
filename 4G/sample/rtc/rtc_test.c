/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "app_main.h"
#include "KingRTC.h"
#include "kingnet.h"
#include "kingcstd.h"
#include "kingcbdata.h"
#include "kingRtos.h"
#include "kingDef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("rtc: "fmt, ##args); } while(0)

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef struct _sysTime_t {
    uint16 year;
    uint8 month;
    uint8 day;
    uint8 hour;
    uint8 minute;
    uint8 second;
    uint16 millisecond;
} sysTime_t;

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
static void rtc_localTimeGet(void)
{
    int ret = -1;
    CALENDAR_TIME sysTime;

    ret = KING_LocalTimeGet(&sysTime);
    if (ret == SUCCESS)
    {
        sysTime_t localSysTime;
        uint16 sysYear = 0;
        
        LogPrintf("rtc_localTimeGet %02x/%02x/%02x %02x:%02x:%x%x %d%d\r\n",
            sysTime.yy, sysTime.MM, sysTime.dd,
            sysTime.hh, sysTime.mm, sysTime.ss_h,
            sysTime.ss_l, sysTime.ew, sysTime.tz);
        sysYear = BCD2BIN(sysTime.yy);
        if (sysYear < 70)
        {
            localSysTime.year = BCD2BIN(sysTime.yy) + 2000;
        }
        else if (sysYear >= 70 &&  sysYear <= 99)
        {
            localSysTime.year = BCD2BIN(sysTime.yy) + 1900;
        }
        else
        {
            return;
        }
        localSysTime.month = BCD2BIN(sysTime.MM);
        localSysTime.day = BCD2BIN(sysTime.dd);
        localSysTime.hour = BCD2BIN(sysTime.hh);
        localSysTime.minute = BCD2BIN(sysTime.mm);
        localSysTime.second = (sysTime.ss_h * 10 + (sysTime.ss_l&0x0f));

        LogPrintf("rtc_localTimeGet %04d/%02d/%02d,%02d:%02d:%02d% 03d\r\n",
            localSysTime.year, localSysTime.month, localSysTime.day,
            localSysTime.hour, localSysTime.minute, localSysTime.second,
            sysTime.tz);
    }
}

static void rtc_Alarm_Notify(uint32 eventID, void* pData, uint32 len)
{
    LogPrintf("rtc_Alarm_Notify eventID %d", eventID);
    if (RTC_EVENT_ALARM_EXPIRED == eventID)
    {
        KH_CB_DATA_U *data = (KH_CB_DATA_U *)pData;
        uint32 alarmID = data->alarm.alarmID;
        
        LogPrintf("rtc_Alarm_Notify alarmID %d", alarmID);
        if (NULL != pData)
        {
            LogPrintf("rtc_Alarm_Notify Alarm-%d notify in ", alarmID);
        } else {
            LogPrintf("rtc_Alarm_Notify Alarm notify in ");
        }
        rtc_localTimeGet();
    }
    else
    {
        LogPrintf("Alarm_Notify unknown event [%d]!!\r\n", eventID);
    }
}

/*******************************************************************************
 ** Functions
 ******************************************************************************/
void rtc_test(void)
{
    int ret = -1;
    CALENDAR_TIME setTime;
    uint32 tick = 0;
    ALARM_CFG_S alarm;
    CALENDAR_TIME sysTime;
    ALARM_CFG_S getAlarm;
    NET_STATE_E state = NET_NO_SERVICE;  
    struct timeval tv;
    
    while (1)
    {
        ret = KING_NetStateGet(&state);
        LogPrintf("rtc_test Get net state %d, ret : %d\r\n", state, ret);
        if (ret == 0 && state > NET_NO_SERVICE && state < NET_EMERGENCY)
        {
            NET_CESQ_S Cesq;
            ret = KING_NetSignalQualityGet(&Cesq);
            LogPrintf("rtc_test Get sinagal ret is %d. rxlev:%d, ber:%d, rscp:%d, ecno:%d, rsrq:%d, rsrp:%d\r\n",
                ret,Cesq.rxlev,Cesq.ber,Cesq.rscp,Cesq.ecno,Cesq.rsrq,Cesq.rsrp);
            break;
        }
        KING_Sleep(1000);
    }

    uint8 data;
    uint8 high = 0;
    uint8 low = 0;
    for (int i = 0x01; i < 0x59; i++)
    {
        high = (i >> 4) & (0x0f);
        if (high > 9)
        {
            continue;
        }
        low = i & (0x0f);
        if (low > 9)
        {
            continue;
        }
        data = BCD2BIN(i);
        LogPrintf("(BCD)0x%02x(%d) <===> (BIN)0x%02x(%d)\r\n", i, i, data, data);
    }
   
    KING_Sleep(5000);
    ret = KING_CurrentTickGet(&tick);
    LogPrintf("KING_CurrentTickGet tick is %ld[0x%X], ret %d\r\n", tick, tick, ret);

    rtc_localTimeGet();
    ret = KING_GetTimeOfDay(&tv, NULL);
    if (ret == 0)
    { 
        LogPrintf("KING_GetTimeOfDay tv_sec is %ld, tv_usec %ld\r\n", tv.tv_sec, tv.tv_usec);
    } 
    //the year starts from 2000
    //set time 20/7/6 11:30:57
    setTime.yy = 32;//0x20
    setTime.MM = 7;//0x07
    setTime.dd = 6;//0x06
    setTime.hh = 17;//0x11
    setTime.mm = 48;//0x30
    setTime.ss_l = 7;//0x7
    setTime.ss_h = 5;//0x5
    setTime.ew = 0;
    setTime.tz = 32;
    ret = KING_LocalTimeSet(&setTime);
    LogPrintf("rtc_test set local time ret %d\r\n", ret);

    rtc_localTimeGet();

    KING_RegNotifyFun(DEV_CLASS_ALARM, ALARM_ID_0, rtc_Alarm_Notify);
    ret = KING_LocalTimeGet(&sysTime);
    if (ret == FAIL)
    {
        LogPrintf("rtc_test KING_LocalTimeGet() Fail!\r\n");
        return;
    }

    memcpy((void*)&alarm.alarmTime, (void*)&sysTime, sizeof(CALENDAR_TIME));
    alarm.alarmTime.mm += 1;
    if (alarm.alarmTime.mm >= 96)
    {
        alarm.alarmTime.mm -= 96;
        alarm.alarmTime.hh += 1;
    }
    alarm.alarmId = ALARM_ID_0;
    alarm.bActive = TRUE;
    if (0 != KING_RtcAlarmSet(alarm))
    {
        LogPrintf("rtc_test Set Alarm-0 fail\r\n");
    }
    LogPrintf("rtc_test Set Alarm-0 in ");
    rtc_localTimeGet();
    memset(&getAlarm, 0x00, sizeof(ALARM_CFG_S));
    if (0 != KING_RtcAlarmGet(ALARM_ID_0, &getAlarm))
    {
        LogPrintf("rtc_test KING_RtcAlarmGet ALARM_ID_0 ail\r\n");
    } 
    LogPrintf("KING_RtcAlarmGet alarmId = %d", getAlarm.alarmId);

    if (0 != KING_RtcAlarmDelete(getAlarm.alarmId))
    {
        LogPrintf("rtc_test KING_RtcAlarmDelete ALARM_ID_0 fail\r\n");
    }    

    /*
     * Set up second ALARM.
     * NOTE: we can define another callback here!
     */
    KING_RegNotifyFun(DEV_CLASS_ALARM, ALARM_ID_1, rtc_Alarm_Notify);
    alarm.alarmTime.mm += 1;
    if (alarm.alarmTime.mm >= 96)
    {
        alarm.alarmTime.mm -= 96;
        alarm.alarmTime.hh += 1;
    }
    alarm.alarmId = ALARM_ID_1;
    alarm.bActive = TRUE;
    if (0 != KING_RtcAlarmSet(alarm))
    {
        LogPrintf("rtc_test Set Alarm-1 fail\r\n");
    }
    LogPrintf("rtc_test Set Alarm-1 in ");
    rtc_localTimeGet();
    memset(&getAlarm, 0x00, sizeof(ALARM_CFG_S));
    if (0 != KING_RtcAlarmGet(ALARM_ID_1, &getAlarm))
    {
        LogPrintf("KING_RtcAlarmGet ALARM_ID_1 ail\r\n");
    } 
    LogPrintf("KING_RtcAlarmGet alarmId = %d", getAlarm.alarmId);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

