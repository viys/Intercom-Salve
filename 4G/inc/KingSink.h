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
 * Sink ��ʼ��
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SinkInit(void);

/**
 * SINK��������
 *
 * @param[in]  type        SINK type
 * @param[in]  level       ����Դ��������ȼ����ߵ�ѹԴ�����ѹ�ȼ���0Ϊ�رա�
 *                         ��ƽ̨��֧�ֵĵȼ���Χ��ͬ����ƽ̨������
 *                         UIS8910DM: ST_CURRENT_LED R/G/B/W 1-64 ��Ӧ1.68mA - 54.6mA������0.84mA
 *                                    ST_VOLTAGE_VIB 1-8����Ӧ2.8V-3.5V������100mV
 *                                    
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SinkConfig(SINK_TYPE type, uint32 level);

/**
 * SINKȥ��ʼ��
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SinkDeinit(void);

#endif

