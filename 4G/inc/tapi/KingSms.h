#ifndef __KING_SMS_H__
#define __KING_SMS_H__

#include "kingUsim.h"

#define SMS_EVENT_SMS_INIT_STATUS   0x0001
#define SMS_EVENT_NEW_SMS_IND       0x0002
#define SMS_EVENT_SMS_INFO_IND      0x0003
#define SMS_EVENT_CB_MSG_IND        0x0004
#define SMS_EVENT_CB_PAGE_IND       0x0005

typedef enum _tagSmsEventState {
    SMS_EVENT_STATE_NONE,
    SMS_EVENT_STATE_INIT,    
    SMS_EVENT_STATE_NEW_SMS,
    SMS_EVENT_STATE_INFO,
    SMS_EVENT_STATE_MAX = 0x7FFFFFFF
} SMS_EVENT_STATE_E;

typedef enum{
    SMS_OPTIONMASK_NO_OPTION = 0x0000, ///< No option
    SMS_OPTIONMASK_OA        = 0x0001, ///< TP Originating Address is present
    SMS_OPTIONMASK_SCTS      = 0x0002, ///< TP Service Centre Time Stamp is present
    SMS_OPTIONMASK_DA        = 0x0004, ///< TP Destination Address is present
    SMS_OPTIONMASK_RA        = 0x0008  ///< TP Recipient Address is present
}SMS_OPTION;

typedef enum
{
    SMS_DELIVER        = 0, ///< SMS-DELIVER (in the direction Service Center
                               ///< to Mobile station).
    SMS_SUBMIT         = 1, ///< SMS-SUBMIT (in the direction Mobile station to Service
                               ///< Center).
    SMS_STATUS_REPORT  = 2, ///< SMS-STATUS-REPORT.
    SMS_PDU            = 3, ///< PDU message.
    SMS_CELL_BROADCAST = 4, ///< SMS Cell Broadcast.
    SMS_UNSUPPORTED    = 5  ///< SMS with unsupported format or encoding.
}SMS_MSG_TYPE;

typedef enum
{
    SMS_PROTOCOL_UNKNOWN = 0,  ///< Unknown message protocol.
    SMS_PROTOCOL_GSM     = 1,  ///< GSM message protocol.
    SMS_PROTOCOL_CDMA    = 2,  ///< CDMA message protocol.
    SMS_PROTOCOL_GW_CB   = 3,   ///< GW Cell Broadcast message protocol.
    SMS_PROTOCOL_PS    = 4   ///< Packet switch domain message protocol.
}SMS_MSG_PROTOCOL;

typedef enum{
    SMS_STORAGE_BROADCAST,// Broadcast message storage location. 
    SMS_STORAGE_SIM,// SIM storage location. 
}SIM_SMSSTORAGE;

//--------------------------------------------------------------------------------------------------
/**
 * 3GPP2 Message Error code when the message sending has failed.
 */
