#include "At_common.h"
#include "At_errcode.h"
#include "At_module.h"
#include "at_utility.h"

#include "KingAccessNetwork.h"
#include "kingDef.h"
#include "kingerrcode.h"
#include "kingPlat.h"
#include "kingatapi.h"
#include "kingnet.h"
#include "KingDualsim.h"
#include "kingcbdata.h"
#include "KingWifi.h"
#include "KingRtos.h"
#include "KingBT.h"

#define COPS_TEST_TIMEOUT (60 * 4)

extern UINT8 g_rspStr[];

char gStrBand[20];
int  copsmode = 0;
int  copsformat = 0;

AT_CMD_ENGINE_T * engine = NULL;
static uint32_t gWifiScan = 0;
static AT_CMD_ENGINE_T *gWifiEngine = NULL;

typedef enum
{
    COPS_ACT_GSM = 0,
    COPS_ACT_GSM_COMPACT = 1,
    COPS_ACT_UTRAN = 2,
    COPS_ACT_GSM_EGPRS = 3,
    COPS_ACT_UTRAN_HSDPA = 4,
    COPS_ACT_UTRAN_HSUPA = 5,
    COPS_ACT_UTRAN_HSDPA_HSUPA = 6,
    COPS_ACT_EUTRAN = 7,
    COPS_ACT_ECGSM = 8,
    COPS_ACT_UNDEFINED,
    COPS_ACT_AUTO = 0xFF
} AT_COPS_ACT;

typedef enum
{
    COPS_MODE_AUTOMATIC = 0,
    COPS_MODE_MANUAL = 1,
    COPS_MODE_DEREGISTER = 2,
    COPS_MODE_SET_ONLY = 3,
    COPS_MODE_MANUAL_AUTOMATIC = 4,
    COPS_MODE_UNDEFINED = 5,
} AT_COPS_MODE;

extern void Bluetooth_Init(void);

static void netEventCb(uint32 eventID, void* pdata, uint32 len)
{
    KING_SysLog("at nw eventID=%d, pdata=%p\r\n", eventID, len);
    char strRsp[50] = {0};

    if (eventID == NW_EVENT_GSM_UNREGISTERED_IND)
    {
        if (gATCurrentCreg != 0)
        {
            sprintf(strRsp,"+CREG: %d,0",gATCurrentCreg);
            at_CmdReportUrcText(0,(const uint8_t*)strRsp);
        }
    }
    else if (eventID == NW_EVENT_GSM_REGISTERED_IND)
    {
        int ret;
        REG_INFO_S info;

        if (gATCurrentCreg != 0)
        {
            ret = KING_NWRegisterInfoGet(0,&info);
            if (ret == 0)
            {
                memset(strRsp, 0x00, 50);
                
                if (gATCurrentCreg == 2)
                {
                    sprintf(strRsp,"+CREG: %d,%d,\"%04X\",\"%04X\",%d",gATCurrentCreg,info.state,
                        info.lac,info.cid,info.act);
                }
                else
                {
                    sprintf(strRsp,"+CREG: %d",info.state);
                }

                at_CmdReportUrcText(0,(const uint8_t*)strRsp);
            }
        }
    }
    else if (eventID == NW_EVENT_GPRS_DEATTACH_IND)
    {
        if (gATCurrentCereg != 0)
        {
            sprintf(strRsp,"+CEREG: %d,0",gATCurrentCreg);
            at_CmdReportUrcText(0,(const uint8_t*)strRsp);
        }
    }
    else if (eventID == NW_EVENT_GPRS_ATTACH_IND)
    {
        int ret;
        REG_INFO_S info;

        if (gATCurrentCereg != 0)
        {
            ret = KING_PSRegisterInfoGet(0,&info);
            if (ret == 0)
            {
                memset(strRsp, 0x00, 50);
                
                if (gATCurrentCereg == 2)
                {
                    if (info.act == 0)
                    {
                        sprintf(strRsp,"+CEREG: %d,%d,\"%04X\",\"%04X\",%d",gATCurrentCereg,info.state,
                            info.lac,info.cid,info.act);
                    }
                    else
                    {
                        sprintf(strRsp,"+CEREG: %d,%d,\"%04X\",\"%07X\",%d",gATCurrentCereg,info.state,
                            info.lac,info.cid,info.act);
                    }
                }
                else
                {
                    sprintf(strRsp,"+CEREG: %d",info.state);
                }

                at_CmdReportUrcText(0,(const uint8_t*)strRsp);
            }
        }
    }
    else if (eventID == NW_EVENT_ATTACH_RSP)
    {
        KH_CB_DATA_U* data = (KH_CB_DATA_U*)pdata;

        if (data->net.info.result == 0)
        {
            at_CmdRespOK(engine);
        }
        else
        {
            at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
        }
    }

}

void atNwInit(void)
{
    KING_RegNotifyFun(DEV_CLASS_NW,0,netEventCb);
    Bluetooth_Init();
}

static void setBand(NETWORK_INFO nw_info)
{
    memset(gStrBand,0,20);
    
    if (nw_info.Type == ACCESS_NW_TYPE_GERAN)
    {
        switch(nw_info.geran.geran_band)
        {
            case GERAN_BAND_P900:
            case GERAN_BAND_R900:
                 strcpy(gStrBand,"PGSM 900");
                 break;
            
            case GERAN_BAND_1800:
                 strcpy(gStrBand,"GSM 1800");
                 break;
            
            case GERAN_BAND_PCS1900:
                 strcpy(gStrBand,"GSM 1900");
                 break;
            
            case GERAN_BAND_E900:
                 strcpy(gStrBand,"EGSM 900");
                 break;
            
            case GERAN_BAND_450:
                 strcpy(gStrBand,"GSM 450");
                 break;
            
            case GERAN_BAND_480:
                 strcpy(gStrBand,"GSM 480");
                 break;
            
            case GERAN_BAND_850:
                 strcpy(gStrBand,"GSM 850");
                 break;
                 
            default:
                 sprintf(gStrBand,"GSM %d",nw_info.geran.geran_band);
                 break;
        }
    }
}

