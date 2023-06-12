/*******************************************************************************
 ** File Name:   plat_test.c
 ** Copyright:   Copyright 2020-2021 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: plat api test code
 *******************************************************************************
 
 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-04-26     suzhiliang         Create.
 ******************************************************************************/

/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "app_main.h"
#include "kingplat.h"
#include "kingcstd.h"
#include "kingerrcode.h"
#include "kingrtos.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("plat: "fmt, ##args); } while(0)

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
 
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
 * plat 测试
 *
 */
void Plat_Test(void)
{
    int ret;
    uint8 imei[16] = {0};
    uint32 simRdy = 0;
    uint32 version = 0;
    uint8 simID = 0;
    uint8 meid[20] = {0};
    uint8 uuid[16] = {0};
    uint32 size = 0;
    SIM_HOT_PLUG_CFG_T sim_hot_plug_cfg;
    AUDIO_CALI_PARAM_CFG_T audio_cali_param_cfg;
    
    LogPrintf("\r\n-----plat test start------\r\n");
    
    //获取SDK版本
    ret = KING_ApiLevel(&version);
    if(FAIL == ret)
    {
        LogPrintf("KING_ApiLevel() errcode=0x%x\r\n", KING_GetLastErrCode());     
    }
    else
    {
        LogPrintf("ApiLevel succ: %d\r\n", version);
    }
    
    ret = KING_SetLastErrCode(GLOBAL_ERR_INVALID_PARAM);
    LogPrintf("KING_SetLastErrCode ret: %d\r\n", ret);
    
    ret = KING_GetLastErrCode();
    LogPrintf("errcode=0x%x\r\n", ret);     
    
    //获取平台版本信息
    LogPrintf("PlatformInfo: %s \r\n", KING_PlatformInfo());
   
    //返回平台的IMEI号
    memset(imei, 0x00, sizeof(imei));
    ret = KING_GetSysImei(imei);
    if (FAIL == ret)
    { 
        LogPrintf("KING_GetSysImei() errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("GetImei succ: %s \r\n", imei);
    }

    //获取sim卡ready状态
    while (simRdy == 0)
    {
        ret = KING_SimReadyGet(&simRdy);
        if (FAIL == ret)
        { 
            LogPrintf("KING_SimReadyGet() errcode=0x%x\r\n", KING_GetLastErrCode());
        }

        LogPrintf("Get SIM status: %d\r\n", simRdy);
        KING_Sleep(1000);
    }
    memset(meid,0x00,sizeof(meid));
    ret = KING_SysMEIDGet(simID,meid);
    if (FAIL == ret)
    { 
        LogPrintf("KING_SysMEIDGet() errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("simID:%d,meid:%s\r\n",simID, meid);
    }

    //获取chipId
    ret = KING_GetChipId(uuid);
    if (FAIL == ret)
    { 
        LogPrintf("KING_GetChipId() failed errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        char uuid_str[64] = {0};
        uint8 offset = 0;
        for(int i = 0; i < 16; i++)
        {
            offset += sprintf(uuid_str + offset, "%02x",uuid[i]);
        }
        LogPrintf("ChipId:%s", uuid_str);  
    }
    
    memset(&sim_hot_plug_cfg, 0x00, sizeof(SIM_HOT_PLUG_CFG_T));
    sim_hot_plug_cfg.simID = 0;
    sim_hot_plug_cfg.enable = 1;//使能热插拔
    sim_hot_plug_cfg.level = 1;//开发板默认高电平在位
    size = sizeof(SIM_HOT_PLUG_CFG_T);
    ret = KING_SysCfg(1, SYS_CFG_ID_SIM_HOT_PLUG, &sim_hot_plug_cfg, &size);
    if (FAIL == ret)
    { 
        LogPrintf("KING_SysCfg() SIM failed errcode=0x%x", KING_GetLastErrCode());
    }

    memset(&sim_hot_plug_cfg, 0x00, sizeof(SIM_HOT_PLUG_CFG_T));
    size = 0;
    ret = KING_SysCfg(0, SYS_CFG_ID_SIM_HOT_PLUG, &sim_hot_plug_cfg, &size);
    if (FAIL == ret)
    { 
        LogPrintf("KING_SysCfg() failed errcode=0x%x", KING_GetLastErrCode());
    }
    if(size == sizeof(SIM_HOT_PLUG_CFG_T))
    {
        LogPrintf("simID:%d, enable:%d, level:%d",sim_hot_plug_cfg.simID,sim_hot_plug_cfg.enable,sim_hot_plug_cfg.level);
    }
    
    memset(&audio_cali_param_cfg, 0x00, sizeof(AUDIO_CALI_PARAM_CFG_T));
    audio_cali_param_cfg.channel = 0;
    memcpy(audio_cali_param_cfg.cfg_file,"/preset/volte_cfg.bin",sizeof("/preset/volte_cfg.bin"));//需先预制文件到文件系统中
    size = sizeof(AUDIO_CALI_PARAM_CFG_T);
    ret = KING_SysCfg(1, SYS_CFG_ID_AUDIO_CALI_PARAM, &audio_cali_param_cfg, &size);
    if (FAIL == ret && ERROR_CODE(EC_PLATFORM, EN_EEXIST) != KING_GetLastErrCode())//当返回码为ERROR_CODE(EC_PLATFORM, EN_EEXIST)时代表要写入的与系统的参数一致无需写入
    {
        LogPrintf("KING_SysCfg() AUDIO failed errcode=0x%x", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("audio calibration success");
    }

}

