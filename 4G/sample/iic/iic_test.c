/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "KingCSTD.h"
#include "KingPlat.h"
#include "KingRtos.h"
#include "KingIIC.h"
#include "app_main.h"

/*******************************************************************************
 ** MACROS
 ******************************************************************************/
#define LogPrintf(fmt, args...)    do { KING_SysLog("iic: "fmt, ##args); } while(0) 
#define KXTJ2_DEVICE_ADDR       0x0E
#define KXTJ2_CHIP_ID           0x0F // default WHO_AM_I value is 0x09
#define KXTJ2_CTRL_REG1         0x1B


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
 *  iic 测试
 *  Cheerzing EVB has one KIONIX KXTJ2-1009 gsensor.
 */  
void Iic_Test(void)
{
    int ret = -1;
    uint8 reg_addr = 0;
    uint8 buffer[3];
    uint32 i2c_freq;
    I2C_DEV_CONFIG_S i2cCfg;
    LogPrintf("\r\n-----iic test start------\r\n");

    ret = KING_IicInit();
    if (FAIL == ret)
    {
        LogPrintf("KING_IicInit error\r\n");
        return;
    }

    ret = KING_IicOpen(I2C_BUS_2);
    if (FAIL == ret)
    {
        LogPrintf("KING_IicOpen() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    
    memset(&i2cCfg,0x00,sizeof(I2C_DEV_CONFIG_S));
    i2cCfg.bus = I2C_BUS_2;
    i2cCfg.slave_addr = KXTJ2_DEVICE_ADDR;
    i2cCfg.cmd = I2C_DEV_FREQ_SET;
    i2c_freq = I2C_FREQ_FAST;
    i2cCfg.data = &i2c_freq;
    
    ret = KING_IicConfig(&i2cCfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_IicConfig() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    
    memset(&i2cCfg,0x00,sizeof(I2C_DEV_CONFIG_S));
    i2cCfg.bus = I2C_BUS_2;
    i2cCfg.slave_addr = KXTJ2_DEVICE_ADDR;
    i2cCfg.cmd = I2C_DEV_FREQ_GET;
    i2c_freq = 0;
    i2cCfg.data = &i2c_freq;  
    ret = KING_IicConfig(&i2cCfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_IicConfig() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    LogPrintf("I2C dev freq get : %d\r\n",i2c_freq);
    
    /*
     * Read KXTJ2-1009 who_am_i regisger.
     */

    reg_addr = KXTJ2_CHIP_ID;
    memset(buffer, 0x00, 3);
    ret = KING_IicRead(I2C_BUS_2,  KXTJ2_DEVICE_ADDR, &reg_addr, 1, buffer, 1);
    if (FAIL == ret)
    {
        LogPrintf("KING_IicRead() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    LogPrintf("Read KXTJ2_CHIP_ID value is 0x%X.\r\n", buffer[0]);
   

    /*
     * Read/Write KXTJ2-1009 control regisger.
     */
    reg_addr = KXTJ2_CTRL_REG1;
    memset(buffer, 0x00, 3);
    ret = KING_IicRead(I2C_BUS_2, KXTJ2_DEVICE_ADDR, &reg_addr, 1,buffer, 1);
    if (FAIL == ret)
    {
        LogPrintf("KING_IicRead() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    LogPrintf("Read KXTJ2_CTRL_REG1 value is 0x%X\r\n", buffer[0]);

    buffer[0] = 0xC2;
    LogPrintf("Write 0x%X value to KXTJ2_CTRL_REG1...\r\n", buffer[0]);
    ret = KING_IicWrite(I2C_BUS_2, KXTJ2_DEVICE_ADDR, &reg_addr, buffer, 1);
    if (FAIL == ret)
    {
        LogPrintf("KING_IicWrite() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }

    memset(buffer, 0x00, 3);
    ret = KING_IicRead(I2C_BUS_2, KXTJ2_DEVICE_ADDR, &reg_addr, 1,buffer, 1);
    if (FAIL == ret)
    {
        LogPrintf("KING_IicRead() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    LogPrintf("Read KXTJ2_CTRL_REG1 value is 0x%X\r\n", buffer[0]);

    ret = KING_IicClose(I2C_BUS_2);
    if (FAIL == ret)
    {
        LogPrintf("KING_IicClose error\r\n");
    }

    ret = KING_IicDeinit();
    if (FAIL == ret)
    {
        LogPrintf("KING_IicDeinit error\r\n");
    }
}


