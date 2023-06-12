/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#include "at_module.h"
#include "at_cfg.h"
#include "at_utils.h"
#include "kingfilesystem.h"

#define AT_CFG_FN_PATTERN "AT_CFG_%d.BIN"
#define AT_CFG_FN_LEN (32)
#define AT_CFG_NUM_MAX (1)

#define AT_AS_CFG_FN "AT_AS_CFG.BIN"

extern const at_setting_t gDefaultSettings;
extern const at_as_setting_t gDefAsSettings;

at_setting_t gAtCurrentSetting; // current settings
at_as_setting_t gAtCurAsSetting; // current auto save settings

static void POSSIBLY_UNUSED at_CfgFileName(uint8_t id, uint8_t *buf)
{
    sprintf((char*)buf, AT_CFG_FN_PATTERN, id);
}

static bool POSSIBLY_UNUSED at_ReadCfgFile(uint8_t set_id, at_setting_t *setting)
{
    uint32 len = 0;
    uint32 bytesRead = 0;
    FS_HANDLE fd = 0;
    uint8_t filename[AT_CFG_FN_LEN];

    at_CfgFileName(set_id, filename);
    if (FAIL == KING_FsFileCreate((char*)filename, FS_MODE_READ|FS_MODE_OPEN_EXISTING, 0, 0, &fd))
    {
        ATLOGE("[atcfg]%s: failed to open config file", __FUNCTION__);
        goto free_failed;
    }

    len = sizeof(at_setting_t);
    if(KING_FsFileRead(fd, (uint8*)setting, len, &bytesRead) != SUCCESS)
    {
        ATLOGE("[atcfg]%s: failed to read config file", __FUNCTION__);        
        KING_FsCloseHandle(fd);
        goto free_failed;
    }
    
    //ATLOGE("[atcfg]%s: %d, %d", __FUNCTION__, sizeof(at_setting_t), bytesRead);
    KING_FsCloseHandle(fd);
    return true;

free_failed:
    return false;
}

static bool POSSIBLY_UNUSED at_WriteCfgFile(uint8_t set_id, at_setting_t *setting)
{
    unsigned length = 0;
    uint32 byteswritten = 0;
    FS_HANDLE fd = 0;
    uint8_t filename[AT_CFG_FN_LEN];

    at_CfgFileName(set_id, filename);
    if (FAIL == KING_FsFileCreate((char*)filename, FS_MODE_WRITE|FS_MODE_CREATE_ALWAYS, 0, 0, &fd))
    {
        ATLOGE("[atcfg]%s: failed to open config file", __FUNCTION__);
        goto free_failed;
    }

    length = sizeof(at_setting_t);
    if (KING_FsFileWrite(fd, (uint8*)setting, length, &byteswritten) != SUCCESS)
    {
        ATLOGE("[atcfg]%s: failed to write config file", __FUNCTION__);
        KING_FsCloseHandle(fd);
        goto free_failed;
    }
    //ATLOGE("[atcfg]%s: %d, %d", __FUNCTION__, sizeof(at_setting_t), byteswritten);
    KING_FsCloseHandle(fd);
    return true;

free_failed:
    return false;
}

// API to save at settings
bool at_CfgSetAtSettings(uint8_t set_id)
{
    if (set_id >= AT_CFG_NUM_MAX)
        return false;

    return at_WriteCfgFile(set_id, &gAtCurrentSetting);
}

// API to get at settings
bool at_CfgGetAtSettings(uint8_t flag, uint8_t set_id)
{
    ATLOGV("[atcfg]get, flag=%u, id=%u", flag, set_id);

    if (flag == MANUFACTURER_DEFALUT_SETING)
    {
        gAtCurrentSetting = gDefaultSettings;
        return true;
    }

    if (flag == USER_SETTING_1 && set_id < AT_CFG_NUM_MAX)
    {
        at_setting_t setting = gDefaultSettings;
        if (!at_ReadCfgFile(set_id, &setting))
            goto failed;
        gAtCurrentSetting = setting;
        return true;
    }

failed:
    gAtCurrentSetting = gDefaultSettings;
    return false;
}

// API to save auto save settings
bool at_CfgSaveAsSettings(void)
{
    unsigned length = 0;
    uint32 byteswritten = 0;
    FS_HANDLE fd = 0;
    char filename[32] = AT_AS_CFG_FN;

    if (FAIL == KING_FsFileCreate((char*)filename, FS_MODE_WRITE|FS_MODE_CREATE_ALWAYS, 0, 0, &fd))
    {
        ATLOGE("[atcfg]%s: failed to open config file", __FUNCTION__);
        goto free_failed;
    }

    length = sizeof(at_as_setting_t);
    if (KING_FsFileWrite(fd, (uint8*)&gAtCurAsSetting, length, &byteswritten) != SUCCESS)
    {
        ATLOGE("[atcfg]%s: failed to write config file", __FUNCTION__);
        KING_FsCloseHandle(fd);
        goto free_failed;
    }
    KING_FsCloseHandle(fd);
    return true;

free_failed:
    return false;
}

// API to get auto save settings
static bool at_CfgLoadAsSettings(void)
{
    uint32 len = 0;
    uint32 bytesRead = 0;
    FS_HANDLE fd = 0;
    char filename[32] = AT_AS_CFG_FN;

    if (FAIL == KING_FsFileCreate((char*)filename, FS_MODE_READ|FS_MODE_OPEN_EXISTING, 0, 0, &fd))
    {
        ATLOGE("[atcfg]%s: failed to open config file", __FUNCTION__);
        goto free_failed;
    }

    len = sizeof(at_as_setting_t);
    if(KING_FsFileRead(fd, (uint8*)&gAtCurAsSetting, len, &bytesRead) != SUCCESS)
    {
        ATLOGE("[atcfg]%s: failed to read config file", __FUNCTION__);        
        KING_FsCloseHandle(fd);
        goto free_failed;
    }
    
    KING_FsCloseHandle(fd);
    return true;

free_failed:
    gAtCurAsSetting = gDefAsSettings;
    return false;
}

bool at_CfgInit(void)
{
    uint8_t filename[AT_CFG_FN_LEN];
    FS_HANDLE fd;
    uint32 written = 0;

    KING_FsInit();
    for (int n = 0; n < AT_CFG_NUM_MAX; n++)
    {
        at_CfgFileName(n, filename);
        if (FAIL == KING_FsFileCreate((char*)filename, FS_MODE_OPEN_EXISTING, 0, 0, &fd))
        {
            ATLOGE("[atcfg] %s not exist", (char*) filename);
            KING_FsFileCreate((char*)filename, FS_MODE_READ|FS_MODE_WRITE|FS_MODE_CREATE_ALWAYS, 0, 0, &fd);
            KING_FsFileWrite(fd, (uint8*)&gDefaultSettings, sizeof(at_setting_t), &written);
            ATLOGE("[atcfg] write %d, %d", sizeof(at_setting_t), written);
        }
        KING_FsCloseHandle(fd);
        fd = 0;
    }

    at_CfgLoadAsSettings();
    return true;
}
