#ifndef __KING_GPIO_H__
#define __KING_GPIO_H__

typedef void (*GpioIntCallback)(void);

typedef enum
{
    GPIO_DIR_INPUT = 0,
    GPIO_DIR_OUTPUT,
    GPIO_DIR_OUTPUT_OPENDRAIN,
    GPIO_DIR_MAX = 0x7FFFFFFF
} GPIO_DIR_E;

typedef enum
{
   GPIO_PULL_UP    = 0,
   GPIO_PULL_DOWN,
   GPIO_PULL_NONE,
   GPIO_PULL_MAX = 0x7FFFFFFF
} GPIO_PULL_MODE;

typedef enum GPIO_INT_tag
{
    GPIO_INT_DISABLE = 0,   ///< Disable the relative gpio interrupt.
    GPIO_INT_LEVEL_HIGH,    ///< detect high level.
    GPIO_INT_LEVEL_LOW,     ///< detect low level.
    GPIO_INT_EDGE_BOTH,     ///< detect the rising edges and falling edges.
    GPIO_INT_EDGE_RISING,   ///< detect the rising edges.
    GPIO_INT_EDGE_FALLING,   ///< detect the falling edges.    
    GPIO_INT_MAX = 0x7FFFFFFF
} GPIO_INT_TYPE;
    

typedef enum
{
    ADC_VDD_REF_0 = 0, ///< UIS8910DM: 1.250V; UIS8811CD: 1.250V
    ADC_VDD_REF_1 = 1, ///< UIS8910DM: 2.444V; UIS8811CD: 1.600V
    ADC_VDD_REF_2 = 2, ///< UIS8910DM: 3.233V; UIS8811CD: 2.800V
    ADC_VDD_REF_3 = 3, ///< UIS8910DM: 5.000V; UIS8811CD: 5.000V
    ADC_VDD_REF_MAX,
    ADC_VDD_MAX = 0x7FFFFFFF
} ADC_VDD_REF_E;

typedef struct
{
    uint16          default_val;
    GPIO_DIR_E      dir;
    GPIO_PULL_MODE  pull_mode;
    GPIO_INT_TYPE   int_type;
    GpioIntCallback int_callback;
} GPIO_CFG_S;

typedef struct
{
    ADC_VDD_REF_E   vdd_ref;
    uint32          resolution;
    uint32          reserved1;
    uint32          reserved2;
} ADC_CFG_S;


/**
 * GPIO系统初始化
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_GpioInit(void);

/**
 * 配置gpio引脚相关属性设置
 * 如输入输出、是否中断、中断触发方式、回调函数
 * @param[in]  id gpio id (id与底层平台相关)
 * @param[in]  pCfg 属性参数
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_GpioModeSet(uint32 id, GPIO_CFG_S* pCfg);

/**
 * gpio引脚相关属性设置获取
 * 如输入输出、是否中断、中断触发方式、回调函数
 * @param[in]  id gpio id (id与底层平台相关)
 * @param[out]  pCfg 属性参数
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_GpioModeGet(uint32 id, GPIO_CFG_S* pCfg);

/**
 * 输出电平设置
 * 仅在gpio配置为 DIR_OUTPUT 时可用
 * @param[in]  id gpio id (id与底层平台相关)
 * @param[in]  value 输出电平 0-低电平  1-高电平
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_GpioSet(uint32 id, uint32 value);

/**
 * 获取gpio输入电平
 *
 * @param[in]  id gpio id (id与底层平台相关)
 * @param[out]  pValue 输入电平 0-低电平  1-高电平
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_GpioGet(uint32 id, uint32* pValue);

/**
 * 对指定的ADC通道进行配置
 *
 * @param[in]   adc_id  ADC channel ID (ID与底层平台相关)
 * @param[in]   pCfg 属性参数
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AdcConfig(uint32 adc_id, ADC_CFG_S* pCfg);

/**
 * 获取某个ADC通道的读取值
 *
 * @param[in]   adc_id  ADC channel ID (ID与底层平台相关)
 * @param[out]  pValue  ADC返回值，需参考底层平台定义。
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AdcGet(uint32 adc_id, uint32* pValue);

/**
 * GPIO系统去初始化
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_GpioDeinit(void);

#endif

