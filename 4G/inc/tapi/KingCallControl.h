#ifndef __KING_CALL_H__
#define __KING_CALL_H__

#include "KingUsim.h"

//--------------------------------------------------------------------------------------------------
/**
 *  Enumeration of the possible reasons for call termination.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    CC_TERM_LOCAL_ENDED = 0,
        ///< Local party ended the call (Normal Call Clearing).
    CC_TERM_REMOTE_ENDED = 1,
        ///< Remote party ended the call (Normal Call Clearing).
    CC_TERM_NETWORK_FAIL = 2,
        ///< Network could not complete the call.
    CC_TERM_UNASSIGNED_NUMBER = 3,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_NO_ROUTE_TO_DESTINATION = 4,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_CHANNEL_UNACCEPTABLE = 5,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_OPERATOR_DETERMINED_BARRING = 6,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_USER_BUSY = 7,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_NO_USER_RESPONDING = 8,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_USER_ALERTING_NO_ANSWER = 9,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_CALL_REJECTED = 10,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_NUMBER_CHANGED = 11,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_PREEMPTION = 12,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_DESTINATION_OUT_OF_ORDER = 13,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_INVALID_NUMBER_FORMAT = 14,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_FACILITY_REJECTED = 15,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_RESP_TO_STATUS_ENQUIRY = 16,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_NORMAL_UNSPECIFIED = 17,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_NO_CIRCUIT_OR_CHANNEL_AVAILABLE = 18,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_NETWORK_OUT_OF_ORDER = 19,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_TEMPORARY_FAILURE = 20,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_SWITCHING_EQUIPMENT_CONGESTION = 21,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_ACCESS_INFORMATION_DISCARDED = 22,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_REQUESTED_CIRCUIT_OR_CHANNEL_NOT_AVAILABLE = 23,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_RESOURCES_UNAVAILABLE_OR_UNSPECIFIED = 24,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_QOS_UNAVAILABLE = 25,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_REQUESTED_FACILITY_NOT_SUBSCRIBED = 26,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_INCOMING_CALLS_BARRED_WITHIN_CUG = 27,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_BEARER_CAPABILITY_NOT_AUTH = 28,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_BEARER_CAPABILITY_UNAVAILABLE = 29,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_SERVICE_OPTION_NOT_AVAILABLE = 30,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_ACM_LIMIT_EXCEEDED = 31,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_BEARER_SERVICE_NOT_IMPLEMENTED = 32,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_REQUESTED_FACILITY_NOT_IMPLEMENTED = 33,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_ONLY_DIGITAL_INFORMATION_BEARER_AVAILABLE = 34,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_SERVICE_OR_OPTION_NOT_IMPLEMENTED = 35,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_INVALID_TRANSACTION_IDENTIFIER = 36,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_USER_NOT_MEMBER_OF_CUG = 37,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_INCOMPATIBLE_DESTINATION = 38,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_INVALID_TRANSIT_NW_SELECTION = 39,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_SEMANTICALLY_INCORRECT_MESSAGE = 40,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_INVALID_MANDATORY_INFORMATION = 41,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_MESSAGE_TYPE_NON_IMPLEMENTED = 42,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE = 43,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_INFORMATION_ELEMENT_NON_EXISTENT = 44,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_CONDITONAL_IE_ERROR = 45,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE = 46,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_RECOVERY_ON_TIMER_EXPIRY = 47,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_PROTOCOL_ERROR_UNSPECIFIED = 48,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_INTERWORKING_UNSPECIFIED = 49,
        ///< cf. 3GPP 24.008 Annex H
    CC_TERM_SERVICE_TEMPORARILY_OUT_OF_ORDER = 50,
        ///< cf. 3GPP 24.008 10.5.3.6
    CC_TERM_NOT_ALLOWED = 51,
        ///< Call operations not allowed
        ///<  (i.e. Radio off).
    CC_TERM_FDN_ACTIVE = 52,
        ///< FDN is active and number is not
        ///< in the FDN.
    CC_TERM_NO_SERVICE = 53,
        ///< No service or bad signal quality
    CC_TERM_PLATFORM_SPECIFIC = 54,
        ///< Platform specific code.
    CC_TERM_UNDEFINED = 55,
        ///< Undefined reason.

    // IMS -> Telephony
    // The passed argument is an invalid
    CODE_LOCAL_ILLEGAL_ARGUMENT = 101,
    // The operation is invoked in invalid call state
    CODE_LOCAL_ILLEGAL_STATE = 102,
    // IMS service internal error
    CODE_LOCAL_INTERNAL_ERROR = 103,
    // IMS service goes down (service connection is lost)
    CODE_LOCAL_IMS_SERVICE_DOWN = 106,
    // No pending incoming call exists
    CODE_LOCAL_NO_PENDING_CALL = 107,
    // IMS Call ended during conference merge process
    CODE_LOCAL_ENDED_BY_CONFERENCE_MERGE = 108,

    // IMS -> Telephony
    // Service unavailable; by power off
    CODE_LOCAL_POWER_OFF = 111,
    // Service unavailable; by low battery
    CODE_LOCAL_LOW_BATTERY = 112,
    // Service unavailable; by out of service (data service state)
    CODE_LOCAL_NETWORK_NO_SERVICE = 121,
    // Service unavailable; by no LTE coverage
    // (VoLTE is not supported even though IMS is registered)
    CODE_LOCAL_NETWORK_NO_LTE_COVERAGE = 122,
    // Service unavailable; by located in roaming area
    CODE_LOCAL_NETWORK_ROAMING = 123,
    // Service unavailable; by IP changed
    CODE_LOCAL_NETWORK_IP_CHANGED = 124,
    // Service unavailable; other
    CODE_LOCAL_SERVICE_UNAVAILABLE = 131,
    // Service unavailable; IMS connection is lost (IMS is not registered)
    CODE_LOCAL_NOT_REGISTERED = 132,

    // IMS <-> Telephony
    // Max call exceeded
    CODE_LOCAL_CALL_EXCEEDED = 141,
    // IMS <- Telephony
    // Call busy
    CODE_LOCAL_CALL_BUSY = 142,
    // Call decline
    CODE_LOCAL_CALL_DECLINE = 143,
    // IMS -> Telephony
    // SRVCC is in progress
    CODE_LOCAL_CALL_VCC_ON_PROGRESSING = 144,
    // Resource reservation is failed (QoS precondition)
    CODE_LOCAL_CALL_RESOURCE_RESERVATION_FAILED = 145,
    // Retry CS call; VoLTE service can't be provided by the network or remote end
    // Resolve the extra code(EXTRA_CODE_CALL_RETRY_*) if the below code is set
    CODE_LOCAL_CALL_CS_RETRY_REQUIRED = 146,
    // Retry VoLTE call; VoLTE service can't be provided by the network temporarily
    CODE_LOCAL_CALL_VOLTE_RETRY_REQUIRED = 147,
    // IMS call is already terminated (in TERMINATED state)
    CODE_LOCAL_CALL_TERMINATED = 148,
    // Handover not feasible
    CODE_LOCAL_HO_NOT_FEASIBLE = 149,

    /**
     * TIMEOUT (IMS -> Telephony)
     */
    // 1xx waiting timer is expired after sending INVITE request (MO only)
    CODE_TIMEOUT_1XX_WAITING = 201,
    // User no answer during call setup operation (MO/MT)
    // MO : 200 OK to INVITE request is not received,
    // MT : No action from user after alerting the call
    CODE_TIMEOUT_NO_ANSWER = 202,
    // User no answer during call update operation (MO/MT)
    // MO : 200 OK to re-INVITE request is not received,
    // MT : No action from user after alerting the call
    CODE_TIMEOUT_NO_ANSWER_CALL_UPDATE = 203,

    //Call was blocked by call barring
    CODE_CALL_BARRED = 240,

    //Call failures for FDN
    CODE_FDN_BLOCKED = 241,

    // Network does not accept the emergency call request because IMEI was used as identification
    // and this capability is not supported by the network.
    CODE_IMEI_NOT_ACCEPTED = 243,

    //STK CC errors
    CODE_DIAL_MODIFIED_TO_USSD = 244,
    CODE_DIAL_MODIFIED_TO_SS = 245,
    CODE_DIAL_MODIFIED_TO_DIAL = 246,
    CODE_DIAL_MODIFIED_TO_DIAL_VIDEO = 247,
    CODE_DIAL_VIDEO_MODIFIED_TO_DIAL = 248,
    CODE_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO = 249,
    CODE_DIAL_VIDEO_MODIFIED_TO_SS = 250,
    CODE_DIAL_VIDEO_MODIFIED_TO_USSD = 251,

    /**
     * STATUSCODE (SIP response code) (IMS -> Telephony)
     */
    // 3xx responses
    // SIP request is redirected
    CODE_SIP_REDIRECTED = 321,
    // 4xx responses
    // 400 : Bad Request
    CODE_SIP_BAD_REQUEST = 331,
    // 403 : Forbidden
    CODE_SIP_FORBIDDEN = 332,
    // 404 : Not Found
    CODE_SIP_NOT_FOUND = 333,
    // 415 : Unsupported Media Type
    // 416 : Unsupported URI Scheme
    // 420 : Bad Extension
    CODE_SIP_NOT_SUPPORTED = 334,
    // 408 : Request Timeout
    CODE_SIP_REQUEST_TIMEOUT = 335,
    // 480 : Temporarily Unavailable
    CODE_SIP_TEMPRARILY_UNAVAILABLE = 336,
    // 484 : Address Incomplete
    CODE_SIP_BAD_ADDRESS = 337,
    // 486 : Busy Here
    // 600 : Busy Everywhere
    CODE_SIP_BUSY = 338,
    // 487 : Request Terminated
    CODE_SIP_REQUEST_CANCELLED = 339,
    // 406 : Not Acceptable
    // 488 : Not Acceptable Here
    // 606 : Not Acceptable
    CODE_SIP_NOT_ACCEPTABLE = 340,
    // 410 : Gone
    // 604 : Does Not Exist Anywhere
    CODE_SIP_NOT_REACHABLE = 341,
    // Others
    CODE_SIP_CLIENT_ERROR = 342,
    // 5xx responses
    // 501 : Server Internal Error
    CODE_SIP_SERVER_INTERNAL_ERROR = 351,
    // 503 : Service Unavailable
    CODE_SIP_SERVICE_UNAVAILABLE = 352,
    // 504 : Server Time-out
    CODE_SIP_SERVER_TIMEOUT = 353,
    // Others
    CODE_SIP_SERVER_ERROR = 354,
    // 6xx responses
    // 603 : Decline
    CODE_SIP_USER_REJECTED = 361,
    // Others
    CODE_SIP_GLOBAL_ERROR = 362,
    // Emergency failure
    CODE_EMERGENCY_TEMP_FAILURE = 363,
    CODE_EMERGENCY_PERM_FAILURE = 364,

    /**
     * MEDIA (IMS -> Telephony)
     */
    // Media resource initialization failed
    CODE_MEDIA_INIT_FAILED = 401,
    // RTP timeout (no audio / video traffic in the session)
    CODE_MEDIA_NO_DATA = 402,
    // Media is not supported; so dropped the call
    CODE_MEDIA_NOT_ACCEPTABLE = 403,
    // Unknown media related errors
    CODE_MEDIA_UNSPECIFIED = 404,

    /**
     * USER
     */
    // Telephony -> IMS
    // User triggers the call end
    CODE_USER_TERMINATED = 501,
    // No action while an incoming call is ringing
    CODE_USER_NOANSWER = 502,
    // User ignores an incoming call
    CODE_USER_IGNORE = 503,
    // User declines an incoming call
    CODE_USER_DECLINE = 504,
    // Device declines/ends a call due to low battery
    CODE_LOW_BATTERY = 505,
    // Device declines call due to blacklisted call ID
    CODE_BLACKLISTED_CALL_ID = 506,
    // IMS -> Telephony
    // The call is terminated by the network or remote user
    CODE_USER_TERMINATED_BY_REMOTE = 510,

    /**
     * Extra codes for the specific code value
     * This value can be referred when the code is CODE_LOCAL_CALL_CS_RETRY_REQUIRED.
     */
    // Try to connect CS call; normal
    EXTRA_CODE_CALL_RETRY_NORMAL = 1,
    // Try to connect CS call without the notification to user
    EXTRA_CODE_CALL_RETRY_SILENT_REDIAL = 2,
    // Try to connect CS call by the settings of the menu
    EXTRA_CODE_CALL_RETRY_BY_SETTINGS = 3,

    /**
     * UT
     */
    CODE_UT_NOT_SUPPORTED = 801,
    CODE_UT_SERVICE_UNAVAILABLE = 802,
    CODE_UT_OPERATION_NOT_ALLOWED = 803,
    CODE_UT_NETWORK_ERROR = 804,
    CODE_UT_CB_PASSWORD_MISMATCH = 821,
    //STK CC errors
    CODE_UT_SS_MODIFIED_TO_DIAL = 822,
    CODE_UT_SS_MODIFIED_TO_USSD = 823,
    CODE_UT_SS_MODIFIED_TO_SS = 824,
    CODE_UT_SS_MODIFIED_TO_DIAL_VIDEO = 825,

    /**
     * ECBM
     */
    CODE_ECBM_NOT_SUPPORTED = 901,

    /**
     * Fail code used to indicate that Multi-endpoint is not supported by the Ims framework.
     */
    CODE_MULTIENDPOINT_NOT_SUPPORTED = 902,

    /**
     * Ims Registration error code
     */
    CODE_REGISTRATION_ERROR = 1000,

    /**
     * CALL DROP error codes (Call could drop because of many reasons like Network not available,
     *  handover, failed, etc)
     */

    /**
     * CALL DROP error code for the case when a device is ePDG capable and when the user is on an
     * active wifi call and at the edge of coverage and there is no qualified LTE network available
     * to handover the call to. We get a handover NOT_TRIGERRED message from the modem. This error
     * code is received as part of the handover message.
     */
    CODE_CALL_DROP_IWLAN_TO_LTE_UNAVAILABLE = 1100,

    /**
     * MT call has ended due to a release from the network
     * because the call was answered elsewhere
     */
    CODE_ANSWERED_ELSEWHERE = 1014,

    /**
     * For MultiEndpoint - Call Pull request has failed
     */
    CODE_CALL_PULL_OUT_OF_SYNC = 1015,

    /**
     * For MultiEndpoint - Call has been pulled from primary to secondary
     */
    CODE_CALL_END_CAUSE_CALL_PULL = 1016,

    /**
     * Supplementary services (HOLD/RESUME) failure error codes.
     * Values for Supplemetary services failure - Failed, Cancelled and Re-Invite collision.
     */
    CODE_SUPP_SVC_FAILED = 1201,
    CODE_SUPP_SVC_CANCELLED = 1202,
    CODE_SUPP_SVC_REINVITE_COLLISION = 1203,

    /**
     * DPD Procedure received no response or send failed
     */
    CODE_IWLAN_DPD_FAILURE = 1300,

    /**
     * Establishment of the ePDG Tunnel Failed
     */
    CODE_EPDG_TUNNEL_ESTABLISH_FAILURE = 1400,

    /**
     * Re-keying of the ePDG Tunnel Failed; may not always result in teardown
     */
    CODE_EPDG_TUNNEL_REKEY_FAILURE = 1401,

    /**
     * Connection to the packet gateway is lost
     */
    CODE_EPDG_TUNNEL_LOST_CONNECTION = 1402,

    /**
     * The maximum number of calls allowed has been reached.  Used in a multi-endpoint scenario
     * where the number of calls across all connected devices has reached the maximum.
     */
    CODE_MAXIMUM_NUMBER_OF_CALLS_REACHED = 1403,

    /**
     * Similar to {@link #CODE_LOCAL_CALL_DECLINE}, except indicates that a remote device has
     * declined the call.  Used in a multi-endpoint scenario where a remote device declined an
     * incoming call.
     */
    CODE_REMOTE_CALL_DECLINE = 1404,

    /**
     * Indicates the call was disconnected due to the user reaching their data limit.
     */
    CODE_DATA_LIMIT_REACHED = 1405,

    /**
     * Indicates the call was disconnected due to the user disabling cellular data.
     */
    CODE_DATA_DISABLED = 1406,

    /**
     * Indicates a call was disconnected due to loss of wifi signal.
     */
    CODE_WIFI_LOST = 1407,

    /**
     * Indicates the registration attempt on IWLAN failed due to IKEv2 authetication failure
     * during tunnel establishment.
     */
    CODE_IKEV2_AUTH_FAILURE = 1408,

    /** The call cannot be established because RADIO is OFF */
    CODE_RADIO_OFF = 1500,

    /** The call cannot be established because of no valid SIM */
    CODE_NO_VALID_SIM = 1501,

    /** The failure is due internal error at modem */
    CODE_RADIO_INTERNAL_ERROR = 1502,

    /** The failure is due to UE timer expired while waiting for a response from network */
    CODE_NETWORK_RESP_TIMEOUT = 1503,

    /** The failure is due to explicit reject from network */
    CODE_NETWORK_REJECT = 1504,

    /** The failure is due to radio access failure. ex. RACH failure */
    CODE_RADIO_ACCESS_FAILURE = 1505,

    /** Call/IMS registration failed/dropped because of a RLF */
    CODE_RADIO_LINK_FAILURE = 1506,

    /** Call/IMS registration failed/dropped because of radio link lost */
    CODE_RADIO_LINK_LOST = 1507,

    /** The call Call/IMS registration failed because of a radio uplink issue */
    CODE_RADIO_UPLINK_FAILURE = 1508,

    /** Call failed because of a RRC connection setup failure */
    CODE_RADIO_SETUP_FAILURE = 1509,

    /** Call failed/dropped because of RRC connection release from NW */
    CODE_RADIO_RELEASE_NORMAL = 1510,

    /** Call failed/dropped because of RRC abnormally released by modem/network */
    CODE_RADIO_RELEASE_ABNORMAL = 1511,

    /** Call failed because of access class barring */
    CODE_ACCESS_CLASS_BLOCKED = 1512,

    /** Call/IMS registration is failed/dropped because of a network detach */
    CODE_NETWORK_DETACH = 1513,

    /**
     * Call failed due to SIP code 380 (Alternative Service response) while dialing an "undetected
     * emergency number".  This scenario is important in some regions where the carrier network will
     * identify other non-emergency help numbers (e.g. mountain rescue) when attempting to dial.
     */
    CODE_SIP_ALTERNATE_EMERGENCY_CALL = 1514,

    /**
     * Call failed because of unobtainable number
     */
    CODE_UNOBTAINABLE_NUMBER = 1515,

    CODE_MAX = 0x7FFFFFFF
        
}CC_TERMINATION_REASON;

