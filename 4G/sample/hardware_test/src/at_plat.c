#include "app_main.h"
#include "uart_init.h"
#include "at.h"

static const char app_versions[] = "app_versions_11_20200917";

/**
 *    提供给硬件的版本号以供查询当前的app
 *    
 *    @param[in]  data  
 *    @param[in]  dataLen 数据长度 
 *    @return 0 SUCCESS  -1 FAIL
 */
int AtAppVersions(char* data, uint32 dataLen)
{
    if ((0 != dataLen) && (data[0] == '=') && (data[1] == '?'))
    {
        return SUCCESS;
    }
    
    if(0 != dataLen)
    {
        return FAIL;
    }
    
    ATPrintf("%s\r\n",app_versions);
    return SUCCESS;
}

/**
 *    获取IMEI号
 *    
 *    @param[in]  data  
 *    @param[in]  dataLen 数据长度 
 *    @return 0 SUCCESS  -1 FAIL
 */
int AtImeiGet(char* data, uint32 dataLen)
{
    uint8 pImei[30];

    if (data[0] == '=' && data[1] == '?')
    {
        return SUCCESS;
    }
    
    if(0 != dataLen)
    {
        return FAIL;
    }
    
    memset(pImei,0x00,sizeof(pImei));
    KING_GetSysImei(pImei);
    ATPrintf("%s\r\n",pImei);
    return SUCCESS;
}

/**
 *    Camera
 *    
 *    @param[in]  data  
 *    @param[in]  dataLen 数据长度 
 *    @return 0 SUCCESS  -1 FAIL
 */
int AtCamera(char* data, uint32 dataLen)
{
    return SUCCESS;
}