void AT_CmdFunc_QNWINFO(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            at_CmdRespOK(pParam->engine);
            break;

        case AT_CMD_EXE:
            {
                int ret = SUCCESS;
                NETWORK_INFO nw_info;
                char strRsp[100] = {0};
                NET_STATE_E net_state = NET_NO_SERVICE;
                
                memset(&nw_info, 0, sizeof(NETWORK_INFO));
                memset(strRsp, 0x00, 100);

                if (0 != KING_NetStateGet(&net_state) || ((NET_NO_SERVICE == net_state)))
                {
                    sprintf(strRsp,"+QNWINFO: No Service");
                    at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
                    at_CmdRespOK(pParam->engine);
                    break;
                }
                
                ret = KING_ServedNetworkInfoGet(&nw_info);
                if (ret != 0)
                {
                    if (KING_GetLastErrCode() == NW_ERR_NO_CELL)
                    {
                        sprintf(strRsp,"+QNWINFO: No Service");
                        at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
                        at_CmdRespOK(pParam->engine);
                    }
                    else
                    {
                        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    }
                    break;
                }
                
                switch (nw_info.Type)
                {
                    case ACCESS_NW_TYPE_GERAN:
                        setBand(nw_info);
                        if (nw_info.eutran.mnc_digit_num == 2)
                        {
                            sprintf(strRsp, "+QNWINFO: \"GSM\",%0X%02X,\"%s\",%d", nw_info.eutran.mcc,
                                nw_info.eutran.mnc,gStrBand,nw_info.eutran.earfcn);
                        }
                        else
                        {
                            sprintf(strRsp, "+QNWINFO: \"GSM\",%0X%03X,\"%s\",%d", nw_info.eutran.mcc,
                                nw_info.eutran.mnc,gStrBand,nw_info.eutran.earfcn);
                        }
                        break;

                    case ACCESS_NW_TYPE_EUTRAN:
                        if (nw_info.eutran.eutran_band > EUTRAN_BAND_25)
                        {
                            if (nw_info.eutran.mnc_digit_num == 2)
                            {
                                sprintf(strRsp, "+QNWINFO: \"TDD LTE\",%0X%02X,\"LTE BAND %d\",%d", nw_info.eutran.mcc,
                                    nw_info.eutran.mnc,nw_info.eutran.eutran_band,nw_info.eutran.earfcn);
                            }
                            else
                            {
                                sprintf(strRsp, "+QNWINFO: \"TDD LTE\",%0X%03X,\"LTE BAND %d\",%d", nw_info.eutran.mcc,
                                    nw_info.eutran.mnc,nw_info.eutran.eutran_band,nw_info.eutran.earfcn);
                            }
                        }
                        else
                        {
                            if (nw_info.eutran.mnc_digit_num == 2)
                            {
                                sprintf(strRsp, "+QNWINFO: \"FDD LTE\",%0X%02X,\"LTE BAND %d\",%d", nw_info.eutran.mcc,
                                    nw_info.eutran.mnc,nw_info.eutran.eutran_band,nw_info.eutran.earfcn);
                            }
                            else
                            {
                                sprintf(strRsp, "+QNWINFO: \"FDD LTE\",%0X%03X,\"LTE BAND %d\",%d", nw_info.eutran.mcc,
                                    nw_info.eutran.mnc,nw_info.eutran.eutran_band,nw_info.eutran.earfcn);
                            }
                        }
                        break;

                    case ACCESS_NW_TYPE_UTRAN:
                    case ACCESS_NW_TYPE_CDMA2000:
                    default:
                        at_CmdRespCmeError(pParam->engine, ERR_AT_CMS_NO_NW_SERVICE);
                        break;
                }
                at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
                at_CmdRespOK(pParam->engine);
            }
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }

    return;
}

static uint8 getBandWidthValue(uint16 bandwidth)
{
    const uint8 bw_map[][2] = {
        {0, }, // 1.4MHz
        {1, 3}, // 3MHz
        {2, 5}, // 5MHz
        {3, 10}, // 10MHz
        {4, 15}, // 15MHz
        {5, 20}  // 20MHz
    };
    uint8 i = 0;
    uint8 size = sizeof(bw_map)/sizeof(bw_map[0]);
    uint8 ret;

    for (i = 0; i < size; i++)
    {
        if (size - 1 == i)
        {
            ret = bw_map[i][0]; //the max one
            break;
        }
        else if ((bandwidth > bw_map[i][1]) && (bandwidth < bw_map[i + 1][1]))
        {
            ret = bw_map[i + 1][0];
            break;
        }
    }

    return ret;
}

