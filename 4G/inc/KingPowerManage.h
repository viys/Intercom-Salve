#ifndef __KING_POWER_MANAGE_H__
#define __KING_POWER_MANAGE_H__

// bootmode in APP_Main(bootmode)
#define BOOT_MODE_AUTO_ON       0   // 上电自动开机
#define BOOT_MODE_PWR_KEY       1   // 按开机键开机
#define BOOT_MODE_RESET_KEY     2   // 硬复位开机, reset signal
#define BOOT_MODE_REBOOT        3   // 软复位开机, Call Reboot API.
#define BOOT_MODE_WAKE_IN       4   // 睡眠后，WAKEIN signal唤醒开机
#define BOOT_MODE_RTC_ALARM     5   // 睡眠后，RTC唤醒开机


typedef enum _wakelockmodes {
    LOCK_SLEEP=0,
    LOCK_SUSPEND,
    LOCK_NONE,
    LOCK_MAX = 0x7FFFFFFF
} WAKE_LOCK_MODES;

typedef enum {
    LDO_ID_LCD,
    LDO_ID_SD,
    LDO_ID_SIM1,
    LDO_ID_MAX=0x7FFFFFFF
}LDO_ID_E;

typedef enum {
    LDO_VOL_1V8=1800,
    LDO_VOL_2V8=2800,
    LDO_VOL_3V0=3000,
    LDO_VOL_3V3=3300,
    LDO_VOL_MAX=0x7FFFFFFF
}LDO_VOL_E;

/**
 * 功耗锁
 *
 * @param[in]  LockMode          阻止进入深度睡眠or阻止系统进入空闲态
 * @param[in]  LockName          锁名字，可以同时存在多个锁
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_WakeLock(WAKE_LOCK_MODES LockMode, const uint8 *LockName);

/**
 * 解锁
 *
 * @param[in]  LockMode          阻止进入深度睡眠or阻止系统进入空闲态
 * @param[in]  LockName          锁名字，解掉对应名字的锁，只有所有锁都解除，系统才能进入睡眠
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_WakeUnlock(WAKE_LOCK_MODES LockMode, const uint8 *LockName);

/**
 * 关机
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_PowerOff(void);

/**
 * 重启
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_PowerReboot(void);

/**
 * LDO参数配置
 * @param[in]     id       LDO ID
 * @param[in]     voltage  LDO电压
 * @param[in]     onoff    打开/关闭LDO      true: 打开,  false: 关闭 
 * @return        0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 */
int KING_LdoConfigSet(LDO_ID_E id, LDO_VOL_E voltage, bool onoff);

#endif

