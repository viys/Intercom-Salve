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
 *  FS_MODE_CREATE_NEW���������ļ�, �����ڴ���ʧ��
 *  FS_MODE_CREATE_ALWAYS���ļ��������򴴽����ļ����ļ�������ɾ���󴴽����ļ�
 *  FS_MODE_OPEN_EXISTING�����Ѵ��ڵ��ļ�
 *  FS_MODE_OPEN_ALWAYS��  �ļ������ڴ��������ļ����ļ���������ļ�
 *  FS_MODE_APPEND�� ����/���ļ�, ����ָ�����ļ�ĩ, д���ļ�����ĩ
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
  STORAGE_TYPE_INTERNAL, //�ڲ��ļ�ϵͳ
  STORAGE_TYPE_SD,  //SD������·�� /mnt/sd
  STORAGE_TYPE_FLASH, //���FLASH����·�� /mnt/flash
  STORAGE_TYPE_MAX = 0x7FFFFFFF
} STORAGE_TYPE_E;

/**
 * �ļ�ϵͳ��ʼ�����ڵ����ļ�ϵͳ��������֮ǰ���ȵ���
 * �˺�����������سɹ��ż������������������������ʧ��
 * �п���ƽ̨��֧���ļ�ϵͳ������
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_FsInit( void );

/**
 * �ļ�ϵͳȥ��ʼ����
 * �����ļ���ز������֮�󣬵��ô˺�����
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_FsDeInit( void );

/**
 * ����Ŀ¼
 *
 * @param[in]  pPathName     Ŀ¼����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_FsDirectoryCreate( char* pPathName );

/**
 * ɾ��Ŀ¼
 *
 * @param[in]  pPathName     Ŀ¼����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_FsDirectoryDelete( char* pPathName );

/**
 * ��������ļ�
 *
 * @param[in]  pFileName     �ļ���
 * @param[in]  accessMode    Access Modes and/or Operation Modes. Check FS_MODE defines.
 * @param[in]  share_mode    ��֧��
 * @param[in]  file_attr     ��֧��
 * @param[out]  pFileHandle  �򿪵��ļ�handle
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_FsFileCreate(char* pFileName, uint32 accessMode, uint32 share_mode, uint32 file_attr, FS_HANDLE* pFileHandle);

/**
 * ɾ���ļ�
 *
 * @param[in]  pFileName     �ļ�����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_FsFileDelete(char* pFileName);

/**
 * ���ļ�
 *
 * @param[in]  fileHandle      KING_FsFileCreate������handle
 * @param[in]  readBuf         �����bufָ��
 * @param[in]  bufLen          �����buf����
 * @param[out] bytesRead       ʵ�ʶ����ĳ���
 * @return 0 SUCCESS(�ļ��ҵ�)  -1 FAIL
 */
int KING_FsFileRead(FS_HANDLE fileHandle, uint8* readBuf, uint32 bufLen, uint32* bytesRead);

/**
 * д�ļ�
 *
 * @param[in]  fileHandle      KING_FsFileCreate������handle
 * @param[in]  writeBuf        Ҫд�������bufָ��
 * @param[in]  bufLen          Ҫд������ݳ���
 * @param[out] byteswritten    ʵ��д��ĳ���
 * @return 0 SUCCESS(�ļ��ҵ�)  -1 FAIL
 */
int KING_FsFileWrite(FS_HANDLE fileHandle, uint8* writeBuf, uint32 bufLen, uint32* byteswritten);

/**
 * �ļ�����
 * ������������ˢ������
 * @param[in]  fileHandle      KING_FsFileCreate������handle
 * @return 0 SUCCESS(�ļ��ҵ�)  -1 FAIL
 */
int KING_FsFileBuffersFlush(FS_HANDLE fileHandle);

/**
 * ��ȡ�ļ���С
 *
 * @param[in]  fileHandle      KING_FsFileCreate������handle
 * @param[out] pFileSize       �ļ���С
 * @return 0 SUCCESS(�ļ��ҵ�)  -1 FAIL
 */
int KING_FsFileSizeGet(FS_HANDLE fileHandle, uint32* pFileSize);

/**
 * �������ļ�
 *
 * @param[in]  sour      ԭ�ļ�����Ŀ¼
 * @param[in]  dest      ���ļ�����Ŀ¼(Ŀ¼����һ��)
 * @return 0 SUCCESS(�ļ��ҵ�)  -1 FAIL
 */
int KING_FsFileRename(const char *sour,const char *dest);


/**
 * ��ȡ��ǰ�ļ�����pointer
 *
 * @param[in]  fileHandle      KING_FsFileCreate������handle
 * @param[in]  origin          FS_SEEK_BEGIN��FS_SEEK_END
 * @param[out] currentpos     �����origin�ĵ�ǰλ��
 * @return 0 SUCCESS(�ļ��ҵ�)  -1 FAIL
 */
int KING_FsFilePointerGet(FS_HANDLE fileHandle , uint32 origin, int* currentpos);

