/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "app_main.h"
#include "KingPowerManage.h"

 /*---------------------------------------------------------------------------
  ** Defines
 ---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("ldo: "fmt, ##args); } while(0)
#define true            1
#define false           0
#define LDO_ONOFF_DEF   true
#define LDO_ID_DEF      LDO_ID_LCD 
#define LDO_VOL_DEF     LDO_VOL_1V8

/*******************************************************************************
 **  Function 
 ******************************************************************************/
void Ldo_Test(void)
{
    bool ldo_onoff = LDO_ONOFF_DEF;
    LDO_ID_E ldo_id = LDO_ID_DEF;
    LDO_VOL_E ldo_vol = LDO_VOL_DEF;
    int ret = -1;;
    
    LogPrintf("Ldo_Test begin\r\n");
    ret = KING_LdoConfigSet(ldo_id, ldo_vol, ldo_onoff);
    if(ret == -1)
    {
        LogPrintf("id:%d vol:%d onoff:%d set fail\r\n", ldo_id, ldo_vol,ldo_onoff);
        return;
    }
    LogPrintf("id:%d vol:%d onoff:%d set success\r\n", ldo_id, ldo_vol,ldo_onoff);
}