//--------------------------------------------------------------------------------------------------
typedef enum {
    ////< SMS Cause code specified in the 3GPP2 N.S0005-0 section 6.5.2.125
    ERROR_ADDR_VACANT,                   ///< The SMS Destination Address is valid but is not.
                                         ///<  currently allocated to an SMS terminal.
    ERROR_ADDR_TRANSLATION_FAILURE,      ///< The SMS Destination Address is invalid.
    ERROR_NETWORK_RESOURCE_SHORTAGE,     ///< The network transmission failed due to lack of
                                         ///<  a network resource or link capacity.
    ERROR_NETWORK_FAILURE,               ///< A network node failed, a link failed, or a
                                         ///< required operation failed.
    ERROR_INVALID_TELESERVICE_ID,        ///< The SMS_TeleserviceIdentifier is not known, is
                                         ///<  not supported, or is not authorized by an
                                         ///<  addressed functional entity.
    ERROR_NETWORK_OTHER,                 ///< A network problem other than identified above.
    ERROR_NO_PAGE_RESPONSE,              ///< The addressed MS-based SME is known, but it
                                         ///<  does not respond to a page.
    ERROR_DEST_BUSY,                     ///< The destination MS-based SME is SMS capable,
                                         ///<  but is currently engaged in a call, a service,
                                         ///<  or a call mode that precludes the use of SMS,
                                         ///<  or the destination SME is congested.
    ERROR_NO_ACK,                        ///< The destination SME does not acknowledge receipt
                                         ///<  of the SMS delivery.
    ERROR_DEST_RESOURCE_SHORTAGE,        ///< A required terminal resource is not available to
                                         ///<  process this message.
    ERROR_SMS_DELIVERY_POSTPONED,        ///< Delivery is not currently possible.
    ERROR_DEST_OUT_OF_SERV,              ///< The addressed destination is out of
                                         ///<  service for an extended period of time.
    ERROR_DEST_NOT_AT_ADDR,              ///< The MS-based SME is no longer at the
                                         ///< temporary SMS routing address.
    ERROR_DEST_OTHER,                    ///< A terminal problem other than described above.
    ERROR_RADIO_IF_RESOURCE_SHORTAGE,    ///< There is no channel available or there is
                                         ///< radio congestion at this time.
    ERROR_RADIO_IF_INCOMPATABILITY,      ///< The MS for an MS-based SME is operating in a
                                         ///<  mode that does not support SMS at this time.
    ERROR_RADIO_IF_OTHER,                ///< A radio interface problem to an MS-based SME
                                         ///<  other than described above.
    ERROR_ENCODING,                      ///< The size of a parameter or field is not
                                         ///<  what is expected.
    ERROR_SMS_ORIG_DENIED,               ///< The originating MIN is not recognized.
    ERROR_SMS_TERM_DENIED,               ///< The destination is not authorized to receive
                                         ///<  the SMS message.
    ERROR_SUPP_SERV_NOT_SUPP,            ///< The originating supplementary service is
                                         ///<  not known or supported.
    ERROR_SMS_NOT_SUPP,                  ///< The originating supplementary service is
                                         ///<  not known or supported.
    ERROR_MISSING_EXPECTED_PARAM,        ///< An optional parameter that is required
                                         ///<  for a particular function.
    ERROR_MISSING_MAND_PARAM,            ///< A parameter is missing that is mandatory.
                                         ///< for a particular message.
    ERROR_UNRECOGNIZED_PARAM_VAL,        ///< A known parameter has an unknown or
                                         ///<  unsupported value.
    ERROR_UNEXPECTED_PARAM_VAL,          ///< A known parameter has a known but unexpected
                                         ///<  value.
    ERROR_USER_DATA_SIZE_ERR,            ///< The User Data size is too large for access
                                         ///< technology, transport network, or call
                                         ///< mode, etc
    ERROR_GENERAL_OTHER,                 ///< Other general problems.

    ERROR_3GPP2_PLATFORM_SPECIFIC,       ///< Platform specific code.
    ERROR_3GPP2_MAX                      ///< Undefined reason.
}SmsErrorCode3GPP2;



//--------------------------------------------------------------------------------------------------
/**
 * Message Error code when the message sending has failed.
 */
