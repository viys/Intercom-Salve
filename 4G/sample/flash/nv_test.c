/*******************************************************************************
 ** @details NV��flash�ϵ�һƬ�洢�ռ䣬��Ҫ���ڴ洢
 ** ���ñ���Ĳ����������豸SN�š������Ƕϵ磬������
 ** �����ļ�ϵͳ������NV������������϶���Ӧ�ö�ʧ��
 ** ��ͬƽ̨���ڱ���NV��flash�ռ��ַ�ʹ洢������ͬ��
 **
 **
 ** @details UIS8910ƽ̨ʹ��ԭʼ��flash�ռ䣬Ŀǰ��֧��5��NV(ID:0-4)��
 ** ÿ��NV�Ŀռ��СΪ1024�ֽڡ�
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
 * Nv ����
 *
 */
void Nv_Test(void)
{
    int ret = -1;
    uint32 nvsize = 0;
    APP_NV_T appNv;
    LogPrintf("\r\n-----nv test start------\r\n");

    //nv��ʼ��
    ret = KING_NvInit();
    if (-1 == ret)
    {
        LogPrintf("KING_NvInit() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }

    //��ȡ��ǰ��ַNV����
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
    
    //дNV
    memset(&appNv, 0, sizeof(APP_NV_T));
    memcpy(appNv.sn, "123456789abcde", sizeof("123456789abcde"));
    memcpy(appNv.key, "!@#$%^&*()_+{}:", sizeof("!@#$%^&*()_+{}:"));
    ret = KING_NvWrite(NV_DEV_SN, &appNv, sizeof(APP_NV_T));
    if (FAIL == ret)
    {
        LogPrintf("KING_NvWrite() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    LogPrintf("Nv Write Success\r\n");

    //��NV
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


