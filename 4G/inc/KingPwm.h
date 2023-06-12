#ifndef __KING_PWM_H__
#define __KING_PWM_H__


/**
 * pwm��ʼ��
 *
 * @param[in]  id  ͨ��id
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2

 * @note  UIS8811ƽ̨id��Χ0-3
 */
int KING_PwmInit(uint32 id);

/**
 * pwm��������
 *
 * @param[in]  id          ͨ��id
 * @param[in]  freq        ����Ƶ��
 * @param[in]  duty_cycle  ����ռ�ձ�
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 *
 * @note  UIS8811ƽ̨��
         -- ռ�ձȾ���1%;
         -- PM0ģʽ�£�ͨ��0�����Ƶ��160Hz;ͨ��1-3�����Ƶ��: 3.12MHz
         -- PM2ģʽ�£�ͨ��0�����Ƶ��160Hz; ͨ��1-3������
         -- PM3ģʽ�£�ͨ��0-3��������
 */
int KING_PwmConfig(uint32 id, uint32 freq, uint16 duty_cycle);

/**
 * pwm��ʼ���
 *
 * @param[in]  id          ͨ��id
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 *
 * @note  UIS8811ƽ̨id��Χ0-3
 */
int KING_PwmStart(uint32 id);

/**
 * pwmֹͣ���
 *
 * @param[in]  id          ͨ��id
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 *
 * @note  UIS8811ƽ̨id��Χ0-3
 */
int KING_PwmStop(uint32 id);

/**
 * pwmȥ��ʼ��
 *
 * @param[in]  id          ͨ��id
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 *
 * @note  UIS8811ƽ̨id��Χ0-3
 */
int KING_PwmDeinit(uint32 id);


#endif

