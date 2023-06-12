#ifndef __KING_UART_H__
#define __KING_UART_H__


#define UART_0       (0)
#define UART_1       (1)
#define UART_2       (2)
#define UART_3       (3)
#define UART_4       (4)
#define UART_5       (5)

// UART event type. NOTIFY_FUN中使用
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
 * uart初始化，配置uart相关参数
 * @param[in]  id 需要初始化的uart id，支持哪些id，请查看对应的硬件规格书
 * @param[in]  cfg 需要配置的uart参数
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_UartInit(uint32 id, UART_CONFIG_S* cfg);

/**
 * uart读数据
 * @param[in]  id 从哪个uart读数据
 * @param[in]  pTargetBuf 读取的数据存放的buffer
 * @param[in]  BufLen 要读取数据的长度
 * @param[out]  pBytesRead 实际读取数据的长度
 * @return >= 0 实际读取的数据长度,  -1 FAIL
 */
int KING_UartRead(uint32 id, uint8* pTargetBuf,uint32 BufLen, uint32* pBytesRead);

/**
 * uart写数据
 * @param[in]  id 要往哪个uart写数据
 * @param[in]  pSourceBuf 写数据存放的buffer
 * @param[in]  BufLen 要写数据的长度
 * @param[out]  pBytesWrite 实际写数据的长度
 * @return >=0 实际写入的数据长度,  -1 FAIL
 */
int KING_UartWrite(uint32 id,uint8* pSourceBuf,uint32 BufLen,uint32* pBytesWrite);

/**
 * 清空UART接收缓冲区数据。
 *
 * @param[in] id UART ID.
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_UartPurge(uint32 id);

/**
 * 查询当前UART接收缓冲区收到的数据字节长度。
 *
 * @param[in] id UART ID.
 * @param[out] pAvailableBytes 当前接收缓冲区存放数据的字节长度
 *
 * @return >=0 当前缓冲区的数据长度  -1 FAIL
 */
int KING_UartGetAvailableBytes(uint32 id, uint32* pAvailableBytes);

/**
 * uart去初始化
 * @param[in]  id 需要去初始化的uart id
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_UartDeinit(uint32 id);


#endif

