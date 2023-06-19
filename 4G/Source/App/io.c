/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "io.h"
#include "uart.h"


/* 输出引脚初始化 */
void sw_init(void)
{
    int ret = 0;
    GPIO_CFG_S cfg;

    ret = KING_GpioInit();
    LOG_P(ret,"KING_GpioInit() Fail!\r\n");

    memset(&cfg, 0x00, sizeof(GPIO_CFG_S));
    cfg.default_val = 0;
    cfg.int_type = GPIO_INT_DISABLE;
    cfg.pull_mode = GPIO_PULL_UP;
    cfg.dir = GPIO_DIR_OUTPUT;

    ret = KING_GpioModeSet(GPIO_7, &cfg);
    ret = KING_GpioModeSet(GPIO_14, &cfg);
}

/* 485输出引脚初始化 */
void rs485_sw_init(void)
{
    int ret = 0;

    GPIO_CFG_S cfg;

    ret = KING_GpioInit();
    LOG_P(ret,"KING_GpioInit() Fail!\r\n");

    memset(&cfg, 0x00, sizeof(GPIO_CFG_S));
    cfg.default_val = 0;
    cfg.int_type = GPIO_INT_DISABLE;
    cfg.pull_mode = GPIO_PULL_UP;
    cfg.dir = GPIO_DIR_OUTPUT;

    ret = KING_GpioModeSet(GPIO_15, &cfg);

}

void input8_callback(void)
{
    /* 产生按键输入标志位 */
    KING_EventSet(INPUT8EH);
}

void input10_callback(void)
{
    /* 产生按键输入标志位 */
    KING_EventSet(INPUT10EH);
}

/* 输入引脚初始化 */
void input_init(void)
{
    int ret = 0;

    GPIO_CFG_S cfg;

    ret = KING_GpioInit();
    LOG_P(ret,"KING_GpioInit() Fail!\r\n");

    memset(&cfg, 0x00, sizeof(GPIO_CFG_S));
    cfg.default_val = 0;
    cfg.int_type = GPIO_INT_EDGE_RISING;
    cfg.dir = GPIO_DIR_INPUT;
    cfg.int_callback = input8_callback;
    ret = KING_GpioModeSet(GPIO_8, &cfg);


    // cfg.int_callback = input10_callback;
    // ret = KING_GpioModeSet(GPIO_10, &cfg);
}
/* 输入引脚初始化 */
void input10_init(void)
{
    int ret = 0;

    GPIO_CFG_S cfg;

    ret = KING_GpioInit();
    LOG_P(ret,"KING_GpioInit() Fail!\r\n");

    memset(&cfg, 0x00, sizeof(GPIO_CFG_S));
    cfg.default_val = 0;
    cfg.int_type = GPIO_INT_LEVEL_HIGH ;
    cfg.dir = GPIO_DIR_INPUT;
    cfg.int_callback = input10_callback;
    ret = KING_GpioModeSet(GPIO_10, &cfg);

}