#ifndef __KING_POWER_MANAGE_H__
#define __KING_POWER_MANAGE_H__

// bootmode in APP_Main(bootmode)
#define BOOT_MODE_AUTO_ON       0   // �ϵ��Զ�����
#define BOOT_MODE_PWR_KEY       1   // ������������
#define BOOT_MODE_RESET_KEY     2   // Ӳ��λ����, reset signal
#define BOOT_MODE_REBOOT        3   // ��λ����, Call Reboot API.
#define BOOT_MODE_WAKE_IN       4   // ˯�ߺ�WAKEIN signal���ѿ���
#define BOOT_MODE_RTC_ALARM     5   // ˯�ߺ�RTC���ѿ���


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
 * ������
 *
 * @param[in]  LockMode          ��ֹ�������˯��or��ֹϵͳ�������̬
 * @param[in]  LockName          �����֣�����ͬʱ���ڶ����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_WakeLock(WAKE_LOCK_MODES LockMode, const uint8 *LockName);

/**
 * ����
 *
 * @param[in]  LockMode          ��ֹ�������˯��or��ֹϵͳ�������̬
 * @param[in]  LockName          �����֣������Ӧ���ֵ�����ֻ���������������ϵͳ���ܽ���˯��
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_WakeUnlock(WAKE_LOCK_MODES LockMode, const uint8 *LockName);

/**
 * �ػ�
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_PowerOff(void);

/**
 * ����
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_PowerReboot(void);

/**
 * LDO��������
 * @param[in]     id       LDO ID
 * @param[in]     voltage  LDO��ѹ
 * @param[in]     onoff    ��/�ر�LDO      true: ��,  false: �ر� 
 * @return        0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 */
int KING_LdoConfigSet(LDO_ID_E id, LDO_VOL_E voltage, bool onoff);

#endif

