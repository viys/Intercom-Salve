#include "app_main.h"
#include "uart_init.h"
#include "at.h"


#define KXTJ2_DEVICE_ADDR       0x0E
#define KXTJ2_CTRL_REG1         0x1B
#define KXTJ2_CHIP_ID           0x0F// default WHO_AM_I value is 0x09

/**
 *    iic¶ÁÐ´
 *    
 *    @param[in]  data  
 *    @param[in]  dataLen 
 *    @return 0 SUCCESS  -1 FAIL
 */

int AtIic(char* data, uint32 dataLen)
{
    int ret = SUCCESS;
    uint8 reg_addr = 0;
    uint8 buffer[3];
    uint32 i2c_freq;
    uint8 rwflag;
    I2C_DEV_CONFIG_S i2cCfg;

    char *pfreq;
    char *prwflag;
    char *pend;
    
    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    data++;

    if (*data == '?')
    {
        ATPrintf("+IIC: (0-1),(0-1)\r\n");
        return SUCCESS;
    }
    
    prwflag = strtok(data,",");
    pfreq = strtok(NULL,",");
    pend = strtok(NULL,",");
    if(NULL == prwflag ||NULL == pfreq || NULL != pend)
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }
    rwflag = atoi(prwflag);
    i2c_freq = (uint32)atoi(pfreq);
    if(i2c_freq > 1 || rwflag > 1)
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }
    
    //IIC open
    ret = KING_IicOpen(I2C_BUS_2);
    if (FAIL == ret)
    {
       LogPrintf("KING_IicOpen() errcode=0x%x\r\n", KING_GetLastErrCode());
       return FAIL;
    }


    //IIC config set
    memset(&i2cCfg,0x00,sizeof(I2C_DEV_CONFIG_S));
    i2cCfg.bus = I2C_BUS_2;
    i2cCfg.slave_addr = KXTJ2_DEVICE_ADDR;
    i2cCfg.cmd = I2C_DEV_FREQ_SET;
    i2cCfg.data = &i2c_freq;
    ret = KING_IicConfig(&i2cCfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_IicConfig() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }

    if(0 == rwflag)
    {
        reg_addr = KXTJ2_CHIP_ID;
        memset(buffer, 0x00, 3);
        ret = KING_IicRead(I2C_BUS_2,  KXTJ2_DEVICE_ADDR, &reg_addr, 1, buffer, 1);
        if (-1 == ret)
        {
            LogPrintf("KING_IicRead() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
            goto EXIT;
           
        }
        ATPrintf("Read KXTJ2_CHIP_ID value is 0x%X.\r\n", buffer[0]);
        if(0x09 != buffer[0])
        {
            LogPrintf("read error\r\n");
            goto EXIT;
        }
    }
    else
    {
        LogPrintf("no write.\r\n");
    }
    ret = SUCCESS;
EXIT:
    KING_IicClose(I2C_BUS_2);
    return ret;
}
