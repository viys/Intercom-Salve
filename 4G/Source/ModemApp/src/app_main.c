#include "app_main.h"

extern bool BAL_ATInit(void);

/*------------------------------------------------------------------------------------------------------
king APP ��ں���
------------------------------------------------------------------------------------------------------*/
void APP_Main(uint32 bootMode)
{
    KING_SysLog("enter APP_Main...");
    BAL_ATInit();
}

