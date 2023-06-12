#ifndef __KING_FILESYSTEM_H__
#define __KING_FILESYSTEM_H__
#include "KingDef.h"

typedef void* FS_HANDLE;

#define FS_SEEK_BEGIN      0  /*Beginning of file */
#define FS_SEEK_CUR        1  /*Current position of file */
#define FS_SEEK_END        2  /*End of file   */

/**------------------------------------------------------------------*
 *     FS operation Mode(bit0-bit4)                                *
 *     only have 6 group can be used:                               *
 *     1  FS_MODE_READ                                             *
 *     2  FS_MODE_WRITE                                            *
 *     3  FS_MODE_READ|FS_MODE_WRITE                              *
 *     4  FS_MODE_SHARE_READ                                       *
 *     5  FS_MODE_SHARE_WRITE                                      *
 *     6  FS_MODE_SHARE_READ|FS_MODE_SHARE_WRITE                  *
 *------------------------------------------------------------------*/
#define FS_MODE_READ             ((uint32)(0x1<<0))
#define FS_MODE_WRITE            ((uint32)(0x1<<1))
#define FS_MODE_SHARE_READ       ((uint32)(0x1<<2))  // Reserve
#define FS_MODE_SHARE_WRITE      ((uint32)(0x1<<3))  // Reserve

/**------------------------------------------------------------------*
 *  FS access Mode(bit4-bit7)                                   *
 *  only have following choice:  
 *  FS_MODE_CREATE_NEW：创建新文件, 若存在创建失败
 *  FS_MODE_CREATE_ALWAYS：文件不存在则创建新文件；文件存在则删除后创建新文件
 *  FS_MODE_OPEN_EXISTING：打开已存在的文件
 *  FS_MODE_OPEN_ALWAYS：  文件不存在创建完后打开文件；文件存在则打开文件
 *  FS_MODE_APPEND： 创建/打开文件, 并且指针在文件末, 写入文件在文末
 *------------------------------------------------------------------*/
#define FS_MODE_CREATE_NEW       ((uint32)(0x1<<4))
#define FS_MODE_CREATE_ALWAYS    ((uint32)(0x2<<4))
#define FS_MODE_OPEN_EXISTING    ((uint32)(0x3<<4))
#define FS_MODE_OPEN_ALWAYS      ((uint32)(0x4<<4))
#define FS_MODE_APPEND           ((uint32)(0x5<<4))

// fat attribute bits
// the value of FS_FIND_DATA_T->attr
#define FS_ATTR_NONE    0x0  /* no attribute bits */
#define FS_ATTR_RO      0x1  /* read-only */
#define FS_ATTR_HIDDEN  0x2  /* hidden */
#define FS_ATTR_SYS     0x4  /* system */
#define FS_ATTR_VOLUME  0x8  /* volume label */    //reserved
#define FS_ATTR_DIR     0x10 /* directory */       //can not used in FS_SetAttr()
#define FS_ATTR_ARCH    0x20 /* archived */

#define FS_MAX_PATH 255

#define FS_ROOT_DIR             "/"
#define FS_SD_DIR               "/mnt/sd"
#define FS_EXT_FLASH_DIR        "/mnt/flash"

/**
 * Find Data Define.
 * The receives information about the found file.
*/
typedef struct
{
    uint8   day;        ///< day of the month - [1,31]
    uint8   mon;        ///< months  - [1,12]
    uint16  year;       ///< years [1980,2109]
} FS_DATE_S;

typedef struct
{
    uint8   sec;        ///< seconds after the minute - [0,59]
    uint8   min;        ///< minutes after the hour - [0,59]
    uint8   hour;       ///< hours since midnight - [0,23]
} FS_TIME_S;

typedef struct
{
    FS_DATE_S    create_Date;
    FS_TIME_S    create_time;
    FS_DATE_S    modify_Date;
    FS_TIME_S    modify_time;
    FS_DATE_S    access_date;
    uint16       attr;                ///< see FS_ATTR defines.
    uint32       length;
    char         name[FS_MAX_PATH+1];
    uint8        short_name[13];
} FS_FIND_DATA_S;

