/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "app_main.h"
#include "KingNet.h"
#include "kingCbData.h"
#include "kingcstd.h"
#include "kingrtos.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("net: "fmt, ##args); } while(0)

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static bool bSimReady = FALSE;

/*******************************************************************************
 ** Functions
 ******************************************************************************/
static void netEventCb(uint32 eventID, void* pdata, uint32 len)
{
    uint32 state ;
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pdata;

    LogPrintf("eventID=%d, len=%d, pdata=%p\r\n", eventID, len, pdata);

    switch(eventID)
    {
    case NW_EVENT_SIM_READY:
        LogPrintf("SIM ready\r\n");
        bSimReady = TRUE;
        break;

    case NW_EVENT_SIM_NOT_READY:
        LogPrintf("SIM not ready\r\n");
        bSimReady = FALSE;
        break;

    case NW_EVENT_GSM_REGISTERED_IND:
        LogPrintf("NET registered\r\n");
        break;

    case NW_EVENT_GSM_UNREGISTERED_IND:
        LogPrintf("NET unregistered\r\n");
        break;

    case NW_EVENT_GPRS_ATTACH_IND:
    {
        LogPrintf("GPRS attached\r\n");

        //KING_Sleep(1000);

        KING_NetPdpStatusGet(1, &state);

        LogPrintf("GPRS pdpstate :%d\r\n", state);

        KING_NetAirplaneModeGet(&state);

        LogPrintf("GPRS airplane mode :%d\r\n", state);
    }

        break;

    case NW_EVENT_GPRS_DEATTACH_IND:

        LogPrintf("GPRS deattached. Attach net again!\r\n");
        KING_NetAttach();
        break;

    case NW_EVENT_PS_ONOFF_RSP:
        LogPrintf("GPRS psonoff rsp\r\n");
        KING_NetAirplaneModeGet(&state);
        if (state)
            KING_NetAirplaneModeSet(0);
        break;

    case NW_EVENT_ATTACH_RSP:
        LogPrintf("GPRS attach rsp\r\n");

        break;

    case NW_EVENT_PDP_ACT_RSP:
    {
        int ret = 0;
        uint8 pdpActCid = (uint8)(data->net.info.value);
        uint32 result = data->net.info.result;
        NET_ACTION action = data->net.info.action;
        LogPrintf("PDP NW_EVENT_PDP_ACT_RSP\r\n");

        if (action == NET_ACTION_ACTIVE)
        {
            if (result == 0)
            {
                LogPrintf("PDP active succ.pdpActCid: %d\r\n", pdpActCid);

                KING_NetPdpStatusGet(1, &state);

                LogPrintf("PDP active is state %d!\r\n", state);
                ret = KING_NetPdpDeactive(1);
                LogPrintf("KING_NetPdpDeactive is ret %d!\r\n", ret);
            }
            else
            {
                LogPrintf("PDP active fail.\r\n");
            }
        }
        else if (action == NET_ACTION_DEACTIVE)
        {
            if (result == 0)
            {
                LogPrintf("PDP deactive succ.pdpActCid: %d\r\n", pdpActCid);

                KING_NetPdpStatusGet(1, &state);
                ret = KING_NetDeattach();
                LogPrintf("KING_NetDeattach is ret %d!\r\n", ret);

                KING_Sleep(100);
                LogPrintf("PDP deactive is state %d!\r\n", state);
                ret = KING_NetAirplaneModeSet(1);
                LogPrintf("KING_NetAirplaneModeSet is ret %d!\r\n", ret);
            }
            else
            {
                LogPrintf("PDP deactive fail.\r\n");
            }
        }
        else
        {
             LogPrintf("PDP NW_EVENT_PDP_ACT_RSP fail\r\n");
        }
    }
        break;

    case NW_EVENT_PDP_DEACT_IND:
        LogPrintf("PDP deactive ind\r\n");
        break;

    case NW_EVENT_NETTIME_SYNC_IND:
        LogPrintf("Net Time sync completes!\r\n");
        break;

    case NW_EVENT_CELL_INFO_RSP:

        break;

    default:
        break;
    }

    return ;
}


void net_test(void)
{
    int ret;
    NET_STATE_E state;
    uint32 mode, pdpState;

    LogPrintf("net_test start...\r\n");

    KING_RegNotifyFun(DEV_CLASS_NW, 0, netEventCb);

    while (1)
    {
        ret = KING_NetModeGet(&mode);
        LogPrintf("Get net mode is %d. ret: %d\r\n", mode, ret);

        ret = KING_NetStateGet(&state);
        LogPrintf("Get net state %d, ret:%d\r\n", state, ret);

        if (state > NET_NO_SERVICE && state < NET_EMERGENCY)
        {
            NET_CESQ_S Cesq;
            ret = KING_NetSignalQualityGet(&Cesq);
            LogPrintf("get sinagal ret is %d. rxlev:%d, ber:%d, rscp:%d, ecno:%d, rsrq:%d, rsrp:%d\r\n",
                    ret, Cesq.rxlev, Cesq.ber, Cesq.rscp, Cesq.ecno, Cesq.rsrq, Cesq.rsrp);

            ret = KING_NetPdpStatusGet(1, &pdpState);
            LogPrintf("net_test() ret=%d, pdpState=%u\r\n", ret, pdpState);
            
            ret = KING_NetPdpActive(1);
            LogPrintf("net_test() ret=%d\r\n", ret);

            ret = KING_NetPdpStatusGet(1, &pdpState);
            LogPrintf("net_test() ret=%d, pdpState=%u\r\n", ret, pdpState);

            break;
        }

        KING_Sleep(5000);
    }

    LogPrintf("net_test complete!\r\n");
}


#ifdef __cplusplus
}
#endif /* __cplusplus */

