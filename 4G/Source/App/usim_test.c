/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "app.h"
#include "kingusim.h"
#include "kingcstd.h"
#include "kingcbdata.h"
#include "kingrtos.h"
#include "kingplat.h"
#include "uart.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
//#define LogPrintf(fmt, args...)    do { KING_SysLog("usim: "fmt, ##args); } while(0)

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
static void Usim_enterPinTest(void)
{
    int ret = 0;
    SIM_STATES simState = SIM_STATE_UNKNOWN;
    SIM_LOCK_STATE LockState = SIM_LOCKEDSTATE_UNKNOWN;
    
    U2_Send("Usim_enterPinTest() start\r\n"); 
    ret = KING_SimCardInformationGet(SIM_0, &simState);
    U2_Send("KING_SimCardInformationGet--pin ret=%d, simState=%ld\r\n", ret, simState);

    ret = KING_SimCardLockStateGet(SIM_0, &LockState);
    U2_Send("KING_SimCardLockStateGet--pin ret=%d, LockState=%ld\r\n", ret, LockState);

    if (LockState == SIM_LOCKEDSTATE_SIM_PIN)
    {
        ret = KING_SimPinEnter(SIM_0, "1234");
        U2_Send("KING_SimPinEnter--SIM_0 ret=%d\r\n", ret);
    }
    else if (LockState == SIM_LOCKEDSTATE_SIM_PUK)
    {
        ret = KING_SimPukEnter(SIM_0, "87822119", "1234");
        U2_Send("Usim_enterPinTest--SIM_0 ret=%d\r\n", ret);
    }
}

static  void Usim_changPinTest(void)
{
    int ret = 0;
    SIM_STATES simState = SIM_STATE_UNKNOWN;
    SIM_LOCK_STATE LockState = SIM_LOCKEDSTATE_UNKNOWN;
    
    U2_Send("Usim_changPinTest() start\r\n"); 
    ret = KING_SimCardInformationGet(SIM_0, &simState);
    U2_Send("KING_SimCardInformationGet--changePin ret=%d, simState=%ld\r\n", ret, simState);

    ret = KING_SimCardLockStateGet(SIM_0, &LockState);
    U2_Send("KING_SimCardLockStateGet--pin ret=%d, LockState=%ld\r\n", ret, LockState);

    ret = KING_SimPinChange(SIM_0, "1234", "4321");
    U2_Send("KING_SimPinChange--SIM_0 ret=%d\r\n", ret);

    ret = KING_SimPinChange(SIM_0, "4321", "1234");
    U2_Send("KING_SimPinChange--SIM_0 11 ret=%d\r\n", ret);
}

static  void Usim_enterPukTest(void)
{
    int ret = 0;
    SIM_STATES simState = SIM_STATE_UNKNOWN;
    SIM_LOCK_STATE LockState = SIM_LOCKEDSTATE_UNKNOWN;
    
    U2_Send("Usim_enterPukTest() start\r\n"); 
    ret = KING_SimCardInformationGet(SIM_0, &simState);
    U2_Send("KING_SimCardInformationGet--puk ret=%d, simState=%ld\r\n", ret, simState);

    ret = KING_SimCardLockStateGet(SIM_0, &LockState);
    U2_Send("KING_SimCardLockStateGet--pin ret=%d, LockState=%ld\r\n", ret, LockState);

    ret = KING_SimPukEnter(SIM_0, "87822119", "1234");
    U2_Send("Usim_enterPukTest--SIM_0 ret=%d\r\n", ret);
    
}

static void Usim_pinLock(int state)
{
    int ret = 0;

    ret = KING_SimPinLock(SIM_0, "1234", state);
    U2_Send("KING_SimPinLock--pin ret=%d\r\n", ret);
}