typedef enum {
  STORAGE_TYPE_INTERNAL, //内部文件系统
  STORAGE_TYPE_SD,  //SD卡访问路径 /mnt/sd
  STORAGE_TYPE_FLASH, //外挂FLASH访问路径 /mnt/flash
  STORAGE_TYPE_MAX = 0x7FFFFFFF
} STORAGE_TYPE_E;

/**
 * 文件系统初始化。在调用文件系统操作函数之前首先调用
 * 此函数。如果返回成功才继续调用其它函数。如果返回失败
 * 有可能平台不支持文件系统操作。
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_FsInit( void );

/**
 * 文件系统去初始化。
 * 所有文件相关操作完成之后，调用此函数。
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_FsDeInit( void );

/**
 * 创建目录
 *
 * @param[in]  pPathName     目录名称
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_FsDirectoryCreate( char* pPathName );

/**
 * 删除目录
 *
 * @param[in]  pPathName     目录名称
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_FsDirectoryDelete( char* pPathName );

/**
 * 创建或打开文件
 *
 * @param[in]  pFileName     文件名
 * @param[in]  accessMode    Access Modes and/or Operation Modes. Check FS_MODE defines.
 * @param[in]  share_mode    不支持
 * @param[in]  file_attr     不支持
 * @param[out]  pFileHandle  打开的文件handle
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_FsFileCreate(char* pFileName, uint32 accessMode, uint32 share_mode, uint32 file_attr, FS_HANDLE* pFileHandle);

/**
 * 删除文件
 *
 * @param[in]  pFileName     文件名称
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_FsFileDelete(char* pFileName);

/**
 * 读文件
 *
 * @param[in]  fileHandle      KING_FsFileCreate给出的handle
 * @param[in]  readBuf         给入的buf指针
 * @param[in]  bufLen          给入的buf长度
 * @param[out] bytesRead       实际读出的长度
 * @return 0 SUCCESS(文件找到)  -1 FAIL
 */
int KING_FsFileRead(FS_HANDLE fileHandle, uint8* readBuf, uint32 bufLen, uint32* bytesRead);

/**
 * 写文件
 *
 * @param[in]  fileHandle      KING_FsFileCreate给出的handle
 * @param[in]  writeBuf        要写入的数据buf指针
 * @param[in]  bufLen          要写入的数据长度
 * @param[out] byteswritten    实际写入的长度
 * @return 0 SUCCESS(文件找到)  -1 FAIL
 */
int KING_FsFileWrite(FS_HANDLE fileHandle, uint8* writeBuf, uint32 bufLen, uint32* byteswritten);

/**
 * 文件存盘
 * 将缓冲区内容刷到盘中
 * @param[in]  fileHandle      KING_FsFileCreate给出的handle
 * @return 0 SUCCESS(文件找到)  -1 FAIL
 */
int KING_FsFileBuffersFlush(FS_HANDLE fileHandle);

/**
 * 获取文件大小
 *
 * @param[in]  fileHandle      KING_FsFileCreate给出的handle
 * @param[out] pFileSize       文件大小
 * @return 0 SUCCESS(文件找到)  -1 FAIL
 */
int KING_FsFileSizeGet(FS_HANDLE fileHandle, uint32* pFileSize);

/**
 * 重命名文件
 *
 * @param[in]  sour      原文件名及目录
 * @param[in]  dest      新文件名及目录(目录保持一致)
 * @return 0 SUCCESS(文件找到)  -1 FAIL
 */
int KING_FsFileRename(const char *sour,const char *dest);


/**
 * 获取当前文件操作pointer
 *
 * @param[in]  fileHandle      KING_FsFileCreate给出的handle
 * @param[in]  origin          FS_SEEK_BEGIN、FS_SEEK_END
 * @param[out] currentpos     相对于origin的当前位置
 * @return 0 SUCCESS(文件找到)  -1 FAIL
 */
int KING_FsFilePointerGet(FS_HANDLE fileHandle , uint32 origin, int* currentpos);

/**
 * 偏移当前文件操作pointer
 *
 * @param[in]  fileHandle      KING_FsFileCreate给出的handle
 * @param[in]  offset          相对的origin的偏移量，为负数时往前找
 * @param[in] origin          FS_SEEK_BEGIN、FS_SEEK_CUR、FS_SEEK_END
 * @return 0 SUCCESS(文件找到)  -1 FAIL
 */
