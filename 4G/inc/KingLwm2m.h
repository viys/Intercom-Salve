#ifndef __KING_LWM2M_H__
#define __KING_LWM2M_H__

#include "KingDef.h"

typedef void* LWM2M_HANDLE;

#define LWM2M_HOST_STR_SIZE          128
#define LWM2M_ENDPOINT_STR_SIZE      128

// LWM2M Object ID List
#define LWM2M_OBJ_ID_0      0   // Security v1.1
#define LWM2M_OBJ_ID_1      1   // Server v1.1
#define LWM2M_OBJ_ID_2      2   // Access Control v1.0
#define LWM2M_OBJ_ID_3      3   // Device v1.1
#define LWM2M_OBJ_ID_4      4   // Connectivity Monitoring v1.2
#define LWM2M_OBJ_ID_5      5   // Firmware Update v1.0
#define LWM2M_OBJ_ID_6      6   // Location v1.0
#define LWM2M_OBJ_ID_7      7   // Connectivity Statistics v1.0
#define LWM2M_OBJ_ID_21     21  // OSCORE v1.0


typedef enum {
    RES_DATA_TYPE_STRING    = 1,
    RES_DATA_TYPE_INTEGER,
    RES_DATA_TYPE_FLOAT,
    RES_DATA_TYPE_BOOL,
    RES_DATA_TYPE_OPAQUE,
    RES_DATA_TYPE_TIME,
    RES_DATA_TYPE_OBJLNK,
    RES_DATA_TYPE_CORELNK,
    RES_DATA_TYPE_NONE,
} LWM2M_RES_DATA_TYPE;

typedef enum {
    LWM2M_BOOTSTRAP_START = 1,
    LWM2M_BOOTSTRAP_SUCCESS,
    LWM2M_BOOTSTRAP_FAILED,
    LWM2M_CONNECT_SUCCESS,
    LWM2M_CONNECT_FAILED,
    LWM2M_REGISTER_SUCCESS,
    LWM2M_REGISTER_FAILED,
    LWM2M_REGISTER_TIMEOUT,
    LWM2M_LIFETIME_TIMEOUT,
    LWM2M_STATUS_HALT,
    LWM2M_UPDATE_SUCCESS,
    LWM2M_UPDATE_FAILED,
    LWM2M_UPDATE_TIMEOUT,
    LWM2M_RESPONSE_FAILED,
    LWM2M_NOTIFY_FAILED,
    LWM2M_CLOSE_SUCCESS,
    LWM2M_CLOSE_FAILED,
    LWM2M_READ_IND,
    LWM2M_WRITE_IND,
    LWM2M_EXECUTE_IND,
    LWM2M_OBSERVE_IND,
    LWM2M_SETPARAM_IND,
    LWM2M_DISCOVER_IND,
} LWM2M_EVENT_TYPE;


typedef enum
{
    LWM2M_SECMODE_NONE = 0,
    LWM2M_SECMODE_DTLS,
} LWM2M_SECURITY_MODE;

typedef struct
{
    LWM2M_SECURITY_MODE security_mode;
    char *pskid;
    char *psk;
} LWM2M_SECURITY_INFO;


typedef struct lwm2m_msg_head_t
{
    uint32 msgId;
    uint16 objId;
    uint16 objInsId;
    uint16 resId;
    uint16 resInsId; ///< resource instance ID. ignore if not used
} LWM2M_MSG_HEAD;

typedef struct{
    LWM2M_MSG_HEAD read_content;
} LWM2M_EVENT_READ_CONTENT;

typedef struct{
    LWM2M_MSG_HEAD write_head;
    LWM2M_RES_DATA_TYPE type;
    uint16 len;
    uint8 *data;
    uint8 count;
} LWM2M_EVENT_WRITE_CONTENT;

typedef struct{
    LWM2M_MSG_HEAD exe_head;
    uint16 len;
    uint8 *data;
} LWM2M_EVENT_EXECUTE_CONTENT;

typedef struct{
    LWM2M_MSG_HEAD obs_head;
    uint8 isAddObserve;
} LWM2M_EVENT_OBSERVE_CONTENT;

typedef struct{
    LWM2M_MSG_HEAD set_head;
    uint16 len;
    uint8 *data;
} LWM2M_EVENT_SETPARAM_CONTENT;

typedef struct{
    LWM2M_MSG_HEAD discover_content;
} LWM2M_EVENT_DISCOVER_CONTENT;


