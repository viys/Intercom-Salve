#ifndef __KING_USB_H__
#define __KING_USB_H__

#include "kingdef.h"

#define USB_EVT_DATA_TO_READ      (0)
#define USB_EVT_WRITE_COMPLETE    (1)
#define USB_EVT_INIT_COMPLETE     (2)
#define USB_EVT_SHUTDOWN_COMPLETE (3)
/**
 * usb初始化
 * @param[in]  id 需要初始化的usb id
 *             具体支持哪些id，请参考对应的硬件规格书
 *             UIS8910DM id范围为5-7
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_UsbInit(uint32 id);

/**
 * usb读数据
 * @param[in]  id 从哪个usb读数据
 * @param[in]  pTargetBuf 读取的数据存放的buffer
 * @param[in]  BufLen 要读取数据的长度
 * @param[out]  pBytesRead 实际读取数据的长度
 * @return 0 SUCCESS  -1 FAIL，大于0时为实际读取的数据长度
 */
int KING_UsbRead(uint32 id, uint8* pTargetBuf, uint32 BufLen, uint32* pBytesRead);

/**
 * usb写数据
 * @param[in]  id 要往哪个usb读数据
 * @param[in]  pSourceBuf 写数据存放的buffer
 * @param[in]  BufLen 要写数据的长度
 * @param[out]  pBytesWrite 实际写数据的长度
 * @return 0 SUCCESS  -1 FAIL，大于0时为实际写入的数据长度
 */
int KING_UsbWrite(uint32 id,uint8* pSourceBuf, uint32 BufLen, uint32* pBytesWrite);

/**
 * usb去初始化
 * @param[in]  id 需要去初始化的usb id
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_UsbDeinit(uint32 id);


#endif

