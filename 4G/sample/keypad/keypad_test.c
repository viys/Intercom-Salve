/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "KingCSTD.h"
#include "KingPlat.h"
#include "KingKeypad.h"
#include "app_main.h"

/*******************************************************************************
 ** MACROS
 ******************************************************************************/
#define LogPrintf(fmt, args...)    do { KING_SysLog("keypad: "fmt, ##args); } while(0)


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
 * keypad ªÿµ˜∫Ø ˝
 *
 */
void Keypad_CallBack(KEYPAD_EVENT_E eventCode,uint32 keyCode)
{
    uint16 keyCodeH = 0; //row value,row index start with 0
    uint16 keyCodeL = 0; //colum value,colum index start with 0

    keyCodeL = keyCode & 0x0000ffff;
    keyCodeH = (keyCode >> 16) & 0x0000ffff;
    LogPrintf("eventCode=0x%x, keyCodeL=0x%x, keyCodeH=0x%x\r\n", eventCode, keyCodeL, keyCodeH);
}
/**
 * keypad ≤‚ ‘
 *
 */
void Keypad_Test(void)
{
    int ret = 0;

    LogPrintf("\r\n-----keypad test start------\r\n");
    
    ret = KING_KpInit();
    if(-1 == ret)
    {
        LogPrintf("KING_KpInit Fail£° errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    ret = KING_KpRegCB(Keypad_CallBack);
    if(-1 == ret)
    {
        LogPrintf("KING_KpRegCB Fail£° errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
}




