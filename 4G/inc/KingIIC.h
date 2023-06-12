#ifndef __KINIG_IIC_H__
#define __KINIG_IIC_H__

#define I2C_BUS_0   0
#define I2C_BUS_1   1
#define I2C_BUS_2   2
#define I2C_BUS_3   3

/**
 * ���ߴ���������ض����ΪԤ����
 * ƽ̨�ɾ���Ĭ��ʹ�ú��ִ������ʣ������Ƿ������û��������ʡ�
 * Becareful if there are more than one slave devices on the bus!
 */
#define I2C_DEV_FREQ_GET    0x0     /* get frequency */
#define I2C_DEV_FREQ_SET    0x1     /* set frequency */
#define I2C_DEV_CFG_REGLEN  0x2     /* ���ô��豸�ڲ��Ĵ�����ַ��� */

#define I2C_FREQ_NORMAL     0 /* 100K bps */
#define I2C_FREQ_FAST       1 /* 400K bps */
#define I2C_FREQ_HIGH       2 /* 3.5M bps */

#define I2C_DEV_REGLEN_8    (8)     // 1byte
#define I2C_DEV_REGLEN_16   (16)    // 2bytes

#define I2C_10BITS_ADDR_MASK     (0x7C00)
#define I2C_10BITS_ADDR_MAGIC    (0x7800) //0b 0111 10xx  xxxx xxxx

typedef struct
{
    uint32  bus;        ///< I2C bus ID
    uint16  slave_addr; ///< 7bits or 10bits slave addr (SAD). Not include RD/WR bit!
    uint32  cmd;        ///< I2C_DEV_FREQ_GET/I2C_DEV_FREQ_SET/I2C_DEV_CFG_REGLEN
    uint32* data;       ///< Set/Get data relative to the cmd.
} I2C_DEV_CONFIG_S;


/**
 * I2c��ʼ������Ҫ�����IIC�������ڲ��Ĵ�����ʼ����
 * ƽ̨���Ծ��������ʵ�֡����磺�ײ��Ѿ���ʼ������
 * ����ֱ�ӷ��سɹ�������ƽ̨��֧��IIC������ֱ�ӷ���ʧ�ܡ�
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_IicInit(void);

/**
 * I2cȥ��ʼ����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_IicDeinit(void);

/**
 * ��IIC����
 *
 * @param[in]  bus_id I2C bus id, which presentes as a specific i2c bus connects to a I2C module.
 *             ����֧����Щid����ο���Ӧ��Ӳ������顣
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_IicOpen(uint32 bus_id);

/**
 * ��IIC�����ϵ�ĳ���豸
 *
 * @param[in]  bus_id        IIC bus ID
 * @param[in]  slave_addr    7bits or 10bits slave addr (SAD). Not include RD/WR bit!
 * @param[in]  reg_addr      register address to write in slave device. can be 1byte or 2bytes. 
 * @param[in]  reg_len       length of register address(reg_addr).
 * @param[in]  buffer        read data buffer.
 * @param[in]  bufLen        read data length.
 * @return >= 0 ʵ�ʶ�ȡ�����ݳ���,  -1 FAIL
 */
int KING_IicRead(uint32 bus_id, uint16 slave_addr, uint8* reg_addr, uint32 reg_len, uint8 *buffer, uint32 bufLen);

/**
 * дIIC�����ϵ�ĳ���豸
 *
 * @param[in]  bus_id        IIC bus ID
 * @param[in]  slave_addr    7bits or 10bits slave addr (SAD). Not include RD/WR bit!
 * @param[in]  reg_addr      register address to write in slave device. can be 1byte or 2bytes.
 * @param[in]  buffer        write data buffer.
 * @param[in]  bufLen        write data length.
 * @return >=0 ʵ��д������ݳ���,  -1 FAIL
 */
int KING_IicWrite(uint32 bus_id, uint16 slave_addr, uint8* reg_addr, uint8 *buffer, uint32 bufLen);

/**
 * ����IIC�����ϴ��豸��ز���
 *
 * @param[in] cfgData   I2C_DEV_CONFIG_S config data.
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_IicConfig(I2C_DEV_CONFIG_S* cfgData);


/**
 * �ر�IIC����
 *
 * @param[in]   bus_id   IIC ����ID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_IicClose(uint32 bus_id);

/**
 * ��IIC�����ϵĵ��ֽ�����
 *
 * @param[in]  bus_id        IIC bus ID
 * @param[in]  start        IIC start condition.
 * @param[in]  stop        IIC stop condition.
 * @param[in]  data        read data buffer.
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_IicReceiveByte(uint32 bus_id,bool start, bool stop,uint8 *data);

/**
 * д���ֽ����ݵ�IIC����
 *
 * @param[in]  bus_id        IIC bus ID
 * @param[in]  data        write data buffer.
 * @param[in]  start       IIC start condition.
 * @param[in]  stop        IIC stop condition.
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_IicSendByte(uint32 bus_id,uint8 data, bool start, bool stop);


#endif

