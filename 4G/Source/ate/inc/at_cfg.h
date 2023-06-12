/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#ifndef AT_CFG_H
#define AT_CFG_H

//#include <stdint.h>
#include "at_common.h"

// ///////////////////////////////////////////////////////////////////////
// Gather definition from other ats module
// ///////////////////////////////////////////////////////////////////////

#define CFW_SIM_COUNT 1

typedef enum{
    SMS_STORAGE_BC,  //SMS_STORAGE_BROADCAST
    SMS_STORAGE_SM,
    SMS_STORAGE_MT,
    SMS_STORAGE_ME,
    SMS_STORAGE_SR,
}SMS_STORAGE_TYPE;


typedef enum {
    GC_RESULTCODE_PRESENTATION_ENABLE = 0,
    GC_RESULTCODE_PRESENTATION_DISABLE
} GC_RESULTCODE_PRESENTATION;

typedef enum {
    GC_RESULTCODE_FORMAT_LIMITED = 0,
    GC_RESULTCODE_FORMAT_VERBOSE
} GC_RESULTCODE_FORMAT;

typedef enum {
    GC_COMMANDECHO_MODE_DISABLE = 0,
    GC_COMMANDECHO_MODE_ENABLE
} GC_COMMANDECHO_MODE;

typedef struct
{
    uint16_t ind_ctrl_mask;  // indicator control mask
    uint16_t sind_ctrl_mask; // system indicator control mask, for ^SIND command
    at_chset_t te_chset;
    at_chset_t mt_chset;
} cfg_info_t;

typedef enum {
    PBK_CHARACTER_SET_TRANSPARENT = 0,
    PBK_CHARACTER_SET_HEX
} PBK_CHARACTER_SET;

typedef enum {
    PBK_LISTENTRY_CALLER_CPBR = 0,
    PBK_LISTENTRY_CALLER_CPBF,
    PBK_LISTENTRY_CALLER_CNUM
} PBK_LISTENTRY_CALLER;

typedef struct _AT_CNMI
{
    uint8_t nMode;
    uint8_t nMt;
    uint8_t nBm;
    uint8_t nDs;
    uint8_t nBfr;
} AT_CNMI;

enum AT_DTR_DATA_BEHAVIOR
{
    AT_DTR_DATA_IGNORE,
    AT_DTR_DATA_ESCAPE,
    AT_DTR_DATA_END
};

typedef struct _at_sms_info
{
    AT_CNMI sCnmi;
    SMS_STORAGE_TYPE nStorage1;
    SMS_STORAGE_TYPE nStorage2;
    SMS_STORAGE_TYPE nStorage3;

    uint32_t nTotal1;
    uint32_t nTotal2;
    uint32_t nTotal3;

    uint32_t nUsed1;
    uint32_t nUsed2;
    uint32_t nUsed3;

    uint8_t nServType;
    uint8_t nOverflowCtrl;
    uint8_t nOverflowMode;
} at_sms_info_t;

#define NW_PREFERRED_OPERATOR_FORMAT_NUMERIC 2
#define NW_GETPREFERREDOPERATORS_CALLER_READ 0
#define GSM_SET 0
#define UCS2_SET 1
#define INSTANCE 0
#define NONE_INSTANCE 1

#define AT_AUDIO_TEST_HANDSET 0
// 耳机
#define AT_AUDIO_TEST_EARPIECE 1
// 听筒
#define AT_AUDIO_TEST_LOUDSPEAKER 2
// 免提

#define NUMBER_OF_SIM 1


// ///////////////////////////////////////////////////////////////////////////
// New at global variables
// ///////////////////////////////////////////////////////////////////////////
// sms related settings
typedef struct at_sms_setting
{
    uint8_t csmpFo;
    uint8_t csdhShow;                     // show header
    at_sms_info_t smsInfo[NUMBER_OF_SIM]; // CPMS命令相关的存储器信息
    uint8_t smsFormat;                    // SMS Foramt: 0: PDU 1:text
} at_sms_setting_t;