//--------------------------------------------------------------------------------------------------
typedef enum {
    ////< Radio Protocol error codes specified in The 3GPP 24.011 ANEXE E Cause definition.
    RP_ERROR_UNASSIGNED_NUMBER,               ///< Unassigned (unallocated) number
    RP_ERROR_OPERATOR_DETERMINED_BARRING,     ///< Operator determined barring
    RP_ERROR_CALL_BARRED,                     ///< Call barred
    RP_ERROR_RESERVED,                        ///< Reserved
    RP_ERROR_SMS_TRANSFER_REJECTED,           ///< Short message transfer rejected
    RP_ERROR_MEMORY_CAP_EXCEEDED,             ///< Memory capacity exceeded
    RP_ERROR_DESTINATION_OUT_OF_ORDER,        ///< Destination out of order
    RP_ERROR_UNIDENTIFIED_SUBSCRIBER,         ///< Unidentified subscriber
    RP_ERROR_FACILITY_REJECTED,               ///< Facility rejected
    RP_ERROR_UNKNOWN_SUBSCRIBER,              ///< Unknown subscriber
    RP_ERROR_NETWORK_OUT_OF_ORDER,            ///< Network out of order
    RP_ERROR_TEMPORARY_FAILURE,               ///< Temporary failure
    RP_ERROR_CONGESTION,                      ///< Congestion
    RP_ERROR_RESOURCES_UNAVAILABLE,           ///< Resources unavailable, unspecified
    RP_ERROR_REQUESTED_FACILITY_NOT_SUBSCRIBED,    ///< Resources facility not subscribed
    RP_ERROR_REQUESTED_FACILITY_NOT_IMPLEMENTED,   ///< Resources facility not implemented
    RP_ERROR_INVALID_SMS_TRANSFER_REFERENCE_VALUE, ///< Invalid short message transfer
                                                   ///<  reference value
    RP_ERROR_SEMANTICALLY_INCORRECT_MESSAGE,  ///< Sementically incorect message
    RP_ERROR_INVALID_MANDATORY_INFO,          ///< Invalid mandatory information
    RP_ERROR_MESSAGE_TYPE_NOT_IMPLEMENTED,    ///< Message type nonexistent or not implemented
    RP_ERROR_MESSAGE_NOT_COMPATABLE_WITH_SMS, ///< Message not compatible with short message
                                              ///<  protocol state
    RP_ERROR_INFO_ELEMENT_NOT_IMPLEMENTED,    ///< Information element nonexistent
                                              ///<   or not implemented
    RP_ERROR_PROTOCOL_ERROR,                  ///< Protocol error, unspecified
    RP_ERROR_INTERWORKING,                    ///< Interworking, unspecified

    ////< Transfer Protocol error codes specified in The 3GPP 23.040 9.2.3.22 TP-Failure-Causes.
    TP_ERROR_TELE_INTERWORKING_NOT_SUPPORTED,      ///< Telematic interworking not supported
    TP_ERROR_SHORT_MESSAGE_TYPE_0_NOT_SUPPORTED,   ///< Short Message Type 0 not supported
    TP_ERROR_SHORT_MESSAGE_CANNOT_BE_REPLACED,     ///< Cannot replace short message
    TP_ERROR_UNSPECIFIED_PID_ERROR,                ///< Unspecified TP-PID error
    TP_ERROR_DCS_NOT_SUPPORTED,                    ///< Data coding scheme (alphabet)
                                                   ///<  not supported
    TP_ERROR_MESSAGE_CLASS_NOT_SUPPORTED,          ///< Message class not supported
    TP_ERROR_UNSPECIFIED_DCS_ERROR,                ///< Unspecified TP-DCS error
    TP_ERROR_COMMAND_CANNOT_BE_ACTIONED,           ///< Command cannot be actioned
    TP_ERROR_COMMAND_UNSUPPORTED,                  ///< Command unsupported
    TP_ERROR_UNSPECIFIED_COMMAND_ERROR,            ///< Unspecified TP-Command error
    TP_ERROR_TPDU_NOT_SUPPORTED,                   ///< TPDU not supported
    TP_ERROR_SC_BUSY,                              ///< SC busy
    TP_ERROR_NO_SC_SUBSCRIPTION,                   ///< No SC subscription
    TP_ERROR_SC_SYS_FAILURE,                       ///< SC system failure
    TP_ERROR_INVALID_SME_ADDRESS,                  ///< Invalid SME address
    TP_ERROR_DESTINATION_SME_BARRED,               ///< Destination SME barred
    TP_ERROR_SM_REJECTED_OR_DUPLICATE,             ///< SM Rejected-Duplicate SM
    TP_ERROR_TP_VPF_NOT_SUPPORTED,                 ///< TP-VPF not supported
    TP_ERROR_TP_VP_NOT_SUPPORTED,                  ///< TP-VP not supporte
    TP_ERROR_SIM_SMS_STORAGE_FULL,                 ///< (U)SIM SMS storage full
    TP_ERROR_NO_SMS_STORAGE_CAP_IN_SIM,            ///< No SMS storage capability in (U)SIM
    TP_ERROR_MS_ERROR,                             ///< Error in MS
    TP_ERROR_MEMORY_CAP_EXCEEDED,                  ///< Memory capacity exceeded
    TP_ERROR_SIM_APP_TOOLKIT_BUSY,                 ///< (U)SIM Application Toolkit busy
    TP_ERROR_SIM_DATA_DOWNLOAD_ERROR,              ///< (U)SIM data download error
    TP_ERROR_UNSPECIFIED_ERROR,                    ///< Unspecified error cause

    ERROR_3GPP_PLATFORM_SPECIFIC,                  ///< Platform specific code.
    ERROR_3GPP_MAX                                 ///< Undefined reason.
}SmsErrorCode;

//--------------------------------------------------------------------------------------------------
/**
 * PDU error code structure
 */
//--------------------------------------------------------------------------------------------------
typedef struct
{
    SmsErrorCode3GPP2  code3GPP2;              ///< Last sending failure error code.
    SmsErrorCode       rp;                     ///< Last sending failure error code.
    SmsErrorCode       tp;                     ///< Last sending failure error code.
    int32              platformSpecific;       ///< Platform specific error code.
}SMS_SENDING_ERROR_CODE;

