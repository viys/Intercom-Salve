/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "app_main.h"
#include "KingDef.h"
#include "KingCSTD.h"
#include "KingPlat.h"
#include "KingRtos.h"
#include "kingnet.h"
#include "kingsocket.h"
#include "httpclient.h"
#include "KingFota.h"
#include "KingPowerManage.h"
#include "KingFileSystem.h"

#define LogPrintf(fmt, args...)    do { KING_SysLog("fota: "fmt, ##args); } while(0)

#define HTTP_URL "http://60.179.35.254:2811/e/file/download?code=f4635f4281133248&id=52016"

#define BUF_SIZE     512

static hardwareHandle hSocketNIC = -1;
static softwareHandle hSocketContext = -1;
static MSG_HANDLE fotaMsgHandle;

//网络初始化
static int NetWork_Init(void)
{
    int ret;
    NIC_INFO info;
    NET_STATE_E state;
    NIC_CONTEXT ApnInfo;

    LogPrintf("NetWork_Init start\r\n");

    while(1)
    {
        ret = KING_NetStateGet(&state); //获取网络状态
        LogPrintf("Get net state %d, ret : %d\r\n", state, ret);
        if (ret == 0 && state > NET_NO_SERVICE && state < NET_EMERGENCY)
        {
            NET_CESQ_S Cesq;
            ret = KING_NetSignalQualityGet(&Cesq); //获取信号质量
            LogPrintf("Get sinagal ret is %d. rxlev:%d, ber:%d, rscp:%d, ecno:%d, rsrq:%d, rsrp:%d\r\n",
            ret,Cesq.rxlev,Cesq.ber,Cesq.rscp,Cesq.ecno,Cesq.rsrq,Cesq.rsrp);

            ret = KING_NetApnSet(1, "", NULL, NULL, 0, IP_ADDR_TYPE_V4); //设置APN

            LogPrintf("KING_NetApnSet ret : %d\r\n", ret);
            if (ret != 0)
            {
                LogPrintf("KING_NetApnSet fail\r\n");
                ret = -1;
                goto ERROR;
            }
            memset(&info, 0x00, sizeof(NIC_INFO));
            info.NIC_Type = NIC_TYPE_WWAN;
            ret = KING_HardwareNICRequest(info, &hSocketNIC); //请求NIC(网络接口控制器)
            LogPrintf("KING_HardwareNICRequest ret=%d, hSocketNIC=%d", ret, hSocketNIC);

            if (hSocketNIC == -1)
            {
                LogPrintf("KING_HardwareNICRequest fail\r\n");
                ret = -1;
                goto ERROR;
            }

            memset(&ApnInfo, 0x00, sizeof(NIC_CONTEXT));
            ApnInfo.IpType = IP_ADDR_TYPE_V4;

            ret = KING_ConnectionAcquire(hSocketNIC, ApnInfo, &hSocketContext); //发起连接
            LogPrintf("KING_ConnectionAcquire ret=%d, hSocketContext=%d", ret, hSocketContext);
            if (hSocketContext == -1)
            {
                LogPrintf("KING_ConnectionAcquire fail\r\n");
                ret = -1;
                goto ERROR;
            }
            LogPrintf("NetWork_Init Success\r\n");
            break;
        }
        KING_Sleep(1000);
    }
    return 0;

ERROR:
    return ret;
}


