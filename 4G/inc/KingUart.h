#ifndef __KING_UART_H__
#define __KING_UART_H__


#define UART_0       (0)
#define UART_1       (1)
#define UART_2       (2)
#define UART_3       (3)
#define UART_4       (4)
#define UART_5       (5)

// UART event type. NOTIFY_FUN��ʹ��
#define UART_EVT_DATA_TO_READ      (0)
#define UART_EVT_WRITE_COMPLETE    (1)
#define UART_EVT_INIT_COMPLETE     (2)
#define UART_EVT_SHUTDOWN_COMPLETE (3)

typedef enum
{
    UART_BAUD_1200      = 1200,
    UART_BAUD_2400      = 2400,
    UART_BAUD_3600      = 3600,
    UART_BAUD_4800      = 4800,
    UART_BAUD_7200      = 7200,
    UART_BAUD_9600      = 9600,
    UART_BAUD_14400     = 14400,
    UART_BAUD_19200     = 19200,
    UART_BAUD_28800     = 28800,
    UART_BAUD_38400     = 38400,
    UART_BAUD_57600     = 57600,
    UART_BAUD_115200    = 115200,
    UART_BAUD_230400    = 230400,
    UART_BAUD_460800    = 460800,
    UART_BAUD_921600    = 921600,
    UART_BAUD_MAX = 0x7FFFFFFF
} UART_BAUD_E;

typedef enum  /* The Byte Len of the UART Frame Format  */
{
    UART_BYTE_LEN_5,        ///< set Byte Length to 5 Bits
    UART_BYTE_LEN_6,        ///< set Byte Length to 6 Bits
    UART_BYTE_LEN_7,        ///< set Byte Length to 7 Bits
    UART_BYTE_LEN_8,        ///< set Byte Length to 8 Bits
    UART_BYTE_LEN_MAX = 0x7FFFFFFF
} UART_BYTE_SIZE_E;


typedef enum  /* The Stop Bits of the UART Frame Format */
{
    UART_ONE_STOP_BIT,
    UART_ONE_AND_HALF_STOP_BITS,
    UART_TWO_STOP_BITS,
    UART_STOP_BITS_MAX = 0x7FFFFFFF
} UART_STOP_BITS_E;

typedef enum  /* The Parity Bits of the UART Frame Format */
{
    UART_NO_PARITY = 0,
    UART_EVEN_PARITY,
    UART_ODD_PARITY,
    UART_PARITY_MAX = 0x7FFFFFFF
} UART_PARITY_E;

typedef enum
{
    UART_NO_FLOW_CONTROL = 0,
    UART_HW_FLOW_CONTROL,
    UART_SW_FLOW_CONTROL,
    UART_FLOW_CTRL_MAX = 0x7FFFFFFF
} UART_FLOW_CTRL_E;

typedef enum
{
    UART_IRDA_MODE_DISABLE = 0, ///< IrDA mode disabled
    UART_IRDA_MODE_ENABLE,      ///< IrDA mode enabled
    UART_IRDA_MODE_MAX = 0x7FFFFFFF
} UART_IRDA_MODE_E;

/* uart deepsleep wakeup control */
typedef struct
{
    UART_BAUD_E         baudrate;
    UART_PARITY_E       parity;
    UART_STOP_BITS_E    stop_bits;
    UART_BYTE_SIZE_E    byte_size;
    UART_FLOW_CTRL_E    flow_ctrl;
    UART_IRDA_MODE_E    irda_mode;
} UART_CONFIG_S;


/**
 * uart��ʼ��������uart��ز���
 * @param[in]  id ��Ҫ��ʼ����uart id��֧����Щid����鿴��Ӧ��Ӳ�������
 * @param[in]  cfg ��Ҫ���õ�uart����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_UartInit(uint32 id, UART_CONFIG_S* cfg);

/**
 * uart������
 * @param[in]  id ���ĸ�uart������
 * @param[in]  pTargetBuf ��ȡ�����ݴ�ŵ�buffer
 * @param[in]  BufLen Ҫ��ȡ���ݵĳ���
 * @param[out]  pBytesRead ʵ�ʶ�ȡ���ݵĳ���
 * @return >= 0 ʵ�ʶ�ȡ�����ݳ���,  -1 FAIL
 */
int KING_UartRead(uint32 id, uint8* pTargetBuf,uint32 BufLen, uint32* pBytesRead);

/**
 * uartд����
 * @param[in]  id Ҫ���ĸ�uartд����
 * @param[in]  pSourceBuf д���ݴ�ŵ�buffer
 * @param[in]  BufLen Ҫд���ݵĳ���
 * @param[out]  pBytesWrite ʵ��д���ݵĳ���
 * @return >=0 ʵ��д������ݳ���,  -1 FAIL
 */
int KING_UartWrite(uint32 id,uint8* pSourceBuf,uint32 BufLen,uint32* pBytesWrite);

/**
 * ���UART���ջ��������ݡ�
 *
 * @param[in] id UART ID.
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_UartPurge(uint32 id);

/**
 * ��ѯ��ǰUART���ջ������յ��������ֽڳ��ȡ�
 *
 * @param[in] id UART ID.
 * @param[out] pAvailableBytes ��ǰ���ջ�����������ݵ��ֽڳ���
 *
 * @return >=0 ��ǰ�����������ݳ���  -1 FAIL
 */
int KING_UartGetAvailableBytes(uint32 id, uint32* pAvailableBytes);

/**
 * uartȥ��ʼ��
 * @param[in]  id ��Ҫȥ��ʼ����uart id
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_UartDeinit(uint32 id);


#endif

