#ifndef __KING_PWM_H__
#define __KING_PWM_H__


/**
 * pwm初始化
 *
 * @param[in]  id  通道id
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2

 * @note  UIS8811平台id范围0-3
 */
int KING_PwmInit(uint32 id);

/**
 * pwm参数配置
 *
 * @param[in]  id          通道id
 * @param[in]  freq        配置频率
 * @param[in]  duty_cycle  配置占空比
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 *
 * @note  UIS8811平台：
         -- 占空比精度1%;
         -- PM0模式下：通道0的最大频率160Hz;通道1-3的最大频率: 3.12MHz
         -- PM2模式下：通道0的最大频率160Hz; 通道1-3不工作
         -- PM3模式下：通道0-3都不工作
 */
int KING_PwmConfig(uint32 id, uint32 freq, uint16 duty_cycle);

/**
 * pwm开始输出
 *
 * @param[in]  id          通道id
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 *
 * @note  UIS8811平台id范围0-3
 */
int KING_PwmStart(uint32 id);

/**
 * pwm停止输出
 *
 * @param[in]  id          通道id
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 *
 * @note  UIS8811平台id范围0-3
 */
int KING_PwmStop(uint32 id);

/**
 * pwm去初始化
 *
 * @param[in]  id          通道id
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 *
 * @note  UIS8811平台id范围0-3
 */
int KING_PwmDeinit(uint32 id);


#endif

