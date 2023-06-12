#ifndef __KING_FOTA_H__
#define __KING_FOTA_H__

/**
 * Fota升级流程说明：
 * 1. 设备和FOTA服务器之间的通讯方式以及下载流程将以开源形式
 * 和KingSDK一起提供给客户。相关步骤和API不在本头文件中定义。
 *
 * 2. Fota升级文件在下载过程中调用KING_FotaWriteFlash()将新的
 * image文件写入到flash中。写入到flash哪个分区或者地址空间依赖
 * 于平台底层实现。APP在写入过程中可以调用KING_FotaSetFlag()
 * 将FOTA_FLAG设置成FOTA_IMAGE_IS_PARTIAL，防止写入过程中断电
 * 或者异常重启，BOOTLOADER/APP可以跟踪到之前的升级状态。
 * APP可自行保存最后成功写入的image大小。
 *
 * 3. 新的image文件写入完成后调用KING_FotaCheckImage()检查flash
 * 中的image文件。如果检查返回成功说明写入文件没有问题。
 *
 * 4. APP检查完成调用KING_FotaSetFlag()将FOTA_FLAG设置成FOTA_IMAGE_IS_READY
 * 并调用重启API重启模块。
 *
 * 5. 模块重启后，BootLoader检查fota flag如果为FOTA_IMAGE_IS_READY，
 * 将继续完成升级操作，升级完成后设置FOTA_FLAG为FOTA_UPDATE_SUCCESS。
 * 之后重启模块（或者直接启动新的APP？）
 *
 * 6. 模块重启后，APP第一时间调用KING_FotaGetFlag()检查当前的
 * FOTA_FLAG，如果为FOTA_UPDATE_SUCCESS/FAILED则调用FotaSetFlag()
 * 将flag设置为FOTA_CLEAR。至此升级完成。
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

