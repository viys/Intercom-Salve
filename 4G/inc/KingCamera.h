#ifndef __KING_CAMERA_H__
#define __KING_CAMERA_H__

#define CAMERA_EVENT_SCANCODE_START     0x0001
#define CAMERA_EVENT_SCANCODE_RESULT    0x0002
#define CAMERA_EVENT_SCANCODE_STOP      0x0003
#define CAMERA_EVENT_CAPTURE            0x0004

typedef enum
{
    CAMERA_ONE_SHOT = 0,
    CAMERA_CONSECUTIVELY,
    CAMERA_MODEL_MAX = 0x7FFFFFFF
} CAMERA_MODEL_E;

typedef enum
{
    CAMERA_CODE_AUTO = 0 << 0,
    CAMERA_CODE_EAN2 = 1 << 0,
    CAMERA_CODE_EAN5 = 1 << 1,
    CAMERA_CODE_EAN8 = 1 << 2,
    CAMERA_CODE_UPCE = 1 << 3,
    CAMERA_CODE_ISBN10 = 1 << 4,
    CAMERA_CODE_UPCA = 1 << 5,
    CAMERA_CODE_EAN13 = 1 << 6,
    CAMERA_CODE_ISBN13 = 1 << 7,
    CAMERA_CODE_EANUPC = 1 << 8,
    CAMERA_CODE_I25 = 1 << 9,
    CAMERA_CODE_DATABAR = 1 << 10,
    CAMERA_CODE_DATABAR_EXP = 1 << 11,
    CAMERA_CODE_CODABAR = 1 << 12,
    CAMERA_CODE_CODE39 = 1 << 13,
    CAMERA_CODE_PDF417 = 1 << 14,
    CAMERA_CODE_QR = 1 << 15,
    CAMERA_CODE_CODE93 = 1 << 16,
    CAMERA_CODE_CODE128 = 1 << 17,
    CAMERA_CODE_DM = 1 << 18,
    CAMERA_CODE_MAX = 0x7FFFFFFF
} CAMERA_CODE_E;

typedef enum
{
    CAMERA_PARA_EXPOSURE = 0,
    CAMERA_PARA_BRIGHTNESS,
    CAMERA_PARA_CONTRAST,
    CAMERA_PARA_AWB,
    CAMERA_PARA_PREVIEW_ON_LCD,
    CAMERA_PARA_MAX = 0x7FFFFFFF
} CAMERA_PARA_E;

typedef struct
{
    uint8 addr;
    uint8 data;
} CAMERA_REG_S;

typedef struct _tagCamEventData {
  uint16    eventID;
  void *    data;
} CAMERA_DATA_S;

typedef struct
{
    uint16 *data;
    uint32 img_width;
    uint32 img_height;
} CAM_CAPTURE_DATA_S;

typedef void (*CameraCallback)(void *data);


/**
 * camera初始化。
 * 平台可以决定具体的实现。例如：底层已经初始化过，
 * 可以直接返回成功。或者平台不支持，可以直接返回失败。
 *
 * @param[in]  regList  用户定制的sensor初始化参数
 * @param[in]  len       初始化参数的条数
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_CameraInit(CAMERA_REG_S *regList, uint32 len);

/**
 * camera去初始化。
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_CameraDeinit(void);

/**
 * 设置camera配置
 * @param[in]  param camera 属性
 * @param[in]  value  属性对应的值
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_Camera_ParameterSet(CAMERA_PARA_E param, uint32 value);

/**
 * 获取camera配置
 * @param[in]  param camera 属性
 * @param[out]  pValue  属性对应的值
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_Camera_ParameterGet(CAMERA_PARA_E param, uint32 *pValue);


/**
 * camera启动
 *
 * @param[in]  model  camera工作模式
 *                   CAMERA_ONE_SHOT:cb 收到event为start->result->stop
 *               CAMERA_CONSECUTIVELY: cb 收到event为start->result->result.....    
 * @param[in]  code camera扫码的目标码制
 * @param[in]  cb  回调函数
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_CameraStart(CAMERA_MODEL_E model, CAMERA_CODE_E code, CameraCallback cb);


/**
 * camera停止
 * KING_CameraStart中设置的回调没有接收到过stop event，会收到stop event
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_CameraStop(void);

/**
 * camera拍照
 *
 * @param[in]  cb  回调函数
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 */
int KING_CameraCapture(CameraCallback cb);
#endif