//--------------------------------------------------------------------------------------------------
/**
 * Message Status.
 */
//--------------------------------------------------------------------------------------------------
typedef enum{
    RX_READ,         ///< Message present in the message storage has been read.
    RX_UNREAD,       ///< Message present in the message storage has not been read.
    STORED_SENT,     ///< Message saved in the message storage has been sent.
    STORED_UNSENT,   ///< Message saved in the message storage has not been sent.
    SENT,            ///< Message has been sent.
    SENDING,         ///< Message has been in the sending pool.
    UNSENT,          ///< Message has not been sent.
    SENDING_FAILED,  ///< Message sending has Failed.
    SENDING_TIMEOUT, ///< Message sending has Failed due to timeout.
    STATUS_UNKNOWN   ///< Unknown message status.
}SMS_STATUS_T;

#define SMS_TEXT_MAX_BYTES (140)
#define SMS_TIMESTAMP_MAX_BYTES (32) 

//--------------------------------------------------------------------------------------------------
/**
 * Message Format.
 */
//--------------------------------------------------------------------------------------------------
typedef enum{
    FORMAT_PDU,       ///< PDU message format.
    FORMAT_TEXT,      ///< Text message format.
}SMS_FORMAT_T;

//--------------------------------------------------------------------------------------------------
/**
 * PDU message type structure.
 */
//--------------------------------------------------------------------------------------------------
typedef struct {
    SMS_OPTION option;                           ///< Option mask
    SMS_STATUS_T     status;                           ///< mandatory, status of msg in memory
    char                addr[PHONE_NUM_MAX_BYTES]; ///< mandatory, originator/destination address
    char                scts[SMS_TIMESTAMP_MAX_BYTES]; ///< mandatory, service center timestamp
    SMS_FORMAT_T     format;                           ///< mandatory, SMS user data format
    SMS_MSG_PROTOCOL       protocol;                    ///< mandatory, protocol used for encoding
    uint8                  data[SMS_TEXT_MAX_BYTES];  ///< mandatory, SMS user data 
    uint32                dataLen;                     ///< mandatory, number of characters
    SMS_SENDING_ERROR_CODE errorCode;                   ///< Last sending failure error code.
}SMS_TEXT_T;

//--------------------------------------------------------------------------------------------------
/**
 * SMS STATUS REPORT  message type structure.
 * Defined in 3GPP TS 23.040 section 9.2.2.3.
 */
//--------------------------------------------------------------------------------------------------
typedef struct {
    uint8      mr;                                 ///< Mandatory, Message Reference
    uint8    ra[PHONE_NUM_MAX_BYTES]; ///< Mandatory, Recipient Address
    uint8    tora;                               ///< Mandatory, Recipient Address Type of Address
    uint8    scts[SMS_TIMESTAMP_MAX_BYTES];   ///< Mandatory, Service Centre Time Stamp
    uint8    dt[SMS_TIMESTAMP_MAX_BYTES];     ///< Mandatory, Discharge Time
    uint8    st;                                 ///< Mandatory, Status
}SMS_STATUS_REPORT_T;

//--------------------------------------------------------------------------------------------------
/**
 * Cell Broad Cast message type structure.
 */
//--------------------------------------------------------------------------------------------------
typedef struct
{
    SMS_MSG_PROTOCOL  protocol;                    ///< mandatory, protocol used for encoding
    SMS_FORMAT_T      format;                      ///< mandatory, SMS user data format
    uint8             data[SMS_TEXT_MAX_BYTES]; ///< mandatory, SMS user data
    uint32            dataLen;                     ///< SMS user data length
    uint16            serialNum;                   ///< SMS CB Serial Number 3GPP 03.41
    uint16            mId;                         ///< SMS CB Message ID 3GPP 03.41
    uint8             dcs;                         ///< SMS CB Data Coding Scheme 3GPP 03.41
    uint8             pp;                          ///< SMS CB Page Parameter 3GPP 03.41
}SMS_CELL_BROADCAST_T;

//--------------------------------------------------------------------------------------------------
/**
 * Generic Message structure.
 *
 */
//--------------------------------------------------------------------------------------------------
typedef struct {
    SMS_MSG_TYPE  type;                         ///< Message Type
    union {                                         ///< Associated data and informations
        SMS_TEXT_T           smsDeliver;       ///< SMS DELIVER (SC to MS)
        SMS_TEXT_T           smsSubmit;        ///< SMS SUBMIT (MS to SC)
        SMS_STATUS_REPORT_T  smsStatusReport;  ///< SMS STATUS REPORT (SC to MS)
        SMS_CELL_BROADCAST_T cellBroadcast;    ///< Cell Broadcast
    };
}SMS_MESSAGE_T;