typedef void (*LWM2M_CALLBACK)(LWM2M_HANDLE lwm2m_handle, LWM2M_EVENT_TYPE event_type, void* event_content);

typedef struct
{
    char hostname[LWM2M_HOST_STR_SIZE + 1];
    uint16 port;
    char endpoint[LWM2M_ENDPOINT_STR_SIZE + 1];
    uint16 lifetime;
    uint16 timeout;
    LWM2M_SECURITY_INFO security_info;
    LWM2M_CALLBACK callback; ///< callback for operation (such as: read/write/execute/...) event.
} LWM2M_CONNECT_INFO;



/**
 * LWM2M INIT.
 * If platform supports LWM2M, this api returns 0, and supported
 * LWM2M protocol version is returned in pro_version.
 *
 * @param[out] pro_version protocol version string.
 * @return 0 SUCCESS, -1 FAIL
 */
int KING_Lwm2mInit(char **pro_version);

int KING_Lwm2mCreate(LWM2M_HANDLE* lwm2m_handle, LWM2M_CONNECT_INFO conn_info);

int KING_Lwm2mDelete(LWM2M_HANDLE lwm2m_handle);

int KING_Lwm2mOpen(LWM2M_HANDLE lwm2m_handle);

int KING_Lwm2mClose(LWM2M_HANDLE lwm2m_handle);

int KING_Lwm2mUpdate(LWM2M_HANDLE lwm2m_handle, uint32 lifetime_update, bool updateobj);

int KING_Lwm2mAddObj(LWM2M_HANDLE lwm2m_handle, uint16 objId, uint16 objInsId, uint8 resnum, char* res);

int KING_Lwm2mDelObj(LWM2M_HANDLE lwm2m_handle,uint16 objId);

int KING_Lwm2mNotifyString(LWM2M_HANDLE lwm2m_handle, LWM2M_MSG_HEAD msgHead, char* value, uint8 index, uint8 flag, uint16 ackid);

int KING_Lwm2mNotifyOpaque(LWM2M_HANDLE lwm2m_handle, LWM2M_MSG_HEAD msgHead, char* value, uint8 index, uint8 flag, uint16 ackid);

int KING_Lwm2mNotifyInteger(LWM2M_HANDLE lwm2m_handle, LWM2M_MSG_HEAD msgHead, uint32 value, uint8 index, uint8 flag, uint16 ackid);

int KING_Lwm2mNotifyFloat(LWM2M_HANDLE lwm2m_handle, LWM2M_MSG_HEAD msgHead, double value, uint8 index, uint8 flag, uint16 ackid);

int KING_Lwm2mNotifyBool(LWM2M_HANDLE lwm2m_handle, LWM2M_MSG_HEAD msgHead, bool value, uint8 index, uint8 flag, uint16 ackid);

int KING_Lwm2mReadRspString(LWM2M_HANDLE lwm2m_handle, LWM2M_MSG_HEAD msgHead, char* value, uint8 index, uint8 flag);

int KING_Lwm2mReadRspOpaque(LWM2M_HANDLE lwm2m_handle, LWM2M_MSG_HEAD msgHead, char* value, uint8 index, uint8 flag);

int KING_Lwm2mReadRspInteger(LWM2M_HANDLE lwm2m_handle, LWM2M_MSG_HEAD msgHead, uint32 value, uint8 index, uint8 flag);

int KING_Lwm2mReadRspFloat(LWM2M_HANDLE lwm2m_handle, LWM2M_MSG_HEAD msgHead, double value, uint8 index, uint8 flag);

int KING_Lwm2mReadRspBool(LWM2M_HANDLE lwm2m_handle, LWM2M_MSG_HEAD msgHead, bool value, uint8 index, uint8 flag);

int KING_Lwm2mWriteRsp(LWM2M_HANDLE lwm2m_handle, uint32 msgId, uint8 result);

int KING_Lwm2mExecuteRsp(LWM2M_HANDLE lwm2m_handle, uint8 result);

int KING_Lwm2mObserveRsp(LWM2M_HANDLE lwm2m_handle, uint32 msgId, uint8 result);

int KING_Lwm2mDiscoverRsp(LWM2M_HANDLE lwm2m_handle, uint32 msgId, uint8 result, char* value);

int KING_Lwm2mDeinit(void);

#endif