//GET回调
static void http_get_callback(uint16 errCode, int16 httprspcode, uint32 contentLen)
{
    int ret = -1;
    char *phttpData = NULL;
    uint32 bytesRead = 0;
    uint32 offset = 0;
    uint32 bytesWr = 0;
    uint8 readEnd = 0;
    LogPrintf("http_get_callback errCode : %d\r\n", errCode);
    LogPrintf("http_get_callback httprspcode : %d\r\n", httprspcode);
    LogPrintf("http_get_callback contentLen : %u\r\n", contentLen);
    if (errCode == 0 && httprspcode == HTTP_RESPONSE_CODE_OK)
    {
        //获取header
        if(http_rspHeaderGet() == 1)
        {
            char *pHeaderStr = NULL;
            uint32 headLen = 0;
            http_readHeaderData(&pHeaderStr, &headLen);
            if (headLen > 0)
            {
              LogPrintf("http_get_callback pHeaderStr : %s", pHeaderStr);
              LogPrintf("http_get_callback headLen : %d", headLen);
            }
        }
        phttpData = malloc(BUF_SIZE + 1);
        if (phttpData == NULL)
        {
            LogPrintf("malloc fail errorcode: 0x%X\r\n", KING_GetLastErrCode());
            return;
        }

        //循环读取body内容直到文件末尾,每次BUF_SIZE字节
        while (!readEnd)
        {
            memset(phttpData, 0x00, BUF_SIZE + 1);
            ret = http_readContentDataAndLen(60, BUF_SIZE, phttpData, &bytesRead,&readEnd);
            if (ret < 0)
            {
                LogPrintf("http_readContentDataAndLen fail");
                break;
            }
            if(0 != bytesRead)
            {
                ret = KING_FotaFlashWrite(offset, phttpData, (uint32)bytesRead, &bytesWr);
                if (ret < 0)
                {
                    LogPrintf("KING_FsFileSizeGet errorcode: 0x%X", KING_GetLastErrCode());
                    break;
                }
                LogPrintf("bytesRead: %d bytesWr: %d", bytesRead, bytesWr);
                if (bytesRead != bytesWr)
                {
                    LogPrintf("error bytesRead != bytesWr");
                    break;
                }
                offset += bytesWr;
            }
            KING_Sleep(70);
        }
        LogPrintf("http_get_callback readEnd : %d", readEnd);
        if(readEnd)
        {
            LogPrintf("HTTP READ END\r\n");
            if(0 != KING_FotaImageCheck(NULL))
            {
                LogPrintf("KING_FotaImageCheck errorcode: 0x%X", KING_GetLastErrCode());
                return;
            }

            if (0 != KING_FotaFlagSet(FOTA_IMAGE_IS_READY))
            {
                LogPrintf("KING_FotaFlagSet errorcode: 0x%X", KING_GetLastErrCode());
                return;
            }
            LogPrintf("Reboot for upgrade!!!\r\n");
            KING_Sleep(1000);
            KING_PowerReboot();
            KING_Sleep(1000);
        }
        
        if (phttpData != NULL)
        {
            free(phttpData);
            phttpData = NULL;
        }
    }

}

static void Http_Get(char* fotaURL)
{  
    int ret;
    ret = http_urlParse(fotaURL);
    if (-1 == ret)
    {
        LogPrintf("http_urlParse fail ret : %d\r\n", ret);
        return;
    }
    http_get(hSocketContext, NULL, 0, 60, (HTTP_RSP_CALLBACK)http_get_callback);
}

void Http_Fota_Test(void)
{
    int ret;
    MSG_S msg;
    
    FOTA_FLAG_E fotaFlag;
    //  模块重启后，APP第一时间调用KING_FotaGetFlag()检查当前的FOTA_FLAG
    //  如果为FOTA_UPDATE_SUCCESS/FAILED则调用FotaSetFlag()
    //  将flag设置为FOTA_CLEAR。至此升级完成。
    if(0 == KING_FotaFlagGet(&fotaFlag))
    {
        if (fotaFlag == FOTA_UPDATE_SUCCESS)
        {
            LogPrintf("Fota SUCC\r\n");
            KING_FotaFlagSet(FOTA_CLEAR);
        }
    }
    
    NetWork_Init();
    
    //msg
    ret = KING_MsgCreate(&fotaMsgHandle);
    if(FAIL == ret)
    {
        LogPrintf("KING_MsgCreate Fail\r\n");
        return;
    }
    Http_Get(HTTP_URL);
    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        KING_MsgRcv(&msg, fotaMsgHandle, WAIT_OPT_INFINITE);
    }
}