static void Usim_startTest(void)
{
    int ret = 0;
    SIM_STATES simState = SIM_STATE_UNKNOWN;
    ICCID_S iccid;
    IMSI_S imsi;
    uint8 simEid[SIM_EID_MAX_LEN + 1] = {0};
    uint8 simPhonenum[PHONE_NUM_MAX_BYTES + 1] = {0};
    uint32 pinTries = 0, pukTries = 0;
    uint8 simHomeNetOptName[SIM_HOME_NET_OPT_NAME_LEN + 1] = {0};
    uint8 simHomeMcc[SIM_HOME_MCC_MNC_LEN + 1] = {0};
    uint8 simHomeMnc[SIM_HOME_MCC_MNC_LEN + 1] = {0};
    SIM_LOCK_STATE LockState = SIM_LOCKEDSTATE_UNKNOWN;
    
    //get sim card state
    U2_Send("KING_SimCardInformationGet() start\r\n"); 
    ret = KING_SimCardInformationGet(SIM_1, &simState);
    U2_Send("KING_SimCardInformationGet--SIM_1 ret=%d, simState=%ld\r\n", ret, simState);
    ret = KING_SimCardInformationGet(SIM_2, &simState);
    U2_Send("KING_SimCardInformationGet--SIM_2 ret=%d, simState=%ld\r\n", ret, simState);
    ret = KING_SimCardInformationGet(SIM_0, &simState);
    U2_Send("KING_SimCardInformationGet--SIM_0 ret=%d, simState=%ld\r\n", ret, simState);

    //get sim card lock state
    U2_Send("KING_SimCardLockStateGet() start\r\n"); 
    ret = KING_SimCardLockStateGet(SIM_1, &LockState);
    U2_Send("KING_SimCardLockStateGet--SIM_1 ret=%d, LockState=%ld\r\n", ret, LockState);
    ret = KING_SimCardLockStateGet(SIM_2, &LockState);
    U2_Send("KING_SimCardLockStateGet--SIM_2 ret=%d, LockState=%ld\r\n", ret, LockState);
    ret = KING_SimCardLockStateGet(SIM_0, &LockState);
    U2_Send("KING_SimCardLockStateGet--SIM_0 ret=%d, LockState=%ld\r\n", ret, LockState);

    //get sim iccid test
    memset(&iccid, 0, sizeof(ICCID_S));
    ret = KING_SimICCIDGet(SIM_1, &iccid);
    U2_Send("KING_SimICCIDGet--SIM_1 ret=%d\r\n", ret);
    U2_Send("KING_SimICCIDGet--SIM_1 iccid.id_num:%s\r\n", iccid.id_num);
    memset(&iccid, 0, sizeof(ICCID_S));
    ret = KING_SimICCIDGet(SIM_2, &iccid);
    U2_Send("KING_SimICCIDGet--SIM_2 ret=%d\r\n", ret);
    U2_Send("KING_SimICCIDGet--SIM_2 iccid.id_num:%s\r\n", iccid.id_num);
    memset(&iccid, 0, sizeof(ICCID_S));
    ret = KING_SimICCIDGet(SIM_0, &iccid);
    U2_Send("KING_SimICCIDGet--SIM_0 ret=%d\r\n", ret);
    U2_Send("KING_SimICCIDGet--SIM_0 iccid.id_num:%s\r\n", iccid.id_num);

    //get sim imsi test
    memset(&imsi, 0, sizeof(IMSI_S));
    ret = KING_SimImsiGet(SIM_1, &imsi);
    U2_Send("KING_SimImsiGet--SIM_1 ret=%d, imsi.imsi_len=%d\r\n", ret, imsi.imsi_len);
    U2_Send("KING_SimImsiGet--SIM_1 imsi.imsi_val:%s\r\n", imsi.imsi_val);
    memset(&imsi, 0, sizeof(IMSI_S));
    ret = KING_SimImsiGet(SIM_2, &imsi);
    U2_Send("KING_SimImsiGet--SIM_2 ret=%d, imsi.imsi_len=%d\r\n", ret, imsi.imsi_len);
    U2_Send("KING_SimImsiGet--SIM_2 imsi.imsi_val:%s\r\n", imsi.imsi_val);
    memset(&imsi, 0, sizeof(IMSI_S));
    ret = KING_SimImsiGet(SIM_0, &imsi);
    U2_Send("KING_SimImsiGet--SIM_0 ret=%d, imsi.imsi_len=%d\r\n", ret, imsi.imsi_len);
    U2_Send("KING_SimImsiGet--SIM_0 imsi.imsi_val:%s\r\n", imsi.imsi_val);

    //get sim eid test
    memset(simEid, 0x00, SIM_EID_MAX_LEN + 1);
    ret = KING_SimEidGet(SIM_1, simEid);
    U2_Send("KING_SimEidGet--SIM_1 ret=%d\r\n", ret);
    U2_Send("KING_SimEidGet--SIM_1 simEid:%s\r\n", simEid);
    memset(simEid, 0x00, SIM_EID_MAX_LEN + 1);
    ret = KING_SimEidGet(SIM_2, simEid);
    U2_Send("KING_SimEidGet--SIM_2 ret=%d\r\n", ret);
    U2_Send("KING_SimEidGet--SIM_2 simEid:%s\r\n", simEid);
    memset(simEid, 0x00, SIM_EID_MAX_LEN + 1);
    ret = KING_SimEidGet(SIM_0, simEid);
    U2_Send("KING_SimEidGet--SIM_0 ret=%d\r\n", ret);
    U2_Send("KING_SimEidGet--SIM_0 simEid:%s\r\n", simEid);

    //get sim pin remain count
    U2_Send("KING_SimRemainingPinTriesGet() start\r\n"); 
    ret = KING_SimRemainingPinTriesGet(SIM_1, &pinTries);
    U2_Send("KING_SimRemainingPinTriesGet--SIM_1 ret=%d, pinTries=%u\r\n", ret, pinTries);
    ret = KING_SimRemainingPinTriesGet(SIM_2, &pinTries);
    U2_Send("KING_SimRemainingPinTriesGet--SIM_2 ret=%d, pinTries=%u\r\n", ret, pinTries);
    ret = KING_SimRemainingPinTriesGet(SIM_0, &pinTries);
    U2_Send("KING_SimRemainingPinTriesGet--SIM_0 ret=%d, pinTries=%u\r\n", ret, pinTries);

    KING_Sleep(1000);
    //get sim puk remain count
    U2_Send("KING_SimRemainingPukTriesGet() start\r\n"); 
    ret = KING_SimRemainingPukTriesGet(SIM_1, &pukTries);
    U2_Send("KING_SimRemainingPukTriesGet--SIM_1 ret=%d, pukTries=%u\r\n", ret, pukTries);
    ret = KING_SimRemainingPukTriesGet(SIM_2, &pukTries);
    U2_Send("KING_SimRemainingPukTriesGet--SIM_2 ret=%d, pukTries=%u\r\n", ret, pukTries);
    ret = KING_SimRemainingPukTriesGet(SIM_0, &pukTries);
    U2_Send("KING_SimRemainingPukTriesGet--SIM_0 ret=%d, pukTries=%u\r\n", ret, pukTries);

    //get sim net opt name test
    memset(simHomeNetOptName, 0x00, SIM_HOME_NET_OPT_NAME_LEN + 1);
    ret = KING_SimHomeNetworkOperatorGet(SIM_1, simHomeNetOptName);
    U2_Send("KING_SimHomeNetworkOperatorGet--SIM_1 ret=%d\r\n", ret);
    U2_Send("KING_SimHomeNetworkOperatorGet--SIM_1 simHomeNetOptName:%s\r\n", simHomeNetOptName);
    memset(simHomeNetOptName, 0x00, SIM_HOME_NET_OPT_NAME_LEN + 1);
    ret = KING_SimHomeNetworkOperatorGet(SIM_2, simHomeNetOptName);
    U2_Send("KING_SimHomeNetworkOperatorGet--SIM_2 ret=%d\r\n", ret);
    U2_Send("KING_SimHomeNetworkOperatorGet--SIM_2 simHomeNetOptName:%s\r\n", simHomeNetOptName);
    memset(simHomeNetOptName, 0x00, SIM_HOME_NET_OPT_NAME_LEN + 1);
    ret = KING_SimHomeNetworkOperatorGet(SIM_0, simHomeNetOptName);
    U2_Send("KING_SimHomeNetworkOperatorGet--SIM_0 ret=%d\r\n", ret);
    U2_Send("KING_SimHomeNetworkOperatorGet--SIM_0 simHomeNetOptName:%s\r\n", simHomeNetOptName);

    //get sim mcc mnc test
    memset(simHomeMcc, 0x00, SIM_HOME_MCC_MNC_LEN + 1);
    memset(simHomeMnc, 0x00, SIM_HOME_MCC_MNC_LEN + 1);
    ret = KING_SimHomePLMNGet(SIM_1, simHomeMcc, simHomeMnc);
    U2_Send("KING_SimHomePLMNGet--SIM_1 ret=%d\r\n", ret);
    U2_Send("KING_SimHomePLMNGet--SIM_0 simHomeMcc:%s\r\n", simHomeMcc);
    U2_Send("KING_SimHomePLMNGet--SIM_0 simHomeMnc=%s\r\n", simHomeMnc);
    memset(simHomeMcc, 0x00, SIM_HOME_MCC_MNC_LEN + 1);
    memset(simHomeMnc, 0x00, SIM_HOME_MCC_MNC_LEN + 1);
    ret = KING_SimHomePLMNGet(SIM_2, simHomeMcc, simHomeMnc);
    U2_Send("KING_SimHomePLMNGet--SIM_2 ret=%d\r\n", ret);
    U2_Send("KING_SimHomePLMNGet--SIM_0 simHomeMcc:%s\r\n", simHomeMcc);
    U2_Send("KING_SimHomePLMNGet--SIM_0 simHomeMnc=%s\r\n", simHomeMnc);
    memset(simHomeMcc, 0x00, SIM_HOME_MCC_MNC_LEN + 1);
    memset(simHomeMnc, 0x00, SIM_HOME_MCC_MNC_LEN + 1);
    ret = KING_SimHomePLMNGet(SIM_0, simHomeMcc, simHomeMnc);
    U2_Send("KING_SimHomePLMNGet--SIM_0 ret=%d\r\n", ret);
    U2_Send("KING_SimHomePLMNGet--SIM_0 simHomeMcc:%s\r\n", simHomeMcc);
    U2_Send("KING_SimHomePLMNGet--SIM_0 simHomeMnc=%s\r\n", simHomeMnc);

    KING_Sleep(5000);
    //get sim phone num
    memset(simPhonenum, 0x00, PHONE_NUM_MAX_BYTES + 1);
    ret = KING_SimPhoneNumberGet(SIM_1, simPhonenum);
    U2_Send("KING_SimPhoneNumberGet--SIM_1 ret=%d\r\n", ret);
    U2_Send("KING_SimPhoneNumberGet--SIM_1 simPhonenum:%s\r\n", simPhonenum);
    memset(simPhonenum, 0x00, PHONE_NUM_MAX_BYTES + 1);
    ret = KING_SimPhoneNumberGet(SIM_2, simPhonenum);
    U2_Send("KING_SimPhoneNumberGet--SIM_2 ret=%d\r\n", ret);
    U2_Send("KING_SimPhoneNumberGet--SIM_2 simPhonenum:%s\r\n", simPhonenum);
    memset(simPhonenum, 0x00, PHONE_NUM_MAX_BYTES + 1);
    ret = KING_SimPhoneNumberGet(SIM_0, simPhonenum);
    U2_Send("KING_SimPhoneNumberGet--SIM_0 ret=%d\r\n", ret);
    U2_Send("KING_SimPhoneNumberGet--SIM_0 simPhonenum:%s\r\n", simPhonenum);
    
}

