/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "kingnet.h"
#include "kingcstd.h"
#include "kingrtos.h"
#include "kingplat.h"
#include "kingCbData.h"
#include "kingcallcontrol.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("callControl: "fmt, ##args); } while(0)

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef enum
{
    CC_OPT_STATE_NONE,
    CC_OPT_STATE_ANSWER,
    CC_OPT_STATE_REJECT,
    CC_OPT_STATE_WAITING,
    CC_OPT_STATE_MAX,
}CC_OPT_STATE;

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static CC_OPT_STATE ccOptState = CC_OPT_STATE_NONE;
static TIMER_HANDLE ccTestTimerHandle = NULL;
static TIMER_HANDLE ccHandUpTimerH = NULL;
static uint32 ccEvtId = CC_EVENT_MAX;
static int testType = 0;

/*******************************************************************************
 **  Function 
 ******************************************************************************/
static void cc_callAnswer(uint32 tmrId)
{
    int ret = -1;
    
    if (ccOptState == CC_OPT_STATE_ANSWER)
    {
        ret = KING_CallAnswer(SIM_1);
        if(-1 == ret)
        {
            LogPrintf("KING_CallAnswer() SIM_1 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        }
        ret = KING_CallAnswer(SIM_2);
        if(-1 == ret)
        {
            LogPrintf("KING_CallAnswer() SIM_2 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        }
        
        ret = KING_CallAnswer(SIM_0);
        if(-1 == ret)
        {
            LogPrintf("KING_CallAnswer() SIM_0 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        }
    }

    LogPrintf("Deactive Ring timer!\r\n");
    KING_TimerDeactive(ccTestTimerHandle);
}

static void cc_ringTimer(void)
{
    int ret = -1;
    TIMER_ATTR_S timerattr;
    
    LogPrintf("cc_ringTimer start!\r\n");
    
    if (ccTestTimerHandle == NULL)
    {
        ret = KING_TimerCreate(&ccTestTimerHandle);
        if(-1 == ret)
        {
            LogPrintf("KING_TimerCreate() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
            return;
        }
    }

    memset(&timerattr, 0x00, sizeof(timerattr));
    timerattr.isPeriod = FALSE;
    timerattr.timeout = 4000;
    timerattr.timer_fun = cc_callAnswer;
    ret = KING_TimerActive(ccTestTimerHandle, &timerattr);
    if(-1 == ret)
    {
        LogPrintf("KING_TimerActive() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    LogPrintf("cc_ringTimer end!\r\n");
}

static void cc_callReject(void)
{
    int ret = -1;

    ret = KING_CallReject(SIM_1);
    if(-1 == ret)
    {
        LogPrintf("KING_CallReject() SIM_1 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_CallReject(SIM_2);
    if(-1 == ret)
    {
        LogPrintf("KING_CallReject() SIM_2 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }

    ret = KING_CallReject(SIM_0);
    if(-1 == ret)
    {
        LogPrintf("KING_CallReject() SIM_0 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
}

static void cc_callHold(uint32 callId)
{
    int ret = -1;
    
    LogPrintf("cc_callHold start!\r\n");
    ret = KING_CallHold(SIM_1, callId);
    if(-1 == ret)
    {
        LogPrintf("KING_CallHold() SIM_1 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_CallHold(SIM_2, callId);
    if(-1 == ret)
    {
        LogPrintf("KING_CallHold() SIM_2 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }

    ret = KING_CallHold(SIM_0, callId);
    if(-1 == ret)
    {
        LogPrintf("KING_CallHold() SIM_0 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    
    LogPrintf("cc_callHold end!\r\n");
}

static void cc_callActive(uint32 callId)
{
    int ret = -1;
    
    LogPrintf("cc_callActive start!\r\n");
    ret = KING_CallActive(SIM_1, callId);
    if(-1 == ret)
    {
        LogPrintf("KING_CallActive() SIM_1 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_CallActive(SIM_2, callId);
    if(-1 == ret)
    {
        LogPrintf("KING_CallActive() SIM_2 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }

    ret = KING_CallActive(SIM_0, callId);
    if(-1 == ret)
    {
        LogPrintf("KING_CallActive() SIM_0 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    
    LogPrintf("cc_callActive end!\r\n");
}

static void cc_callHangUp(uint32 callId)
{
    int ret = -1;
    
    LogPrintf("cc_callHangUp start!\r\n");
    ret = KING_CallHangUp(SIM_1, callId);
    if(-1 == ret)
    {
        LogPrintf("KING_CallHangUp() SIM_1 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_CallHangUp(SIM_2, callId);
    if(-1 == ret)
    {
        LogPrintf("KING_CallHangUp() SIM_2 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }

    ret = KING_CallHangUp(SIM_0, callId);
    if(-1 == ret)
    {
        LogPrintf("KING_CallHangUp() SIM_0 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    
    LogPrintf("cc_callHangUp end!\r\n");
}

static void cc_callHangUpAll(uint32 tmrId)
{
    int ret = -1;

    if (ccEvtId == CC_EVENT_CONNECTED)
    {
        ret = KING_CallHangUpAll(SIM_1);
        if(-1 == ret)
        {
            LogPrintf("KING_CallHangUpAll() SIM_1 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        }
        ret = KING_CallHangUpAll(SIM_2);
        if(-1 == ret)
        {
            LogPrintf("KING_CallHangUpAll() SIM_2 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        }

        ret = KING_CallHangUpAll(SIM_0);
        if(-1 == ret)
        {
            LogPrintf("KING_CallHangUpAll() SIM_0 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        }
    }
    KING_TimerDeactive(ccHandUpTimerH);
}

static void cc_handUpTimer(void)
{
    int ret = -1;
    TIMER_ATTR_S timerattr;
    
    LogPrintf("cc_handUpTimer start!\r\n");
    
    if (ccHandUpTimerH == NULL)
    {
        ret = KING_TimerCreate(&ccHandUpTimerH);
        if(-1 == ret)
        {
            LogPrintf("cc_handUpTimer() create timer Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
            return;
        }
    }

    memset(&timerattr, 0x00, sizeof(timerattr));
    timerattr.isPeriod = FALSE;
    timerattr.timeout = 5000;
    timerattr.timer_fun = cc_callHangUpAll;
    ret = KING_TimerActive(ccHandUpTimerH, &timerattr);
    if(-1 == ret)
    {
        LogPrintf("cc_handUpTimer time active Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    LogPrintf("cc_handUpTimer end!\r\n");
}

static void cc_opsImsBySimID(SIM_ID sim)
{
    int ret = -1;
    IMS_TRANSPORT_TYPE TransportType;
    uint32 capabilities = 0;
    
    LogPrintf("cc_opsImsBySimID sim=%d!\r\n", sim);
    ret = KING_IMSRegisterStateGet(sim, NULL);
    if(-1 == ret)
    {
        LogPrintf("KING_IMSRegisterStateGet() sim = %d, TransportType is NULL Failed! errcode=0x%x\r\n", sim, KING_GetLastErrCode());
    }  
    
    TransportType = TRANSPORT_TYPE_INVALID;
    ret = KING_IMSRegisterStateGet(sim, &TransportType);
    if(-1 == ret)
    {
        LogPrintf("KING_IMSRegisterStateGet() sim = %d Failed! errcode=0x%x\r\n", sim, KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_IMSRegisterStateGet() sim = %d TransportType=%d\r\n", sim, TransportType);
    }
    
    ret = KING_IMSCapabilityGet(sim, NULL);
    if(-1 == ret)
    {
        LogPrintf("KING_IMSCapabilityGet() sim = %d capabilities is NULL Failed! errcode=0x%x\r\n", sim, KING_GetLastErrCode());
    }
    
    capabilities = 0;
    ret = KING_IMSCapabilityGet(sim, &capabilities);
    if(-1 == ret)
    {
        LogPrintf("KING_IMSCapabilityGet() sim = %d Failed! errcode=0x%x\r\n", sim, KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_IMSCapabilityGet() sim = %d capabilities=%u\r\n", sim, capabilities);
    } 
    
    TransportType = TRANSPORT_TYPE_INVALID;
    ret = KING_IMSRegister(sim, TransportType);
    if(-1 == ret)
    {
        LogPrintf("KING_IMSRegister() sim = %d TRANSPORT_TYPE_INVALID Failed! errcode=0x%x\r\n", sim, KING_GetLastErrCode());
    }
    
    TransportType = TRANSPORT_TYPE_WLAN;
    ret = KING_IMSRegister(sim, TransportType);
    if(-1 == ret)
    {
        LogPrintf("KING_IMSRegister() sim = %d TRANSPORT_TYPE_WLAN Failed! errcode=0x%x\r\n", sim, KING_GetLastErrCode());
    }
    
    TransportType = TRANSPORT_TYPE_WWAN;
    ret = KING_IMSRegister(sim, TransportType);
    if(-1 == ret)
    {
        LogPrintf("KING_IMSRegister() sim = %d TRANSPORT_TYPE_WWAN Failed! errcode=0x%x\r\n", sim, KING_GetLastErrCode());
    }

}

static void cc_imsUnregisterBySimID(SIM_ID sim)
{
    int ret = -1;
    IMS_TRANSPORT_TYPE TransportType;
    
    LogPrintf("cc_imsUnregisterBySimID sim=%d!\r\n", sim);
    TransportType = TRANSPORT_TYPE_INVALID;
    ret = KING_IMSUnRegister(sim, TransportType);
    if(-1 == ret)
    {
        LogPrintf("KING_IMSUnRegister() sim = %d TRANSPORT_TYPE_INVALID Failed! errcode=0x%x\r\n", sim, KING_GetLastErrCode());
    }

    TransportType = TRANSPORT_TYPE_WLAN;
    ret = KING_IMSUnRegister(sim, TransportType);
    if(-1 == ret)
    {
        LogPrintf("KING_IMSUnRegister() sim = %d TRANSPORT_TYPE_WLAN Failed! errcode=0x%x\r\n", sim, KING_GetLastErrCode());
    }

    TransportType = TRANSPORT_TYPE_WWAN;
    ret = KING_IMSUnRegister(sim, TransportType);
    if(-1 == ret)
    {
        LogPrintf("KING_IMSUnRegister() sim = %d TRANSPORT_TYPE_WWAN Failed! errcode=0x%x\r\n", sim, KING_GetLastErrCode());
    }
}

static void cc_callWaitingSetAndGet(void)
{
    int ret = -1;
    bool isActive = FALSE;
    
    LogPrintf("cc_callWaitingSetAndGet start!\r\n");
    ret = KING_CallWaitingServiceGet(SIM_1, &isActive);
    if(-1 == ret)
    {
        LogPrintf("KING_CallWaitingServiceGet() SIM_1 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_CallWaitingServiceGet() SIM_1 isActive=%d\r\n", isActive);
    }
    
    isActive = FALSE;
    ret = KING_CallWaitingServiceGet(SIM_2, &isActive);
    if(-1 == ret)
    {
        LogPrintf("KING_CallWaitingServiceGet() SIM_2 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_CallWaitingServiceGet() SIM_2 isActive=%d\r\n", isActive);
    }
    
    isActive = FALSE;
    ret = KING_CallWaitingServiceGet(SIM_0, &isActive);
    if(-1 == ret)
    {
        LogPrintf("KING_CallWaitingServiceGet() SIM_0 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_CallWaitingServiceGet() SIM_0 isActive=%d\r\n", isActive);
    }

    isActive = TRUE;
    ret = KING_CallWaitingServiceSet(SIM_2, isActive);
    if(-1 == ret)
    {
        LogPrintf("KING_CallWaitingServiceSet() SIM_2 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_CallWaitingServiceSet() SIM_2 isActive=%d\r\n", isActive);
    }
    
    isActive = TRUE;
    ret = KING_CallWaitingServiceSet(SIM_1, isActive);
    if(-1 == ret)
    {
        LogPrintf("KING_CallWaitingServiceSet() SIM_1 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_CallWaitingServiceSet() SIM_1 isActive=%d\r\n", isActive);
    } 

    isActive = TRUE;
    ret = KING_CallWaitingServiceSet(SIM_0, isActive);
    if(-1 == ret)
    {
        LogPrintf("KING_CallWaitingServiceSet() SIM_0 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_CallWaitingServiceSet() SIM_0 isActive=%d\r\n", isActive);
    } 
    
    isActive = FALSE;
    ret = KING_CallWaitingServiceGet(SIM_0, &isActive);
    if(-1 == ret)
    {
        LogPrintf("KING_CallWaitingServiceGet() SIM_0 11 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_CallWaitingServiceGet() SIM_0 11 isActive=%d\r\n", isActive);
    }  
    
    LogPrintf("cc_callWaitingSetAndGet end!\r\n");
}

static void cc_eventCb(uint32 eventID, void* pdata, uint32 len)
{
    KH_CB_DATA_U* data = pdata;

    LogPrintf("cc_eventCb eventID=%d, pdata=%p\r\n", eventID, len);
    switch (eventID)
    {
        case CC_EVENT_INCOMING:
            LogPrintf("CC_EVENT_INCOMING call id %d number is %s",
                                data->CCEvent.callID, data->CCEvent.pNumber);
            ccEvtId = CC_EVENT_INCOMING;
            if (ccOptState == CC_OPT_STATE_ANSWER)
            {
                cc_ringTimer();
            }
            else if (ccOptState == CC_OPT_STATE_REJECT)
            {
                KING_Sleep(1000);
                cc_callReject();
            }
            break;
                
        case CC_EVENT_ORIGINATING:
            ccEvtId = CC_EVENT_ORIGINATING;
            LogPrintf("CC_EVENT_ORIGINATING call id %d", data->CCEvent.callID);
            break;
                
        case CC_EVENT_ALERTING:
            ccEvtId = CC_EVENT_ALERTING;
            LogPrintf("CC_EVENT_ALERTING call id %d", data->CCEvent.callID);
            break;
            
        case CC_EVENT_CONNECTED:
            LogPrintf("CC_EVENT_CONNECTED call id %d", data->CCEvent.callID);
            ccEvtId = CC_EVENT_CONNECTED;
            if (ccOptState == CC_OPT_STATE_NONE)
            {
                cc_handUpTimer();
            }
            else if (ccOptState == CC_OPT_STATE_ANSWER)
            {
                KING_Sleep(5000);
                cc_callHold(data->CCEvent.callID);
                KING_Sleep(5000);
                cc_callActive(data->CCEvent.callID);
                KING_Sleep(5000);
                cc_callHangUp(data->CCEvent.callID);
            }
            break;
                
        case CC_EVENT_TERMINATED:
            LogPrintf("CC_EVENT_TERMINATED call id %d", data->CCEvent.callID);
            ccEvtId = CC_EVENT_TERMINATED;
            if (ccOptState == CC_OPT_STATE_NONE)
            {
                ccOptState = CC_OPT_STATE_ANSWER;
            }
            else if (ccOptState == CC_OPT_STATE_ANSWER)
            {
                ccOptState = CC_OPT_STATE_REJECT;
            }
            else if (ccOptState == CC_OPT_STATE_REJECT)
            {
                if (testType == 2)
                {
                    cc_imsUnregisterBySimID(1);
                    KING_Sleep(1000);
                    cc_imsUnregisterBySimID(2);
                    KING_Sleep(1000);
                    cc_imsUnregisterBySimID(0);
                    KING_Sleep(1000);
                }
                ccOptState = CC_OPT_STATE_WAITING;
                cc_callWaitingSetAndGet();
            }
            break;
            
        case CC_EVENT_WAITING:
            LogPrintf("CC_EVENT_WAITING call id %d", data->CCEvent.callID);
            ccEvtId = CC_EVENT_WAITING;
            break;
                
        case CC_EVENT_ON_HOLD:
            LogPrintf("CC_EVENT_ON_HOLD call id %d", data->CCEvent.callID);
            ccEvtId = CC_EVENT_ON_HOLD;
            break;
            
        case CC_EVENT_ERROR:
            LogPrintf("CC_EVENT_ERROR call id %d", data->CCEvent.callID);
            ccEvtId = CC_EVENT_ERROR;
            break;

        default:
            LogPrintf("UNKOWN_EVENT call id %d", data->CCEvent.callID);
            ccEvtId = CC_EVENT_MAX;
            break;
    }
}

static void cc_voiceDial(void)
{
    int ret = -1;
    ORIG_CALL_INFO CallingInfo;

    ret = KING_CallVoiceDial(SIM_0, NULL);
    if (ret == -1)
    {
        LogPrintf("KING_CallVoiceDial fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    
    memset(&CallingInfo, 0, sizeof(ORIG_CALL_INFO));
    CallingInfo.phoneNumberPtr = (uint8*)"17850524520";
    CallingInfo.clir = CC_NO_CLIR;
    CallingInfo.cug = CC_NO_CUG;
    
    ret = KING_CallVoiceDial(SIM_1, &CallingInfo);
    if (ret == -1)
    {
        LogPrintf("KING_CallVoiceDial SIM_1 fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    
    ret = KING_CallVoiceDial(SIM_2, &CallingInfo);
    if (ret == -1)
    {
        LogPrintf("KING_CallVoiceDial SIM_2 fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }

    ret = KING_CallVoiceDial(SIM_0, &CallingInfo);
    if (ret == -1)
    {
        LogPrintf("KING_CallVoiceDial SIM_0 fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    
}

static void cc_AmrWbCapabilitySetAndGet(void)
{
    int ret = -1;
    bool isActive = FALSE;
    
    ret = KING_AmrWbCapabilityGet(SIM_0, NULL);
    if (ret == -1)
    {
        LogPrintf("KING_AmrWbCapabilityGet SIM_0 fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    } 
    
    //Not related to SIM card
    ret = KING_AmrWbCapabilityGet(SIM_0, &isActive);
    if (ret == -1)
    {
        LogPrintf("KING_AmrWbCapabilityGet SIM_0 fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    LogPrintf("KING_AmrWbCapabilityGet SIM_0 isActive=%d\r\n", isActive);

    isActive = TRUE;
    //not supported
    ret = KING_AmrWbCapabilitySet(SIM_0, isActive);
    if (ret == -1)
    {
        LogPrintf("KING_AmrWbCapabilitySet SIM_0 fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
}

static void cc_IMSTest(void)
{
    cc_opsImsBySimID(1);
    KING_Sleep(1000);
    cc_opsImsBySimID(2);
    KING_Sleep(1000);
    cc_opsImsBySimID(0);
    KING_Sleep(1000);
}

static void cc_startTest(int type)
{
    int ret;
    NET_STATE_E state;

    LogPrintf("\r\n======== cc_startTest start... ========\r\n");
    while (1)
    {
        ret = KING_NetStateGet(&state);
        LogPrintf("Get net state %d, ret : %d\r\n", state, ret);
        if (ret == 0 && state > NET_NO_SERVICE && state < NET_EMERGENCY)
        {
            NET_CESQ_S Cesq;
            ret = KING_NetSignalQualityGet(&Cesq);
            LogPrintf("Get sinagal ret is %d. rxlev:%d, ber:%d, rscp:%d, ecno:%d, rsrq:%d, rsrp:%d\r\n",
                ret,Cesq.rxlev,Cesq.ber,Cesq.rscp,Cesq.ecno,Cesq.rsrq,Cesq.rsrp);
            
            cc_AmrWbCapabilitySetAndGet();
            if (type == 2)
            {
                cc_IMSTest();
            }
            cc_voiceDial();

            LogPrintf("cc_startTest complete\r\n");
            break;
        }
        
        KING_Sleep(1000);
    }

    LogPrintf("\r\n======== cc_startTest complete ========\r\n");
}

void cc_test(int type)
{
    KING_RegNotifyFun(DEV_CLASS_CALL_CONTROL, 0, cc_eventCb);
    testType = type;
    cc_startTest(type);
}
