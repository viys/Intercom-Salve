#ifndef __KING_USB_H__
#define __KING_USB_H__

#include "kingdef.h"

#define USB_EVT_DATA_TO_READ      (0)
#define USB_EVT_WRITE_COMPLETE    (1)
#define USB_EVT_INIT_COMPLETE     (2)
#define USB_EVT_SHUTDOWN_COMPLETE (3)
/**
 * usb��ʼ��
 * @param[in]  id ��Ҫ��ʼ����usb id
 *             ����֧����Щid����ο���Ӧ��Ӳ�������
 *             UIS8910DM id��ΧΪ5-7
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_UsbInit(uint32 id);

/**
 * usb������
 * @param[in]  id ���ĸ�usb������
 * @param[in]  pTargetBuf ��ȡ�����ݴ�ŵ�buffer
 * @param[in]  BufLen Ҫ��ȡ���ݵĳ���
 * @param[out]  pBytesRead ʵ�ʶ�ȡ���ݵĳ���
 * @return 0 SUCCESS  -1 FAIL������0ʱΪʵ�ʶ�ȡ�����ݳ���
 */
int KING_UsbRead(uint32 id, uint8* pTargetBuf, uint32 BufLen, uint32* pBytesRead);

/**
 * usbд����
 * @param[in]  id Ҫ���ĸ�usb������
 * @param[in]  pSourceBuf д���ݴ�ŵ�buffer
 * @param[in]  BufLen Ҫд���ݵĳ���
 * @param[out]  pBytesWrite ʵ��д���ݵĳ���
 * @return 0 SUCCESS  -1 FAIL������0ʱΪʵ��д������ݳ���
 */
int KING_UsbWrite(uint32 id,uint8* pSourceBuf, uint32 BufLen, uint32* pBytesWrite);

/**
 * usbȥ��ʼ��
 * @param[in]  id ��Ҫȥ��ʼ����usb id
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_UsbDeinit(uint32 id);


#endif