typedef struct at_tcpip_setting
{
    uint8_t cipSCONT_unSaved;   //AT+CIPSCONT Save TCPIP Application Context
    uint8_t cipMux_multiIp;     //AT+CIPMUX Start Up Multi-IP Connection
    uint8_t cipHEAD_addIpHead;  //AT+CIPHEAD Add an IP Head at the Beginning of a Package Received
    uint8_t cipSHOWTP_dispTP;   //AT+CIPSHOWTP Display Transfer Protocol in IP Head When Received Data
    uint8_t cipSRIP_showIPPort; //AT+CIPSRIP Show Remote IP Address and Port When Received Data
    uint8_t cipATS_setTimer;    //AT+CIPATS Set Auto Sending Timer
    uint8_t cipATS_time;
    uint8_t cipSPRT_sendPrompt;  //AT+CIPSPRT Set Prompt of > When Module Sends Data
    uint8_t cipQSEND_quickSend;  //AT+CIPQSEND Select Data Transmitting Mode
    uint8_t cipMODE_transParent; //AT+CIPMODE Select TCPIP Application Mode
    uint8_t cipCCFG_NmRetry;     //AT+CIPCCFG Configure Transparent Transfer Mode
    uint8_t cipCCFG_WaitTm;
    uint16_t cipCCFG_SendSz;
    uint8_t cipCCFG_esc;
    uint8_t cipCCFG_Rxmode;
    uint16_t cipCCFG_RxSize;
    uint16_t cipCCFG_Rxtimer;
    uint8_t cipDPDP_detectPDP; //AT+CIPDPDP Set Whether to Check State of GPRS Network Timing
    uint8_t cipDPDP_interval;
    uint8_t cipDPDP_timer;
    uint8_t cipCSGP_setGprs;        //AT+CIPCSGP Set CSD or GPRS for Connection Mode
    uint16_t cipRDTIMER_rdsigtimer; //AT+CIPRDTIMER Set Remote Delay Timer
    uint16_t cipRDTIMER_rdmuxtimer;
    uint8_t cipRXGET_manualy; //AT+CIPRXGET Get Data from Network Manually
    uint8_t cipApn[51];
    uint8_t cipUserName[51];
    uint8_t cipPassWord[51];
    uint8_t qishowla_showlocaddr;
    uint8_t qidnsip_dnsip;
} at_tcpip_setting_t;

#if defined(AT_MODULE_SUPPORT_OTA)||defined(AT_MODULE_SUPPORT_APN)

#define AT_E_DC_MAX_PROFILE_NAME        15      /* Maximum length of profile name */
#define AT_E_DC_MAX_URL_LEN             255     /* Maximum length of URL */
#define AT_E_DC_MAX_IP_LEN              15      /* Maximum length of IP address  */
#define AT_E_DC_MAX_PHONE_NUMBER_LEN    21      /* Maximum length of phone number  */ 
#define AT_E_DC_MAX_NAPID_LEN           15      /* Maximum length of nap id  */
#define AT_E_DC_MAX_PROXYID_LEN         15      /* Maximum length of proxy id  */
#define AT_MAX_DATA_CONN_APN_SIZE		255
#define AT_MAX_DATA_CONN_USERNAME_SIZE	255
#define AT_MAX_DATA_CONN_PASSWORD_SIZE	255
#define AT_MAX_OTA_PROFILE              3
#define AT_MAX_OPERATOR_NAME            6

#if 1
/* Bearer type  */
typedef enum
{
    AT_E_DC_BEARER_CSD = 0,         /* CSD */
    AT_E_DC_BEARER_GPRS,            /*GPRS */
    AT_E_DC_BEARER_MAX             
}AT_E_DC_BEARER;

typedef enum
{
    AT_E_DC_CONNECTION_ORIENTED = 0,        /*WSP connection */
    AT_E_DC_CONNECTION_ORIENTED_SECURE,     /* WSP connection with secure*/
    AT_E_DC_WPHTTP_PROXY,                   /* HTTP connection with proxy*/
    AT_E_DC_WPHTTP_DIRECT,                  /* HTTP direct connection */
    AT_E_DC_CONN_MODE_MAX                  
}AT_E_DC_CONN_MODE;

/* connection type  */

typedef enum
{
    AT_GPRS_AUTH_PAP ,
    AT_GPRS_AUTH_CHAP ,    
    AT_GPRS_AUTH_NONE 
}AT_E_DC_GPRS_AUTH_TYPE;

/**
 * @brief This enumeration specifies the type of the profile.
 */
typedef enum  
{
    AT_E_PROF_BROWSER, 
    AT_E_PROF_MMS 
}AT_E_PROV_PROFILE_TYPE ; 

