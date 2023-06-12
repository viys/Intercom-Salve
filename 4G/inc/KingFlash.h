#ifndef __KING_FLASH_H__
#define __KING_FLASH_H__

typedef enum
{
    FLASH_INTER = 0,
    FLASH_EXTERN
}FLASH_INTR_EXTR_E;


/**
 * @brief 平台NV读写功能初始化
 * @details NV是flash上的一片存储空间，主要用于存储
 * 永久保存的参数，比如设备SN号、密钥等。无论是断电，
 * 重启或是文件系统奔溃，NV里的数据理论上都不应该丢失。
 * NV存储主要用于只写一次或者极少更新的情况下使用，频繁
 * 更新的操作，请使用文件系统存储文件的方式。
 * 不同平台用于保存NV的flash空间地址和存储方法不同。
 *
 * @return 0 SUCCESS  -1 FAIL
 * @note UIS8910平台使用原始的flash空间，目前仅支持5个NV(ID:0-4)，
 * 每个NV的空间大小为1024字节。
 */
int KING_NvInit(void);

/**
 * 写入某个NV数据
 * @param[in]    nvID    该NV数据对应的index. APP自己管理ID和Name的对照表
 *               UIS8910DM平台支持5个NV，nvID取值范围为0-4
 * @param[in]    pData   内容数据
 * @param[in]    size    内容数据长度
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NvWrite(uint32 nvID, void* pData, uint32 size);

/**
 * 读取某个NV数据
 * @param[in]    nvID    写入NV数据时对应的index.
 * @param[out]   pData   返回的内容数据
 * @param[inout]   size    返回的内容数据长度
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NvRead(uint32 nvID, void* pData, uint32* size);

/**
 * @brief 平台NV读写功能去初始化
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NvDeinit(void);


/**
 * @brief spi  flash初始化
 * @param[in]  isextern  操作是内部flash还是外挂flash，
 * 后续flash的操作均基于初始化时设置的flash类型。
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SpiFlashInit(FLASH_INTR_EXTR_E isextern);


/**
 * spi flash数据读取
 *
 * @param[in]  addr       读取数据的地址信息
 * @param[in]  len        要读取数据的长度
 * @param[in]  rbuf       存储数据的buf指针
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SpiFlashRead(uint32 addr, uint32 len, uint8 *rbuf);


/**
 * spi  flash写数据
 *
 * @param[in]  addr       写数据的地址信息
 * @param[in]  len        要写数据的长度
 * @param[in]  wbuf       存储数据的buf指针
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SpiFlashWrite(uint32 addr, uint32 len, const uint8 *wbuf);


/**
 * spi  flash擦除
 *
 * @param[in]  addr       要擦除起始地址
 * @param[in]  len        要擦除的长度
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SpiFlashErase(uint32 addr, uint32 len);


/**
 * @brief spi nor flash去初始化
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SpiFlashDeinit(void);


#endif

