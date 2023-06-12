#ifndef __KING_BT_H__
#define __KING_BT_H__
#include "kingdef.h"

#define BT_DEVICE_NAME_SIZE 22 //蓝牙名称最大22个字符
#define ADV_DATA_LEN 31        //蓝牙广播数据最大31个字节

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
    BT_UUID_DISCOVER_SERVICE_UUID, //设置Client要查找的Primary Service的UUID
    BT_UUID_TYPE_MAX
} BT_UUID_TYPE_E;

typedef struct {
    uint16 min_time; // 单位为0.625ms，范围32-16384对应20ms-10.24s
    uint16 max_time; // 单位为0.625ms，范围32-16384对应20ms-10.24s
    BT_ADV_TYPE_E type;
    BT_ADDR_TYPE_E addr; // 广播地址类型，必选
    uint8 channel; // 广播信道：bit0 - 37信道； bit1 - 38信道； bit2 - 39信道；可位或
} BT_ADV_PARAM_T;

typedef struct {
    uint8 addr[6];
} BT_ADDR_T;

typedef struct {
    uint8 enable; // BLE SCAN开关. 0: 关闭; 1: 开启
    uint8 scan_type; // 0: passive; 1: active
    uint16 interval; // 扫描间隔，单位为0.625ms，范围4-16384对应2.5ms-10.24s
    uint16 window; // 扫描窗口，单位为0.625ms，范围4-16384对应2.5ms-10.24s，小于等于扫描间隔时间
    uint8 filter_policy; // 过滤策略
    BT_ADDR_TYPE_E addr_type; // BT_ADDR_PUBLIC or BT_ADDR_RANDOM_STATIC
} BT_BLE_SCAN_PARAM_T;

typedef struct {
    uint8 name_length;
    uint8 data[32]; //当name_length大于0时，存放搜索到的蓝牙名称；当name_length为0时，存放接收到的广播数据
    uint8 addr_type;
    BT_ADDR_T bdAddress;
    uint8 event_type;
    uint8 data_length;
    int8 rssi;
} BT_SCAN_INFO;
/**
 * 初始化蓝牙
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtInit(void);

/**
 * 反初始化蓝牙
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtDeinit(void);

/**
 * 开关蓝牙
 * 
 * @param[in] enable 0   关闭蓝牙
 *                   非0 打开蓝牙
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtEnable(uint32 enable);


/**
 * 获取蓝牙开关状态
 * 
 * @param[in] state 保存获取到的蓝牙状态。 0 蓝牙已关闭， 1 蓝牙已打开
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtStateGet(uint32 *state);

/**
 * 设置蓝牙名称
 * 
 * @param[in] name   要设置的蓝牙名称
 * @param[in] len    要设置的蓝牙名称长度, 最长 BT_DEVICE_NAME_SIZE
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtNameSet(const char *name, uint32 len);

/**
 * 获取蓝牙名称
 * 
 * @param[in] name   用于保存蓝牙名称的buffer
 * @param[out] len    获取到的蓝牙名称长度, 最长 BT_DEVICE_NAME_SIZE
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtNameGet(char name[BT_DEVICE_NAME_SIZE + 1], uint32 *len);

/**
 * 设置蓝牙设备地址
 *
 * @note 如果要修改蓝牙设备地址，需要在打开蓝牙之前设置。
 * 
 * @param[in] type 地址类型
 * @param[in] addr 蓝牙地址，小端格式
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtAddrSet(BT_ADDR_TYPE_E type, const BT_ADDR_T *addr);

/**
 * 获取蓝牙设备地址
 *
 * 如果要修改蓝牙设备地址，需要在打开蓝牙之前设置。
 * 
 * @param[in] type 地址类型
 * @param[in] addr 蓝牙地址，小端格式
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtAddrGet(BT_ADDR_TYPE_E type, BT_ADDR_T *addr);

/**
 * 设置iBeacon广播数据
 *
 * @param[in]  data iBeacon协议定义的数据
 *  
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtIbeaconDataSet(const BT_IBEACON_DATA_T *data);

/**
 * 设置广播参数
 *
 * @param[in]  param 广播参数
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtAdvParamSet(const BT_ADV_PARAM_T *param);

/**
 * 设置广播数据
 *
 * @param[in]  data 用户自定义的广播数据报文
 * @param[in]  len  用户自定义的广播数据报文长度 最长BT_ADV_DATA_LEN
 *
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 */
int King_BtAdvDataSet(const uint8 *data, uint32 len);

/**
 * 开关蓝牙广播

 * @param[in] enable 0   关闭广播
 *                   非0 打开广播
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtAdvEnable(uint32 enable);

/**
 * BLE扫描控制
 *
 * @param[in]  param 扫描参数
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtBleScan(const BT_BLE_SCAN_PARAM_T *param);

/**
 * 连接指定的BLE设备
 *
 * @param[in]  addr BLE设备蓝牙地址
 * @param[in]  type 地址类型
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtBleConnect(const BT_ADDR_T *addr, BT_ADDR_TYPE_E type);

/**
 * 断开指定的BLE设备
 *
 * @param[in]  addr BLE设备蓝牙地址，如果设置为NULL，断开所有连接
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtBleDisconnect(const BT_ADDR_T *addr);

/**
 * 发送数据
 *
 * @param[in]  data 发送的数据内容
 * @param[in]  len  发送的数据长度
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BtBleSendData(const uint8 *data, uint16 len);

/**
 * 设置UUID
 *
 * @param[in]  uuid 设置的UUID
 * @param[in]  type  UUID类型
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 */
int King_BtGattUuidsSet(const uint8 uuid[16], uint8 type);

#endif