#endif

 struct at_datacon_profile
{
    UINT32  profile_id;                                 /* Profile unique id */
    //INT16   profile_name[AT_E_DC_MAX_PROFILE_NAME + 1];     /* Profile name */
    //INT8    homepage[AT_E_DC_MAX_URL_LEN + 1];              /* Home page */
    INT8    apn[50];                   /* Access point name */
    INT8    username[50];         /* Username */
    INT8    password[50];           /* Password */
    INT8    proxy_ip[AT_E_DC_MAX_IP_LEN + 1];               /*IP address */
    U8 szOperatorName[AT_MAX_OPERATOR_NAME+1];
    //INT8    dialup_number[AT_E_DC_MAX_PHONE_NUMBER_LEN + 1];/*CSD dialup number */
   // INT8    dns1[AT_E_DC_MAX_IP_LEN + 1];                   /*DNS1 */
   // INT8    dns2[AT_E_DC_MAX_IP_LEN + 1];                   /* DNS2 */
   // INT8    nap_id[AT_E_DC_MAX_NAPID_LEN + 1];              /*Nap id */
    //INT8    proxy_id[AT_E_DC_MAX_PROXYID_LEN + 1];          /* Proxy id */
    //UINT32  baudrate;       /* CSD buad rate. */
    //UINT32  timeout;        /* CSD timeout interval. */
    UINT16  port;           /* Port number. */

    AT_E_DC_BEARER              bear_type;                      /* Bearer type */
    AT_E_DC_CONN_MODE           conn_mode;                      /* Connection mode */
    
    AT_E_DC_GPRS_AUTH_TYPE      gprs_auth_type;           /* GPRS auth type */    
    AT_E_PROV_PROFILE_TYPE		profile_type;
    CFW_SIM_ID                     nSimID ;
};

typedef struct at_datacon_profile AT_DATACONN_PROFILE;
#endif

typedef enum
{
    AT_URC_PORT_UART1 = 0,
    AT_URC_PORT_USBAT,    
    AT_URC_PORT_MAX 
} AT_URC_PORT_TYPE;

typedef struct at_setting
{
    // //////////////////////////////////
    // gc related
    // //////////////////////////////////
    // whether or not the DCE transmits result codes to the DTE (ATQ)
    GC_RESULTCODE_PRESENTATION resultCodePresent;

    // Determine the contents of the header and trailer transmitted with result codes
    // and information responses (ATV)
    GC_RESULTCODE_FORMAT resultCodeFormat;

    // Determine whether or not the DCE echoes characters received from the DTE
    // during command state and online command state (ATE).
    GC_COMMANDECHO_MODE commandEchoMode;

    // Response formatting character (S3,S4)
    uint8_t nS3;
    uint8_t nS4;

    // Command line editing character (S5)
    uint8_t nS5;

    // Disable or enables the use of result code +CME ERROR (AT+CMEE)
    uint8_t cmee;

    uint8_t creg;       //AT+CREG
    // +CGREG urc control
    uint8_t cgreg;
    uint8_t cereg;       //AT+CEREG

    // +CRC (1: enable extended format)
    uint8_t crc;

    // +IFC
    uint8_t rxfc;
    uint8_t txfc;

    // +ICF
    uint8_t icfFormat;
    uint8_t icfParity;

    // &D
    uint8_t dtrData;

    uint8_t secUartEnable;
    uint32_t secUartBaud;
    uint8_t secUartIcfFormat;
    uint8_t secUartIcfParity;

    // ndicator event reporting way (AT+CMER)
    uint8_t cmerInd[NUMBER_OF_SIM];

    // Save 4 config parameters:
    // ind_ctrl_mask:     not used
    // sind_ctrl_mask:  not used
    // which character set <chset> is used by the TE (GSM or UCS2)
    // not used
    cfg_info_t cfgInfo;

    // /////////////////////////////////////////////////
    // cc related
    // /////////////////////////////////////////////////
    // Duration of the tone in 1/10 second, which is the para of cmd VTD
    uint8_t cc_VTD;

    // This S-parameter control the automatic answering feature of the DCE.
    // 0:disable automatic answering
    // non-zero: the number of times (inticated by the value) to answer an incoming call
    uint8_t cc_s0;

    // Operator select the auto answer status (0-3)
    // ken
    uint8_t gprsAuto;
    uint32_t baudRate; // add by yangtt 08-04-18
    // ////////////////////////////////////////////////////////////
    // nw related
    // /////////////////////////////////////////////////////////////
    // Operator select format (AT+COPS)
    uint8_t operFormat[NUMBER_OF_SIM];

    // Operator selcet mode (AT+COPS)
    uint8_t copsModeTemp;

    // Parameter used in cmd (AT+CPOL)
    uint8_t preferredOperFormat;
    uint8_t preferredOperator[6];
    uint8_t preferredOperatorIndex;

    // Save the operator caller
    uint8_t preferredOperatorCaller;

    // /////////////////////////////////////////////////////
    // pbk related
    // /////////////////////////////////////////////////////
    // Read current phonebook entries used in cmd (AT+CPBR)
    PBK_LISTENTRY_CALLER pbkListEntryCaller;

    // para used in cmd (AT+WPCS)
    PBK_CHARACTER_SET pbkCharacterSet;

    // Select memory storage  (AT+CPBS)
    uint32_t tempStorage;

    // ////////////////////////////////////////////////
    // sat related
    // ////////////////////////////////////////////////
    // AT^STA
    uint8_t alphabet;

    // ////////////////////////////////////////////////
    // sms related
    // ///////////////////////////////////////////////
    at_sms_setting_t smsSetting;

#if defined(AT_MODULE_SUPPORT_SS)
    // ///////////////////////////////////////////////
    // ss related
    // ///////////////////////////////////////////////
    uint8_t callWaitingPresentResultCode; // CCWA 命令用到的cfg
    uint8_t ussd;                         // URC control signal
    uint8_t ucClip;
    uint8_t ucClir;
    uint8_t ucColp;
    uint8_t ucCSSU;
    uint8_t ucCSSI;
#endif
    // ///////////////////////////////////////////////
    // special related
    // ///////////////////////////////////////////////
    // Retrieve information of the serving/dedicated cell automatically every n seconds
    uint8_t moniPara;

    // Retrieve information of up to six neighbour cells automatically every n seconds
    uint8_t monpPara;

    // Flag of whether the URC ^CBCI:<bcs>,<bcl>,<Bat_state>,<Bat_volt> is reported
    uint8_t pmIndMark;

    // Receive gain selection
    uint8_t vgrVolume;

    // Transmit gain selection
    uint8_t vgtVolume;

    // enable or disable the uplink voice muting during a voice call
    uint8_t cmutMute;

    // Select and execute audio cycle test mode :0--aux,1--mic,2--loud (AT^AUST)
    uint8_t audioTestMode;
    
    // TCPIP related
    at_tcpip_setting_t tcpip;

#if defined(AT_MODULE_SUPPORT_OTA)
    //AT_DATACONN_PROFILE    g_OTA_dataconnProfile;
    AT_DATACONN_PROFILE    g_OTA_dataconnProfile[NUMBER_OF_SIM][AT_MAX_OTA_PROFILE];
#endif
    //AT_Gprs_CidInfo g_staAtGprsCidInfo[4][AT_PDPCID_MAX + 1];

    uint8_t sleepMode;
} at_setting_t;