typedef struct _tagSMS_NotifyInfo{
    SMS_STATUS_T      code;
    uint32            Param;
    SmsErrorCode3GPP2 ErrorCode1;
    SmsErrorCode      ErrorCode2;
}SMS_NOTIFY_INFO;


/**
 * 获取SMS的中心号码
 *
 * @param[in]  sim SIM_0,SIM_1, SIM_2
 * @param[out]  smscPtr    返回中心号码   
 * @param[out]  BufferLen 返回中心号码的长度  
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SmsSmscGet(SIM_ID sim, uint8 * smscPtr, uint32 BufferLen);//string

/**
 * 设置SMS的中心号码
 *
 * @param[in]  sim SIM_0,SIM_1, SIM_2
 * @param[in]  smscPtr    中心号码   
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SmsSmscSet(SIM_ID sim, uint8 * smscPtr);//string

/**
 * 激活SMS的广播小区
 *
 * @param[in]  sim SIM_0,SIM_1, SIM_2
 * @param[in]  protocol   SMS_PROTOCOL_UNKNOWN,SMS_PROTOCOL_GSM,SMS_PROTOCOL_CDMA,etc.
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SmsCellBroadcastActivate(SIM_ID sim, SMS_MSG_PROTOCOL protocol);

/**
 * 去激活SMS的广播小区
 *
 * @param[in]  sim SIM_0,SIM_1, SIM_2
 * @param[in]  protocol   SMS_PROTOCOL_UNKNOWN,SMS_PROTOCOL_GSM,SMS_PROTOCOL_CDMA,etc.
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SmsCellBroadcastDeactivate(SIM_ID sim, SMS_MSG_PROTOCOL protocol);

/**
 * 添加SMS的广播小区
 *
 * @param[in]  sim SIM_0,SIM_1, SIM_2
 * @param[in]  fromId
 * @param[in]  toId
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SmsCellBroadcastIdsAdd(SIM_ID sim, uint16 fromId, uint16 toId);

/**
 * 删除SMS的广播小区
 *
 * @param[in]  sim SIM_0,SIM_1, SIM_2
 * @param[in]  fromId
 * @param[in]  toId
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SmsCellBroadcastIdsRemove(SIM_ID sim, uint16 fromId, uint16 toId);

/**
 * 清除SMS的广播小区
 *
 * @param[in]  sim SIM_0,SIM_1, SIM_2
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SmsCellBroadcastIdsClear(SIM_ID sim);

/**
 * 发送SMS
 *
 * @param[in]  sim SIM_0,SIM_1, SIM_2
 * @param[in]  loc SMS_STORAGE_BROADCAST,SMS_STORAGE_SIM
 * @param[in]  msg 发送SMS的内容
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SmsMsgSend(SIM_ID sim, SIM_SMSSTORAGE loc, SMS_MESSAGE_T *msg);

/**
 * 获取第index条的SMS
 *
 * @param[in]  sim SIM_0,SIM_1, SIM_2
 * @param[in]  loc SMS_STORAGE_BROADCAST,SMS_STORAGE_SIM
 * @param[in]  index 第index条的短信
 * @param[out]  msg 获取SMS的内容
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SmsMsgGet(SIM_ID sim, SIM_SMSSTORAGE loc, uint32 index, SMS_MESSAGE_T *msg);

/**
 * 删除第index条的SMS
 *
 * @param[in]  sim SIM_0,SIM_1, SIM_2
 * @param[in]  loc SMS_STORAGE_BROADCAST,SMS_STORAGE_SIM
 * @param[in]  index 第index条的短信
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SmsMsgDelete(SIM_ID sim, SIM_SMSSTORAGE loc, uint32 index);

/**
 * 获取SMS的storage的状态
 *
 * @param[in]  sim SIM_0,SIM_1, SIM_2
 * @param[in]  loc SMS_STORAGE_BROADCAST,SMS_STORAGE_SIM
 * @param[out]  Used 已经占用
 * @param[out]  Total 总的
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SmsStorageStatusGet(SIM_ID sim, SIM_SMSSTORAGE loc, uint32 *Used, uint32 *Total);


#endif /*__KING_SMS_H__*/