//--------------------------------------------------------------------------------------------------
/**
 *  Enumeration of the possible events that may be reported to a call event handler.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    CC_EVENT_SETUP,
        ///< Call is being set up.
    CC_EVENT_INCOMING,
        ///< Incoming call attempt (new call).
    CC_EVENT_ORIGINATING,
        ///< Outgoing call attempt.
    CC_EVENT_ALERTING,
        ///< Far end is now alerting its user (outgoing call).
    CC_EVENT_CONNECTED,
        ///< Call has been established, and is media is active.
    CC_EVENT_TERMINATED,
        ///< Call has terminated.
    CC_EVENT_WAITING,
        ///< Call is waiting
    CC_EVENT_ON_HOLD,
        ///< Remote party has put the call on hold.
    CC_EVENT_ERROR,
        ///< Operation failed
    CC_EVENT_COMMAND_RESULT,

    CC_EVENT_MAX = 0x7FFFFFFF 
        ///< Enumerate max value.
}CC_STATE_EVENT;

typedef enum
{
    CC_ACTIVATE_CLIR   = 0, ///< Disable presentation of own phone number to remote.
    CC_DEACTIVATE_CLIR = 1, ///< Enable presentation of own phone number to remote.
    CC_NO_CLIR         = 2,  ///< Do not change presentation of own phone number to remote mode.
    CC_CLIR_MAX = 0x7FFFFFFF 
}CC_CLIR_T;

typedef enum
{
    CC_ACTIVATE_CUG   = 0, ///< Activate
    CC_DEACTIVATE_CUG = 1, ///< Deactivate
    CC_NO_CUG         = 2,  ///< Do not invoke CUG
    CC_CUG_MAX = 0x7FFFFFFF 
}CC_CUG_T;


typedef enum{
    TRANSPORT_TYPE_WWAN,
    TRANSPORT_TYPE_WLAN,
    TRANSPORT_TYPE_INVALID,
    TRANSPORT_TYPE_MAX = 0x7FFFFFFF
}IMS_TRANSPORT_TYPE;

#define CAPABILITY_TYPE_VOICE BIT0
#define CAPABILITY_TYPE_VIDEO BIT1
#define CAPABILITY_TYPE_UT    BIT2
#define CAPABILITY_TYPE_SMS   BIT3

typedef struct _tagOriginalCallInfo{
    uint8     *phoneNumberPtr;
    uint8     CallerID;
    CC_CLIR_T clir;
    CC_CUG_T  cug;
}ORIG_CALL_INFO;

/**
 * 发起呼叫
 *
 * @param[in] sim  对应的卡ID?
 * @param[inout] CallingInfo  发起呼叫的号码信息
 * @return 0 success  -1 fail
 */