typedef struct at_as_setting //auto save setting
{
    AT_URC_PORT_TYPE urcport;
} at_as_setting_t;

extern at_setting_t gAtCurrentSetting; // current settings
extern at_as_setting_t gAtCurAsSetting; // current auto save settings

// ///////////////////////////////////////////////////////////////////////////
// Macro definition of configuration setttings
// ///////////////////////////////////////////////////////////////////////////
// Flag of switching AT current settings
#define MANUFACTURER_DEFALUT_SETING 0
#define USER_SETTING_1 1

// /////////////////////////////////////////////////////////////////////////////////////////
// Macro to facilitate the use of Current AT settings
// /////////////////////////////////////////////////////////////////////////////////////////
// gc related
#define gATCurrenteResultcodePresentation gAtCurrentSetting.resultCodePresent
#define gATCurrenteResultcodeFormat gAtCurrentSetting.resultCodeFormat
#define gATCurrenteCommandEchoMode gAtCurrentSetting.commandEchoMode
#define gATCurrentnS3 gAtCurrentSetting.nS3
#define gATCurrentnS4 gAtCurrentSetting.nS4
#define gATCurrentnS5 gAtCurrentSetting.nS5
#define gATCurrentuCmee gAtCurrentSetting.cmee
#define gATCurrentCreg gAtCurrentSetting.creg
#define gATCurrentCgreg gAtCurrentSetting.cgreg
#define gATCurrentCereg gAtCurrentSetting.cereg
#define gATCurrentCrc gAtCurrentSetting.crc
#define gATCurrentRxfc gAtCurrentSetting.rxfc
#define gATCurrentTxfc gAtCurrentSetting.txfc
#define gATCurrentDtrData gAtCurrentSetting.dtrData
#define gATCurrentIcfFormat gAtCurrentSetting.icfFormat
#define gATCurrentIcfParity gAtCurrentSetting.icfParity
#define gATCurrentSecUartEnable gAtCurrentSetting.secUartEnable
#define gAtCurrentSecUartBaud gAtCurrentSetting.secUartBaud
#define gAtCurrentSecUartIcfFormat gAtCurrentSetting.secUartIcfFormat
#define gAtCurrentSecUartIcfParity gAtCurrentSetting.secUartIcfParity
#define gATCurrentuCmer_ind gAtCurrentSetting.cmerInd
// TODO..
#define gATCurrentCfgInfoInd_ctrl_mask gAtCurrentSetting.cfgInfo.ind_ctrl_mask
#define gATCurrentCfgInfoTe_chset gAtCurrentSetting.cfgInfo.te_chset
#define gATCurrentCfgInfoMt_chset gAtCurrentSetting.cfgInfo.mt_chset

