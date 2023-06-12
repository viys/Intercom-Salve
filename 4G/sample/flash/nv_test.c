/*******************************************************************************
 ** @details NV是flash上的一片存储空间，主要用于存储
 ** 永久保存的参数，比如设备SN号。无论是断电，重启，
 ** 或是文件系统奔溃，NV里的数据理论上都不应该丢失。
 ** 不同平台用于保存NV的flash空间地址和存储方法不同。
 **
 **
 ** @details UIS8910平台使用原始的flash空间，目前仅支持5个NV(ID:0-4)，
 ** 每个NV的空间大小为1024字节。
 *******************************************************************************/

 /*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "KingCSTD.h"
#include "KingPlat.h"
#include "KingFlash.h"

#include "app_main.h"

/*******************************************************************************
 ** MACROS
 ******************************************************************************/
#define LogPrintf(fmt, args...)    do { KING_SysLog("nv: "fmt, ##args); } while(0)
#define SN_LEN              16
#define KEY_LEN             16

#define NV_DEV_SN           0

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
 
typedef struct
{
    uint8 sn[SN_LEN];
    uint8 key[KEY_LEN];
} APP_NV_T;

/*******************************************************************************
 ** Variables
 ******************************************************************************/

/*******************************************************************************
 ** External Function Declerations
 ******************************************************************************/

/*******************************************************************************
 ** Local Function Declerations
 ******************************************************************************/

/*******************************************************************************
 ** Functions
 ******************************************************************************/
 /**
 * Nv 测试
 *
 */
void Nv_Test(void)
{
    int ret = -1;
    uint32 nvsize = 0;
    APP_NV_T appNv;
    LogPrintf("\r\n-----nv test start------\r\n");

    //nv初始化
    ret = KING_NvInit();
    if (-1 == ret)
    {
        LogPrintf("KING_NvInit() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }

    //读取当前地址NV内容
    memset(&appNv, 0, sizeof(APP_NV_T));
    nvsize = sizeof(APP_NV_T);
    ret = KING_NvRead(NV_DEV_SN, &appNv, &nvsize);
    if (FAIL == ret)
    {
        LogPrintf("KING_NvRead() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    LogPrintf("NV Read Sn:%s\r\n",appNv.sn);
    LogPrintf("NV Read Key:%s\r\n",appNv.key);
    LogPrintf("NV Read len:%d\r\n",nvsize);
    
    //写NV
    memset(&appNv, 0, sizeof(APP_NV_T));
    memcpy(appNv.sn, "123456789abcde", sizeof("123456789abcde"));
    memcpy(appNv.key, "!@#$%^&*()_+{}:", sizeof("!@#$%^&*()_+{}:"));
    ret = KING_NvWrite(NV_DEV_SN, &appNv, sizeof(APP_NV_T));
    if (FAIL == ret)
    {
        LogPrintf("KING_NvWrite() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    LogPrintf("Nv Write Success\r\n");

    //读NV
    memset(&appNv, 0, sizeof(APP_NV_T));
    nvsize = sizeof(APP_NV_T);
    ret = KING_NvRead(NV_DEV_SN, &appNv, &nvsize);
    if (FAIL == ret)
    {
        LogPrintf("KING_NvRead() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    LogPrintf("NV Read Sn:%s\r\n",appNv.sn);
    LogPrintf("NV Read Key:%s\r\n",appNv.key);
    LogPrintf("NV Read len:%d\r\n",nvsize);

    ret = KING_NvDeinit();
    if (FAIL == ret)
    {
        LogPrintf("KING_NvDeinit() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
}