int KING_CallVoiceDial(SIM_ID sim, ORIG_CALL_INFO *CallingInfo);

/**
 * 接听来电
 * 如果是callwaiting,接听并挂断原来的通话
 *
 * @param[in] sim  对应的卡ID
 * @return 0 success  -1 fail
 */
int KING_CallAnswer(SIM_ID sim);

/**
 * 拒接来电
 *
 * @param[in] sim  对应的卡ID
 * @return 0 success  -1 fail
 */
int KING_CallReject(SIM_ID sim);

/**
 * 拒接来电或挂断通话
 *
 * @param[in] sim  对应的卡ID
 * @param[in] callID 通话ID
 * @return 0 success  -1 fail
 */
int KING_CallHangUp(SIM_ID sim, uint32 callID);

/**
 * 结束所有通话
 *
 * @param[in] sim  对应的卡ID
 * @return 0 success  -1 fail
 */
int KING_CallHangUpAll(SIM_ID sim);

/**
 *设置呼叫等待状态
 *
 * @param[in] sim  对应的卡ID
 * @param[in] active  呼叫等待状态0关闭1激活
 * @return 0 success  -1 fail
 */
int KING_CallWaitingServiceSet(SIM_ID sim, bool active);

/**
 *获取呼叫等待状态
 *
 * @param[in] sim  对应的卡ID
 * @param[out] active  呼叫等待状态0关闭1激活
 * @return 0 success  -1 fail
 */
