/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "app_main.h"
#include "kingcstd.h"
#include "kingrtos.h"
#include "kingplat.h"
#include "kingnet.h"
#include "KingAccessNetwork.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("accessnetwork: "fmt, ##args); } while(0)

/*
输入时：
nw_info->Type指 需要获取的邻小区信息的网络类型，底包按照这个类型填入index对应的小区信息

输出时：
nw_info->Type指网络类型

Index适用于蜂窝网络类型，一般来说底包已经去掉了重复小区信息，所以index采用递增的方式，直到返回-1时认为获取完所有网络信息
*/
/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
static void accessnetwork_servingCellInfoGet(void)
{
    int ret = 0;
    NETWORK_INFO nw_info;

    memset(&nw_info, 0x00, sizeof(NETWORK_INFO));
    ret = KING_ServedNetworkInfoGet(&nw_info);
    if (ret == 0)
    {
        if (nw_info.Type == ACCESS_NW_TYPE_EUTRAN)
        {
            LogPrintf("servingCellInfoGet ACCESS_NW_TYPE_EUTRAN scell mcc%X,mnc%X,arfcn%d,cid%d,band%d,Dbm%d,lac%d",
                        nw_info.eutran.mcc, nw_info.eutran.mnc, nw_info.eutran.earfcn,
                        nw_info.eutran.cid, nw_info.eutran.eutran_band, nw_info.eutran.Dbm,
                        nw_info.eutran.tac);
        }
        else
        {
            LogPrintf("servingCellInfoGet other scell mcc%X,mnc%X,arfcn%d,cid%d,band%d,Dbm%d,lac%d",
                    nw_info.geran.mcc, nw_info.geran.mnc, nw_info.geran.arfcn,
                    nw_info.geran.cid, nw_info.geran.geran_band, nw_info.geran.Dbm,
                    nw_info.geran.lac);
        }
    }
    else
    {
        LogPrintf("servingCellInfoGet get serveice cell info is fail");
    }
}

static void accessnetwork_neighborCellInfoGet(void)
{
    int ret = 0;
    NETWORK_INFO nw_info;
    int i = 0;
    while(1)
    {
        memset(&nw_info, 0x00, sizeof(NETWORK_INFO));
        nw_info.Type = ACCESS_NW_TYPE_EUTRAN;//or ACCESS_NW_TYPE_GERAN
        ret = KING_NeighborCellInfoGet(&nw_info,i);
        if(-1 == ret)
        {
            LogPrintf("neighborCellInfoGet get neighbor cell info is null");
            break;
        }
        
        if (nw_info.Type == ACCESS_NW_TYPE_EUTRAN)
        {
            LogPrintf("neighborCellInfoGet ACCESS_NW_TYPE_EUTRAN scell mcc%X,mnc%X,arfcn%d,cid%d,band%d,Dbm%d,lac%d",
                    nw_info.eutran.mcc, nw_info.eutran.mnc, nw_info.eutran.earfcn,
                    nw_info.eutran.cid, nw_info.eutran.eutran_band, nw_info.eutran.Dbm,
                    nw_info.eutran.tac);
        }
        else
        {
            LogPrintf("neighborCellInfoGet other scell mcc%X,mnc%X,arfcn%d,cid%d,band%d,Dbm%d,lac%d",
                    nw_info.geran.mcc, nw_info.geran.mnc, nw_info.geran.arfcn,
                    nw_info.geran.cid, nw_info.geran.geran_band, nw_info.geran.Dbm,
                    nw_info.geran.lac);
        }
        i++;
    }
    LogPrintf("Neighbor Cell Info sum:%d",i);
}

void accessnetwork_test(void)
{
    int ret = -1;
    NET_STATE_E state;
    
    while (1)
    {
        ret = KING_NetStateGet(&state);
        LogPrintf("%s: Get net state %d, ret : %d\r\n", __FUNCTION__, state, ret);
        if (ret == 0 && state > NET_NO_SERVICE && state < NET_EMERGENCY)
        {
            NET_CESQ_S Cesq;
            ret = KING_NetSignalQualityGet(&Cesq);
            LogPrintf("%s: Get sinagal ret is %d. rxlev:%d, ber:%d, rscp:%d, ecno:%d, rsrq:%d, rsrp:%d\r\n", __FUNCTION__,
                ret,Cesq.rxlev,Cesq.ber,Cesq.rscp,Cesq.ecno,Cesq.rsrq,Cesq.rsrp);

            KING_Sleep(1000);
            accessnetwork_servingCellInfoGet();
            KING_Sleep(60000);
            accessnetwork_neighborCellInfoGet();

            LogPrintf("%s: complete\r\n", __FUNCTION__);
            break;
        }
        
        KING_Sleep(1000);
    }
}

