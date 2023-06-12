#include "app_main.h"
#include "uart_init.h"
#include "at.h"

#define FS_TEST_SD_FILE1      FS_SD_DIR "/cheerzing1.txt"

/**
 *    sd卡读取
 *    
 *    @param[in]  data  
 *    @param[in]  dataLen 数据长度 
 *    @return 0 SUCCESS  -1 FAIL
 */
int AtSd(char* data, uint32 dataLen)
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
    data++;

    if (*data == '?')
    {
        ATPrintf("+SD: (0-1)\r\n");
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
        ret = KING_FsFileCreate(FS_TEST_SD_FILE1, FS_MODE_OPEN_EXISTING | FS_MODE_WRITE | FS_MODE_READ, 0, 0, &FileHandle);
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
        ret = KING_FsFileCreate(FS_TEST_SD_FILE1, FS_MODE_CREATE_ALWAYS | FS_MODE_WRITE | FS_MODE_READ, 0, 0, &FileHandle);
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