void AT_CmdFunc_QENG(AT_CMD_PARA *pParam)
{
    UINT8 iCount = 0;
    UINT8 cfw_para[20] = {0};
    UINT16 len = 20;
    INT32 iResult;    
    int ret = SUCCESS;
    NETWORK_INFO nw_info;
    uint32 index = 0;
    uint32 rsp_len = 0;
    ACCESS_NW_TYPE srv_type = ACCESS_NW_TYPE_UNKNOWN;
    NET_CESQ_S cesq;
    uint8 gsm_band = 2; // 0: DCS1800; 1: PCS1900; 2: other band
    ACCESS_NW_TYPE ncell_types[2] = {ACCESS_NW_TYPE_EUTRAN, ACCESS_NW_TYPE_GERAN};
    int i;

    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            sprintf((char *)g_rspStr, "+QENG: \"servingcell\",\"neighbourcell\"");            
            at_CmdRespInfoText(pParam->engine, g_rspStr);
            at_CmdRespOK(pParam->engine);
            break;

        case AT_CMD_SET:
            iResult = AT_Util_GetParaCount(pParam->pPara, &iCount);
            
            if (iResult != 0 || (iCount != 1))
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            
            iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, (PVOID)cfw_para, &len);
            
            if (0 != iResult || len == 0)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_INVALID_CHAR_INTEXT);
                return;
            }
            
            if (0 == strcmp((char *)cfw_para, "servingcell"))
            {
                memset(&nw_info, 0, sizeof(NETWORK_INFO));  
                ret = KING_ServedNetworkInfoGet(&nw_info);
                if (FAIL == ret)
                {
                    ret = KING_GetLastErrCode();
                    KING_SysLog("+QENG: get servingcell fail: 0x%x", ret);
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                }
                else
                {
                    memset(&cesq, 0, sizeof(NET_CESQ_S));  
                    ret = KING_NetSignalQualityGet(&cesq);
                    rsp_len += snprintf((char *)g_rspStr + rsp_len, AT_RSP_STR_LEN - rsp_len, "+QENG: \"servingcell\",");
                    if (ACCESS_NW_TYPE_EUTRAN == nw_info.Type)
                    {
                        /* +QENG: "servingcell",<state>,"LTE",<is_tdd>,<mcc>,<mnc>,<cellid>,<pcid>,<earfcn>,<freq_band_ind>,
                                   <ul_bandwidth>,<dl_bandwidth>,<tac>,<rsrp>,<rsrq>,<rssi>,<sinr>,<srxlev>
                        */
                        uint8 is_tdd = (nw_info.eutran.eutran_band > EUTRAN_BAND_32) ? 1 : 0;

                        rsp_len += snprintf((char *)g_rspStr + rsp_len, AT_RSP_STR_LEN - rsp_len,
                                    "\"CONNECT\",\"LTE\",%d,%03X,%02X,%X,%X,%u,%d,%d,%d,%X,%d,%d,%d,%d,%d",
                                    is_tdd, nw_info.eutran.mcc, nw_info.eutran.mnc, nw_info.eutran.cid, nw_info.eutran.pci, // is_tdd,mcc,mnc,cid,pci
                                    nw_info.eutran.earfcn, nw_info.eutran.eutran_band, getBandWidthValue(5), getBandWidthValue(nw_info.eutran.Bandwidth), nw_info.eutran.tac, // earfcn,band,ul_bw,dl_bw,tac
                                    nw_info.eutran.rsrp, nw_info.eutran.rsrq, nw_info.eutran.rssi, nw_info.eutran.snr, nw_info.eutran.Dbm // rsrp, rsrq, rssi, snr, srxlev
                                    );
                    }
                    else if (ACCESS_NW_TYPE_GERAN == nw_info.Type)
                    {
                        /* +QENG: "servingscell",<state>,"GSM",<mcc>,<mnc>,<lac>,<cellid>,<bsic>,<arfcn>,<band>,<rxlev>,<txp>,
                                  <rla>,<drx>,<c1>,<c2>,<gprs>,<tch>,<ts>,<ta>,<maio>,<hsn>,<rxlevsub>,<rxlevfull>,<rxqualsub>,
                                  <rxqualfull>,<voicecodec>
                        */
                        switch (nw_info.geran.geran_band)
                        {
                        case GERAN_BAND_1800:
                            gsm_band = 0;
                            break;

                        case GERAN_BAND_PCS1900:
                            gsm_band = 1;
                            break;

                        default:
                            gsm_band = 2;
                            break;
                        }
                        rsp_len += snprintf((char *)g_rspStr + rsp_len, AT_RSP_STR_LEN - rsp_len,
                                    "\"CONNECT\",\"GSM\",%03X,%02X,%X,%X,%u,%d,%d,%d,"
                                    "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-", //txp,rla,drx,c1,c2,gprs,tch,ts,ta,maio,hsn,rxlevsub,rxlevfull,rxqualsub,rxqualfull,voicecodec
                                    nw_info.geran.mcc, nw_info.geran.mnc, nw_info.geran.lac, nw_info.geran.cid, nw_info.geran.bsic, // mcc,mnc,lac,cid,bsic
                                    nw_info.geran.arfcn, gsm_band, cesq.rxlev // arfcn,band,rxlev                                    
                                    );
                    }
                    else
                    {
                        rsp_len += snprintf((char *)g_rspStr + rsp_len, AT_RSP_STR_LEN - rsp_len, "\"SEARCH\"");
                    }
                    
                    at_CmdRespInfoText(pParam->engine, g_rspStr);
                    at_CmdRespOK(pParam->engine);
                }
            }
            else if (0 == strcmp((char *)cfw_para, "neighbourcell"))
            {
                memset(&nw_info, 0, sizeof(NETWORK_INFO));  
                ret = KING_ServedNetworkInfoGet(&nw_info);
                if (FAIL == ret)
                {
                    ret = KING_GetLastErrCode();
                    KING_SysLog("+QENG: get servingcell fail: 0x%x", ret);
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    goto exit;
                }

                srv_type = nw_info.Type;
                if ((ACCESS_NW_TYPE_EUTRAN != srv_type) && (ACCESS_NW_TYPE_GERAN != srv_type))
                {
                    KING_SysLog("+QENG: unknown access type - %d", srv_type);
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    goto exit;
                }

                for (i = 0; i < sizeof(ncell_types)/sizeof(ncell_types[0]); i++)
                {
                    do
                    {
                        memset(&nw_info, 0, sizeof(NETWORK_INFO));
                        nw_info.Type = ncell_types[i];
                        ret = KING_NeighborCellInfoGet(&nw_info, index);
                        if (FAIL == ret)
                        {
                            ret = KING_GetLastErrCode();
                            KING_SysLog("+QENG: get neighbourcell fail: 0x%x", ret);
                            break;
                        }
                        else
                        {
                            index++;
                            rsp_len += snprintf((char *)g_rspStr + rsp_len, AT_RSP_STR_LEN - rsp_len, "+QENG: ");
                            if (ACCESS_NW_TYPE_EUTRAN == nw_info.Type)
                            {
                                if (ACCESS_NW_TYPE_EUTRAN == srv_type)
                                {
                                    /* 
                                       +QENG: "neighbourcell","LTE",<mcc>,<mnc>,<cellid>,<pcid>,<earfcn>,<tac>,<rsrp>,<rsrq>,<rssi>,<sinr>,<srxlev>,
                                            <threshX_low>,<threshX_high>,<cell_resel_priority>[...]
                                    */
                                    rsp_len += snprintf((char *)g_rspStr + rsp_len, AT_RSP_STR_LEN - rsp_len,
                                                "\"neighbourcell\",\"LTE\",%03X,%02X,%X,%X,%d,%X,%d,%d,%d,%d,%d,%d,%d,%d",
                                                nw_info.eutran.mcc, nw_info.eutran.mnc, nw_info.eutran.cid, // <mcc>,<mnc>,<cellid>
                                                nw_info.eutran.pci, nw_info.eutran.earfcn, nw_info.eutran.tac, nw_info.eutran.rsrp, nw_info.eutran.rsrq, nw_info.eutran.rssi, // pci,earfcn,tac,rsrp,rsrq,rssi
                                                nw_info.eutran.snr, nw_info.eutran.Dbm, 0, 0, 0 // sinr,srxlev,threshX_low,threshX_high,cell_resel_priority
                                                );
                                }
                                else
                                {
                                    /* +QENG:"neighbourcell","LTE",<mcc>,<mnc>,<cellid>,<pcid>,<earfcn>,<tac>,<rsrp>,<rsrq>[бн]
                                    */
                                    rsp_len += snprintf((char *)g_rspStr + rsp_len, AT_RSP_STR_LEN - rsp_len,
                                                "\"neighbourcell\",\"LTE\",%03X,%02X,%X,%X,%d,%X,%d,%d", nw_info.eutran.mcc, nw_info.eutran.mnc, nw_info.eutran.cid, // <mcc>,<mnc>,<cellid>
                                                nw_info.eutran.pci, nw_info.eutran.earfcn, nw_info.eutran.tac, nw_info.eutran.rsrp, nw_info.eutran.rsrq // pci,earfcn,tac,rsrp,rsrq,
                                                );
                                }                    
                            }
                            else if (ACCESS_NW_TYPE_GERAN == nw_info.Type)
                            {
                                if (ACCESS_NW_TYPE_EUTRAN == srv_type)
                                {
                                    /* +QENG: "neighbourcell","GSM",<mcc>,<mnc>,<arfcn>,<cell_resel_priority>,<thresh_gsm_high>,<thresh_gsm_low>,
                                            <ncc_permitted>,<band>,<bsic_id>,<rssi>,<srxlev>[бн]
                                    */
                                    switch (nw_info.geran.geran_band)
                                    {
                                    case GERAN_BAND_1800:
                                        gsm_band = 0;
                                        break;

                                    case GERAN_BAND_PCS1900:
                                        gsm_band = 1;
                                        break;

                                    default:
                                        gsm_band = 2;
                                        break;
                                    }
                                    rsp_len += snprintf((char *)g_rspStr + rsp_len, AT_RSP_STR_LEN - rsp_len,
                                                "\"neighbourcell\",\"GSM\",%03X,%02X,%X,%X,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                                nw_info.geran.mcc, nw_info.geran.mnc, nw_info.geran.lac, nw_info.geran.cid,// <mcc>,<mnc>,<lac>,<cellid>
                                                nw_info.geran.arfcn, 0, 0, 0, 0, // arfcn,cell_resel_priority,thresh_gsm_high,thresh_gsm_low,ncc_permitted
                                                gsm_band, nw_info.geran.bsic, nw_info.geran.Dbm, nw_info.geran.Dbm // band,bsic_id,rssi,srxlev
                                                );
                                }
                                else
                                {
                                    /* +QENG: "neighbourcell","GSM",<mcc>,<mnc>,<lac>,<cellid>,<bsic>,<arfcn>,<rxlev>,<c1>,<c2>,<c31>,<c32>[бн]
                                    */
                                    rsp_len += snprintf((char *)g_rspStr + rsp_len, AT_RSP_STR_LEN - rsp_len,
                                                "\"neighbourcell\",\"GSM\",%03X,%02X,%X,%X,%d,%d,%d,%d,%d,%d,%d",
                                                nw_info.geran.mcc, nw_info.geran.mnc, nw_info.geran.lac, nw_info.geran.cid, nw_info.geran.bsic, // mcc,mnc,lac,cellid,bsic
                                                nw_info.geran.arfcn, nw_info.geran.Dbm, 0, 0, 0, 0 // arfcn,rxlev,c1,c2,c31,c32
                                                );
                                }
                            }
                            rsp_len += snprintf((char *)g_rspStr + rsp_len, AT_RSP_STR_LEN - rsp_len, "\r\n");
                        }
                    }
                    while (SUCCESS == ret);
                }

                if ((0 == index) && (NW_ERR_NO_CELL == ret))
                {
                    index++;
                    //no neighbour cell found
                    if (ACCESS_NW_TYPE_EUTRAN == srv_type)
                    {
                        /* 
                           +QENG: "neighbourcell","LTE",<mcc>,<mnc>,<cellid>,<pcid>,<earfcn>,<tac>,<rsrp>,<rsrq>,<rssi>,<sinr>,<srxlev>,
                                <threshX_low>,<threshX_high>,<cell_resel_priority>[...]
                        */
                        rsp_len += snprintf((char *)g_rspStr + rsp_len, AT_RSP_STR_LEN - rsp_len, "+QENG: \"neighbourcell\",\"LTE\",0,0,0,0,0,0,0,0,0,0,0,0,0,0");
                    }
                    else
                    {
                        /* +QENG: "neighbourcell","GSM",<mcc>,<mnc>,<lac>,<cellid>,<bsic>,<arfcn>,<rxlev>,<c1>,<c2>,<c31>,<c32>[бн]
                        */
                        rsp_len += snprintf((char *)g_rspStr + rsp_len, AT_RSP_STR_LEN - rsp_len, "+QENG: \"neighbourcell\",\"GSM\",0,0,0,0,0,0,0,0,0,0,0");
                    }
                }

                if (index > 0)
                {
                    //delete the last \r\n
                    memset(g_rspStr + (rsp_len - 2), 0x00, 2);
                    at_CmdRespInfoText(pParam->engine, g_rspStr);
                    at_CmdRespOK(pParam->engine);
                }
                else
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                }
            }
            else
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }

