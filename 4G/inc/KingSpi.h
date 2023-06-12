#ifndef __KING_SPI_H__
#define __KING_SPI_H__


/*available spi bit length*/
#define SPI_BIT_LEN8    8
#define SPI_BIT_LEN16   16
#define SPI_BIT_LEN_32  32


typedef enum
{
    SPI_CPOL0_CPHA0 = 0,    ///< sampling on rising edge, clk idle '0'
    SPI_CPOL0_CPHA1,        ///< sampling on falling edge, clk idle '0'
    SPI_CPOL1_CPHA0,        ///< sampling on falling edge, clk idle '1'
    SPI_CPOL1_CPHA1,        ///< sampling on rising edge, clk idle '1'
} SPI_MODE_E;

typedef struct
{
    SPI_MODE_E mode;
    uint32 tx_bit_length;   ///< 8bits/16bits/32bits
    uint32 freq;            ///< default is 5000000(5M)
} SPI_CONFIG_S;

/**
 * spi初始化
 *
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_SpiInit(void);

/**
 * spi去初始化
 *
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_SpiDeinit(void);

/**
 * spi 打开设备id
 *
 * @return 0 SUCCESS  -1 FAIL
 */
 
int KING_SpiOpen(uint32 id);

/**
 * spi 关闭设备id
 *
 * @return 0 SUCCESS  -1 FAIL
 */
 
int KING_SpiClose(uint32 id);

/**
 * spi参数变更
 *
 * @param[in] id  设备逻辑id (如果有多条总线那么每条总线包含8个逻辑id，0-7 spibus0 / 8-15 spibus1)
 * @param[in] cfg spi设备参数
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_SpiConfig(uint32 id, SPI_CONFIG_S* cfg);

/**
 * spi数据读取
 *
 * @param[in]  id         设备逻辑id (如果有多条总线那么每条总线包含8个逻辑id，0-7 spibus0 / 8-15 spibus1)
 * @param[in]  pWriteBuf  写数据的pWriteBuf指针
 * @param[in]  writeLen   写数据的writeLen长度
 * @param[in]  pReadBuf   读数据的pReadBuf指针
 * @param[in]  readLen    读数据的readLen长度
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_SpiRead(uint32 id, uint8* pWriteBuf,uint32 writeLen, uint8* pReadBuf,uint32 readLen);

/**
 * spi数据读取
 *
 * @param[in]  id         设备逻辑id (如果有多条总线那么每条总线包含8个逻辑id，0-7 spibus0 / 8-15 spibus1)
 * @param[in]  pWriteBuf  存储写数据的pWriteBuf指针
 * @param[in]  writeLen   存储写数据的writeLen长度
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_SpiWrite(uint32 id, uint8* pWriteBuf, uint32 writeLen);

#endif
