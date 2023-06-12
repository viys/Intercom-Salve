#include "app_main.h"
#include "demo_fota.h"
#include "KingNet.h"
#include "KingRtos.h"
MSG_HANDLE           g_main_msg_handle = NULL;

#define DEMO_FOTA 1
void Net_FunCB(uint32 eventID, void* pdata, uint32 len)
{
    MSG_S msg;

    KING_SysLog("[Test]eventID=%d, len=%d, pdata=%p",eventID, len, pdata);
        
    switch(eventID)
    {
    case NW_EVENT_SIM_READY:
        KING_SysLog("[Test]SIM ready"); 
        
        break;
        
    case NW_EVENT_SIM_NOT_READY:
        KING_SysLog("[Test]SIM not ready");
        
        break;
        
    case NW_EVENT_GSM_REGISTERED_IND:
        KING_SysLog("[Test]NET registered");
        break;

    case NW_EVENT_GSM_UNREGISTERED_IND:
        KING_SysLog("[Test]NET unregistered");
        break;

    case NW_EVENT_GPRS_ATTACH_IND:
        KING_SysLog("[Test]GPRS attached");
        KING_NetPdpActive(1); //附着成功，手动激活PDP

        break;

    case NW_EVENT_GPRS_DEATTACH_IND:
        KING_SysLog("[Test]GPRS deattached");
        
        break;

    case NW_EVENT_PS_ONOFF_RSP:
        KING_SysLog("[Test]GPRS psonoff rsp");
        break;

    case NW_EVENT_ATTACH_RSP:
        KING_SysLog("[Test]GPRS attach rsp");
        break;

    case NW_EVENT_PDP_ACT_RSP:
        KING_SysLog("[Test]PDP NW_EVENT_PDP_ACT_RSP!\r\n");
        msg.messageID = DEMO_FOTA;      //PDP激活后开启fota线程
        if (g_main_msg_handle == NULL)
        {
            KING_SysLog("[Test]fail");
        }
        else
        {
            KING_MsgSend(&msg, g_main_msg_handle);
        }
        break;

    case NW_EVENT_PDP_DEACT_IND:
        KING_SysLog("[Test]PDP deactive ind");
        
        break;

    case NW_EVENT_CELL_INFO_RSP:
        KING_SysLog("[Test]NW_EVENT_CELL_INFO_RSP");
        break;

    case NW_EVENT_NETTIME_SYNC_IND:
        KING_SysLog("[Test]NW_EVENT_NETTIME_SYNC_IND");
        break;
        
    default:
        break;
    }

    return ;
}


/*------------------------------------------------------------------------------------------------------
king APP 入口函数
------------------------------------------------------------------------------------------------------*/
void APP_Main(uint32 bootMode)
{
    int             ret = -1;
    MSG_S           msg = {0};


    // 创建main函数消息队列
    ret = KING_MsgCreate(&g_main_msg_handle);
    if (ret == -1)
    {
        KING_SysLog("[Test]Failed to create Msg Handle!");
        KING_ThreadExit();
        return;
    }

    ret = KING_RegNotifyFun(DEV_CLASS_NW, 0, Net_FunCB);

    while(1)
    {
        KING_MsgRcv(&msg, g_main_msg_handle, WAIT_OPT_INFINITE);

        if(msg.messageID == DEMO_FOTA)
        {
            CZ_Fota_Active();
            //CZ_Fota_ResetNV();
        }

    }

    KING_SysLog("[Test]Exit");
    return;
}