exit:
    return;
}

static void COPSEventCb(AT_CMD_TYPE cmd, void* pData, uint32 result)
{
    KING_SysLog("cops receive cmd %d result %d",cmd,result);
    if (cmd == CMD_TYPE_COPS_TEST)
    {
        if (result == 0)
        {
            COPS_NW_INFO_S *pOperInfo = NULL;
            unsigned char *pOperName = NULL;
            unsigned char *pOpershortName = NULL;
            char rsp[400];
            char *prsp = rsp;
            COPS_INFO_S* info = (COPS_INFO_S*)pData;

            memset(rsp, 0x00, 400);
            
            pOperInfo = info->info;
            prsp += sprintf(prsp, "+COPS: ");
            for (int i = 0; i < info->count; i++)
            {
    
                int nRet = KING_OperatorNameGet(pOperInfo[i].nOperatorId, &pOperName, &pOpershortName);
                if (i > 0)
                {
                    prsp += sprintf(prsp, ",");
                }
    
                if (0x0f == pOperInfo[i].nOperatorId[5])
                {
                    if (nRet != 0)
                    {
                        prsp += sprintf(prsp, "(%d,\"%d%d%d%d%d\",\"%d%d%d%d%d", pOperInfo[i].nStatus,
                                        pOperInfo[i].nOperatorId[0], pOperInfo[i].nOperatorId[1], pOperInfo[i].nOperatorId[2],
                                        pOperInfo[i].nOperatorId[3], pOperInfo[i].nOperatorId[4],
                                        pOperInfo[i].nOperatorId[0], pOperInfo[i].nOperatorId[1], pOperInfo[i].nOperatorId[2],
                                        pOperInfo[i].nOperatorId[3], pOperInfo[i].nOperatorId[4]);
                    }
                    else
                    {
                        prsp += sprintf(prsp, "(%d,\"%s\",\"%s", pOperInfo[i].nStatus, pOperName, pOpershortName);
                    }
                    prsp += sprintf(prsp, "\",\"%d%d%d%d%d", pOperInfo[i].nOperatorId[0], pOperInfo[i].nOperatorId[1], pOperInfo[i].nOperatorId[2],
                                    pOperInfo[i].nOperatorId[3], pOperInfo[i].nOperatorId[4]);
                }
                else
                {
                    if (nRet != 0)
                    {
                        prsp += sprintf(prsp, "(%d,\"%d%d%d%d%d%d\",\"%d%d%d%d%d%d", pOperInfo[i].nStatus,
                                        pOperInfo[i].nOperatorId[0], pOperInfo[i].nOperatorId[1], pOperInfo[i].nOperatorId[2],
                                        pOperInfo[i].nOperatorId[3], pOperInfo[i].nOperatorId[4], pOperInfo[i].nOperatorId[5],
                                        pOperInfo[i].nOperatorId[0], pOperInfo[i].nOperatorId[1], pOperInfo[i].nOperatorId[2],
                                        pOperInfo[i].nOperatorId[3], pOperInfo[i].nOperatorId[4], pOperInfo[i].nOperatorId[5]);
                    }
                    else
                    {
                        prsp += sprintf(prsp, "(%d,\"%s\",\"%s", pOperInfo[i].nStatus, pOperName, pOpershortName);
                    }
                    prsp += sprintf(prsp, "\",\"%d%d%d%d%d%d", pOperInfo[i].nOperatorId[0], pOperInfo[i].nOperatorId[1], pOperInfo[i].nOperatorId[2],
                                    pOperInfo[i].nOperatorId[3], pOperInfo[i].nOperatorId[4], pOperInfo[i].nOperatorId[5]);
                }
                
                prsp += sprintf(prsp, "\",%d)", pOperInfo[i].nRat);
            }
            prsp += sprintf(prsp, ",,(0,1,2,3,4),(0,1,2)");
            at_CmdRespInfoText(engine, (uint8 *)rsp);
            at_CmdRespOK(engine);
        }
        else
        {
            at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
        }
    }
    else if (cmd == CMD_TYPE_COPS_REGISTER)
    {
        if (result == 0)
        {
            at_CmdRespOK(engine);
        }
        else if (result == NW_ERR_NO_CELL)
        {
            copsmode = 0;
        }
        else
        {
            //at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
            at_CmdRespOK(engine);
        }
    }
    else if (cmd == CMD_TYPE_COPS_DEREGISTER)
    {
        if (result == 0)
        {
            at_CmdRespOK(engine);
        }
        else
        {
            at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
        }
    }
}


