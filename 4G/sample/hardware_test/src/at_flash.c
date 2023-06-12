#include "app_main.h"
#include "uart_init.h"
#include "at.h"

#define FS_TEST_FLASH_FILE1   FS_EXT_FLASH_DIR "/cheerzing1.txt"

static int gFlashInit = 0;

/**
 *    spiflash 初始化
 *
 */

static void SpiFlash_Init(void)
{
    if (0 == gFlashInit)
    {
        int ret = SUCCESS;
        ret = KING_SpiFlashInit(FLASH_EXTERN);
        if (FAIL == ret)
        {
            LogPrintf("KING_SpiFlashInit() errcode=0x%x\r\n", KING_GetLastErrCode());
        }
        else
        {
            ret = KING_FSMount(STORAGE_TYPE_FLASH, 0x8000, 0x100000, 0x8000, 0x200);
            if (FAIL == ret)
            {
                LogPrintf("KING_FSMount(extFLASH) errcode=0x%x\r\n", KING_GetLastErrCode());
            }
            else
            {
                gFlashInit = 1;
            }
        }
    }
}


/**
 *    spi flash 读取
 *    
 *    @param[in]  data  
 *    @param[in]  dataLen 
 *    @return 0 SUCCESS  -1 FAIL
 */

int AtSpiFlash(char* data, uint32 dataLen)
{
    int ret = 0;
    uint8 buff_w[10] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    uint8 buff_r[10];
    int i;
    uint8 rwflag;
    char *prwflag;
    char *pend;

    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }

    SpiFlash_Init();

    data++;

    if (*data == '?')
    {
        ATPrintf("+SPIFLASH: (0-1)\r\n");
        return SUCCESS;
    }
    
    prwflag = strtok(data,",");
    pend = strtok(NULL,",");
    if(NULL == prwflag || NULL != pend)
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }

    //读写
    rwflag = atoi(prwflag);
    if(rwflag > 1)
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }
    
    if(0 == rwflag)
    {
        //spi flash read
        memset(buff_r,0x00,sizeof(buff_r));
        ret = KING_SpiFlashRead(0x00, 10, buff_r);
        if (FAIL == ret)
        {
            LogPrintf("KING_SpiFlashRead() errcode=0x%x\r\n", KING_GetLastErrCode());
            return FAIL;
        }

        ATPrintf("read len: %d data:",ret);
        for(i = 0; i < 10;i++)
        {
            ATPrintf("%x  ", buff_r[i]);
        }
        ATPrintf("\r\n");
        if(memcmp(buff_r, buff_w, 10))
        {
            LogPrintf("read error\r\n");
            return FAIL;
        }
    }
    else
    {
        //spi flash write
        ret = KING_SpiFlashErase(0x00,4*1024);
        if (FAIL == ret)
        {
            LogPrintf("KING_SpiFlashErase() errcode=0x%x\r\n", KING_GetLastErrCode());
            return FAIL;
        }
        
        KING_Sleep(75);
        
        ret = KING_SpiFlashWrite(0x00, 10, buff_w);
        if (FAIL == ret)
        {
            LogPrintf("KING_SpiFlashWrite() errcode=0x%x\r\n", KING_GetLastErrCode());
            return FAIL;
        }
        //spi flash write
        LogPrintf("write len: %d write data: ", ret);
        for(i = 0; i < 10;i++)
        {
            LogPrintf("%x  ", buff_w[i]);
        }
        LogPrintf("\r\n");
    }
    return SUCCESS;
}

/**
 *    外部FLASH文件系统读写
 *    
 *    @param[in]  data  
 *    @param[in]  dataLen 数据长度 
 *    @return 0 SUCCESS  -1 FAIL
 */
int AtSpiFs(char* data, uint32 dataLen)
{

    char *prwflag;
    char *pend;
    uint8 rwflag;

    int ret = SUCCESS;
    FS_HANDLE FileHandle = NULL;
    uint32 size;
    uint8 buf[255] = {0};

    //判断是否有参数
    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    SpiFlash_Init();
    data++;

    if (*data == '?')
    {
        ATPrintf("+SPIFS: (0-1)\r\n");
        return SUCCESS;
    }

    prwflag = strtok(data,",");
    pend = strtok(NULL,",");
    if(NULL == prwflag || NULL != pend)
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }
    rwflag = atoi(prwflag);
    if(rwflag > 1)
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }
    
    if(0 == rwflag)
    {
        //打开文件
        ret = KING_FsFileCreate(FS_TEST_FLASH_FILE1, FS_MODE_OPEN_EXISTING | FS_MODE_WRITE | FS_MODE_READ, 0, 0, &FileHandle);
        if (FAIL == ret)
        {
            LogPrintf("KING_FsFileCreate fail errorcode: 0x%X\r\n", KING_GetLastErrCode());
            return FAIL;
        }
        
        LogPrintf("open existing fileHandle；%d\r\n", FileHandle);
        //文件读
        ret = KING_FsFileRead(FileHandle, buf, 20, &size);
        if (FAIL == ret)
        {
            LogPrintf("read file errorcode: 0x%x\r\n", KING_GetLastErrCode());
            goto EXIT;
        }
        LogPrintf("read size: %d\r\n", size);
        ATPrintf("read %s\r\n", buf);
    }
    else
    {
         //文件不存在创建完后打开文件；文件存在则打开文件
        ret = KING_FsFileCreate(FS_TEST_FLASH_FILE1, FS_MODE_CREATE_ALWAYS | FS_MODE_WRITE | FS_MODE_READ, 0, 0, &FileHandle);
        if (FAIL == ret)
        {
            LogPrintf("KING_FsFileCreate fail errorcode: 0x%X\r\n", KING_GetLastErrCode());
            return FAIL;
        }
        LogPrintf("open existing fileHandle；%d\r\n", FileHandle);
        //写文件
        ret = KING_FsFileWrite(FileHandle, (uint8*)"1234567890", 10, &size);
        if (FAIL == ret)
        {
            LogPrintf("write file fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
            goto EXIT;
        }
        LogPrintf("write size %d\r\n", size);
        LogPrintf("write 1234567890\r\n");
    }
    
    ret = SUCCESS;
EXIT:
    //关闭文件
    KING_FsCloseHandle(FileHandle);
    return ret;
}