static void Usim_SimEventProcess(uint32 eventID, void* pdata, uint32 len)
{
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pdata;
    
    U2_Send(" Usim_SimEventProcess() eventID=%d, dataLen=%d\r\n", eventID, len);
    switch(eventID)
    {
        case SIM_EVENT_PIN_PUK_BLOCK:
            U2_Send("Usim_SimEventProcess() SIM_EVENT_PIN_PUK_BLOCK lockState=%d\r\n", data->usimEvent.lockState);
            if (data->usimEvent.lockState == SIM_LOCKEDSTATE_READY)
            {
                KING_Sleep(1000);
                Usim_changPinTest();
                KING_Sleep(1000);
                Usim_pinLock(0);
            }
            else if (data->usimEvent.lockState == SIM_LOCKEDSTATE_SIM_PUK)
            {
                Usim_enterPukTest();
            }
            else if (data->usimEvent.lockState == SIM_LOCKEDSTATE_SIM_PIN)
            {
                Usim_enterPinTest();
            }
            break;

        case SIM_EVENT_PIN_CHANGE:
            U2_Send("Usim_SimEventProcess() SIM_EVENT_PIN_CHANGE lockState=%d\r\n", data->usimEvent.lockState);
            break;

        default:
            break;
    }
}

static void king_usimStateCb(uint32 eventID, void* pData, uint32 len)
{
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pData;

    U2_Send(" king_usimStateCb() eventID=%d, classID=%d\r\n", eventID, data->usimState.classID);
    if (eventID == SIM_EVENT_STATE && data->usimState.classID == DEV_CLASS_SIM_STATE)
    {
        uint32 state = data->usimState.state;
        U2_Send(" king_usimStateCb() state=%d\r\n", state);

        switch(state)
        {
            case SIM_ABSENT:
                U2_Send(" king_usimStateCb() SIM_ABSENT\r\n");
                break;

            case SIM_INSERTED:
                U2_Send(" king_usimStateCb() SIM_INSERTED\r\n");
                break;

            case SIM_READY:
                U2_Send(" king_usimStateCb() SIM_READY\r\n");
                Usim_startTest();
                Usim_pinLock(1);
                break;

            case SIM_BLOCKED:
                U2_Send(" king_usimStateCb() SIM_BLOCKED\r\n");
                Usim_startTest();
                KING_Sleep(1000);
                Usim_enterPinTest();
                break;

            default:
                break;
        }
    }
}

/*******************************************************************************
 ** Functions
 ******************************************************************************/
void usim_test(void)
{
    KING_RegNotifyFun(DEV_CLASS_SIM_STATE, 0, king_usimStateCb);
    KING_RegNotifyFun(DEV_CLASS_SIM_EVENT, 0, Usim_SimEventProcess);
}