// cc related
#define gATCurrentcc_VTD gAtCurrentSetting.cc_VTD
#define gATCurrentcc_s0 gAtCurrentSetting.cc_s0

// nw related
#define gATCurrentOperFormat gAtCurrentSetting.operFormat
#define gATCurrentCOPS_Mode_Temp gAtCurrentSetting.copsModeTemp
#define gATCurrentnPreferredOperFormat gAtCurrentSetting.preferredOperFormat
#define gATCurrentpArrPreferredOperater gAtCurrentSetting.preferredOperator
#define gATCurrentnPreferredOperatorIndex gAtCurrentSetting.preferredOperatorIndex
#define gATCurrentnGetPreferredOperatorsCaller gAtCurrentSetting.preferredOperatorCaller

// pbk related
#define gATCurrentpbkListEntryCaller gAtCurrentSetting.pbkListEntryCaller
#define gATCurrentpbkCharacterSet gAtCurrentSetting.pbkCharacterSet
#define gATCurrentnTempStorage gAtCurrentSetting.tempStorage

// sat related
#define gATCurrentAlphabet gAtCurrentSetting.alphabet

// sms related
#define gATCurrentAtSmsSettingSCSMP_fo gAtCurrentSetting.smsSetting.csmpFo
#define gATCurrentAtSmsSettingSCSDH_show gAtCurrentSetting.smsSetting.csdhShow
#define gATCurrentAtSmsSettingSg_SMSFormat gAtCurrentSetting.smsSetting.smsFormat
#define gATCurrentAtSMSInfo gAtCurrentSetting.smsSetting.smsInfo

// ss related
#define gATCurrentucSsCallWaitingPresentResultCode gAtCurrentSetting.callWaitingPresentResultCode
#define gATCurrentss_ussd gAtCurrentSetting.ussd
#define gATCurrentucClip gAtCurrentSetting.ucClip
#define gATCurrentucClir gAtCurrentSetting.ucClir
#define gATCurrentucColp gAtCurrentSetting.ucColp
#define gATCurrentucCSSU gAtCurrentSetting.ucCSSU
#define gATCurrentucCSSI gAtCurrentSetting.ucCSSI

// special related
#define gATCurrentu8nMONIPara gAtCurrentSetting.moniPara
#define gATCurrentu8nMONPPara gAtCurrentSetting.monpPara
#define gATCurrentnPMIndMark gAtCurrentSetting.pmIndMark
#define gATCurrentVGR_Volume gAtCurrentSetting.vgrVolume
#define gATCurrentVGT_Volume gAtCurrentSetting.vgtVolume
#define gATCurrentCMUT_mute gAtCurrentSetting.cmutMute
#define gATCurrentnAudioTestMode gAtCurrentSetting.audioTestMode

// gprs related
#define gATCurrentu8nURCAuto gAtCurrentSetting.gprsAuto

// added by yangtt at 04-18
#define gATCurrentu32nBaudRate gAtCurrentSetting.baudRate

// added by wy at 2017-11-09
#if defined(AT_MODULE_SUPPORT_OTA)
#define gATOtaDataconnProfile     gAtCurrentSetting.g_OTA_dataconnProfile
#endif
//#define g_staAtGprsCidInfo_e     gAtCurrentSetting.g_staAtGprsCidInfo

#define gATCurrentSleepMode     gAtCurrentSetting.sleepMode

// /////////////////////////////////////////////////////////////////////////////////
// API to operate at settings
// /////////////////////////////////////////////////////////////////////////////////
bool at_CfgInit(void);
bool at_CfgSetAtSettings(uint8_t set_id);
bool at_CfgGetAtSettings(uint8_t flag, uint8_t set_id);
bool at_CfgSaveAsSettings(void);

#endif