int KING_FsFilePointerSet(FS_HANDLE fileHandle , int32 offset, uint32 origin);

/**
 * 关闭文件
 *
 * @param[in]  fileHandle      KING_FsFileCreate给出的handle
 * @return 0 SUCCESS(文件找到)  -1 FAIL
 */
int KING_FsCloseHandle(FS_HANDLE fileHandle);

/**
 * 查找文件
 *
 * @param[in]  pFileName 查找的文件名或类型，需要带路径：如"/mnt/sd/1.mp3","/ *.3gp",or "/123.mp3"
 *                       内部文件系统根目录为"/"，SD卡根目录/mnt/sd，外挂FLASH根目录/mnt/flash
 * @param[out]  pFindFileData    如果找到匹配的文件，通过这个参数上报文件信息
 * @param[out]  pFindFile        0-未找到匹配文件   非0-找到匹配文件 且后续可用pFindFile继续找下个匹配文件
 * @return 0 SUCCESS  -1 FAIL 大于0时返回找到的文件handle
 */
int KING_FsFindFirstFile(char* pFileName, FS_FIND_DATA_S* pFindFileData, FS_HANDLE* pFindFile);

/**
 * 继续查找下一个文件
 *
 * @param[in]  findFile          KING_FsFindFirstFile给出的handle
 * @param[out]  pFindFileData    如果找到匹配的文件，通过这个参数上报文件信息
 * @return 0 SUCCESS  -1 FAIL 大于0时返回找到的文件handle
 */
int KING_FsFindNextFile(FS_HANDLE findFile, FS_FIND_DATA_S* pFindFileData);

/**
 * 继续查找上一个文件
 *
 * @param[in]  findFile          KING_FsFindFirstFile给出的handle
 * @param[out]  pFindFileData    如果找到匹配的文件，通过这个参数上报文件信息
 * @return 0 SUCCESS  -1 FAIL 大于0时返回找到的文件handle
 */
int KING_FsFindPreFile(FS_HANDLE findFile, FS_FIND_DATA_S* pFindFileData);

/**
 * 关闭查找
 *
 * @param[in]  findFile          KING_FsFindFirstFile给出的handle
 * @return 0 SUCCESS(文件找到)  -1 FAIL
 */
int KING_FsFindFileClose(FS_HANDLE findFile );

/**
 * 挂载外部存储设备
 * @param[in] type 存储设备类型，不能使用STORAGE_TYPE_INTERNAL
 * @param[in] phys_start 存放文件系统的物理起始地址，本参数只在type为STORAGE_TYPE_FLASH时有效
 * @param[in] phys_size 文件系统占用的物理空间大小，单位为byte，本参数只在type为STORAGE_TYPE_FLASH时有效
 * @param[in] phys_erase_size Flash单次擦除空间大小，单位为byte，本参数只在type为STORAGE_TYPE_FLASH时有效
 * @param[in] block_size 文件系统逻辑块大小，单位为byte，本参数只在type为STORAGE_TYPE_FLASH时有效
 * @return 0 SUCCESS(文件找到)  -1 FAIL
 */
int KING_FSMount(STORAGE_TYPE_E type, uint32 phys_start, uint32 phys_size, uint32 phys_erase_size, uint32 block_size);

/**
 * 卸载外部存储设备
 * @param[in] type 存储设备类型，不能使用STORAGE_TYPE_INTERNAL
 * @return 0 SUCCESS(文件找到)  -1 FAIL
 */
int KING_FSUnmount(STORAGE_TYPE_E type);

/**
 * 获取指定存储设备剩余空间
 * @param[in] type 存储设备类型
 * @param[out]  pDiskFreeSize      给出空间大小
 * @return 0 SUCCESS(文件找到)  -1 FAIL
 */
int KING_FsFreeSpaceGet(STORAGE_TYPE_E type, uint32 * pDiskFreeSize);

/**
 * 获取指定存储设备已用空间大小
 * @param[in] type 存储设备类型
 * @param[out]  pDiskUsedSize      给出空间大小
 * @return 0 SUCCESS(文件找到)  -1 FAIL
 */
int KING_FsUsedSpaceGet(STORAGE_TYPE_E type, uint32 * pDiskUsedSize);

#endif

