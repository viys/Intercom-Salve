#ifndef __KING_BT_H__
#define __KING_BT_H__
#include "kingdef.h"

#define BT_DEVICE_NAME_SIZE 22 //�����������22���ַ�
#define ADV_DATA_LEN 31        //�����㲥�������31���ֽ�

typedef enum
{
    BT_BLE_CLI_SCAN_RSP_IND = 0,
    BT_BLE_CLI_CONNECT_IND,
    BT_BLE_CLI_DISCONNECT_IND,
    BT_BLE_SRV_CONNECT_IND,
    BT_BLE_SRV_DISCONNECT_IND,
    BT_BLE_SEND_DATA_COMPLETE_IND,
    BT_BLE_RECV_DATA_IND,
    BT_BLE_MTU_IND,
    BT_CHIP_RESET_IND,
    BT_CALLBACK_EVENT_MAX = 0x7FFFFFFF
} BT_CALLBACK_EVENT_E;

typedef enum
{
    BT_ADDR_MAC = 0,
    BT_ADDR_PUBLIC = BT_ADDR_MAC,
    BT_ADDR_RANDOM_STATIC,
    BT_ADDR_RANDOM_PRIVATE_RESOLVABLE,
    BT_ADDR_RANDOM_PRIVATE_NON_RESOLVABLE,
    BT_ADDR_MAX = 0x7FFFFFFF
} BT_ADDR_TYPE_E;

typedef struct {
    uint8 uuid[16];
    uint8 major[2];
    uint8 minor[2];
} BT_IBEACON_DATA_T;

typedef enum
{
    BT_ADV_IND = 0,
    BT_ADV_CONN_HIGH_DUTY_DIRECT,
    BT_ADV_SCAN_IND,
    BT_ADV_NONCONN_IND,
    BT_ADV_CONN_LOW_DUTY_DIRECT,
    BT_ADV_MAX = 0x7FFFFFFF
} BT_ADV_TYPE_E;

typedef enum
{
    BT_UUID_PRIMARY_SERVICE = 0,
    BT_UUID_RW_CHARACTERISTIC,
    BT_UUID_NOTIFY_CHARACTERISTIC,
    BT_UUID_DISCOVER_SERVICE_UUID, //����ClientҪ���ҵ�Primary Service��UUID
    BT_UUID_TYPE_MAX
} BT_UUID_TYPE_E;

typedef struct {
    uint16 min_time; // ��λΪ0.625ms����Χ32-16384��Ӧ20ms-10.24s
    uint16 max_time; // ��λΪ0.625ms����Χ32-16384��Ӧ20ms-10.24s
    BT_ADV_TYPE_E type;
    BT_ADDR_TYPE_E addr; // �㲥��ַ���ͣ���ѡ
    uint8 channel; // �㲥�ŵ���bit0 - 37�ŵ��� bit1 - 38�ŵ��� bit2 - 39�ŵ�����λ��
} BT_ADV_PARAM_T;

typedef struct {
    uint8 addr[6];
} BT_ADDR_T;

typedef struct {
    uint8 enable; // BLE SCAN����. 0: �ر�; 1: ����
    uint8 scan_type; // 0: passive; 1: active
    uint16 interval; // ɨ��������λΪ0.625ms����Χ4-16384��Ӧ2.5ms-10.24s
    uint16 window; // ɨ�贰�ڣ���λΪ0.625ms����Χ4-16384��Ӧ2.5ms-10.24s��С�ڵ���ɨ����ʱ��
    uint8 filter_policy; // ���˲���
    BT_ADDR_TYPE_E addr_type; // BT_ADDR_PUBLIC or BT_ADDR_RANDOM_STATIC
} BT_BLE_SCAN_PARAM_T;

typedef struct {
    uint8 name_length;
    uint8 data[32]; //��name_length����0ʱ��������������������ƣ���name_lengthΪ0ʱ����Ž��յ��Ĺ㲥����
    uint8 addr_type;
    BT_ADDR_T bdAddress;
    uint8 event_type;
    uint8 data_length;
    int8 rssi;
} BT_SCAN_INFO;
/**
 * ��ʼ������
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtInit(void);

/**
 * ����ʼ������
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtDeinit(void);

/**
 * ��������
 * 
 * @param[in] enable 0   �ر�����
 *                   ��0 ������
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtEnable(uint32 enable);


/**
 * ��ȡ��������״̬
 * 
 * @param[in] state �����ȡ��������״̬�� 0 �����ѹرգ� 1 �����Ѵ�
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtStateGet(uint32 *state);

/**
 * ������������
 * 
 * @param[in] name   Ҫ���õ���������
 * @param[in] len    Ҫ���õ��������Ƴ���, � BT_DEVICE_NAME_SIZE
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtNameSet(const char *name, uint32 len);

/**
 * ��ȡ��������
 * 
 * @param[in] name   ���ڱ����������Ƶ�buffer
 * @param[out] len    ��ȡ�����������Ƴ���, � BT_DEVICE_NAME_SIZE
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtNameGet(char name[BT_DEVICE_NAME_SIZE + 1], uint32 *len);

/**
 * ���������豸��ַ
 *
 * @note ���Ҫ�޸������豸��ַ����Ҫ�ڴ�����֮ǰ���á�
 * 
 * @param[in] type ��ַ����
 * @param[in] addr ������ַ��С�˸�ʽ
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtAddrSet(BT_ADDR_TYPE_E type, const BT_ADDR_T *addr);

/**
 * ��ȡ�����豸��ַ
 *
 * ���Ҫ�޸������豸��ַ����Ҫ�ڴ�����֮ǰ���á�
 * 
 * @param[in] type ��ַ����
 * @param[in] addr ������ַ��С�˸�ʽ
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtAddrGet(BT_ADDR_TYPE_E type, BT_ADDR_T *addr);

/**
 * ����iBeacon�㲥����
 *
 * @param[in]  data iBeaconЭ�鶨�������
 *  
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtIbeaconDataSet(const BT_IBEACON_DATA_T *data);

/**
 * ���ù㲥����
 *
 * @param[in]  param �㲥����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtAdvParamSet(const BT_ADV_PARAM_T *param);

/**
 * ���ù㲥����
 *
 * @param[in]  data �û��Զ���Ĺ㲥���ݱ���
 * @param[in]  len  �û��Զ���Ĺ㲥���ݱ��ĳ��� �BT_ADV_DATA_LEN
 *
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 */
int King_BtAdvDataSet(const uint8 *data, uint32 len);

/**
 * ���������㲥

 * @param[in] enable 0   �رչ㲥
 *                   ��0 �򿪹㲥
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtAdvEnable(uint32 enable);

/**
 * BLEɨ�����
 *
 * @param[in]  param ɨ�����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtBleScan(const BT_BLE_SCAN_PARAM_T *param);

/**
 * ����ָ����BLE�豸
 *
 * @param[in]  addr BLE�豸������ַ
 * @param[in]  type ��ַ����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtBleConnect(const BT_ADDR_T *addr, BT_ADDR_TYPE_E type);

/**
 * �Ͽ�ָ����BLE�豸
 *
 * @param[in]  addr BLE�豸������ַ���������ΪNULL���Ͽ���������
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtBleDisconnect(const BT_ADDR_T *addr);

/**
 * ��������
 *
 * @param[in]  data ���͵���������
 * @param[in]  len  ���͵����ݳ���
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtBleSendData(const uint8 *data, uint16 len);

/**
 * ����UUID
 *
 * @param[in]  uuid ���õ�UUID
 * @param[in]  type  UUID����
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 */
int King_BtGattUuidsSet(const uint8 uuid[16], uint8 type);

#endif