void AT_CmdFunc_COPS(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_READ:
            {
                int ret = SUCCESS;
                NETWORK_INFO nw_info;
                char strRsp[100] = {0};
                SIM_ID nSimId = SIM_0;
                uint8 oper_id[6];
                uint8* pName = NULL;
                uint8* pShortName = NULL;

                KING_DualSimCurrent(&nSimId);

                memset(&nw_info, 0, sizeof(NETWORK_INFO));
                memset(strRsp, 0x00, 100);

                ret = KING_CurOperatorInfoGet(nSimId,oper_id,&pName,&pShortName);
                if (ret != 0)
                {
                    if (KING_GetLastErrCode() == NW_ERR_NO_CELL)
                    {
                        sprintf(strRsp,"+COPS: %d",copsmode);
                        at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
                        at_CmdRespOK(pParam->engine);
                    }
                    else
                    {
                        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    }

                    break;
                }

                int act = 7;
                NET_STATE_E state = NET_NO_SERVICE;
                KING_NetStateGet(&state);
                if (state == NET_NORMAL_GSM)
                {
                    act = 0;
                }

                if (copsformat == 0)
                {
                    if (NULL == pName)
                    {
                        sprintf(strRsp,"+COPS: %d,%d,\"\",%d",copsmode,copsformat,act);
                    }
                    else
                    {
                        sprintf(strRsp,"+COPS: %d,%d,\"%s\",%d",copsmode,copsformat,pName,act);
                    }
                }
                else if (copsformat == 1)
                {
                    if (NULL == pShortName)
                    {
                        sprintf(strRsp,"+COPS: %d,%d,\"\",%d",copsmode,copsformat,act);
                    }
                    else
                    {
                        sprintf(strRsp,"+COPS: %d,%d,\"%s\",%d",copsmode,copsformat,pShortName,act);
                    }
                }
                else
                {
                    
                    if (oper_id[5] == 0xF)
                    {
                        sprintf(strRsp,"+COPS: %d,%d,\"%X%X%X%X%X\",%d",copsmode,copsformat,
                            oper_id[0],oper_id[1],oper_id[2],oper_id[3],oper_id[4],act);
                    }
                    else
                    {
                        sprintf(strRsp,"+COPS: %d,%d,\"%X%X%X%X%X%X\",%d",copsmode,copsformat,
                            oper_id[0],oper_id[1],oper_id[2],oper_id[3],oper_id[4],oper_id[5],act);
                    }
                }
                
                at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
                at_CmdRespOK(pParam->engine);
            }
            break;

        case AT_CMD_SET:
            {
                bool paramok = true;
                int ret;
                uint8 oper_id[6] = {};
                
                SIM_ID nSimId = SIM_0;
                uint32 mode = at_ParamUintInRange(pParam->params[0],COPS_MODE_AUTOMATIC,COPS_MODE_MANUAL_AUTOMATIC,&paramok);
                uint32 format = at_ParamDefUintInRange(pParam->params[1], 0, 0, 2, &paramok);
                const uint8* oper = at_ParamDefStr(pParam->params[2],(const uint8_t*)"",&paramok);
                uint32 act = at_ParamDefUintInRange(pParam->params[3],COPS_ACT_AUTO,COPS_ACT_GSM,COPS_ACT_ECGSM,&paramok);

                KING_DualSimCurrent(&nSimId);

                KING_SysLog("cops set get param ok %d receive cmd %d",paramok,pParam->paramCount);
                if (!paramok || pParam->paramCount > 4)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    break;
                }

                if ((mode == COPS_MODE_AUTOMATIC && pParam->paramCount > 2) ||
                    (mode == COPS_MODE_DEREGISTER && pParam->paramCount > 1) ||
                    (mode == COPS_MODE_SET_ONLY && pParam->paramCount > 2))
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    break;
                }

                if (mode != COPS_MODE_SET_ONLY)
                {
                    copsmode = mode;
                }
                copsformat = format;

                if (mode == COPS_MODE_SET_ONLY)
                {
                    at_CmdRespOK(pParam->engine);
                }
                else if (mode == COPS_MODE_AUTOMATIC)
                {
                    ret = KING_NWRegisterModeSet(nSimId,oper_id,mode,COPS_ACT_AUTO,COPSEventCb);
                    if (ret != SUCCESS)
                    {
                        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    }
                    else
                    {
                        engine = pParam->engine;
                        AT_SetAsyncTimerMux(pParam->engine,COPS_TEST_TIMEOUT);
                    }
                }
                else if (mode == COPS_MODE_MANUAL || mode == COPS_MODE_MANUAL_AUTOMATIC)
                {
                    if (format == COPS_FORMAT_LONG || format == COPS_FORMAT_SHORT)
                    {
                        uint8_t *roper_id;
                        
                        ret = KING_OperatorIdGet(&roper_id,format,(uint8*)oper);
                        if (ret != SUCCESS)
                        {
                            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                            break;
                        }

                        memcpy(oper_id, roper_id, 6);
                    }
                    else
                    {
                        int len = strlen((char*)oper);
                        if (len != 5 && len != 6)
                        {
                            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                            break;
                        }
                        
                        oper_id[5] = 0xf;
                        
                        for (int n = 0; n < len; n++)
                        {
                            if (oper[n] >= '0' && oper[n] <= '9')
                            {
                                oper_id[n] = oper[n] - '0';
                            }
                            else
                            {
                                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                                break;
                            }
                        }
                    }
                    
                    ret = KING_NWRegisterModeSet(nSimId,oper_id,mode,act,COPSEventCb);
                    if (ret != SUCCESS)
                    {
                        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    }
                    else
                    {
                        engine = pParam->engine;
                        AT_SetAsyncTimerMux(pParam->engine,COPS_TEST_TIMEOUT);
                    }
                }
                else if (mode == COPS_MODE_DEREGISTER)
                {
                    ret = KING_NwDeRegister(nSimId,COPSEventCb);
                    if (ret != SUCCESS)
                    {
                        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    }
                    else
                    {
                        engine = pParam->engine;
                        AT_SetAsyncTimerMux(pParam->engine,COPS_TEST_TIMEOUT);
                    }
                }
            }
            break;

        case AT_CMD_TEST:
            {
                int ret = SUCCESS;
                SIM_ID nSimId = SIM_0;

                KING_DualSimCurrent(&nSimId);

                ret = KING_AvailableOperatorsGet(nSimId, COPSEventCb);
                if (ret != 0)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    break;
                }

                engine = pParam->engine;
                AT_SetAsyncTimerMux(pParam->engine,COPS_TEST_TIMEOUT);
            }
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }
}

