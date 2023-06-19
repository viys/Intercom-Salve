#ifndef __IO_H
#define __IO_H

#include "app.h"

/* 485是否启用自收发 1:未启用 0:启用 */
#define RS485_AUTO  1

#define GPIO_7         7
#define GPIO_8         8
#define GPIO_10        10
#define GPIO_14        14
#define GPIO_15        15

/* 输出引脚初始化 */
void sw_init(void);
/* 485收发控制引脚初始化 */
void rs485_sw_init(void);
/* 输入引脚初始化 */
void input_init(void);
void input10_init(void);

#endif /* __IO_H */
