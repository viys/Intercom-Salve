/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "app_main.h"
#include "KingFileSystem.h"
#include "kingcstd.h"
#include "kingplat.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("fs: "fmt, ##args); } while(0)

#define FS_TEST_DIR1    "/app"
#define FS_TEST_DIR2    "/app/test"
#define FS_TEST_DIR3    "/app/test1"

#define FS_TEST_FILE1   "/app/cheerzing1.txt"
#define FS_TEST_FILE2   "/app/cheerzing2.txt"
#define FS_TEST_FILE3   "/app/cheerzing3.txt"

/*******************************************************************************
 **  Function 
 ******************************************************************************/
//open write read close
void FS_FirstTest()
{
    FS_HANDLE FileHandle = NULL;
    int ret;
    uint32 size;
    uint8 buf[255] = {0};

    ret = KING_FsDirectoryCreate(FS_TEST_DIR1);
    LogPrintf("KING_FsDirectoryCreate %s, ret is %d\r\n", FS_TEST_DIR1, ret);


    ret = KING_FsFileCreate(FS_TEST_FILE1, FS_MODE_CREATE_NEW | FS_MODE_WRITE | FS_MODE_READ, 0, 0, &FileHandle);
    if (ret == -1)
    {
        LogPrintf("creat new file fail errorcode: 0x%X\r\n", KING_GetLastErrCode());
        return;
    }
    else
    {
        LogPrintf("open existing ret is %d, fileHandle£»%d\r\n", ret, FileHandle);
    }

    ret = KING_FsFileWrite(FileHandle, (uint8*)"1234567890", 10, &size);
    if(ret == -1)
    {
        LogPrintf("write file fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("write size %d, ret %d\r\n", size, ret);
    }

    ret = KING_FsFileSizeGet(FileHandle, &size);
    if(ret == -1)
    {
        LogPrintf("get file size fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("get file size %d , ret %d\r\n", size, ret);
    }

    ret = KING_FsFilePointerSet(FileHandle, 0, FS_SEEK_BEGIN);
    if(ret == -1)
    {
        LogPrintf("pointer to begin errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("pointer to begin ret %d\r\n", ret);
    }

    ret = KING_FsFileRead(FileHandle, buf, 20, &size);
    if(ret == -1)
    {
        LogPrintf("read file errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("read file content: %s, size: %d\r\n", (char *)buf, size);
    }

    ret = KING_FsCloseHandle(FileHandle);
    if(ret == -1)
    {
        LogPrintf("close file errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("close ret is %d\r\n", ret);
    }
}

//open type
void FS_SecondTest()
{
    FS_HANDLE FileHandle = NULL;
    int ret, curPos = 0;
    uint32 size;
    uint8 buf[255] = {0};

    LogPrintf("FS_test 2\r\n");

    //create always
    ret = KING_FsFileCreate(FS_TEST_FILE1, FS_MODE_CREATE_ALWAYS | FS_MODE_WRITE | FS_MODE_READ, 0, 0, &FileHandle);
    LogPrintf("open existing file ret is %d\r\n", ret);

    if (-1 == ret || NULL == FileHandle)
    {
        return;
    }

    ret = KING_FsFileSizeGet(FileHandle, &size);
    LogPrintf("get file size %d, ret %d\r\n", size, ret);

    ret = KING_FsFileWrite(FileHandle, (uint8*)"2345678901", 10, &size);
    LogPrintf("write size %d, ret %d\r\n", size, ret);

    ret = KING_FsFileSizeGet(FileHandle, &size);
    LogPrintf("get file size %d, ret %d\r\n", size, ret);

    ret = KING_FsFilePointerSet(FileHandle, 0, FS_SEEK_BEGIN);
    LogPrintf("Set seek to begin ret %d\r\n", ret);

    ret = KING_FsFileRead(FileHandle, buf, 60, &size);
    LogPrintf("read ret %d, size %d, content: %s\r\n", ret, size, (char *)buf);

    ret = KING_FsCloseHandle(FileHandle);
    LogPrintf("close ret is %d\r\n", ret);

    //create and pointer to end
    ret = KING_FsFileCreate(FS_TEST_FILE1, FS_MODE_APPEND | FS_MODE_WRITE | FS_MODE_READ, 0, 0, &FileHandle);
    LogPrintf("open append ret is %d\r\n", ret);

    ret = KING_FsFileSizeGet(FileHandle, &size);
    LogPrintf("get size %d ,ret %d\r\n", size, ret);

    ret = KING_FsFileWrite(FileHandle, (uint8*)"abcdefghijklmnopqrst", 20, &size);
    LogPrintf("append write end %d, ret %d\r\n", size, ret);

    ret = KING_FsFileSizeGet(FileHandle, &size);
    LogPrintf("get file size %d, ret %d\r\n", size, ret);

    ret = KING_FsFilePointerSet(FileHandle, 0, FS_SEEK_BEGIN);
    LogPrintf("set seek to begin ret %d\r\n", ret);

    ret = KING_FsFileRead(FileHandle, buf, 100, &size);
    LogPrintf("read ret %d, size %d, content: %s\r\n", ret, size, (char *)buf);

    ret = KING_FsCloseHandle(FileHandle);
    LogPrintf("close ret is %d\r\n", ret);

    //open exist
    ret = KING_FsFileCreate(FS_TEST_FILE1, FS_MODE_OPEN_EXISTING | FS_MODE_WRITE | FS_MODE_READ, 0, 0,&FileHandle);
    LogPrintf("open exist ret is %d\r\n", ret);

    ret = KING_FsFileSizeGet(FileHandle, &size);
    LogPrintf("get size %d, ret %d\r\n", size, ret);

    ret = KING_FsFileWrite(FileHandle, (uint8*)"uvwxyz", 6, &size);
    LogPrintf("write size %d, ret %d\r\n", size, ret);

    ret = KING_FsFilePointerSet(FileHandle, 0, FS_SEEK_BEGIN);
    LogPrintf("set seek to begin ret %d\r\n", ret);

    ret = KING_FsFileSizeGet(FileHandle, &size);
    LogPrintf("get file size %d, ret %d\r\n", size, ret);

    ret = KING_FsFileRead(FileHandle, buf, 100, &size);
    LogPrintf("read ret %d size %d, content: %s\r\n", ret, size, (char *)buf);

    ret = KING_FsCloseHandle(FileHandle);
    LogPrintf("close ret is %d\r\n", ret);

    FileHandle = NULL;

    //open not-exist
    ret = KING_FsFileCreate(FS_TEST_FILE3, FS_MODE_OPEN_EXISTING | FS_MODE_WRITE | FS_MODE_READ, 0, 0, &FileHandle);

    if (-1 == ret || NULL == FileHandle)
    {
        LogPrintf("Fail to open not existing file. Error code %X\r\n", KING_GetLastErrCode());
    }
    else
    {
        ret = KING_FsCloseHandle(FileHandle);
        LogPrintf("close ret is %d\r\n", ret);
    }

    //open exist
    ret = KING_FsFileCreate(FS_TEST_FILE3, FS_MODE_CREATE_ALWAYS | FS_MODE_WRITE | FS_MODE_READ, 0, 0, &FileHandle);
    LogPrintf("create cheerzing3  ret is %d\r\n", ret);

    ret = KING_FsFileSizeGet(FileHandle, &size);
    LogPrintf("get size %d ,ret %d\r\n", size, ret);

    ret = KING_FsFileWrite(FileHandle, (uint8*)"uvwxyz", 6, &size);
    LogPrintf("write begin %d ,ret %d\r\n", size, ret);

    ret = KING_FsFilePointerSet(FileHandle, 0, FS_SEEK_BEGIN);
    LogPrintf("get to begin ret %d\r\n", ret);

    ret = KING_FsFileSizeGet(FileHandle, &size);
    LogPrintf("get 2 size %d ,ret %d\r\n", size, ret);

    memset(buf, 0x00, sizeof(buf));

    ret = KING_FsFileRead(FileHandle, buf, 100, &size);
    LogPrintf("read ret %d size %d:%s\r\n", ret, size, buf);

    ret = KING_FsCloseHandle(FileHandle);
    LogPrintf("close ret is %d\r\n", ret);

    //open exist
    ret = KING_FsFileCreate(FS_TEST_FILE3, FS_MODE_OPEN_EXISTING | FS_MODE_WRITE | FS_MODE_READ, 0, 0, &FileHandle);
    LogPrintf("open cheerzing3 exist ret is %d\r\n", ret);

    ret = KING_FsFileSizeGet(FileHandle, &size);
    LogPrintf("get size %d ,ret %d\r\n", size, ret);

    ret = KING_FsFileWrite(FileHandle, (uint8*)"uvwxyz\r\n", 6, &size);
    LogPrintf("write begin %d ,ret %d\r\n", size, ret);
    
    ret = KING_FsFileBuffersFlush(FileHandle);
    LogPrintf("file flush %d ,ret %d\r\n", size, ret);
    
    ret = KING_FsFilePointerSet(FileHandle, 0, FS_SEEK_BEGIN);
    LogPrintf("set seek to begin ret %d\r\n", ret);

    ret = KING_FsFileSizeGet(FileHandle, &size);
    LogPrintf("get file size %d ,ret %d\r\n", size, ret);

    memset(buf, 0x00, sizeof(buf));

    ret = KING_FsFileRead(FileHandle, buf, 100, &size);
    LogPrintf("read ret %d size %d:%s\r\n", ret, size, buf);

    //not support
    ret = KING_FsFilePointerGet(FileHandle, size, &curPos);
    LogPrintf("get pointer ret %d curPos %d\r\n", ret, curPos);
    
    ret = KING_FsCloseHandle(FileHandle);
    LogPrintf("close ret is %d\r\n", ret);
}

//create find rename find
void FS_ThirdTest()
{
    FS_HANDLE FileHandle = NULL;
    int ret;
    uint32 size, freeSize = 0, usedSize = 0;
    uint8 buf[255] = {0};
    FS_FIND_DATA_S FindData;

    LogPrintf("FS_test 3\r\n`");

    ret = KING_FsFileRename(FS_TEST_FILE1, FS_TEST_FILE2);

    LogPrintf("rename ret is %d\r\n",ret);

    ret = KING_FsFileCreate(FS_TEST_FILE1, FS_MODE_CREATE_ALWAYS, 0, 0, &FileHandle);
    LogPrintf("create new file ret is %d\r\n",ret);

    if (-1 == ret || NULL == FileHandle)
    {
        return;
    }

    ret = KING_FsFileRead(FileHandle, buf, 100, &size);
    LogPrintf("read ret %d, size %d, content: %s\r\n", ret, size, (char *)buf);


    ret = KING_FsCloseHandle(FileHandle);
    LogPrintf("close ret is %d\r\n",ret);


    ret = KING_FsFileCreate(FS_TEST_FILE2, FS_MODE_OPEN_EXISTING | FS_MODE_READ, 0, 0, &FileHandle);
    LogPrintf("open existing ret is %d\r\n",ret);

    if (-1 == ret || NULL == FileHandle)
    {
        return;
    }

    ret = KING_FsFileRead(FileHandle, buf, 100, &size);
    LogPrintf("read ret %d  size %d:%s\r\n", ret, size, buf);


    ret = KING_FsCloseHandle(FileHandle);
    LogPrintf("close ret is %d\r\n", ret);


    ret = KING_FsDirectoryCreate(FS_TEST_DIR2);
    LogPrintf(" KING_FsDirectoryCreate is %d\r\n",ret);


    ret = KING_FsFileCreate("/app/test/cheerzing2.txt", FS_MODE_CREATE_ALWAYS | FS_MODE_WRITE | FS_MODE_READ, 0, 0, &FileHandle);
    LogPrintf("create new file ret is %d\r\n",ret);

    if (-1 == ret || NULL == FileHandle)
    {
        return;
    }

    ret = KING_FsFileWrite(FileHandle, (uint8*)"33333", 5, &size);
    LogPrintf(" write end %d ,ret %d\r\n", size, ret);

    ret = KING_FsCloseHandle(FileHandle);
    LogPrintf("close ret is %d\r\n", ret);

    memset(&FindData, 0x00, sizeof(FS_FIND_DATA_S));

    ret = KING_FsFindFirstFile("cheerzing2.txt", &FindData, &FileHandle);
    LogPrintf("find first FileHandle is %d, name %s, size %d\r\n", ret, FindData.name, FindData.length);


    ret = KING_FsFindNextFile(FileHandle, &FindData);
    LogPrintf("find next FileHandle is %d, name %s, size %d\r\n", ret, FindData.name, FindData.length);

    ret = KING_FsFindFileClose(FileHandle);
    LogPrintf("find close ret is %d\r\n", ret);
    
    ret = KING_FsFreeSpaceGet(STORAGE_TYPE_INTERNAL, &freeSize);
    LogPrintf("get space  STORAGE_TYPE_INTERNAL ret is %d,freeSize=%u\r\n", ret, freeSize);

    freeSize = 0;
    ret = KING_FsFreeSpaceGet(STORAGE_TYPE_SD, &freeSize);
    LogPrintf("get space  STORAGE_TYPE_SD ret is %d,freeSize=%u\r\n", ret, freeSize);

    freeSize = 0;
    ret = KING_FsFreeSpaceGet(STORAGE_TYPE_FLASH, &freeSize);
    LogPrintf("get space  STORAGE_TYPE_FLASH ret is %d,freeSize=%u\r\n", ret, freeSize);

    ret = KING_FsUsedSpaceGet(STORAGE_TYPE_INTERNAL, &usedSize);
    LogPrintf("get used  STORAGE_TYPE_INTERNAL ret is %d,freeSize=%u\r\n", ret, usedSize);

    usedSize = 0;
    ret = KING_FsUsedSpaceGet(STORAGE_TYPE_SD, &usedSize);
    LogPrintf("get used  STORAGE_TYPE_SD ret is %d,usedSize=%u\r\n", ret, usedSize);

    usedSize = 0;
    ret = KING_FsUsedSpaceGet(STORAGE_TYPE_FLASH, &freeSize);
    LogPrintf("get used  STORAGE_TYPE_FLASH ret is %d,usedSize=%u\r\n", ret, usedSize);

    ret = KING_FsDirectoryCreate(FS_TEST_DIR3);
    LogPrintf("create directory ret is %d\r\n", ret);

    ret = KING_FsDirectoryDelete(FS_TEST_DIR3);
    LogPrintf("delete directory ret is %d\r\n", ret);  

    ret = KING_FsFileCreate("/app/test/cheerzing7.txt", FS_MODE_CREATE_ALWAYS | FS_MODE_WRITE | FS_MODE_READ, 0, 0, &FileHandle);
    LogPrintf("create new file7 ret is %d\r\n",ret);
    if (-1 == ret || NULL == FileHandle)
    {
        return;
    }
    ret = KING_FsFileWrite(FileHandle, (uint8*)"456789654", 9, &size);
    LogPrintf(" write end %d ,ret %d\r\n", size, ret);
    
    ret = KING_FsCloseHandle(FileHandle);
    LogPrintf("find close file 7 ret is %d\r\n", ret);
    
    memset(&FindData, 0x00, sizeof(FS_FIND_DATA_S));
    ret = KING_FsFindFirstFile("cheerzing7.txt", &FindData, &FileHandle);
    LogPrintf("find first FileHandle is %d, name %s, size %d\r\n", ret, FindData.name, FindData.length);

    memset(&FindData, 0x00, sizeof(FS_FIND_DATA_S));
    //not support
    ret = KING_FsFindPreFile(FileHandle, &FindData);
    LogPrintf("find pre FileHandle is %d, name %s, size %d\r\n", ret, FindData.name, FindData.length);

    ret = KING_FsFindFileClose(FileHandle);
    LogPrintf("find close ret is %d\r\n", ret);
    
    ret = KING_FsFileDelete("/app/test/cheerzing7.txt");
    LogPrintf("delete new file7 ret is %d\r\n",ret);
}

void FS_test(int type)
{
    LogPrintf("FS_test\r\n");

    KING_FsInit();

    switch (type)
    {
    case 1:
        FS_FirstTest();
        break;

    case 2:
        FS_SecondTest();
        break;

    case 3:
        FS_ThirdTest();
        break;

    default:
        break;
    }

    KING_FsDeInit();
}



