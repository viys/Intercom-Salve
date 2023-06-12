/*
 *
 * Automatically generated file; DO NOT EDIT.
 * RDA Feature Phone Configuration
 *
 */

#ifndef __FP_CONFIG_H__
#define __FP_CONFIG_H__

/* --- BOOT APP Software Configuration --- */

/* FLASH layout */
#define FLASH_SIZE 0x400000
#define FLASH_BOOT_OFFSET 0x10000
#define FOTA_BACKUP_AREA_START 0x300000
#define FOTA_BACKUP_AREA_SIZE 0x10000
#define FLASH_BOOT_APP_OFFSET 0x310000
#define FLASH_BOOT_APP_SIZE 0x8000
#define FLASH_WSOP_APP_OFFSET 0x320000
#define FLASH_WSOP_APP_SIZE 0x40000
#define SPIFFS_FLASH_START 0x360000
#define SPIFFS_FLASH_SIZE 0x78000
#define SPIFFS_BACKUP_FLASH_START 0x3D8000
#define SPIFFS_BACKUP_FLASH_SIZE 0x20000
#define SFFS_FLASH_EB_SIZE 0x8000
#define SFFS_FLASH_PB_SIZE 0x200
#define USER_DATA_BASE 0x3F8000
#define USER_DATA_SIZE 0x0
#define USER_DATA_FLASH_EB_SIZE 0x1000
#define USER_DATA_FLASH_PB_SIZE 0x200
#define CALIB_BASE 0x3F8000
#define CALIB_RF2G_BASE 0x3FA000
#define CALIB_RFNB_BASE 0x3FC000
#define CALIB_SIZE 0x6000
#define FACT_SETTINGS_BASE 0x3FE000
#define FACT_SETTINGS_SIZE 0x2000

#define FBDEV_DEFAULT_V2
#define FLASH_WP_ENABLE
#define FLASH_DEFAULT_WP_ALL


#endif