void AT_CmdFunc_CREG(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            at_CmdRespInfoText(pParam->engine, (const uint8_t*)"+CREG: (0-2)");
            at_CmdRespOK(pParam->engine);
            break;

        case AT_CMD_READ:
            {
                int ret;
                REG_INFO_S info;
                char strRsp[50] = {0};
                SIM_ID nSimId = SIM_0;

                KING_DualSimCurrent(&nSimId);

                ret = KING_NWRegisterInfoGet(nSimId,&info);
                if (ret != 0)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    break;
                }

                memset(strRsp, 0x00, 50);

                if (gATCurrentCreg == 2)
                {
                    if (info.state == 0)
                    {
                        sprintf(strRsp,"+CREG: %d,%d",gATCurrentCreg,info.state);
                    }
                    else
                    {
                        sprintf(strRsp,"+CREG: %d,%d,\"%04X\",\"%04X\",%d",gATCurrentCreg,info.state,
                            info.lac,info.cid,info.act);
                    }
                }
                else
                {
                    sprintf(strRsp,"+CREG: %d,%d",gATCurrentCreg,info.state);
                }
                
                at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
                at_CmdRespOK(pParam->engine);
            }
            break;

        case AT_CMD_SET:
            {
                bool paramok = true;
                uint32 mode = at_ParamUintInRange(pParam->params[0],0,2,&paramok);

                if (!paramok)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    break;
                }

                gATCurrentCreg = mode;
                at_CmdRespOK(pParam->engine);
            }
            break;

        case AT_CMD_EXE:
            gATCurrentCreg = 0;
            at_CmdRespOK(pParam->engine);
            break;
            
        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }
}

