#include "app_main.h"
#include "uart_init.h"
#include "at.h"


#define SPI_ID      1 
#define SPI_FREQ    1000

/**
 *    SPI读取
 *    
 *    @param[in]  data  
 *    @param[in]  dataLen 数据长度 
 *    @return 0 SUCCESS  -1 FAIL
 */

int AtSpi(char* data, uint32 dataLen)
{
    int ret = SUCCESS;
    char *prwflag;
    char *pfreq;
    char *pend;
    uint8 rwflag;
    uint32 freq;
    SPI_CONFIG_S spiCfg;
    uint8 cmd_addr[4];
    uint8 buff[20];
    uint8 buff_r[20];
    uint8 buff_w[20] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    uint8 enable = 0x06;
    int i;
    //判断是否有参数
    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    data++;

    if (*data == '?')
    {
        ATPrintf("+SPI: (0-1),(1-103000)\r\n");
        return SUCCESS;
    }

    prwflag = strtok(data,",");
    pfreq = strtok(NULL,",");
    pend = strtok(NULL,",");
    if(NULL == prwflag || NULL == pfreq || NULL != pend)
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }
    rwflag = atoi(prwflag);
    freq = atoi(pfreq);
    
    if(rwflag > 1)
    {
        LogPrintf("read write param error\r\n");
        return FAIL;
    }
    if(freq < 1 || freq >103000)
    {
        LogPrintf("freq param error\r\n");
        return FAIL;
    }
    //打开spi
    ret = KING_SpiOpen(SPI_ID);
    if (FAIL == ret)
    {
        LogPrintf("KING_SpiOpen() errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }

    //spi配置
    memset(&spiCfg, 0x00, sizeof(SPI_CONFIG_S));
    spiCfg.freq = freq * SPI_FREQ;
    spiCfg.mode = SPI_CPOL0_CPHA0;
    spiCfg.tx_bit_length = SPI_BIT_LEN8;
    ret = KING_SpiConfig(SPI_ID, &spiCfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_SpiConfig() errcode=0x%x\r\n", KING_GetLastErrCode());
        goto EXIT;
    }
    LogPrintf("freq = %d spiCfg.freq: %d\r\n", freq, spiCfg.freq);
    KING_Sleep(50);
    if(rwflag == 0)
    {

        //读
        memset(buff_r,0x00,sizeof(buff_r));
        memset(cmd_addr,0x00,sizeof(cmd_addr));
        cmd_addr[0] = 0x03;
        cmd_addr[1] = 0x00;
        cmd_addr[2] = 0x01;
        cmd_addr[3] = 0x00;
        ret = KING_SpiRead(SPI_ID, cmd_addr, 4, buff_r, 10);
        if (-1 == ret)
        {
            LogPrintf("KING_SpiRead() errcode=0x%x\r\n", KING_GetLastErrCode());
            goto EXIT;
        }
        LogPrintf("spi read len:%d\r\n",ret);
        ATPrintf("read  ");
        for(i = 0; i < ret;i++)
        {
            ATPrintf("0x%x  ", buff_r[i]);
        }
        ATPrintf("\r\n");
        if(memcmp(buff_r, buff_w, 10))
        {
            LogPrintf("read error!\r\n");
            ret = FAIL;
            goto EXIT;
        }
    }
    else
    {

        enable = 0x06;
        //写
        ret = KING_SpiWrite(1,&enable,1); //使能
        if (FAIL == ret)
        {
            LogPrintf("KING_SpiWrite() errcode=0x%x\r\n", KING_GetLastErrCode());
            goto EXIT;
        }
        
        //清空
        memset(cmd_addr,0x00,sizeof(cmd_addr));
        cmd_addr[0] = 0x20;
        cmd_addr[1] = 0x00;
        cmd_addr[2] = 0x00;
        cmd_addr[3] = 0x00;
        ret = KING_SpiWrite(1,cmd_addr,4);
        if (FAIL == ret)
        {
            LogPrintf("KING_SpiWrite() errcode=0x%x\r\n", KING_GetLastErrCode());
            goto EXIT;
        }
        LogPrintf("SpiWrite Erase %d\r\n",ret);
        KING_Sleep(100);
        //使能
        ret = KING_SpiWrite(1,&enable,1);
        if (FAIL == ret)
        {
            LogPrintf("KING_SpiWrite() errcode=0x%x\r\n", KING_GetLastErrCode());
            goto EXIT;
        }
        
        //写
        memset(buff,0x00,sizeof(buff));
        buff[0] = 0x02;
        buff[1] = 0x00;
        buff[2] = 0x01;
        buff[3] = 0x00;
        memcpy(&buff[4], buff_w, 10);
        LogPrintf("write  ");
        for(i = 0; i < 10;i++)
        {
            LogPrintf("%x  ", buff[i + 4]);
        }
        LogPrintf("\r\n");
        
        ret = KING_SpiWrite(SPI_ID, buff,14);
        if (-1 == ret)
        {
            LogPrintf("KING_SpiWrite() errcode=0x%x\r\n", KING_GetLastErrCode());
            goto EXIT;
        }
        LogPrintf("spi write len:%d\r\n",ret-4);
        
    }
    ret = SUCCESS;
EXIT:
    
    KING_SpiClose(SPI_ID);
    return ret;
}
