/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "KingCSTD.h"
#include "KingPlat.h"
#include "KingRtos.h"
#include "KingSpi.h"
#include "app_main.h"

/*******************************************************************************
 ** MACROS
 ******************************************************************************/
#define LogPrintf(fmt, args...)    do { KING_SysLog("spi: "fmt, ##args); } while(0)

#define SPI_ID 1

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
 * spi 测试
 *
 */
void Spi_Test(void)
{
    int ret = -1;
  
    SPI_CONFIG_S spiCfg;
    uint8 cmd_addr[4];
    uint8 buff[300];
    LogPrintf("Spi Test starts...\r\n");

    ret = KING_SpiInit();
    if (-1 == ret)
    {
        LogPrintf("KING_SpiInit() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    ret = KING_SpiOpen(SPI_ID);
    if (-1 == ret)
    {
        LogPrintf("KING_SpiOpen() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    memset(&spiCfg, 0x00, sizeof(SPI_CONFIG_S));
    spiCfg.freq = 10000000;
    spiCfg.mode = SPI_CPOL0_CPHA0;
    spiCfg.tx_bit_length = SPI_BIT_LEN8;
    ret = KING_SpiConfig(SPI_ID, &spiCfg);
    if (-1 == ret)
    {
        LogPrintf("KING_SpiConfig() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }    
    
    uint8 enable = 0x06; 
    //使能
    ret = KING_SpiWrite(1,&enable,1); 
    if (-1 == ret)
    {
        LogPrintf("KING_SpiWrite() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    LogPrintf("SpiWrite Enable %d\r\n",ret);
    
    
    //清空
    memset(cmd_addr,0x00,sizeof(cmd_addr));
    cmd_addr[0] = 0x20;
    cmd_addr[1] = 0x00;
    cmd_addr[2] = 0x00;
    cmd_addr[3] = 0x00;
    ret = KING_SpiWrite(1,cmd_addr,4); 
    if (-1 == ret)
    {
        LogPrintf("KING_SpiWrite() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    LogPrintf("SpiWrite Erase %d\r\n",ret);
    KING_Sleep(500);    
    
    //使能
    ret = KING_SpiWrite(1,&enable,1); 
    if (-1 == ret)
    {
        LogPrintf("KING_SpiWrite() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    LogPrintf("SpiWrite Enable %d\r\n",ret);

    //写
    memset(buff,0x00,sizeof(buff));
    buff[0] = 0x02;
    buff[1] = 0x00;
    buff[2] = 0x00;
    buff[3] = 0x00;
    int i;
    LogPrintf("write  ");
    for(i = 0; i < 256;i++)
    {
        buff[i + 4] = i;
        LogPrintf("%x  ", buff[i + 4]);
    }
    
    ret = KING_SpiWrite(SPI_ID, buff,260);
    if (-1 == ret)
    {
        LogPrintf("KING_SpiWrite() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    LogPrintf("SpiWrite %d\r\n",ret-4);
    KING_Sleep(500);    
    
    memset(buff,0x00,sizeof(buff));
    memset(cmd_addr,0x00,sizeof(cmd_addr));
    cmd_addr[0] = 0x03;
    cmd_addr[1] = 0x00;
    cmd_addr[2] = 0x00;
    cmd_addr[3] = 0x00;
    ret = KING_SpiRead(SPI_ID, cmd_addr, 4, buff, 256);
    if (-1 == ret)
    {
        LogPrintf("KING_SpiRead() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    LogPrintf("read len:%d\r\n",ret);

    LogPrintf("read  ");
    for(i = 0; i < ret;i++)
    {
        LogPrintf("%x  ", buff[i]);
    }    
    
    ret = KING_SpiClose(SPI_ID);
    if (FAIL == ret)
    {
        LogPrintf("KING_SpiClose errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    LogPrintf("\r\n");
    
    ret = KING_SpiDeinit();
    if (FAIL == ret)
    {
        LogPrintf("KING_SpiFlashDeinit errcode=0x%x\r\n", KING_GetLastErrCode());
    }
   
}