void AT_CmdFunc_CEREG(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            at_CmdRespInfoText(pParam->engine, (const uint8_t*)"+CEREG: (0-2)");
            at_CmdRespOK(pParam->engine);
            break;

        case AT_CMD_READ:
            {
                int ret;
                REG_INFO_S info;
                char strRsp[50] = {0};
                SIM_ID nSimId = SIM_0;

                KING_DualSimCurrent(&nSimId);

                ret = KING_PSRegisterInfoGet(nSimId,&info);
                if (ret != 0)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    break;
                }

                memset(strRsp, 0x00, 50);

                if (gATCurrentCereg == 2)
                {
                    if (info.state == 0)
                    {
                        sprintf(strRsp,"+CEREG: %d,%d",gATCurrentCereg,info.state);
                    }
                    else
                    {
                        if (info.act == 0)
                        {
                            sprintf(strRsp,"+CEREG: %d,%d,\"%04X\",\"%04X\",%d",gATCurrentCereg,info.state,
                                info.lac,info.cid,info.act);
                        }
                        else
                        {
                            sprintf(strRsp,"+CEREG: %d,%d,\"%04X\",\"%07X\",%d",gATCurrentCereg,info.state,
                                info.lac,info.cid,info.act);
                        }
                    }
                }
                else
                {
                    sprintf(strRsp,"+CEREG: %d,%d",gATCurrentCereg,info.state);
                }
                
                at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
                at_CmdRespOK(pParam->engine);
            }
            break;

        case AT_CMD_SET:
            {
                bool paramok = true;
                uint32 mode = at_ParamUintInRange(pParam->params[0],0,2,&paramok);

                if (!paramok)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    break;
                }

                gATCurrentCereg = mode;
                at_CmdRespOK(pParam->engine);
            }
            break;

       case AT_CMD_EXE:
            gATCurrentCereg = 0;
            at_CmdRespOK(pParam->engine);
            break;
            
        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }
}

