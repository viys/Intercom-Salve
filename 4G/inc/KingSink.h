#ifndef __KING_SINK_H__
#define __KING_SINK_H__

typedef enum _SINK_TYPE
{
    ST_CURRENT_LED_R,
    ST_CURRENT_LED_G,
    ST_CURRENT_LED_B,
    ST_CURRENT_LED_W,
    ST_CURRENT_KEYPAD,
    ST_CURRENT_FLASH_LIGHT,
    ST_VOLTAGE_VIB,
    ST_MAX = 0x7FFFFFFF
} SINK_TYPE;

/**
 * Sink 初始化
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SinkInit(void);

/**
 * SINK参数配置
 *
 * @param[in]  type        SINK type
 * @param[in]  level       电流源输出电流等级或者电压源输出电压等级，0为关闭。
 *                         各平台能支持的等级范围不同，由平台决定。
 *                         UIS8910DM: ST_CURRENT_LED R/G/B/W 1-64 对应1.68mA - 54.6mA，步长0.84mA
 *                                    ST_VOLTAGE_VIB 1-8，对应2.8V-3.5V，步长100mV
 *                                    
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SinkConfig(SINK_TYPE type, uint32 level);

/**
 * SINK去初始化
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SinkDeinit(void);

#endif

