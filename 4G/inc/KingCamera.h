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
 * camera��ʼ����
 * ƽ̨���Ծ��������ʵ�֡����磺�ײ��Ѿ���ʼ������
 * ����ֱ�ӷ��سɹ�������ƽ̨��֧�֣�����ֱ�ӷ���ʧ�ܡ�
 *
 * @param[in]  regList  �û����Ƶ�sensor��ʼ������
 * @param[in]  len       ��ʼ������������
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_CameraInit(CAMERA_REG_S *regList, uint32 len);

/**
 * cameraȥ��ʼ����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_CameraDeinit(void);

/**
 * ����camera����
 * @param[in]  param camera ����
 * @param[in]  value  ���Զ�Ӧ��ֵ
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_Camera_ParameterSet(CAMERA_PARA_E param, uint32 value);

/**
 * ��ȡcamera����
 * @param[in]  param camera ����
 * @param[out]  pValue  ���Զ�Ӧ��ֵ
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_Camera_ParameterGet(CAMERA_PARA_E param, uint32 *pValue);


/**
 * camera����
 *
 * @param[in]  model  camera����ģʽ
 *                   CAMERA_ONE_SHOT:cb �յ�eventΪstart->result->stop
 *               CAMERA_CONSECUTIVELY: cb �յ�eventΪstart->result->result.....    
 * @param[in]  code cameraɨ���Ŀ������
 * @param[in]  cb  �ص�����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_CameraStart(CAMERA_MODEL_E model, CAMERA_CODE_E code, CameraCallback cb);


/**
 * cameraֹͣ
 * KING_CameraStart�����õĻص�û�н��յ���stop event�����յ�stop event
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_CameraStop(void);

/**
 * camera����
 *
 * @param[in]  cb  �ص�����
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 */
int KING_CameraCapture(CameraCallback cb);
#endif

