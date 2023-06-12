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
 * spi��ʼ��
 *
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_SpiInit(void);

/**
 * spiȥ��ʼ��
 *
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_SpiDeinit(void);

/**
 * spi ���豸id
 *
 * @return 0 SUCCESS  -1 FAIL
 */
 
int KING_SpiOpen(uint32 id);

/**
 * spi �ر��豸id
 *
 * @return 0 SUCCESS  -1 FAIL
 */
 
int KING_SpiClose(uint32 id);

/**
 * spi�������
 *
 * @param[in] id  �豸�߼�id (����ж���������ôÿ�����߰���8���߼�id��0-7 spibus0 / 8-15 spibus1)
 * @param[in] cfg spi�豸����
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_SpiConfig(uint32 id, SPI_CONFIG_S* cfg);

/**
 * spi���ݶ�ȡ
 *
 * @param[in]  id         �豸�߼�id (����ж���������ôÿ�����߰���8���߼�id��0-7 spibus0 / 8-15 spibus1)
 * @param[in]  pWriteBuf  д���ݵ�pWriteBufָ��
 * @param[in]  writeLen   д���ݵ�writeLen����
 * @param[in]  pReadBuf   �����ݵ�pReadBufָ��
 * @param[in]  readLen    �����ݵ�readLen����
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_SpiRead(uint32 id, uint8* pWriteBuf,uint32 writeLen, uint8* pReadBuf,uint32 readLen);

/**
 * spi���ݶ�ȡ
 *
 * @param[in]  id         �豸�߼�id (����ж���������ôÿ�����߰���8���߼�id��0-7 spibus0 / 8-15 spibus1)
 * @param[in]  pWriteBuf  �洢д���ݵ�pWriteBufָ��
 * @param[in]  writeLen   �洢д���ݵ�writeLen����
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_SpiWrite(uint32 id, uint8* pWriteBuf, uint32 writeLen);

#endif