void AT_CmdFunc_CESQ(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            {
                char strRsp[60] = {0};
                memset(strRsp, 0x00, 60);

                strcpy(strRsp,"+CESQ: (0-99),(0-99),(0-255),(0-255),(0-255),(0-255)");
                at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
                at_CmdRespOK(pParam->engine);
            }
            break;
        case AT_CMD_EXE:
            {
                int ret = SUCCESS;
                NET_CESQ_S Cesq;
                char strRsp[30] = {0};
                
                memset(strRsp, 0x00, 30);

                ret = KING_NetSignalQualityGet(&Cesq);
                if (ret != 0)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    break;
                }
                
                sprintf(strRsp,"+CESQ: %d,%d,%d,%d,%d,%d",
                    Cesq.rxlev,Cesq.ber,Cesq.rscp,Cesq.ecno,Cesq.rsrq,Cesq.rsrp);
                
                at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
                at_CmdRespOK(pParam->engine);
            }
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }

    return;

}

void AT_CmdFunc_CGATT(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            {
                char strRsp[20] = {0};
                memset(strRsp, 0x00, 20);

                strcpy(strRsp,"+CGATT: (0,1)");
                at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
                at_CmdRespOK(pParam->engine);
            }
            break;

        case AT_CMD_READ:
            {
                int ret;
                uint32 value;
                char strRsp[10] = {0};
                
                memset(strRsp, 0x00, 10);
                ret = KING_NetAttachStatusGet(&value);
                if (ret != 0)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    break;
                }

                sprintf(strRsp,"+CGATT: %d",value);
                at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
                at_CmdRespOK(pParam->engine);
            }
            break;
            
        case AT_CMD_SET:
            {
                int ret = SUCCESS;
                bool paramok = true;
                uint32 mode = at_ParamUintInRange(pParam->params[0],0,1,&paramok);

                if (!paramok)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    break;
                }

                if (mode == 1)
                {
                    ret = KING_NetAttach();
                }
                else
                {
                    ret = KING_NetDeattach();
                }

                if (ret != 0)
                {
                    if (KING_GetLastErrCode() == NW_ERR_GPRS_ATTACH_NODO)
                    {
                        at_CmdRespOK(pParam->engine);
                    }
                    else
                    {
                        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    }
                    break;
                }
                else
                {
                    engine = pParam->engine;
                    AT_SetAsyncTimerMux(pParam->engine,COPS_TEST_TIMEOUT);
                }
            }
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }

    return;
}

static void wifiScanCB(uint32 eventID, void* pData, uint32 len)
{
    KH_CB_DATA_U *p;
    uint32 i;
    char info[40];

    switch(eventID)
    {
    case WIFI_EVENT_SCAN_RESULT:
        p = (KH_CB_DATA_U *)pData;
        for (i = 0; i < len; i++)
        {
            snprintf(info, 40, "+CWLSCANIND: %04X%08X,%d,%d", p->wifi.result.aps[i].bssid_high, p->wifi.result.aps[i].bssid_low,
                     p->wifi.result.aps[i].rssival, p->wifi.result.aps[i].channel);
            at_CmdRespInfoText(gWifiEngine, (uint8 *)info);
            KING_Sleep(5);
        }
        break;

    case WIFI_EVENT_SCAN_STOP:
        snprintf(info, 40, "+CWLSCANIND: 0,0,0");
        at_CmdRespInfoText(gWifiEngine, (uint8 *)info);
        break;

    default:
        break;
    }
}

void AT_CmdFunc_CWLSCAN(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    uint32_t value;
    int ret = SUCCESS;

    switch (pParam->iType)
    {
    case AT_CMD_TEST:
        sprintf((char *)g_rspStr,"+CWLSCAN: (0,1)");
        at_CmdRespInfoText(pParam->engine, (uint8 *)g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_SET:
        if (1 != pParam->paramCount)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        value = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        if (true != paramok)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        if (gWifiScan != value)
        {
            if (1 == value)
            {
                ret = KING_RegNotifyFun(DEV_CLASS_WIFI, 0, wifiScanCB);
                ret = KING_WifiScanInit();
                if (FAIL == ret)
                {
                    ret = KING_GetLastErrCode();
                    KING_SysLog("KING_WifiScanInit Fail: 0x%08X", ret);
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    goto exit;
                }
                KING_Sleep(300);
                ret = KING_WifiScanStart(1);
                if (FAIL == ret)
                {
                    ret = KING_GetLastErrCode();
                    KING_SysLog("KING_WifiScanStart Fail: 0x%08X", ret);
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    goto exit;
                }
            }
            else
            {
                ret = KING_WifiScanStop();
                if (FAIL == ret)
                {
                    ret = KING_GetLastErrCode();
                    KING_SysLog("KING_WifiScanStop Fail: 0x%08X", ret);
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    goto exit;
                }

                ret = KING_WifiScanDeinit();
                if (FAIL == ret)
                {
                    ret = KING_GetLastErrCode();
                    KING_SysLog("KING_WifiScanDeinit Fail: 0x%08X", ret);
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    goto exit;
                }

                ret = KING_UnRegNotifyFun(DEV_CLASS_WIFI, 0, wifiScanCB);
            }
            gWifiScan = value;
            gWifiEngine = pParam->engine;

            at_CmdRespOK(pParam->engine);
        }
        else
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            goto exit;
        }
        
        break;

    case AT_CMD_READ:
        sprintf((char *)g_rspStr,"+CWLSCAN: %d", gWifiScan);
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;
        
    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }

exit:
    return;
}