int KING_CallWaitingServiceGet(SIM_ID sim, bool* active);

/**
 *Hold 指定通话
 *
 * @param[in] sim  对应的卡ID
 * @param[in] callID 通话ID
 * @return 0 success  -1 fail
 */
int KING_CallHold(SIM_ID sim, uint32 callID);

/**
 *激活指定通话
 *
 * @param[in] sim  对应的卡ID
 * @param[in] callID 通话ID
 * @return 0 success  -1 fail
 */
int KING_CallActive(SIM_ID sim, uint32 callID);

/**
 *设置设备对AmrWb支持情况
 *
 *实际通话中是否使用这个编码格式由网络决定
 * @param[in] sim  对应的卡ID
 * @param[in] active 支持情况
 * @return 0 success  -1 fail
 */
int KING_AmrWbCapabilitySet(SIM_ID sim, bool active);

/**
 *获取设备对AmrWb支持情况
 *
 * @param[in] sim  对应的卡ID
 * @param[out]  active 支持情况
 * @return 0 success  -1 fail
 */
int KING_AmrWbCapabilityGet(SIM_ID sim, bool* active);

/**
 *注册IMS 服务
 *注册成功后通话短信等功能通过IMS实现
 *
 * @param[in] sim  对应的卡ID
 * @param[in]  TransportType  IMS服务类型，参考IMS_TRANSPORT_TYPE定义
 * @return 0 success  -1 fail
 */
int KING_IMSRegister(SIM_ID sim, IMS_TRANSPORT_TYPE TransportType);

/**
 *去注册IMS 服务
 *
 * @param[in] sim  对应的卡ID
 * @param[in]  TransportType   IMS服务类型，参考IMS_TRANSPORT_TYPE定义
 * @return 0 success  -1 fail
 */
int KING_IMSUnRegister(SIM_ID sim, IMS_TRANSPORT_TYPE TransportType);

/**
 *获取IMS 服务状态
 *
 * @param[in] sim  对应的卡ID
 * @param[out]  TransportType   IMS服务类型，TRANSPORT_TYPE_INVALID表示无服务
 * @return 0 success  -1 fail
 */
int KING_IMSRegisterStateGet(SIM_ID sim, IMS_TRANSPORT_TYPE* TransportType);

/**
 *获取IMS 服务能力
 *
 * @param[in] sim  对应的卡ID
 * @param[out]  Capabilities   CAPABILITY_TYPE_VOICE CAPABILITY_TYPE_SMS
 * @return 0 success  -1 fail
 */
int KING_IMSCapabilityGet(SIM_ID sim, uint32 *Capabilities);

#endif
