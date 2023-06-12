/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "kingsms.h"
#include "kingusim.h"
#include "kingcstd.h"
#include "kingcbData.h"
#include "kingplat.h"
#include "kingrtos.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("sms: "fmt, ##args); } while(0)

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static uint8 smscNumber[PHONE_NUM_MAX_BYTES + 2] = {0};
static bool isPdu = FALSE;

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
static void SMS_startTest(void)
{
    int ret = 0;
    uint32 used = 0, total = 0;
    
    //get sms smsc
    LogPrintf("KING_SmsSmscGet() start\r\n"); 
    memset(smscNumber, 0x00, PHONE_NUM_MAX_BYTES + 2);
    ret = KING_SmsSmscGet(SIM_1, smscNumber, PHONE_NUM_MAX_BYTES);
    LogPrintf("KING_SmsSmscGet--SIM_1 ret=%d\r\n", ret);
    if (ret == 0)
    {
        LogPrintf("KING_SmsSmscGet--SIM_1 smscNumber:%s\r\n", smscNumber);
    }
    ret =  KING_SmsSmscGet(SIM_2, smscNumber, PHONE_NUM_MAX_BYTES);
    LogPrintf("KING_SmsSmscGet--SIM_2 ret=%d\r\n", ret);
    if (ret == 0)
    {
        LogPrintf("KING_SmsSmscGet--SIM_2 smscNumber:%s\r\n", smscNumber);
    }
    ret =  KING_SmsSmscGet(SIM_0, smscNumber, PHONE_NUM_MAX_BYTES);
    LogPrintf("KING_SmsSmscGet--SIM_0 ret=%d\r\n", ret);
    if (ret == 0)
    {
        LogPrintf("KING_SmsSmscGet--SIM_0 smscNumber:%s\r\n", smscNumber);
    }
    
    ret = KING_SmsSmscSet(SIM_1, (uint8 *)"+18312345678");
    LogPrintf("KING_SmsSmscSet--SIM_1 SMS_STORAGE_SIM ret=%d\r\n", ret);
    ret = KING_SmsSmscSet(SIM_2, (uint8 *)"+18312345678");
    LogPrintf("KING_SmsSmscSet--SIM_2 SMS_STORAGE_SIM ret=%d\r\n", ret);
    
    //sms cellbroadcast act
    LogPrintf("KING_SmsCellBroadcastActivate() start\r\n"); 
    ret = KING_SmsCellBroadcastActivate(SIM_1, SMS_PROTOCOL_GSM);
    LogPrintf("KING_SmsCellBroadcastActivate--SIM_1 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());
    ret = KING_SmsCellBroadcastActivate(SIM_2, SMS_PROTOCOL_GSM);
    LogPrintf("KING_SmsCellBroadcastActivate--SIM_2 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());
    ret = KING_SmsCellBroadcastActivate(SIM_0, SMS_PROTOCOL_GSM);
    LogPrintf("KING_SmsCellBroadcastActivate--SIM_0 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());

    //sms cellbroadcast deact
    LogPrintf("KING_SmsCellBroadcastDeactivate() start\r\n"); 
    ret = KING_SmsCellBroadcastDeactivate(SIM_1, SMS_PROTOCOL_GSM);
    LogPrintf("KING_SmsCellBroadcastDeactivate--SIM_1 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());
    ret = KING_SmsCellBroadcastDeactivate(SIM_2, SMS_PROTOCOL_GSM);
    LogPrintf("KING_SmsCellBroadcastDeactivate--SIM_2 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());
    ret = KING_SmsCellBroadcastDeactivate(SIM_0, SMS_PROTOCOL_GSM);
    LogPrintf("KING_SmsCellBroadcastDeactivate--SIM_0 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());

    //sms cellbroadcast ids add
    LogPrintf("KING_SmsCellBroadcastIdsAdd() start\r\n"); 
    ret = KING_SmsCellBroadcastIdsAdd(SIM_1, 1, 5);
    LogPrintf("KING_SmsCellBroadcastIdsAdd--SIM_1 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());
    ret = KING_SmsCellBroadcastIdsAdd(SIM_2, 1, 5);
    LogPrintf("KING_SmsCellBroadcastIdsAdd--SIM_2 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());
    ret = KING_SmsCellBroadcastIdsAdd(SIM_0, 1, 5);
    LogPrintf("KING_SmsCellBroadcastIdsAdd--SIM_0 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());

    //sms cellbroadcast ids Remove
    LogPrintf("KING_SmsCellBroadcastIdsRemove() start\r\n"); 
    ret = KING_SmsCellBroadcastIdsRemove(SIM_1, 1, 5);
    LogPrintf("KING_SmsCellBroadcastIdsRemove--SIM_1 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());
    ret = KING_SmsCellBroadcastIdsRemove(SIM_2, 1, 5);
    LogPrintf("KING_SmsCellBroadcastIdsRemove--SIM_2 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());
    ret = KING_SmsCellBroadcastIdsRemove(SIM_0, 1, 5);
    LogPrintf("KING_SmsCellBroadcastIdsRemove--SIM_0 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());

    //sms cellbroadcast ids Remove
    LogPrintf("KING_SmsCellBroadcastIdsClear() start\r\n"); 
    ret = KING_SmsCellBroadcastIdsClear(SIM_1);
    LogPrintf("KING_SmsCellBroadcastIdsClear--SIM_1 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());
    ret = KING_SmsCellBroadcastIdsClear(SIM_2);
    LogPrintf("KING_SmsCellBroadcastIdsClear--SIM_2 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());
    ret = KING_SmsCellBroadcastIdsClear(SIM_0);
    LogPrintf("KING_SmsCellBroadcastIdsClear--SIM_0 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());

    //get storage status
    LogPrintf("KING_SmsStorageStatusGet() start\r\n"); 
    ret = KING_SmsStorageStatusGet(SIM_1, SMS_STORAGE_BROADCAST, &used, &total);
    LogPrintf("KING_SmsStorageStatusGet--SIM_1 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());
    ret = KING_SmsStorageStatusGet(SIM_2, SMS_STORAGE_BROADCAST, &used, &total);
    LogPrintf("KING_SmsStorageStatusGet--SIM_2 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());
    ret = KING_SmsStorageStatusGet(SIM_0, SMS_STORAGE_BROADCAST, &used, &total);
    LogPrintf("KING_SmsStorageStatusGet--SIM_0 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());

    //get storage status
    LogPrintf("KING_SmsStorageStatusGet() start\r\n"); 
    ret = KING_SmsStorageStatusGet(SIM_1, SMS_STORAGE_SIM, &used, &total);
    LogPrintf("KING_SmsStorageStatusGet--SIM_1 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());
    LogPrintf("KING_SmsStorageStatusGet--SIM_1 used=%ld, total=%ld\r\n", used, total);
    ret = KING_SmsStorageStatusGet(SIM_2, SMS_STORAGE_SIM, &used, &total);
    LogPrintf("KING_SmsStorageStatusGet--SIM_2 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());
    LogPrintf("KING_SmsStorageStatusGet--SIM_2 used=%ld, total=%ld\r\n", used, total);
    ret = KING_SmsStorageStatusGet(SIM_0, SMS_STORAGE_SIM, &used, &total);
    LogPrintf("KING_SmsStorageStatusGet--SIM_0 ret=%d, error=0x%x\r\n", ret, KING_GetLastErrCode());
    LogPrintf("KING_SmsStorageStatusGet--SIM_0 used=%ld, total=%ld\r\n", used, total);

}

static void sms_sendPduMsg(void)
{
    int ret = 0;
    SMS_MESSAGE_T msg;
    
    LogPrintf("sms_sendPduMsg() start\r\n"); 
    memset(&msg, 0x00, sizeof(SMS_MESSAGE_T));
    msg.type = SMS_SUBMIT;
    msg.smsSubmit.option = SMS_OPTIONMASK_OA;
    //memcpy(msg.smsSubmit.addr, "13400692096", strlen("13400692096"));
    msg.smsSubmit.format = FORMAT_PDU;
    
    msg.smsSubmit.dataLen = strlen("0891683108502905F011000B813104602990F60008B0108BD59A8C4E2D658777ED4FE151855BB9");
    memcpy(msg.smsSubmit.data, "0891683108502905F011000B813104602990F60008B0108BD59A8C4E2D658777ED4FE151855BB9", msg.smsSubmit.dataLen);
    
    ret = KING_SmsMsgSend(SIM_1, SMS_STORAGE_BROADCAST, &msg);
    LogPrintf("sms_sendPduMsg--SIM_1 SMS_STORAGE_BROADCAST ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("sms_sendPduMsg--SIM_1 SMS_STORAGE_BROADCAST error=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_SmsMsgSend(SIM_2, SMS_STORAGE_BROADCAST, &msg);
    LogPrintf("sms_sendPduMsg--SIM_2 SMS_STORAGE_BROADCAST ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("sms_sendPduMsg--SIM_2 SMS_STORAGE_BROADCAST error=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_SmsMsgSend(SIM_0, SMS_STORAGE_BROADCAST, &msg);
    LogPrintf("sms_sendPduMsg--SIM_0 SMS_STORAGE_BROADCAST ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("sms_sendPduMsg--SIM_0 SMS_STORAGE_BROADCAST error=0x%x\r\n", KING_GetLastErrCode());
    }

    ret = KING_SmsMsgSend(SIM_1, SMS_STORAGE_SIM, &msg);
    LogPrintf("sms_sendPduMsg--SIM_1 SMS_STORAGE_SIM ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("sms_sendPduMsg--SIM_1 SMS_STORAGE_SIM error=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_SmsMsgSend(SIM_2, SMS_STORAGE_SIM, &msg);
    LogPrintf("sms_sendPduMsg--SIM_2 SMS_STORAGE_SIM ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("sms_sendPduMsg--SIM_2 SMS_STORAGE_SIM error=0x%x\r\n", KING_GetLastErrCode());
    }
    
    ret = KING_SmsMsgSend(SIM_0, SMS_STORAGE_SIM, &msg);
    LogPrintf("sms_sendPduMsg--SIM_0 SMS_STORAGE_SIM ret=%d\r\n", ret);
}

static void sms_sendMsg(void)
{
    int ret = 0;
    SMS_MESSAGE_T msg;
    
    LogPrintf("sms_sendMsg() start\r\n"); 
    memset(&msg, 0x00, sizeof(SMS_MESSAGE_T));
    msg.type = SMS_SUBMIT;
    msg.smsSubmit.option = SMS_OPTIONMASK_OA;
    memcpy(msg.smsSubmit.addr, "13400692096", strlen("13400692096"));
    msg.smsSubmit.format = FORMAT_TEXT;
    memcpy(msg.smsSubmit.data, "asdfghjklopiuytreqzxcb", strlen("asdfghjklopiuytreqzxcb"));
    msg.smsSubmit.dataLen = 22;
    
    ret = KING_SmsMsgSend(SIM_1, SMS_STORAGE_BROADCAST, &msg);
    LogPrintf("KING_SmsMsgSend--SIM_1 SMS_STORAGE_BROADCAST ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgSend--SIM_1 SMS_STORAGE_BROADCAST error=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_SmsMsgSend(SIM_2, SMS_STORAGE_BROADCAST, &msg);
    LogPrintf("KH_SmsMsgSend--SIM_2 SMS_STORAGE_BROADCAST ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgSend--SIM_2 SMS_STORAGE_BROADCAST error=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_SmsMsgSend(SIM_0, SMS_STORAGE_BROADCAST, &msg);
    LogPrintf("KING_SmsMsgSend--SIM_0 SMS_STORAGE_BROADCAST ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgSend--SIM_0 SMS_STORAGE_BROADCAST error=0x%x\r\n", KING_GetLastErrCode());
    }

    ret = KING_SmsMsgSend(SIM_1, SMS_STORAGE_SIM, &msg);
    LogPrintf("KING_SmsMsgSend--SIM_1 SMS_STORAGE_SIM ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgSend--SIM_1 SMS_STORAGE_SIM error=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_SmsMsgSend(SIM_2, SMS_STORAGE_SIM, &msg);
    LogPrintf("KING_SmsMsgSend--SIM_2 SMS_STORAGE_SIM ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgSend--SIM_2 SMS_STORAGE_SIM error=0x%x\r\n", KING_GetLastErrCode());
    }
    
    ret = KING_SmsMsgSend(SIM_0, SMS_STORAGE_SIM, &msg);
    LogPrintf("KING_SmsMsgSend--SIM_0 SMS_STORAGE_SIM ret=%d\r\n", ret);
}

static void sms_readMsg(uint16 index)
{
    int ret = 0;
    SMS_MESSAGE_T msg;
    
    LogPrintf("[KING]sms_readMsg() start\r\n"); 

    memset(&msg, 0x00, sizeof(SMS_MESSAGE_T));
    ret = KING_SmsMsgGet(SIM_1, SMS_STORAGE_BROADCAST, index, &msg);
    LogPrintf("KING_SmsMsgGet--SIM_1 SMS_STORAGE_BROADCAST ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgGet--SIM_1 SMS_STORAGE_BROADCAST error=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_SmsMsgGet(SIM_2, SMS_STORAGE_BROADCAST, index, &msg);
    LogPrintf("KING_SmsMsgGet--SIM_2 SMS_STORAGE_BROADCAST ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgGet--SIM_2 SMS_STORAGE_BROADCAST error=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_SmsMsgGet(SIM_0, SMS_STORAGE_BROADCAST, index, &msg);
    LogPrintf("KING_SmsMsgGet--SIM_0 SMS_STORAGE_BROADCAST ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgGet--SIM_0 SMS_STORAGE_BROADCAST error=0x%x\r\n", KING_GetLastErrCode());
    }

    ret = KING_SmsMsgGet(SIM_1, SMS_STORAGE_SIM, index, &msg);
    LogPrintf("KING_SmsMsgGet--SIM_1 SMS_STORAGE_SIM ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgGet--SIM_1 SMS_STORAGE_SIM error=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_SmsMsgGet(SIM_2, SMS_STORAGE_SIM, index, &msg);
    LogPrintf("KING_SmsMsgGet--SIM_2 SMS_STORAGE_SIM ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgGet--SIM_2 SMS_STORAGE_SIM error=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_SmsMsgGet(SIM_0, SMS_STORAGE_SIM, index, &msg);
    LogPrintf("KING_SmsMsgGet--SIM_0 SMS_STORAGE_SIM ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgGet--SIM_0 SMS_STORAGE_SIM error=0x%x\r\n", KING_GetLastErrCode());
    }
    LogPrintf("KING_SmsMsgGet--SIM_0 type:%d\r\n", msg.type);
    if (msg.type == SMS_SUBMIT)
    {
        LogPrintf("KING_SmsMsgGet--SIM_0 smsSubmit.status:%d\r\n", msg.smsSubmit.status);
        LogPrintf("KING_SmsMsgGet--SIM_0 smsSubmit.addr:%s\r\n", msg.smsSubmit.addr);
        LogPrintf("KING_SmsMsgGet--SIM_0 smsSubmit.scts:%s\r\n", msg.smsSubmit.scts);
        LogPrintf("KING_SmsMsgGet--SIM_0 smsSubmit.addr:%s\r\n", msg.smsSubmit.data);
        LogPrintf("KING_SmsMsgGet--SIM_0 smsSubmit.option:%d\r\n", msg.smsSubmit.option);
        LogPrintf("KING_SmsMsgGet--SIM_0 smsSubmit.format:%d\r\n", msg.smsSubmit.format);
    }
    else
    {
        LogPrintf("KING_SmsMsgGet--SIM_0 smsDeliver.status:%d\r\n", msg.smsDeliver.status);
        LogPrintf("KING_SmsMsgGet--SIM_0 smsDeliver.addr:%s\r\n", msg.smsDeliver.addr);
        LogPrintf("KING_SmsMsgGet--SIM_0 smsDeliver.scts:%s\r\n", msg.smsDeliver.scts);
        LogPrintf("KING_SmsMsgGet--SIM_0 smsDeliver.addr:%s\r\n", msg.smsDeliver.data);
        LogPrintf("KING_SmsMsgGet--SIM_0 smsDeliver.option:%d\r\n", msg.smsDeliver.option);
        LogPrintf("KING_SmsMsgGet--SIM_0 smsDeliver.format:%d\r\n", msg.smsDeliver.format);
    }
}

static  void sms_delMsg(uint16 index)
{
    int ret = 0;
    
    LogPrintf("[KING]sms_delMsg() start\r\n"); 

    ret = KING_SmsMsgDelete(SIM_1, SMS_STORAGE_BROADCAST, index);
    LogPrintf("KING_SmsMsgDelete--SIM_1 SMS_STORAGE_BROADCAST ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgDelete--SIM_1 SMS_STORAGE_BROADCAST error=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_SmsMsgDelete(SIM_2, SMS_STORAGE_BROADCAST, index);
    LogPrintf("KING_SmsMsgDelete--SIM_2 SMS_STORAGE_BROADCAST ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgDelete--SIM_2 SMS_STORAGE_BROADCAST error=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_SmsMsgDelete(SIM_0, SMS_STORAGE_BROADCAST, index);
    LogPrintf("KING_SmsMsgDelete--SIM_0 SMS_STORAGE_BROADCAST ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgDelete--SIM_0 SMS_STORAGE_BROADCAST error=0x%x\r\n", KING_GetLastErrCode());
    }

    ret = KING_SmsMsgDelete(SIM_1, SMS_STORAGE_SIM, index);
    LogPrintf("KING_SmsMsgDelete--SIM_1 SMS_STORAGE_SIM ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgDelete--SIM_1 SMS_STORAGE_SIM error=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_SmsMsgDelete(SIM_2, SMS_STORAGE_SIM, index);
    LogPrintf("KING_SmsMsgDelete--SIM_2 SMS_STORAGE_SIM ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgDelete--SIM_2 SMS_STORAGE_SIM error=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_SmsMsgDelete(SIM_0, SMS_STORAGE_SIM, index);
    LogPrintf("KING_SmsMsgDelete--SIM_0 SMS_STORAGE_SIM ret=%d\r\n", ret);
    if (ret != 0)
    {
        LogPrintf("KING_SmsMsgDelete--SIM_0 SMS_STORAGE_SIM error=0x%x\r\n", KING_GetLastErrCode());
    }
    
}

static void SMS_EventProcessCb(uint32 eventID, void* pdata, uint32 len)
{
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pdata;
    
    LogPrintf("SMS_EventProcessCb() eventID=%d, dataLen=%d\r\n", eventID, len);
    switch(eventID)
    {
        case SMS_EVENT_SMS_INIT_STATUS:
            KING_Sleep(1000);
            LogPrintf("SMS_EVENT_SMS_INIT_STATUS nParam=%d\r\n", data->smsEvent.nParam);
            if (data->smsEvent.nParam == 1)
            {
                SMS_startTest();
                if (isPdu)
                {
                    sms_sendPduMsg();
                }
                else
                {
                    sms_sendMsg();
                }
            }
            break;

        case SMS_EVENT_NEW_SMS_IND:
            LogPrintf("SMS_EVENT_NEW_SMS_IND nParam=%d\r\n", data->smsEvent.nParam);
            sms_readMsg((uint16)data->smsEvent.nParam);
            KING_Sleep(1000);
            sms_delMsg((uint16)data->smsEvent.nParam);
            break;

        default:
            break;
    }
}

/*******************************************************************************
 ** Functions
 ******************************************************************************/
void sms_test(void)
{
		KING_RegNotifyFun(DEV_CLASS_SMS, 0, SMS_EventProcessCb);
}
