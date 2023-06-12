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
 * keypad��ʼ��
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_KpInit(void);

/**
 * keypadע��ص�����
 * 
 * @param[in]  fun        keypad�а����¼�ʱͨ���˻ص��ϱ����ϱ���Ӧ�¼�����ֵ
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_KpRegCB(KEYPAD_CALLBACK fun);

/**
 * keypadȥע��ص�����
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_KpUnregCB(void);

/**
 * keypadȥ��ʼ��
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_KpDeinit(void);

#endif