/**
 * ƫ�Ƶ�ǰ�ļ�����pointer
 *
 * @param[in]  fileHandle      KING_FsFileCreate������handle
 * @param[in]  offset          ��Ե�origin��ƫ������Ϊ����ʱ��ǰ��
 * @param[in] origin          FS_SEEK_BEGIN��FS_SEEK_CUR��FS_SEEK_END
 * @return 0 SUCCESS(�ļ��ҵ�)  -1 FAIL
 */
int KING_FsFilePointerSet(FS_HANDLE fileHandle , int32 offset, uint32 origin);

/**
 * �ر��ļ�
 *
 * @param[in]  fileHandle      KING_FsFileCreate������handle
 * @return 0 SUCCESS(�ļ��ҵ�)  -1 FAIL
 */
int KING_FsCloseHandle(FS_HANDLE fileHandle);

/**
 * �����ļ�
 *
 * @param[in]  pFileName ���ҵ��ļ��������ͣ���Ҫ��·������"/mnt/sd/1.mp3","/ *.3gp",or "/123.mp3"
 *                       �ڲ��ļ�ϵͳ��Ŀ¼Ϊ"/"��SD����Ŀ¼/mnt/sd�����FLASH��Ŀ¼/mnt/flash
 * @param[out]  pFindFileData    ����ҵ�ƥ����ļ���ͨ����������ϱ��ļ���Ϣ
 * @param[out]  pFindFile        0-δ�ҵ�ƥ���ļ�   ��0-�ҵ�ƥ���ļ� �Һ�������pFindFile�������¸�ƥ���ļ�
 * @return 0 SUCCESS  -1 FAIL ����0ʱ�����ҵ����ļ�handle
 */
int KING_FsFindFirstFile(char* pFileName, FS_FIND_DATA_S* pFindFileData, FS_HANDLE* pFindFile);

/**
 * ����������һ���ļ�
 *
 * @param[in]  findFile          KING_FsFindFirstFile������handle
 * @param[out]  pFindFileData    ����ҵ�ƥ����ļ���ͨ����������ϱ��ļ���Ϣ
 * @return 0 SUCCESS  -1 FAIL ����0ʱ�����ҵ����ļ�handle
 */
int KING_FsFindNextFile(FS_HANDLE findFile, FS_FIND_DATA_S* pFindFileData);

/**
 * ����������һ���ļ�
 *
 * @param[in]  findFile          KING_FsFindFirstFile������handle
 * @param[out]  pFindFileData    ����ҵ�ƥ����ļ���ͨ����������ϱ��ļ���Ϣ
 * @return 0 SUCCESS  -1 FAIL ����0ʱ�����ҵ����ļ�handle
 */
int KING_FsFindPreFile(FS_HANDLE findFile, FS_FIND_DATA_S* pFindFileData);

/**
 * �رղ���
 *
 * @param[in]  findFile          KING_FsFindFirstFile������handle
 * @return 0 SUCCESS(�ļ��ҵ�)  -1 FAIL
 */
int KING_FsFindFileClose(FS_HANDLE findFile );

/**
 * �����ⲿ�洢�豸
 * @param[in] type �洢�豸���ͣ�����ʹ��STORAGE_TYPE_INTERNAL
 * @param[in] phys_start ����ļ�ϵͳ��������ʼ��ַ��������ֻ��typeΪSTORAGE_TYPE_FLASHʱ��Ч
 * @param[in] phys_size �ļ�ϵͳռ�õ�����ռ��С����λΪbyte��������ֻ��typeΪSTORAGE_TYPE_FLASHʱ��Ч
 * @param[in] phys_erase_size Flash���β����ռ��С����λΪbyte��������ֻ��typeΪSTORAGE_TYPE_FLASHʱ��Ч
 * @param[in] block_size �ļ�ϵͳ�߼����С����λΪbyte��������ֻ��typeΪSTORAGE_TYPE_FLASHʱ��Ч
 * @return 0 SUCCESS(�ļ��ҵ�)  -1 FAIL
 */
int KING_FSMount(STORAGE_TYPE_E type, uint32 phys_start, uint32 phys_size, uint32 phys_erase_size, uint32 block_size);

/**
 * ж���ⲿ�洢�豸
 * @param[in] type �洢�豸���ͣ�����ʹ��STORAGE_TYPE_INTERNAL
 * @return 0 SUCCESS(�ļ��ҵ�)  -1 FAIL
 */
int KING_FSUnmount(STORAGE_TYPE_E type);

/**
 * ��ȡָ���洢�豸ʣ��ռ�
 * @param[in] type �洢�豸����
 * @param[out]  pDiskFreeSize      �����ռ��С
 * @return 0 SUCCESS(�ļ��ҵ�)  -1 FAIL
 */
int KING_FsFreeSpaceGet(STORAGE_TYPE_E type, uint32 * pDiskFreeSize);

/**
 * ��ȡָ���洢�豸���ÿռ��С
 * @param[in] type �洢�豸����
 * @param[out]  pDiskUsedSize      �����ռ��С
 * @return 0 SUCCESS(�ļ��ҵ�)  -1 FAIL
 */
int KING_FsUsedSpaceGet(STORAGE_TYPE_E type, uint32 * pDiskUsedSize);

#endif

