/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "KingCSTD.h"
#include "KingPlat.h"
#include "KingRtos.h"
#include "KingFlash.h"
#include "app_main.h"

/*******************************************************************************
 ** MACROS
 ******************************************************************************/
#define LogPrintf(fmt, args...)    do { KING_SysLog("flash: "fmt, ##args); } while(0)


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
 * spi flash 测试
 *
 */
void Spi_Flash_Test(void)
{
    int ret = -1;
    int i;
    uint8 buff[300];
    LogPrintf("\r\n-----Spi Flash Test start------\r\n");

    ret = KING_SpiFlashInit(FLASH_EXTERN);
    if (FAIL == ret)
    {
        LogPrintf("KING_SpiFlashInit() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }  
    
    ret = KING_SpiFlashErase(0x00, 4*1024);//按块擦除具体芯片看规格书4K 32K 64K
    if (FAIL == ret)
    {
        LogPrintf("KING_SpiFlashErase() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    LogPrintf("Flash Erase Success\r\n");

    
    LogPrintf("write  ");
    for(i = 0; i < 256;i++)
    {
        buff[i] = i;
        LogPrintf("%x  ", buff[i]);
    }

    ret = KING_SpiFlashWrite(0x00, 256, buff);
    if (-1 == ret)
    {
        LogPrintf("KING_SpiFlashWrite() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    //LogPrintf("Write len:%d\r\n",ret);
    
    KING_Sleep(500);
    ret = KING_SpiFlashRead(0x00, 256, buff);
    if (-1 == ret)
    {
        LogPrintf("KING_SpiFlashRead() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    //LogPrintf("read len:%d\r\n",ret);

    LogPrintf("read  ");
    for(i = 0; i < 256;i++)
    {
        LogPrintf("%x  ", buff[i]);
    }    
    
    ret = KING_SpiFlashDeinit();
    if (FAIL == ret)
    {
        LogPrintf("KING_SpiFlashDeinit error\r\n");
    }
}

