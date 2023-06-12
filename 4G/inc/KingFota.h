#ifndef __KING_FOTA_H__
#define __KING_FOTA_H__

/**
 * Fota��������˵����
 * 1. �豸��FOTA������֮���ͨѶ��ʽ�Լ��������̽��Կ�Դ��ʽ
 * ��KingSDKһ���ṩ���ͻ�����ز����API���ڱ�ͷ�ļ��ж��塣
 *
 * 2. Fota�����ļ������ع����е���KING_FotaWriteFlash()���µ�
 * image�ļ�д�뵽flash�С�д�뵽flash�ĸ��������ߵ�ַ�ռ�����
 * ��ƽ̨�ײ�ʵ�֡�APP��д������п��Ե���KING_FotaSetFlag()
 * ��FOTA_FLAG���ó�FOTA_IMAGE_IS_PARTIAL����ֹд������жϵ�
 * �����쳣������BOOTLOADER/APP���Ը��ٵ�֮ǰ������״̬��
 * APP�����б������ɹ�д���image��С��
 *
 * 3. �µ�image�ļ�д����ɺ����KING_FotaCheckImage()���flash
 * �е�image�ļ��������鷵�سɹ�˵��д���ļ�û�����⡣
 *
 * 4. APP�����ɵ���KING_FotaSetFlag()��FOTA_FLAG���ó�FOTA_IMAGE_IS_READY
 * ����������API����ģ�顣
 *
 * 5. ģ��������BootLoader���fota flag���ΪFOTA_IMAGE_IS_READY��
 * �������������������������ɺ�����FOTA_FLAGΪFOTA_UPDATE_SUCCESS��
 * ֮������ģ�飨����ֱ�������µ�APP����
 *
 * 6. ģ��������APP��һʱ�����KING_FotaGetFlag()��鵱ǰ��
 * FOTA_FLAG�����ΪFOTA_UPDATE_SUCCESS/FAILED�����FotaSetFlag()
 * ��flag����ΪFOTA_CLEAR������������ɡ�
 */


typedef enum
{
    FOTA_CLEAR = 0,
    FOTA_IMAGE_IS_PARTIAL,
    FOTA_IMAGE_IS_READY,
    FOTA_UPDATE_SUCCESS,
    FOTA_UPDATE_FAILED,
} FOTA_FLAG_E;

typedef struct
{
    char*  imageName;
    uint32 imageSize;
    uint8  majorVer;
    uint8  minorVer;
} FOTA_IMAGE_INFO;

/**
 *  Write new image to flash.
 * @param[in] offset addr offset where data buff to be written.
 * @param[in] data pointer to data buff to be written.
 * @param[in] dataLen Length of dataBuf.
 * @param[out] written the length of data has been written.
 * @return 0 SUCCESS -1 Fail
 */
int KING_FotaFlashWrite(uint32 offset, void* data, uint32 dataLen, uint32* written);


/**
 * sanity check if the image written in flash is valid.
 * Mainly check the image header.
 *
 * @return 0 SUCCESS -1 Fail
 */
int KING_FotaImageCheck(FOTA_IMAGE_INFO* imageInfo);


/**
 * Set flag status flag.
 * For APP, it should set flag to FOTA_IMAGE_IS_READY after
 * fota image is written complete, and should call reset API
 * if set successfully.
 *
 * APP should also call this API to clear the flag (FOTA_CLEAR)
 * after reset.
 *
 * @param[in]  flag  FOTA_IMAGE_IS_READY/FOTA_CLEAR
 * @return 0 SUCCESS -1 Fail
 */
int KING_FotaFlagSet(FOTA_FLAG_E flag);

/**
 * Get fota status flag.
 *
 * @return 0 SUCCESS -1 Fail
 */
int KING_FotaFlagGet(FOTA_FLAG_E* flag);


#endif

