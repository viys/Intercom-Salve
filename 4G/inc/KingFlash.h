#ifndef __KING_FLASH_H__
#define __KING_FLASH_H__

typedef enum
{
    FLASH_INTER = 0,
    FLASH_EXTERN
}FLASH_INTR_EXTR_E;


/**
 * @brief ƽ̨NV��д���ܳ�ʼ��
 * @details NV��flash�ϵ�һƬ�洢�ռ䣬��Ҫ���ڴ洢
 * ���ñ���Ĳ����������豸SN�š���Կ�ȡ������Ƕϵ磬
 * ���������ļ�ϵͳ������NV������������϶���Ӧ�ö�ʧ��
 * NV�洢��Ҫ����ֻдһ�λ��߼��ٸ��µ������ʹ�ã�Ƶ��
 * ���µĲ�������ʹ���ļ�ϵͳ�洢�ļ��ķ�ʽ��
 * ��ͬƽ̨���ڱ���NV��flash�ռ��ַ�ʹ洢������ͬ��
 *
 * @return 0 SUCCESS  -1 FAIL
 * @note UIS8910ƽ̨ʹ��ԭʼ��flash�ռ䣬Ŀǰ��֧��5��NV(ID:0-4)��
 * ÿ��NV�Ŀռ��СΪ1024�ֽڡ�
 */
int KING_NvInit(void);

/**
 * д��ĳ��NV����
 * @param[in]    nvID    ��NV���ݶ�Ӧ��index. APP�Լ�����ID��Name�Ķ��ձ�
 *               UIS8910DMƽ̨֧��5��NV��nvIDȡֵ��ΧΪ0-4
 * @param[in]    pData   ��������
 * @param[in]    size    �������ݳ���
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NvWrite(uint32 nvID, void* pData, uint32 size);

/**
 * ��ȡĳ��NV����
 * @param[in]    nvID    д��NV����ʱ��Ӧ��index.
 * @param[out]   pData   ���ص���������
 * @param[inout]   size    ���ص��������ݳ���
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NvRead(uint32 nvID, void* pData, uint32* size);

/**
 * @brief ƽ̨NV��д����ȥ��ʼ��
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NvDeinit(void);


/**
 * @brief spi  flash��ʼ��
 * @param[in]  isextern  �������ڲ�flash�������flash��
 * ����flash�Ĳ��������ڳ�ʼ��ʱ���õ�flash���͡�
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SpiFlashInit(FLASH_INTR_EXTR_E isextern);


/**
 * spi flash���ݶ�ȡ
 *
 * @param[in]  addr       ��ȡ���ݵĵ�ַ��Ϣ
 * @param[in]  len        Ҫ��ȡ���ݵĳ���
 * @param[in]  rbuf       �洢���ݵ�bufָ��
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SpiFlashRead(uint32 addr, uint32 len, uint8 *rbuf);


/**
 * spi  flashд����
 *
 * @param[in]  addr       д���ݵĵ�ַ��Ϣ
 * @param[in]  len        Ҫд���ݵĳ���
 * @param[in]  wbuf       �洢���ݵ�bufָ��
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SpiFlashWrite(uint32 addr, uint32 len, const uint8 *wbuf);


/**
 * spi  flash����
 *
 * @param[in]  addr       Ҫ������ʼ��ַ
 * @param[in]  len        Ҫ�����ĳ���
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SpiFlashErase(uint32 addr, uint32 len);


/**
 * @brief spi nor flashȥ��ʼ��
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SpiFlashDeinit(void);


#endif

