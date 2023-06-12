#ifndef __KING_KEYPAD_H__
#define __KING_KEYPAD_H__

// Definition for Keypad message
typedef enum
{
    KEYPAD_KEYDOWN,
    KEYPAD_KEYUP,
    KEYPAD_MSG_MAX = 0x7fffffff
} KEYPAD_EVENT_E;

#define POWER_KEY_SCANCODE 0x00FF00FF

typedef void (*KEYPAD_CALLBACK)(KEYPAD_EVENT_E eventCode,uint32 keyCode);

/**
 * keypad初始化
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_KpInit(void);

/**
 * keypad注册回调函数
 * 
 * @param[in]  fun        keypad有按键事件时通过此回调上报，上报相应事件及键值
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_KpRegCB(KEYPAD_CALLBACK fun);

/**
 * keypad去注册回调函数
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_KpUnregCB(void);

/**
 * keypad去初始化
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_KpDeinit(void);

#endif

