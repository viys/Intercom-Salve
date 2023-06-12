#include "At_common.h"
#include "At_errcode.h"
#include "At_module.h"

#include "kingDef.h"
#include "kingPlat.h"
#include "kingrtos.h"
#include "kingcbdata.h"
#include "kingerrcode.h"
#include "kingsocket.h"
#include "kingfota.h"
#include "Kingflash.h"
#include "KingPowerManage.h"
#include "KingNet.h"
#include "KingMd5.h"
#include "KingUsim.h"
#include "at_cmd_tcpip.h"
#include "at_cmd_fota.h"
#include "app_version.h"
/*******************************************************************************
** MACROS
*******************************************************************************/
#define INVALID_SOCKET                 -1
#define FAIL                           -1
#define FOTA_CID                        1

//字符串类宏定义
#define HW_VERSION                     "1.00"
#define MODEL                          "LA1104-3"
#define MANUFACTURE                    "CZ"
#define PLAT_VERSION                   "RB1744"
#define PRIVATE_KEY                    "0000000011111111"
#define CZ_FOTA_PACKAGE_MAGIC          'C'
#define CZ_FOTA_SERVER_IP              "115.29.209.181"
#define CZ_FOTA_SERVER_HOSTNAME        "fotav3.cheerzing.com"
#define CZ_FOTA_SERVER_PORT            23177

//打包要用到的各种数据的长度宏
#define CZ_APP_MAGIC_LEN               8
#define CZ_TERMINAL_ID_LEN             8
#define CZ_MANUFACTURE_LEN             8
#define CZ_MODEL_LEN                   8
#define CZ_HWVER_LEN                   8
#define CZ_PLATFORMVER_LEN             8
#define CZ_MD5_LEN                     16
#define CZ_FOTA_SERVER_ADDRESS_LEN     32
#define CZ_FOTA_PACKAGE_DATALEN        512
#define CZ_FOTA_PROTOCOL_VER           0x10
#define CZ_FOTA_PACKAGE_MAXLEN         (CZ_FOTA_PACKAGE_DATALEN + 128)

//FotaTask中用到的Message ID
#define EV_CZ_FOTA_CONNECT_REQ         0
#define EV_CZ_FOTA_SOCKET_CONNECT_RSP  1
#define EV_CZ_FOTA_SOCKET_WRITE_REQ    2
#define EV_CZ_FOTA_SOCKET_READ_RSP     3
#define EV_CZ_FOTA_PARSE_DATA_RSP      4
#define EV_CZ_FOTA_RESEND_REQ          5
#define EV_CZ_FOTA_EXIT_REQ            6
#define EV_CZ_FOTA_REBOOT_REQ          7

//NV相关的宏定义
#define CZ_FOTA_INDEX_PS               0
#define CZ_FOTA_INDEX_KINGAPP          1
#define CZ_FOTA_INDEX_BOOTAPP          2
#define CZ_FOTA_NUM                    3
#define CZ_FOTA_NV_ID                  0

//Fota协议报文中的上下行Cmd ID
#define CZ_FOTA_CMD_CHECKUPDATE_REQ    0x8001
#define CZ_FOTA_CMD_CHECKUPDATE_RSP    0x8002
#define CZ_FOTA_CMD_GETFILEINFO_REQ    0x8003
#define CZ_FOTA_CMD_GETFILEINFO_RSP    0x8004
#define CZ_FOTA_CMD_DOWNLOADFILE_REQ   0x8005
#define CZ_FOTA_CMD_DOWNLOADFILE_RSP   0x8006
#define CZ_FOTA_CMD_REPORTRESULT_REQ   0x8007
#define CZ_FOTA_CMD_REPORTRESULT_RSP   0x8008
#define CZ_FOTA_CMD_ABNORAML_RSP       0x8100

//Fota下载错误码
#define ERR_AT_CME_CZFOTA_ILLEGAL_PARAM                 6010
#define ERR_AT_CME_CZFOTA_OPERATE_NOT_ALLOWED           6011
#define ERR_AT_CME_CZFOTA_DNS_ANALYZE_FAIL              6012
#define ERR_AT_CME_CZFOTA_SOCKET_CONNECT_FAIL           6013
#define ERR_AT_CME_CZFOTA_SOCKET_SEND_FAIL              6014
#define ERR_AT_CME_CZFOTA_SOCKET_RECV_FAIL              6015
#define ERR_AT_CME_CZFOTA_FS_NO_ENOUGH_SPACE            6016
#define ERR_AT_CME_CZFOTA_NET_ERROR                     6017
#define ERR_AT_CME_CZFOTA_NOT_NEED_UPDATE               6018
#define ERR_AT_CME_CZFOTA_GET_FILE_SIZE                 6019
#define ERR_AT_CME_CZFOTA_READ_FILE                     6020
#define ERR_AT_CME_CZFOTA_NO_IMG_TO_UPDATE              6021
#define ERR_AT_CME_CZFOTA_IMG_ERROR                     6022    //错误的固件包


#define CZFOTA_LOG(fmt, ...) \
        do \
        { \
            KING_SysLog("[CZFOTA] "fmt, ##__VA_ARGS__); \
        } while (0)

/*******************************************************************************
 ** Type Definitions
*******************************************************************************/

/**************************Enumeration Definitions*****************************/
typedef enum
{
    CZ_FOTA_STATUS_UPDATE_ERROR = 0,
    CZ_FOTA_STATUS_UPDATE_OK,
    CZ_FOTA_STATUS_DL_OK,
    CZ_FOTA_STATUS_DL_ERROR
} CZ_FOTA_FILE_STATUS_E;

typedef enum
{
    CZ_FOTA_STATE_INIT = 0,
    CZ_FOTA_STATE_CHECK,
    CZ_FOTA_STATE_GETFILEINFO,
    CZ_FOTA_STATE_DOWNLOAD,
    CZ_FOTA_STATE_UPDATE,
    CZ_FOTA_STATE_CONTINUE,
    CZ_FOTA_STATE_MAX
} CZ_FOTA_STATE_E;

typedef enum
{
    CZ_FOTA_NO_ERROR = 0,
    CZ_FOTA_NOT_NEED_UPDATE,//
    CZ_FOTA_ILLEGAL_PARAM,//Illegal parameter error
    CZ_FOTA_OPERATE_NOT_ALLOWED,//Operation is not allowed
    CZ_FOTA_NET_ERROR,
    CZ_FOTA_NDS_ANALYZE_FAIL,//nds Analyze Fail  //5
    CZ_FOTA_SOCKET_CONNECT_FAIL,
    CZ_FOTA_SOCKET_SEND_FAIL,
    CZ_FOTA_SOCKET_RECV_FAIL,
    CZ_FOTA_SOCKET_SELECT_FAIL,
    CZ_FOTA_APPID_ERROR,    //10
    CZ_FOTA_MAGIC_ERROR,
    CZ_FOTA_PROTOCOL_ERROR,
    CZ_FOTA_CMDID_ERROR,
    CZ_FOTA_MD5_ERROR,
    CZ_FOTA_ENCRYPT_ERROR,  //15
    CZ_FOTA_DECRYPT_ERROR,
    CZ_FOTA_DATALEN_ERROR,
    CZ_FOTA_FILEID_ERROR,
    CZ_FOTA_OFFSET_ERROR,
    CZ_FOTA_IMAGECHECK_FAIL, //20
    CZ_FOTA_FLAGSET_FAIL,
    CZ_FOTA_UNKONW_ERR,
} CZ_FOTA_ERROR_E;

typedef enum
{
    CZ_FOTA_COMPLETE,     //升级任务结束，已经上报平台，等待下一次FOTA任务
    CZ_FOTA_NEEDDOWNLOAD, //检测到更新，需要下载
    //CZ_FOTA_NEEDUPGRADE,  //已经下载固件，需要升级
    //CZ_FOTA_NEEDREPORT,   //已经升级成功或失败，需要上报平台
    //CZ_FOTA_NEEDCHECK,    //需要发起更新检测
    CZ_FOTA_MAX
}CZ_FOTA_NV_STATE_E;

typedef enum
{
    CZ_FOTA_FILE_TYPE_PS  = 1,
    CZ_FOTA_FILE_TYPE_KINGAPP,
    CZ_FOTA_FILE_TYPE_BOOTAPP,
    CZ_FOTA_FILE_TYPE_MAX
} CZ_FOTA_FILE_TYPE_E;

typedef enum
{
    CZ_FOTA_DL_IDLE,
    CZ_FOTA_DL_DOING,
    CZ_FOTA_DL_FAIL,
    CZ_FOTA_DL_DONE
} CZ_FOTA_DL_STATE_E;

typedef enum
{
    CZ_AT_FOTA_CHKINIT,     //未查询
    CZ_AT_FOTA_CHKING,      //查询中
    CZ_AT_FOTA_CHKFAIL,     //查询失败，网络原因或DNS解析失败
    CZ_AT_FOTA_CHKDONE      //查询完毕
} CZ_AT_FOTA_STATE_E;

/**************************Structure Definitions*******************************/
typedef struct
{
    char    serverHost[CZ_FOTA_SERVER_ADDRESS_LEN+1];
    uint16  serverPort;
}CZ_SERVER_HOST_S;

typedef struct
{
    THREAD_HANDLE     FotaTaskId;
    MSG_HANDLE        FotaMsgHandle;
}CZ_FOTA_TASK_MSG_S;

typedef struct
{
    TIMER_HANDLE      fota_timerHandle;
    TIMER_ATTR_S      fota_timerattr;
    bool              NeedResend;
}CZ_FOTA_TIMER_S;

typedef struct
{
    uint8   AppId;
    char    Manufacture[CZ_MANUFACTURE_LEN];
    char    Model[CZ_MODEL_LEN];
    uint8   FileVerMajor;
    uint8   FileVerMinor;
}CZ_FOTA_FILE_VERINFO_S;

typedef struct
{
    CZ_FOTA_NV_STATE_E   State[CZ_FOTA_NUM];
    uint32               FileId;
    uint32               FileVerifyCode;
    uint32               Offset;
    uint32               FileLenth;
} FOTA_NV_S;

typedef struct
{
    int32    RecvBytes;
    uint8    RecvDataBuf[CZ_FOTA_PACKAGE_MAXLEN];
    int32    SendBytes;
    uint8    SendDataBuf[CZ_FOTA_PACKAGE_MAXLEN];
} CZ_FOTA_TRXBUFF_S;

typedef struct
{
    char     Manufacture[CZ_MANUFACTURE_LEN];
    char     Model[CZ_MODEL_LEN];
    char     HwVer[CZ_HWVER_LEN];
    char     PlatformVer[CZ_PLATFORMVER_LEN];
}CZ_FOTA_TERMINAL_INFO_S;

typedef struct
{
    CZ_FOTA_TERMINAL_INFO_S TerminalInfo;
    CZ_FOTA_FILE_VERINFO_S  FileVerInfo[CZ_FOTA_NUM];
}CZ_FOTA_FILE_CHECKINFO_S;

typedef struct
{
    char    Magic;
    uint8   ProtocolVer;
    uint16  PkgLength;
    uint32  CmdId;
    uint8   MD5[CZ_MD5_LEN];
    char    ID[CZ_TERMINAL_ID_LEN];
}CZ_FOTA_REQ_PKG_HEAD_S;

typedef struct
{
    char    Magic;
    uint8   ProtocolVer;
    uint16  PkgLength;
    uint32  CmdId;
    uint8   MD5[CZ_MD5_LEN];
}CZ_FOTA_RSP_PKG_HEAD_S;

typedef struct 
{
    uint8   AppId;
    uint8   UpdateFlag;
}CZ_FOTA_FILE_UPDATEFLAG_S;

typedef struct
{
    uint32  AppId;
    uint32  FileId;
    uint32  FileLength;
    uint32  FileVerifyCode;
    uint8   FileVerMajor;
    uint8   FileVerMinor;
}CZ_FOTA_FILE_GETINFO_S;

typedef struct
{
    uint32  FileId;
    uint32  GetLength;
    uint32  Offset;
    uint32  FileVerifyCode;
}CZ_FOTA_FILE_DATA_REQ_S;

typedef struct
{
    uint32  FileId;
    uint32  DataLength;
    uint32  Offset;
    uint8   FileData[CZ_FOTA_PACKAGE_DATALEN];
}CZ_FOTA_FILE_DATA_RSP_S;

typedef struct
{
    uint32  FileId;
    uint32  Status; //FOTA升级状态: 0升级失败，1已升级，2下载完成，3下载异常
    uint8   FileVerMajor;
    uint8   FileVerMinor;
}CZ_FOTA_FILE_STATUS_IND_S;

typedef struct
{
    uint32  FileId;
    uint32  Status;
}CZ_FOTA_FILE_STATUS_CNF_S;

typedef struct
{
    CZ_FOTA_DL_STATE_E  dl_state;
    uint8               dl_progress;
    uint32              dl_errorcode;
    int8                dl_file_vaild; //下载的包是否正确，是否能通过校验。
}CZ_FOTA_DL_STAT_S;

typedef struct tagTEACTX 
{ 
    uint8 buf[8] ; 
    uint8 bufPre[8] ; 
    const uint8 *pKey ; //指向16字节的key
    uint8 *pCrypt ; 
    uint8 *pCryptPre ; 
} CZ_TEACTX, *CZ_LPTEACTX ;

extern int KING_CzPrdSoftVersion(char *softver);

/*******************************************************************************
 ** Local Function Declerations
 ******************************************************************************/
static void  CZ_FotaPrintfHex(char *title, uint8 *buf, int32 buf_len);
static void  CZ_Get_PS_Svn_Ver(uint16* svn_ver);
static void  CZ_Get_BApp_Svn_Ver(uint16* svn_ver);

static void  CZ_FotaSetHost(char* host);
static void  CZ_FotaSetPort(uint16 port);
static CZ_FOTA_STATE_E CZ_FotaGetState(void);
static void CZ_FotaSetState(CZ_FOTA_STATE_E state);
static int32 CZ_FotaHandleSockRsp(uint32 eventID, void* pdata, uint32 len);


//报文解析处理函数相关声明
typedef int32 (*CZ_FotaCmdParse)(uint8* input, uint32 inputlen);
static int32 CZ_FotaParse_Null(uint8* input, uint32 inputlen);
static int32 CZ_FotaCheckUpdateParse(uint8* input, uint32 inputlen);
static int32 CZ_FotaGetFileInfoParse(uint8* input, uint32 inputlen);
static int32 CZ_FotaDownloadParse(uint8* input, uint32 inputlen);
static int32 CZ_FotaReportParse(uint8* input, uint32 inputlen);
static int32 CZ_FotaPreParseData(void);
static int32 CZ_FotaParseData(void);

//发起任务相关函数声明
static void  CZ_Fota_Connect(void);
static void  CZ_Fota_ConnectRsp(void);
static bool  CZ_Fota_Check_IncompleteMission(void);
static int32 CZ_FotaSendUpdateCheckReq(void);
static int32 CZ_FotaSendGetFileInfoReq(uint8 AppIds);
static int32 CZ_FotaSendDownloadReq(void);
static int32 CZ_FotaSendReportReq(CZ_FOTA_FILE_STATUS_E status);

//FotaNV相关函数声明
static void CZ_Init_FotaNv(void);
static void CZ_Get_FotaNv(FOTA_NV_S* fota_nv);
static void CZ_Set_FotaNv(FOTA_NV_S* fota_nv);
static uint8 CZ_Fota_Get_SpecNvStateIndex(FOTA_NV_S* fota_state, CZ_FOTA_NV_STATE_E specstate);
static int32 CZ_Fota_CheckImage(void);

static void URC_CmdFunc_CZUPDL(uint8 result, uint32 status);
static void URC_CmdFunc_CZUPCHK(uint16 need_dl);
static void CZ_Fota_ConnectClose(void);
static void CZ_FotaSetErrorCode(uint32 errorcode);
static int8 CZ_Fota_Get_ActCid(void);
static int32 CZ_FotaConnectServer(SOCK_IN_ADDR_T ip,uint16 port);
static int32  CZ_CheckNetState(void);

/*******************************************************************************
 ** Variables
 ******************************************************************************/
extern UINT8                g_rspStr[];//URC
static AT_CMD_ENGINE_T *    gFotaEngine;
static softwareHandle       hSocketContext          = -1;
static CZ_AT_FOTA_STATE_E   s_cz_fota_chk_flag      = CZ_AT_FOTA_CHKINIT;
static CZ_FOTA_STATE_E      s_cz_fotaState          = CZ_FOTA_STATE_MAX;
static int32                s_cz_FotaSocket         = INVALID_SOCKET;
static MUTEX_HANDLE         s_fota_check_mutex      = NULL;
static CZ_SERVER_HOST_S     s_cz_fota_server        = {0};
static CZ_FOTA_TASK_MSG_S   s_cz_fota_task_info     = {0};
static CZ_FOTA_TRXBUFF_S    s_cz_socket_trxbuff     = {0};
static FOTA_NV_S            s_cz_fota_nv            = {0};
static CZ_FOTA_DL_STAT_S    s_cz_fota_dl_state      = {0};
static uint16 g_czErrCode = 0;
static bool isDownLoading = FALSE;
const  CZ_FotaCmdParse      Fota_Cmd_Parse[5]       = 
{
    CZ_FotaParse_Null,
    CZ_FotaCheckUpdateParse,
    CZ_FotaGetFileInfoParse,
    CZ_FotaDownloadParse,
    CZ_FotaReportParse,
};
//TEA加密相关函数
static uint32 Host2NetLong(uint32 ulHost) 
{ 
    const uint16 us = 0x1234 ; 
    return ((uint8 *)&us)[0] == 0x12 ? ulHost : (((ulHost>>8) & 0xFF00) |  
        ((ulHost<<8) & 0xFF0000) | (ulHost<<24) | (ulHost>>24)) ; 
} 
  
static uint32 Net2HostLong(uint32 ulHost) 
{ 
    return Host2NetLong(ulHost) ; 
} 
  
static void EnCipher(const uint32 *const v, const uint32 *const k, uint32 *const w) 
{ 
    register uint32  
        y     = Host2NetLong(v[0]), 
        z     = Host2NetLong(v[1]), 
        a     = Host2NetLong(k[0]), 
        b     = Host2NetLong(k[1]), 
        c     = Host2NetLong(k[2]), 
        d     = Host2NetLong(k[3]), 
        n     = 0x10,       /* do encrypt 16 (0x10) times */ 
        sum   = 0, 
        delta = 0x9E3779B9; /*  0x9E3779B9 - 0x100000000 = -0x61C88647 */ 
  
    while (n-- > 0) 
    { 
        sum += delta; 
        y += ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b); 
        z += ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d); 
    } 
  
    w[0] = Net2HostLong(y); 
    w[1] = Net2HostLong(z); 
} 
  
static void DeCipher(const uint32 *const v, const uint32 *const k, uint32 *const w) 
{ 
    register uint32 
        y     = Host2NetLong(v[0]), 
        z     = Host2NetLong(v[1]), 
        a     = Host2NetLong(k[0]), 
        b     = Host2NetLong(k[1]), 
        c     = Host2NetLong(k[2]), 
        d     = Host2NetLong(k[3]), 
        n     = 0x10, 
        sum   = 0xE3779B90,  
        /* why this ? must be related with n value*/ 
        delta = 0x9E3779B9; 
  
    /* sum = delta<<5, in general sum = delta * n */ 
    while (n-- > 0) 
    { 
        z -= ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d); 
        y -= ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b); 
        sum -= delta; 
    } 
  
    w[0] = Net2HostLong(y); 
    w[1] = Net2HostLong(z); 
} 
  
static uint32 Random(void) 
{ 
    return (uint32)rand(); 
}
  
static void EncryptEach8Bytes(CZ_TEACTX *pCtx) 
{ 
#ifdef CRYPT_ONE_BYTE 
    uint32 i ; 
    uint8 *pPlain8, *pPlainPre8, *pCrypt8, *pCryptPre8 ; 
    pPlain8 = (uint8 *)pCtx->buf ; 
    pPlainPre8 = (uint8 *)pCtx->bufPre ; 
    pCrypt8 = (uint8 *)pCtx->pCrypt ; 
    pCryptPre8 = (uint8 *)pCtx->pCryptPre ; 
    for(i=0; i<8; i++) 
        pPlain8[i] ^= pCryptPre8[i] ; 
    EnCipher((uint32 *)pPlain8, (uint32 *)pCtx->pKey, (uint32 *)pCrypt8) ; 
    for(i=0; i<8; i++) 
        pCrypt8[i] ^= pPlainPre8[i] ; 
    // 
    for(i=0; i<8; i++) 
        pPlainPre8[i] = pPlain8[i] ; 
#else 
    uint32 *pPlain8, *pPlainPre8, *pCrypt8, *pCryptPre8 ; 
    pPlain8 = (uint32 *)pCtx->buf ; 
    pPlainPre8 = (uint32 *)pCtx->bufPre ; 
    pCrypt8 = (uint32 *)pCtx->pCrypt ; 
    pCryptPre8 = (uint32 *)pCtx->pCryptPre ; 
    pPlain8[0] ^= pCryptPre8[0] ; 
    pPlain8[1] ^= pCryptPre8[1] ; 
    EnCipher(pPlain8, (const uint32 *)pCtx->pKey, pCrypt8) ; 
    pCrypt8[0] ^= pPlainPre8[0] ; 
    pCrypt8[1] ^= pPlainPre8[1] ; 
    pPlainPre8[0] = pPlain8[0] ; 
    pPlainPre8[1] = pPlain8[1] ; 
#endif 
    pCtx->pCryptPre = pCtx->pCrypt ; 
    pCtx->pCrypt += 8 ; 
} 
  
uint32 CZ_Tea_Encrypt(CZ_TEACTX *pCtx, const uint8 *pPlain, uint32 ulPlainLen,  
    const uint8 *pKey, uint8 *pOut, uint32 *pOutLen) 
{ 
    uint32 ulPos, ulPadding, ulOut ; 
    const uint8 *p ; 
    if(pPlain == NULL || ulPlainLen == 0 || pOutLen == NULL) 
        return 0 ; 
    ulPos = (8 - ((ulPlainLen + 10) & 0x07)) & 0x07 ; 
    ulOut = 1 + ulPos + 2 + ulPlainLen + 7 ; 
    if(*pOutLen < ulOut) 
    { 
        *pOutLen = ulOut ; 
        return 0 ; 
    } 
    *pOutLen = ulOut ; 
    memset(pCtx, 0, sizeof(CZ_TEACTX)) ; 
    pCtx->pCrypt = pOut ; 
    pCtx->pCryptPre = pCtx->bufPre ; 
    pCtx->pKey = pKey ; 
    pCtx->buf[0] = (uint8)((Random() & 0xF8) | ulPos) ; 
    memset(pCtx->buf+1, (uint8)Random(), ulPos++) ; 
    for(ulPadding=0; ulPadding<2; ulPadding++) 
    { 
        if(ulPos == 8) 
        { 
            EncryptEach8Bytes(pCtx) ; 
            ulPos = 0 ; 
        } 
        pCtx->buf[ulPos++] = (uint8)Random() ; 
    } 
    p = pPlain ; 
    while(ulPlainLen > 0) 
    { 
        if(ulPos == 8) 
        { 
            EncryptEach8Bytes(pCtx) ; 
            ulPos = 0 ; 
        } 
        pCtx->buf[ulPos++] = *(p++) ; 
        ulPlainLen-- ; 
    } 
    for(ulPadding=0; ulPadding<7; ulPadding++) 
        pCtx->buf[ulPos++] = 0x00 ; 
    EncryptEach8Bytes(pCtx) ; 
    return ulOut ; 
} 
  
static void DecryptEach8Bytes(CZ_TEACTX *pCtx) 
{ 
#ifdef CRYPT_ONE_BYTE 
    uint32 i ; 
    uint8 bufTemp[8] ; 
    uint8 *pBuf8, *pBufPre8, *pCrypt8, *pCryptPre8 ; 
    pBuf8 = (uint8 *)pCtx->buf ; 
    pBufPre8 = (uint8 *)pCtx->bufPre ; 
    pCrypt8 = (uint8 *)pCtx->pCrypt ; 
    pCryptPre8 = (uint8 *)pCtx->pCryptPre ; 
    for(i=0; i<8; i++) 
        bufTemp[i] = pCrypt8[i] ^ pBufPre8[i] ; 
    DeCipher((uint32 *)bufTemp, (uint32 *)pCtx->pKey, (uint32 *)pBufPre8) ; 
    for(i=0; i<8; i++) 
        pBuf8[i] = pBufPre8[i] ^ pCryptPre8[i] ; 
#else 
    uint32 bufTemp[2] ; 
    uint32 *pBuf8, *pBufPre8, *pCrypt8, *pCryptPre8 ; 
    pBuf8 = (uint32 *)pCtx->buf ; 
    pBufPre8 = (uint32 *)pCtx->bufPre ; 
    pCrypt8 = (uint32 *)pCtx->pCrypt ; 
    pCryptPre8 = (uint32 *)pCtx->pCryptPre ; 
    bufTemp[0] = pCrypt8[0] ^ pBufPre8[0] ; 
    bufTemp[1] = pCrypt8[1] ^ pBufPre8[1] ; 
    DeCipher(bufTemp, (const uint32 *)pCtx->pKey, pBufPre8) ; 
    pBuf8[0] = pBufPre8[0] ^ pCryptPre8[0] ; 
    pBuf8[1] = pBufPre8[1] ^ pCryptPre8[1] ; 
#endif 
    pCtx->pCryptPre = pCtx->pCrypt ; 
    pCtx->pCrypt += 8 ; 
} 
  
uint32 CZ_Tea_Decrypt(CZ_TEACTX *pCtx, const uint8 *pCipher, uint32 ulCipherLen,  
    const uint8 *pKey, uint8 *pOut, uint32 *pOutLen) 
{ 
    uint32 ulPos, ulPadding, ulOut, ul ; 
    if(pCipher == NULL || pOutLen == NULL ||  
            ulCipherLen < 16 || (ulCipherLen & 0x07) != 0) 
        return 0 ;

    memset(pCtx, 0, sizeof(CZ_TEACTX)) ; 
    pCtx->pCrypt = pOut ; 
    pCtx->pCryptPre = pCtx->bufPre ; 
    pCtx->pKey = pKey ;

    DeCipher((const uint32 *)pCipher, (const uint32 *)pKey, (uint32 *)pCtx->bufPre) ; 
    for(ul=0; ul<8; ul++) 
        pCtx->buf[ul] = pCtx->bufPre[ul] ; 
    ulPos = pCtx->buf[0] & 0x07 ; 
    if(ulPos > 1) 
    { 
        for(ulOut=2; ulOut<=ulPos; ulOut++) 
        { 
            if(pCtx->buf[1] != pCtx->buf[ulOut]) 
            { 
                *pOutLen = 0 ; 
                return 0 ;
            } 
        } 
    } 
    ulOut = ulCipherLen - ulPos - 10 ; 
    if(ulPos + 10 > ulCipherLen || *pOutLen < ulOut) 
        return 0 ; 
    pCtx->pCryptPre = (uint8 *)pCipher ; 
    pCtx->pCrypt = (uint8 *)pCipher + 8 ; 
    ulPos++ ; 
    for(ulPadding=0; ulPadding<2; ulPadding++) 
    { 
        if(ulPos == 8) 
        { 
            DecryptEach8Bytes(pCtx) ; 
            ulPos = 0 ; 
        } 
        ulPos++ ; 
    } 
    // 
    for(ul=0; ul<ulOut; ul++) 
    { 
        if(ulPos == 8) 
        { 
            DecryptEach8Bytes(pCtx) ; 
            ulPos = 0 ; 
        } 
        pOut[ul] = pCtx->buf[ulPos] ; 
        ulPos++ ; 
    } 
    // 
    for(ulPadding=0; ulPadding<7; ulPadding++) 
    { 
        if(ulPos < 8) 
        { 
            if(pCtx->buf[ulPos] != 0x00) 
            { 
                *pOutLen = 0 ; 
                return 0 ; 
            } 
        } 
        ulPos++ ; 
    } 
    *pOutLen = ulOut ; 
    return 1 ; 
} 

static void FotaDnsReqCb(void* pData, uint32 len)
{
    ASYN_DNS_PARSE_T *dnsParse = (ASYN_DNS_PARSE_T *)pData;
    
    CZFOTA_LOG("socket_dnsReqCb hostname=%s\r\n", dnsParse->hostname);
    CZFOTA_LOG("socket_dnsReqCb dnsParse->result=%d\n", dnsParse->result);
    if (dnsParse->result == DNS_PARSE_SUCCESS)
    {
        SOCK_IN_ADDR_T ipaddr = dnsParse->addr;
        if(CZ_FotaConnectServer(ipaddr, s_cz_fota_server.serverPort) != 0)
        {
            goto fail_exit;
        }
    }
    else
    {
        CZFOTA_LOG("socket_dnsReqCb fail!\n");
        goto fail_exit;
    }
    
    return;

fail_exit:

    if(CZ_CheckNetState() != 0)
    {
        CZ_FotaSetErrorCode(ERR_AT_CME_CZFOTA_SOCKET_CONNECT_FAIL);
    }
    
    if(CZ_FotaGetState() == CZ_FOTA_STATE_CHECK)
    {
        KING_MutexLock(s_fota_check_mutex, WAIT_OPT_INFINITE);
        s_cz_fota_chk_flag = CZ_AT_FOTA_CHKFAIL;
        g_czErrCode = ERR_AT_CME_CZFOTA_DNS_ANALYZE_FAIL;
        KING_MutexUnLock(s_fota_check_mutex);
        URC_CmdFunc_CZUPCHK(ERR_AT_CME_CZFOTA_DNS_ANALYZE_FAIL);
    }
    else if (CZ_FotaGetState() == CZ_FOTA_STATE_CONTINUE)
    {
        URC_CmdFunc_CZUPDL(CZ_FOTA_DL_FAIL, ERR_AT_CME_CZFOTA_DNS_ANALYZE_FAIL);
        isDownLoading = FALSE;
    }
    else if (CZ_FotaGetState() == CZ_FOTA_STATE_UPDATE)
    {
        memset(g_rspStr, 0x00, 1024);
        sprintf((char *)g_rspStr, "+CZUPDATE: %d", ERR_AT_CME_CZFOTA_DNS_ANALYZE_FAIL);
        at_CmdRespUrcText(gFotaEngine, g_rspStr);
    }
    CZ_Fota_ConnectClose();
    return;
}

/*************************************************************
函数名称:AT_GPRS_IPAddrAnalyzer
函数描述:检测输入进来的地址是IP还是域名
*************************************************************/
static int32 AT_GPRS_IPAddrAnalyzer(const char *pPdpAddr, uint8 uSize)
{
    uint8 i = 0;
    uint8 j = 0;
    uint8 m = 0;
    char uBuf[100] = {0};
    uint8 uIpNum = 0;

    if ((NULL == pPdpAddr) || (0 == uSize))
    {
        return -1;
    }

    for (i = 0; i < 4; i++)
    {
        m = 0;

        while ((pPdpAddr[j] != '.') && (j < uSize))
        {
            uBuf[m++] = pPdpAddr[j++];
        }

        uIpNum = atoi(uBuf);

        if ((uBuf[0] != '0') && (uIpNum < 1 || uIpNum > 254))
        {
            return -1;
        }

        memset(uBuf, 0, 4);

        j++;
    }

    return 1;
}


/*************************************************************
函数名称:CZ_Fota_Connect
函数描述:发起一次socket连接请求
*************************************************************/
static void CZ_Fota_Connect(void)
{
    MSG_S msg={0};
    msg.messageID = EV_CZ_FOTA_CONNECT_REQ;

    if (s_cz_fota_task_info.FotaMsgHandle == NULL)
    {
        CZFOTA_LOG("Fota thread is not ready!");
    } 
    else 
    {
        KING_MsgSend(&msg, s_cz_fota_task_info.FotaMsgHandle);
    }
}

/*************************************************************
函数名称:CZ_Fota_ConnectRsp
函数描述:Socket连接成功的响应，根据FotaNV确定下一步要做的任务
*************************************************************/
static void CZ_Fota_ConnectRsp(void)
{
    CZFOTA_LOG("%s",__FUNCTION__);
    switch(CZ_FotaGetState())
    {
        case CZ_FOTA_STATE_CHECK:
            {
                CZ_FotaSendUpdateCheckReq();
            }
            break;

        case CZ_FOTA_STATE_CONTINUE:
            {
                CZ_Fota_Check_IncompleteMission();
            }
            break;

        case CZ_FOTA_STATE_UPDATE:
            {
                CZ_FOTA_FILE_STATUS_E result;
                result = (s_cz_fota_dl_state.dl_file_vaild == 0) ? CZ_FOTA_STATUS_UPDATE_OK : CZ_FOTA_STATUS_UPDATE_ERROR;
                CZ_FotaSendReportReq(result);
            }
            break;
        default:
            break;
    }
}

/*************************************************************
函数名称:CZ_Fota_ConnectClose
函数描述:关闭socket连接
*************************************************************/
static void CZ_Fota_ConnectClose(void)
{
    if(s_cz_FotaSocket != INVALID_SOCKET)
    {
        KING_SocketClose(s_cz_FotaSocket);
        s_cz_FotaSocket = INVALID_SOCKET;
        KING_UnRegNotifyFun(DEV_CLASS_SOCKET, s_cz_FotaSocket, (NOTIFY_FUN)CZ_FotaHandleSockRsp);
    }
}

/*************************************************************
函数名称:CZ_FotaSetState
函数描述:设置当前fota状态
*************************************************************/
static void CZ_FotaSetState(CZ_FOTA_STATE_E state)
{
    s_cz_fotaState = state;
}

/*************************************************************
函数名称:CZ_FotaGetState
函数描述:获取当前fota状态
*************************************************************/
static CZ_FOTA_STATE_E CZ_FotaGetState(void)
{
    return s_cz_fotaState;
}

/*************************************************************
函数名称:CZ_FotaSetHost
函数描述:设置fota服务器主机名
*************************************************************/
static void CZ_FotaSetHost(char* host)
{
    strncpy(s_cz_fota_server.serverHost, host, CZ_FOTA_SERVER_ADDRESS_LEN+1);
}

/*************************************************************
函数名称:CZ_FotaSetPort
函数描述:设置目标IP的端口
*************************************************************/
static void CZ_FotaSetPort(uint16 port)
{
    s_cz_fota_server.serverPort = port;
}

/*************************************************************
函数名称:CZ_FotaSetErrorCode
函数描述:设置错误码
*************************************************************/
static void CZ_FotaSetErrorCode(uint32 errorcode)
{
    s_cz_fota_dl_state.dl_errorcode = errorcode;
}

/*************************************************************
函数名称:CZ_FotaGetErrorCode
函数描述:获取错误码
*************************************************************/
static uint32 CZ_FotaGetErrorCode(void)
{
    return s_cz_fota_dl_state.dl_errorcode;
}

/*************************************************************
函数名称:CZ_Fota_Get_ActCid
函数描述:获取当前激活的CID,没有任何CID激活则返回-1
*************************************************************/
static int8 CZ_Fota_Get_ActCid(void)
{
    int8 ret = FAIL;
    uint32 value = 0;
    uint8  cid;
    for(cid = 1; cid <= 4 ; cid++)
    {
        KING_NetPdpStatusGet(cid, &value);
        if(value)
        {
            break;
        }
    }
    if(value != 0)
    {
        ret = cid;
    }

    return ret;
}

/*************************************************************
函数名称:CZ_CheckNetState
函数描述:检测PS附着状态和PDP激活状态
*************************************************************/
static int32  CZ_CheckNetState(void)
{
    uint32 value = 0;
    KING_NetAttachStatusGet(&value);
    if (value == 1)
    {
        value = 0;
        KING_NetPdpStatusGet(FOTA_CID, &value);
        if (value == 1)
        {
            return 0;
        }
    }

    CZ_FotaSetErrorCode(ERR_AT_CME_CZFOTA_NET_ERROR);
    return -1;
}

/*************************************************************
函数名称:CZ_FotaGetPrivateKey
函数描述:获取终端秘钥
*************************************************************/
static int32 CZ_FotaGetPrivateKey(uint8* pKey)
{
    int32 ret = 0;

    if (pKey == NULL)
    {
        ret = -1;
    }
    else
    {
        int32 len = 0;
        len = strlen((char*)pKey);
        if (0 == len)
        {
            memcpy(pKey,PRIVATE_KEY,16);
        }
    }

    return ret;
}

/*************************************************************
函数名称:CZ_FotaGetTerminalId
函数描述:获取IMEI
*************************************************************/
static void CZ_FotaGetTerminalId(uint8 *pTerminalId)
{
    uint8 imeiStr[16] = {0};
    uint8 i = 0;

    KING_GetSysImei(imeiStr);

    CZFOTA_LOG("IMEI=%s",imeiStr);

    for (i=0; i<CZ_TERMINAL_ID_LEN; i++)
    {
        if (i == (CZ_TERMINAL_ID_LEN-1))
        {
            pTerminalId[i]= (((imeiStr[i*2]-'0') << 4) & 0xf0) | 0x0f;
        }
        else
        {
            pTerminalId[i]= (((imeiStr[i*2]-'0') << 4) & 0xf0) | ((imeiStr[i*2+1]-'0') & 0x0f);
        }
    }
}

/*************************************************************
函数名称:CZ_FotaGetProtocolVer
函数描述:返回fota协议版本号
*************************************************************/
static uint8 CZ_FotaGetProtocolVer(void)
{
    uint8 ret;

    ret = CZ_FOTA_PROTOCOL_VER;
    return ret;
}

/*************************************************************
函数名称:CZ_FotaGetHWVerInfo
函数描述:返回硬件版本号
*************************************************************/
static void CZ_FotaGetHWVerInfo(char * pHWInfo)
{
    memset(pHWInfo, 0x00, 8);
    strncpy(pHWInfo,HW_VERSION, 8);
}

/*************************************************************
函数名称:CZ_FotaGetPlatFormVerInfo
函数描述:返回平台版本号
*************************************************************/
static void CZ_FotaGetPlatFormVerInfo(char *pPlatFormVerInfo)
{
    memset(pPlatFormVerInfo, 0x00, 8);
    strncpy(pPlatFormVerInfo,PLAT_VERSION, 8);
}

/*************************************************************
函数名称:CZ_FotaGetTerminalInfo
函数描述:打包厂商信息
*************************************************************/
static void CZ_FotaGetTerminalInfo(CZ_FOTA_TERMINAL_INFO_S *pTerminalInfo)
{
    char str[80];
    char *p;

    memcpy(pTerminalInfo->Manufacture,MANUFACTURE,sizeof(MANUFACTURE));

    KING_CzPrdSoftVersion(str);
    p = strchr(str, '_');
    memcpy(pTerminalInfo->Model, str, p - str);

    CZ_FotaGetHWVerInfo(pTerminalInfo->HwVer);
    CZ_FotaGetPlatFormVerInfo(pTerminalInfo->PlatformVer);
}

/*************************************************************
函数名称:CZ_FotaPrintfHex
函数描述:以HEX格式打印数据
*************************************************************/
static void CZ_FotaPrintfHex(char *title, uint8 *buf, int32 buf_len)
{
    char temp[128];
    int32 i;

    memset(temp, 0, sizeof(temp));
    CZFOTA_LOG("%s hex len:%d", title, buf_len);
    if (buf_len <= 0)
    {
        return;
    }

    for (i = 0; i < buf_len; i++)
    {
        sprintf(temp + strlen(temp),"%02X ",buf[i]);
        if ((i%16) == 15)
        {
            CZFOTA_LOG("%s",temp);
            memset(temp, 0, sizeof(temp));
        }
    }
    CZFOTA_LOG("%s",temp);
}

/*************************************************************
函数名称:CZ_FotaHandleSockRsp
函数描述:socket连接的回调
*************************************************************/
static int32 CZ_FotaHandleSockRsp(uint32 eventID, void* pdata, uint32 len)
{
    int32 ret = FAIL;
    MSG_S msg = {0};
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pdata;
    s_cz_socket_trxbuff.RecvBytes = data->socket.availSize;
    CZFOTA_LOG("%s:eventID=0x%X, dataLen=%d",__FUNCTION__, eventID, s_cz_socket_trxbuff.RecvBytes);

    switch (eventID)
    {
        case SOCKET_CONNECT_EVENT_RSP:
            CZFOTA_LOG("SOCKET_CONNECT_EVENT_RSP");
            if (s_cz_FotaSocket == data->socket.socketID)
            {
                msg.messageID = EV_CZ_FOTA_SOCKET_CONNECT_RSP;
                msg.DataLen = data->socket.availSize;
                if (s_cz_fota_task_info.FotaMsgHandle == NULL)
                {
                    CZFOTA_LOG("Fota thread is not ready!");
                } 
                else 
                {
                    KING_MsgSend(&msg, s_cz_fota_task_info.FotaMsgHandle);
                }
            }
            else
            {
                CZFOTA_LOG("connect fail. nSocket=%d",data->socket.socketID);
            }

            break;

        case SOCKET_CLOSE_EVENT_RSP:
            CZFOTA_LOG("SOCKET_CLOSE_EVENT_RSP");
            break;

        case SOCKET_ERROR_EVENT_IND:
        case SOCKET_FULL_CLOSED_EVENT_IND:
            CZFOTA_LOG("SOCKET_ERROR_EVENT_IND");
            if (s_cz_FotaSocket == data->socket.socketID)
            {
                CZ_Fota_ConnectClose();
                if (CZ_FotaGetState() == CZ_FOTA_STATE_CHECK)
                {
                    KING_MutexLock(s_fota_check_mutex, WAIT_OPT_INFINITE);
                    s_cz_fota_chk_flag = CZ_AT_FOTA_CHKFAIL;
                    g_czErrCode = ERR_AT_CME_CZFOTA_NET_ERROR;
                    KING_MutexUnLock(s_fota_check_mutex);
                    URC_CmdFunc_CZUPCHK(ERR_AT_CME_CZFOTA_NET_ERROR);
                }
                else if (CZ_FotaGetState() == CZ_FOTA_STATE_CONTINUE || (CZ_FotaGetState() == CZ_FOTA_STATE_DOWNLOAD)
                    || (CZ_FotaGetState() == CZ_FOTA_STATE_GETFILEINFO))
                {
                    URC_CmdFunc_CZUPDL(CZ_FOTA_DL_FAIL, ERR_AT_CME_CZFOTA_NET_ERROR);
                    isDownLoading = FALSE;
                    s_cz_fota_dl_state.dl_state = CZ_FOTA_DL_FAIL;
                    if ((CZ_FotaGetState() == CZ_FOTA_STATE_DOWNLOAD) || (CZ_FotaGetState() == CZ_FOTA_STATE_GETFILEINFO))
                    {
                        CZ_FotaSendReportReq(CZ_FOTA_STATUS_DL_ERROR);
                    }
                }
                else if (CZ_FotaGetState() == CZ_FOTA_STATE_UPDATE)
                {
                    memset(g_rspStr, 0x00, 1024);
                    sprintf((char *)g_rspStr, "+CZUPDATE: %d", ERR_AT_CME_CZFOTA_NET_ERROR);
                    at_CmdRespUrcText(gFotaEngine, g_rspStr);
                }
            }
            break;

        case SOCKET_READ_EVENT_IND:
            CZFOTA_LOG("EV_CFW_TCPIP_REV_DATA_IND");
            if (s_cz_FotaSocket == data->socket.socketID)
            {
                msg.messageID = EV_CZ_FOTA_SOCKET_READ_RSP;
                msg.DataLen = data->socket.availSize;
                if (s_cz_fota_task_info.FotaMsgHandle == NULL)
                {
                    CZFOTA_LOG("Fota thread is not ready!");
                } 
                else
                {
                    KING_MsgSend(&msg, s_cz_fota_task_info.FotaMsgHandle);
                }
            }
            break;

        default:
            CZFOTA_LOG("TCPIP unexpect asynchrous event/response %d",eventID);
            break;
    }
    return ret;
}

/*************************************************************
函数名称:CZ_FotaPacketReq
函数描述:打包对应cmdid的报文
*************************************************************/
static int32 CZ_FotaPacketReq(uint32 cmdid, uint8 *input, int32 intputLen, uint8 *output, uint32 outBufLen)
{
    CZ_FOTA_REQ_PKG_HEAD_S * pReqHead = (CZ_FOTA_REQ_PKG_HEAD_S *)&output[0];
    uint32 ulEnc = 0;
    uint32 sendBytes = 0;
    CZ_TEACTX ctx;
    uint32 key[5] = {0};
    uint32 offSetOfMd5 = 0;
    CZFOTA_LOG("CZ_FotaPacketReq cmdid=%x",cmdid);

    pReqHead->Magic = CZ_FOTA_PACKAGE_MAGIC;

    pReqHead->ProtocolVer = CZ_FotaGetProtocolVer();
    pReqHead->CmdId = KING_htonl(cmdid);
    CZ_FotaGetTerminalId((uint8 *)pReqHead->ID);

    sendBytes = sizeof(CZ_FOTA_REQ_PKG_HEAD_S);

    memset(key,0x00,sizeof(key));

    if (CZ_FotaGetPrivateKey((uint8*)key)!= 0)
    {
        CZFOTA_LOG("ERROR! Data encrypt error,no key!");
        return -1;
    }

    memset(&ctx,0x00,sizeof(ctx));
    ulEnc = outBufLen - sendBytes;

    if (0 == CZ_Tea_Encrypt(&ctx, (const uint8 *)input, intputLen,(const uint8 *)key,
        (uint8 *)(output+sendBytes), &ulEnc))
    {
        CZFOTA_LOG("ERROR! Data encrypt error,src data too long[%d]!",intputLen);
        return -1;
    }

    sendBytes = sendBytes + ulEnc;

    pReqHead->PkgLength = KING_htons(sendBytes);
    offSetOfMd5 = (uint32)((uint8 *)(&(pReqHead->ID)) - (uint8 *)pReqHead);

    KING_Md5((uint8 *)(output+offSetOfMd5), sendBytes-offSetOfMd5,pReqHead->MD5);

    return sendBytes;
}

/*************************************************************
函数名称:CZ_Get_PS_Svn_Ver
函数描述:获取底包的svn版本号
*************************************************************/
static void CZ_Get_PS_Svn_Ver(uint16* svn_ver)
{
    char *ptr;
    char *ptr2;
    char svnver[6]={0};
    ptr = KING_PlatformInfo();

    if (ptr != NULL)
    {
        //先截取掉ML110_1.X.X.XXX_XXXXX_X前3个点之前的字符
        for(uint8 count = 0; count < 3 ; count ++)
        {
            ptr = strchr(ptr, '.') + 1;
        }

        ptr2 = strchr(ptr, '_') + 1;
        memcpy(svnver, ptr, ptr2-ptr);
        *svn_ver = atoi(svnver);

    }
    else
    {
        //底包版本非ML110
        * svn_ver = 1;
    }
}

/*************************************************************
函数名称:CZ_Get_BApp_Svn_Ver
函数描述:获取BootApp的svn版本号
备注：
1.由于KingSDK接口不显示svn版本号，这里取buildnum作为svn版本号
2.如果没有烧写BootApp分区，则将版本号设置为1
*************************************************************/
static void CZ_Get_BApp_Svn_Ver(uint16* svn_ver)
{
    char *ptr;
    //major.minor.buildnum
    ptr = KING_BootAppVer();
    if (ptr != NULL)
    {
        for(uint8 count = 0; count < 2 ; count ++)
        {
            ptr = strchr(ptr, '.') + 1;
        }

        *svn_ver = atoi(ptr);
    }
    else
    {
        *svn_ver = 1;
    }
}

/*************************************************************
函数名称:CZ_Init_FotaNv
函数描述:将FotaNV中的数据读取到对应的全局结构体
备注:为避免频繁地读取NV，仅在线程初始化的时候将FotaNV从NV区中
读取出来，放入全局结构体中。各函数需要读取NV信息的时候通过接口
CZ_Get_FotaNv将全局结构体中的信息读取出来。当使用CZ_Set_FotaNv
时，将修改的数据同步写入NV和全局结构体中。
*************************************************************/
static void CZ_Init_FotaNv(void)
{
    uint32 size = sizeof(FOTA_NV_S);
    memset((void*)&s_cz_fota_nv, 0x00, size);
    KING_NvRead(CZ_FOTA_NV_ID, (void *)&s_cz_fota_nv, &size);
}
/*************************************************************
函数名称:CZ_Get_FotaNv
函数描述:获取当前的fotaNV全局结构体数据
*************************************************************/
static void CZ_Get_FotaNv( FOTA_NV_S* fota_nv)
{
    if (fota_nv != NULL)
    {
        memcpy((void*)fota_nv, &s_cz_fota_nv, sizeof(FOTA_NV_S));
    }
}

/*************************************************************
函数名称:CZ_Set_FotaNv
函数描述:设置当前的fotaNV全局结构体并同步到NV中
*************************************************************/
static void CZ_Set_FotaNv( FOTA_NV_S* fota_nv)
{
    if (fota_nv != NULL)
    {
        memcpy((void*)&s_cz_fota_nv, (void*)fota_nv, sizeof(FOTA_NV_S));
        KING_NvWrite(CZ_FOTA_NV_ID, (void *)&s_cz_fota_nv, sizeof(FOTA_NV_S));
    }
}

/*************************************************************
函数名称:CZ_Cali_FotaNv
函数描述:校准FotaNv,主要针对刚出厂的数据全为0xFF的flash
*************************************************************/
static void CZ_Cali_FotaNv(void)
{
    FOTA_NV_S fota_state = {0};
    uint8 fota_index = CZ_FOTA_INDEX_PS;
    CZ_Init_FotaNv();
    CZ_Get_FotaNv(&fota_state);
    //校准FotaNV的值
    //1.FLASH的出厂设置为全1，需要置0
    //2.Fota完成后，state = CZ_FOTA_COMPLETE，在此处对其他的结构体成员进行重置

    for (fota_index = CZ_FOTA_INDEX_PS; fota_index < CZ_FOTA_NUM; fota_index++ )
    {
        CZFOTA_LOG("fota_state.state[%d]:%d",fota_index,fota_state.State[fota_index]);
        if (fota_state.State[fota_index] != CZ_FOTA_NEEDDOWNLOAD)
        {
            fota_state.State[fota_index]= CZ_FOTA_COMPLETE;
        }
    }

    if (fota_state.State[CZ_FOTA_INDEX_PS]==CZ_FOTA_COMPLETE && fota_state.State[CZ_FOTA_INDEX_KINGAPP]==CZ_FOTA_COMPLETE \
                       && fota_state.State[CZ_FOTA_INDEX_BOOTAPP]==CZ_FOTA_COMPLETE)
    {
        fota_state.FileId         = 0;
        fota_state.FileVerifyCode = 0;
        fota_state.FileLenth      = 0;
        fota_state.Offset         = 0;
    }

    CZ_Set_FotaNv(&fota_state);
}
/*************************************************************
函数名称:CZ_Fota_Get_SpecNvStateIndex
函数描述:按优先级获取第一个查询到的指定的FotaNv状态的index
*************************************************************/
static uint8 CZ_Fota_Get_SpecNvStateIndex(FOTA_NV_S* fota_state, CZ_FOTA_NV_STATE_E specstate)
{
    uint8 fota_index = CZ_FOTA_INDEX_PS;
    while (fota_index < CZ_FOTA_NUM)
    {
        if (fota_state->State[fota_index] == specstate)
        {
            break;
        }
        fota_index++;
    }

    return fota_index;
}

/*************************************************************
函数名称:CZ_Fota_Check_NeedUpdate
函数描述:查询是否有分区处于待升级状态
*************************************************************/
static bool CZ_Fota_Check_NeedUpdate(FOTA_NV_S * fota_nv)
{
    if( (fota_nv->State[CZ_FOTA_INDEX_PS] == CZ_FOTA_COMPLETE) \
                        && (fota_nv->State[CZ_FOTA_INDEX_KINGAPP] == CZ_FOTA_COMPLETE) \
                        &&(fota_nv->State[CZ_FOTA_INDEX_BOOTAPP] == CZ_FOTA_COMPLETE))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/*************************************************************
函数名称:CZ_Fota_Check_IncompleteTask
函数描述:检测是否有未完成的任务，依次检测已升级未上报、
已下载未升级、未下载完成、未请求更新查询的任务
*************************************************************/
static bool CZ_Fota_Check_IncompleteMission(void)
{
    bool ret = FALSE;
    FOTA_NV_S fota_state = {0};
    uint8 fota_index = CZ_FOTA_INDEX_PS;
    
    CZ_Get_FotaNv(&fota_state);

    //检查是否有未下载完成的任务
    fota_index = CZ_Fota_Get_SpecNvStateIndex(&fota_state, CZ_FOTA_NEEDDOWNLOAD);
    if (fota_index <= CZ_FOTA_INDEX_BOOTAPP)
    {
        if (fota_state.Offset == 0)//未开始下载的任务
        {
            CZ_FotaSendGetFileInfoReq(fota_index +1);
        }
        else//下载一半的任务
        {
            CZ_FotaSetState(CZ_FOTA_STATE_DOWNLOAD);
            CZ_FotaSendDownloadReq();
        }
        
        ret = TRUE;
        goto end;
    }

end:
    return ret;
}


/*************************************************************
函数名称:CZ_FotaConnectServer
函数描述:连接指定的ip和端口
*************************************************************/
static int32 CZ_FotaConnectServer(SOCK_IN_ADDR_T ip,uint16 port)
{
    int32 ret = FAIL;
    union sockaddr_aligned sock_addr;
    struct sockaddr_in *sock_addr4 = (struct sockaddr_in *)&(sock_addr);
    char *ipAddr = NULL;

    ret = KING_SocketCreate(AF_INET, SOCK_STREAM, 0, &s_cz_FotaSocket);
    if (ret == FAIL)
    {
        CZFOTA_LOG("KING_SocketCreate() fail");
        goto end;
    }

    memset(&sock_addr, 0, sizeof(struct sockaddr));
    sock_addr4->sin_family = AF_INET;
    sock_addr4->sin_port = KING_htons(port);
    sock_addr4->sin_addr.addr = ip.u_addr.ip4.addr;
    sock_addr4->sin_len = sizeof(struct sockaddr_in);
    KING_ipntoa(ip, &ipAddr);

    CZFOTA_LOG("SocketConnect ip_addr=%u, socket=%d ,ipAddr=%s",sock_addr4->sin_addr, s_cz_FotaSocket,ipAddr);
    KING_RegNotifyFun(DEV_CLASS_SOCKET, s_cz_FotaSocket, (NOTIFY_FUN)CZ_FotaHandleSockRsp);
    ret = KING_SocketConnect(s_cz_FotaSocket, &sock_addr, sizeof(struct sockaddr));
    if (ret == FAIL)
    {
        CZFOTA_LOG("KING_SocketConnect fail");
        KING_SocketClose(s_cz_FotaSocket);
        s_cz_FotaSocket = INVALID_SOCKET;
        goto end;
    }
    CZFOTA_LOG("SocketConnect RegNotify socket=%d",s_cz_FotaSocket);

end:
    return ret;
}

/*************************************************************
函数名称:CZ_FotaTask
函数描述:fota线程
*************************************************************/
static void CZ_FotaTask(uint32 argc, void *argv)
{
    int32 ret = FAIL;
    MSG_S msg = {0};

    CZFOTA_LOG("FotaThread is running...");

    ret = KING_MsgCreate(&s_cz_fota_task_info.FotaMsgHandle);
    if (ret == FAIL)
    {
        CZFOTA_LOG("FotaThread: Failed to create Msg Handle!");
        KING_ThreadExit();
        return;
    }

    while (1)
    {
        KING_MsgRcv(&msg, s_cz_fota_task_info.FotaMsgHandle, WAIT_OPT_INFINITE);
        CZFOTA_LOG("messageID=%d, dataLen=%d", msg.messageID, msg.DataLen);

        switch (msg.messageID)
        {
            case EV_CZ_FOTA_CONNECT_REQ:
                {
                    SOCK_IN_ADDR_T serverIp = {0};
                    if(AT_GPRS_IPAddrAnalyzer(s_cz_fota_server.serverHost, strlen(s_cz_fota_server.serverHost)) == 1)
                    {
                        //输入的是IP
                        KING_ipaton(s_cz_fota_server.serverHost, &serverIp);
                        if (CZ_FotaConnectServer(serverIp, s_cz_fota_server.serverPort) != 0)
                        {
                            if(CZ_CheckNetState() != 0)
                            {
                                CZ_FotaSetErrorCode(ERR_AT_CME_CZFOTA_SOCKET_CONNECT_FAIL);
                            }
                            
                            if(CZ_FotaGetState() == CZ_FOTA_STATE_CHECK)
                            {
                                KING_MutexLock(s_fota_check_mutex, WAIT_OPT_INFINITE);
                                s_cz_fota_chk_flag = CZ_AT_FOTA_CHKFAIL;
                                g_czErrCode = ERR_AT_CME_CZFOTA_SOCKET_CONNECT_FAIL;
                                KING_MutexUnLock(s_fota_check_mutex);
                                URC_CmdFunc_CZUPCHK(ERR_AT_CME_CZFOTA_SOCKET_CONNECT_FAIL);
                            }
                            else if (CZ_FotaGetState() == CZ_FOTA_STATE_CONTINUE)
                            {
                                URC_CmdFunc_CZUPDL(CZ_FOTA_DL_FAIL, ERR_AT_CME_CZFOTA_SOCKET_CONNECT_FAIL);
                                isDownLoading = FALSE;
                            }
                            else if (CZ_FotaGetState() == CZ_FOTA_STATE_UPDATE)
                            {
                                memset(g_rspStr, 0x00, 1024);
                                sprintf((char *)g_rspStr, "+CZUPDATE: %d", ERR_AT_CME_CZFOTA_SOCKET_CONNECT_FAIL);
                                at_CmdRespUrcText(gFotaEngine, g_rspStr);
                            }
                            CZ_Fota_ConnectClose();
                        }
                    }
                    else
                    {
                        //进行域名解析
                        uint8 cid = CZ_Fota_Get_ActCid();
                        hSocketContext = tcpip_getSwHandleByCID(cid);
                        CZFOTA_LOG("hSocketContext %d",hSocketContext);
                        ret = KING_NetGetHostByName(hSocketContext, s_cz_fota_server.serverHost, &serverIp, 10000, (DNS_CALLBACK)FotaDnsReqCb);
                        if(ret == FAIL)
                        {
                            if(CZ_FotaGetState() == CZ_FOTA_STATE_CHECK)
                            {
                                KING_MutexLock(s_fota_check_mutex, WAIT_OPT_INFINITE);
                                s_cz_fota_chk_flag = CZ_AT_FOTA_CHKFAIL;
                                g_czErrCode = ERR_AT_CME_CZFOTA_DNS_ANALYZE_FAIL;
                                KING_MutexUnLock(s_fota_check_mutex);
                                URC_CmdFunc_CZUPCHK(ERR_AT_CME_CZFOTA_DNS_ANALYZE_FAIL);
                            }
                            else if (CZ_FotaGetState() == CZ_FOTA_STATE_CONTINUE)
                            {
                                URC_CmdFunc_CZUPDL(CZ_FOTA_DL_FAIL, ERR_AT_CME_CZFOTA_DNS_ANALYZE_FAIL);
                                isDownLoading = FALSE;
                            }
                            else if (CZ_FotaGetState() == CZ_FOTA_STATE_UPDATE)
                            {
                                memset(g_rspStr, 0x00, 1024);
                                sprintf((char *)g_rspStr, "+CZUPDATE: %d", ERR_AT_CME_CZFOTA_DNS_ANALYZE_FAIL);
                                at_CmdRespUrcText(gFotaEngine, g_rspStr);
                            }
                            CZ_Fota_ConnectClose();
                            CZ_FotaSetErrorCode(ERR_AT_CME_CZFOTA_DNS_ANALYZE_FAIL);
                        }
                    }
                }
                break;

            case EV_CZ_FOTA_SOCKET_CONNECT_RSP:
                {
                    CZ_Fota_ConnectRsp();
                }
                break;

            case EV_CZ_FOTA_SOCKET_READ_RSP:
                {
                    //读取收到的socket数据，按照报文格式进行预处理，保留TEA加密部分的数据
                    CZ_FotaPreParseData();
                }
                break;

            case EV_CZ_FOTA_SOCKET_WRITE_REQ:
                {
                    if (s_cz_FotaSocket != INVALID_SOCKET)
                    {
                        ret = KING_SocketSend(s_cz_FotaSocket, msg.pData, msg.DataLen, 0);
                        CZFOTA_LOG("SocketSend ret = %d", ret);
                        if(ret == FAIL)
                        {
                            if(CZ_CheckNetState() != 0)
                            {
                                CZ_FotaSetErrorCode(ERR_AT_CME_CZFOTA_SOCKET_SEND_FAIL);
                            }
                            if(CZ_FotaGetState() == CZ_FOTA_STATE_DOWNLOAD)
                            {
                                URC_CmdFunc_CZUPDL(CZ_FOTA_DL_FAIL, ERR_AT_CME_CZFOTA_SOCKET_SEND_FAIL);
                                isDownLoading = FALSE;
                            }
                            CZ_Fota_ConnectClose();
                        }
                    }
                }
                break;

            case EV_CZ_FOTA_PARSE_DATA_RSP:
                {
                    //TEA解密预处理完后的数据并做对应处理
                    CZ_FotaParseData();
                }
                break;
/*
            case EV_CZ_FOTA_RESEND_REQ:
                {
                    ret = KING_SocketSend(s_cz_FotaSocket, s_cz_socket_trxbuff.SendDataBuf, s_cz_socket_trxbuff.SendBytes, 0);
                    CZFOTA_LOG("SocketReSend ret = %d", ret);
                }
                break;

            case EV_CZ_FOTA_EXIT_REQ:
                {

                }
                break;

            case EV_CZ_FOTA_REBOOT_REQ:
                {
                    CZFOTA_LOG("Reboot for Upgrade");
                    KING_Sleep(1000);
                    KING_PowerReboot();
                }
                break;
*/
        }
    }
}


/*************************************************************
函数名称:CZ_FotaTaskInit
函数描述:初始化fota线程
*************************************************************/
int8 CZ_FotaTaskInit(void)
{
    int8 ret = FAIL;

    CZFOTA_LOG("CZ_FotaTaskInit %d",s_cz_fota_task_info.FotaTaskId);
    if (s_cz_fota_task_info.FotaTaskId == NULL)
    {
        THREAD_ATTR_S FotaThreadAttr = {0};

        memset(&FotaThreadAttr, 0x00, sizeof(THREAD_ATTR_S));
        FotaThreadAttr.fun = (THREAD_ENTRY_FUN)CZ_FotaTask;
        FotaThreadAttr.priority = THREAD_PRIORIT3;
        FotaThreadAttr.stackSize = 5 * 1024;
    
        ret = KING_ThreadCreate("FotaThread", &FotaThreadAttr, &s_cz_fota_task_info.FotaTaskId);
        if (ret == FAIL)
        {
            CZFOTA_LOG("Create Test Thread Failed!! Errcode=0x%x", KING_GetLastErrCode());
        }
    }

    return ret;
}


/*************************************************************
函数名称:CZ_FotaInit
函数描述:fotatask和全局fotaNV初始化，设置IP、端口
*************************************************************/
int8 CZ_FotaInit(void)
{
    int8 ret = FAIL;
    ret = CZ_FotaTaskInit();
    if (ret == 0)
    {
        KING_NvInit();
        CZ_Cali_FotaNv();
        CZ_FotaSetHost(CZ_FOTA_SERVER_IP);
        CZ_FotaSetPort(CZ_FOTA_SERVER_PORT);
        KING_MutexCreate("fota_check_state", 0, &s_fota_check_mutex);
    }
    
    return ret;
}

/*************************************************************
函数名称:CZ_FotaSendUpdateCheckReq
函数描述:发送所有分区升级检测请求
*************************************************************/
static int32  CZ_FotaSendUpdateCheckReq(void)
{
    int32 ret = FAIL;
    MSG_S msg= {0};
    CZ_FOTA_FILE_CHECKINFO_S  FileCheckReqInfo = {0};
    FOTA_NV_S fota_state = {0};
    uint32 size = sizeof(FOTA_NV_S);
    uint16 ps_ver = 0;
    uint16 bootapp_ver = 0;
    memset((void *)&fota_state, 0x00, size);
    CZ_Get_PS_Svn_Ver(&ps_ver);
    CZ_Get_BApp_Svn_Ver(&bootapp_ver);
    //将底包、kingapp、bootapp的状态改为需要查询更新

    CZ_Get_FotaNv(&fota_state);
    
    //FileVerMajor为svn版本号的高八位，FileVerMinor为svn版本号的低八位。
    //在fota服务器上传固件包时：
    //Appid：1-底包，2-KingApp，3-bootapp
    //MCU供应商：0
    //部件号：0
    //主版本号：0
    //次版本号：svn版本号
    //应用校验码：0-0xFFFFFFFE
    FileCheckReqInfo.FileVerInfo[CZ_FOTA_INDEX_PS].AppId = CZ_FOTA_FILE_TYPE_PS;
    FileCheckReqInfo.FileVerInfo[CZ_FOTA_INDEX_PS].FileVerMajor = ps_ver >> 8;
    FileCheckReqInfo.FileVerInfo[CZ_FOTA_INDEX_PS].FileVerMinor = ps_ver & 0xFF;

    FileCheckReqInfo.FileVerInfo[CZ_FOTA_INDEX_KINGAPP].AppId = CZ_FOTA_FILE_TYPE_KINGAPP;
    FileCheckReqInfo.FileVerInfo[CZ_FOTA_INDEX_KINGAPP].FileVerMajor = CZ_SVN_REV >> 8;
    FileCheckReqInfo.FileVerInfo[CZ_FOTA_INDEX_KINGAPP].FileVerMinor = CZ_SVN_REV & 0xFF;

    FileCheckReqInfo.FileVerInfo[CZ_FOTA_INDEX_BOOTAPP].AppId = CZ_FOTA_FILE_TYPE_BOOTAPP;
    FileCheckReqInfo.FileVerInfo[CZ_FOTA_INDEX_BOOTAPP].FileVerMajor = bootapp_ver >> 8;
    FileCheckReqInfo.FileVerInfo[CZ_FOTA_INDEX_BOOTAPP].FileVerMinor = bootapp_ver & 0xFF;
    CZFOTA_LOG("%s PSver:%u, KingAppver:%u, BootAppver:%u", __FUNCTION__, ps_ver, CZ_SVN_REV, bootapp_ver);

    memset(s_cz_socket_trxbuff.SendDataBuf, 0x00, CZ_FOTA_PACKAGE_MAXLEN);

    CZ_FotaGetTerminalInfo(&FileCheckReqInfo.TerminalInfo);


    s_cz_socket_trxbuff.SendBytes = CZ_FotaPacketReq(CZ_FOTA_CMD_CHECKUPDATE_REQ, (uint8 *)&FileCheckReqInfo, \
                        sizeof(CZ_FOTA_FILE_CHECKINFO_S), s_cz_socket_trxbuff.SendDataBuf, sizeof(s_cz_socket_trxbuff.SendDataBuf));
    if (s_cz_socket_trxbuff.SendBytes <= 0)
    {
        ret = CZ_FOTA_SOCKET_SEND_FAIL;
        goto end;
    }
    CZ_FotaPrintfHex("CZ_FotaSendUpdateCheckReq", s_cz_socket_trxbuff.SendDataBuf, s_cz_socket_trxbuff.SendBytes);

    //CZ_FotaSetState(CZ_FOTA_STATE_CHECK);
    
    msg.messageID = EV_CZ_FOTA_SOCKET_WRITE_REQ;
    msg.pData = (void*)s_cz_socket_trxbuff.SendDataBuf;
    msg.DataLen = s_cz_socket_trxbuff.SendBytes;

    ret = KING_MsgSend(&msg, s_cz_fota_task_info.FotaMsgHandle);

end:
    CZFOTA_LOG("%s: ret = %d",__FUNCTION__, ret);
    return ret;
}


/*************************************************************
函数名称:CZ_FotaSendGetFileInfoReq
函数描述:发送获取fota文件信息的请求
*************************************************************/
static int32  CZ_FotaSendGetFileInfoReq(uint8 AppIds)
{
    int32 ret = FAIL;
    MSG_S msg= {0};
    memset(s_cz_socket_trxbuff.SendDataBuf, 0x00, CZ_FOTA_PACKAGE_MAXLEN);

    s_cz_socket_trxbuff.SendBytes = CZ_FotaPacketReq(CZ_FOTA_CMD_GETFILEINFO_REQ, &AppIds, 1, s_cz_socket_trxbuff.SendDataBuf, \
                                                        sizeof(s_cz_socket_trxbuff.SendDataBuf));
    if (s_cz_socket_trxbuff.SendBytes <= 0)
    {
        ret = CZ_FOTA_SOCKET_SEND_FAIL;
        goto end;
    }

    CZ_FotaSetState(CZ_FOTA_STATE_GETFILEINFO);
    
    msg.messageID = EV_CZ_FOTA_SOCKET_WRITE_REQ;
    msg.pData = (void*)s_cz_socket_trxbuff.SendDataBuf;
    msg.DataLen = s_cz_socket_trxbuff.SendBytes;
    ret = KING_MsgSend(&msg, s_cz_fota_task_info.FotaMsgHandle);

end:
    CZFOTA_LOG("%s: ret = %d",__FUNCTION__, ret);
    return ret;
}

/*************************************************************
函数名称:CZ_FotaSendDownloadReq
函数描述:发送下载fota文件的请求
*************************************************************/
static int32  CZ_FotaSendDownloadReq(void)
{
    int32 ret = FAIL;
    uint32 srcLen = 0;
    uint8 progress_rate = 0;
    MSG_S msg= {0};
    FOTA_NV_S fota_state = {0};
    uint32 size = sizeof(FOTA_NV_S);
    CZ_FOTA_FILE_DATA_REQ_S  dlfile = {0};
    memset((void*)&fota_state, 0x00, size);
    CZ_Get_FotaNv(&fota_state);
    
    progress_rate = 100*fota_state.Offset/fota_state.FileLenth;
    s_cz_fota_dl_state.dl_progress = progress_rate;
    CZFOTA_LOG("%s: progress_rate = %d%%, FileLenth %d, offset %d",__FUNCTION__, progress_rate,fota_state.FileLenth,fota_state.Offset);
    memset(s_cz_socket_trxbuff.SendDataBuf, 0x00, CZ_FOTA_PACKAGE_MAXLEN);
    
    if (fota_state.FileLenth > fota_state.Offset)
    {
        dlfile.FileId          = KING_htonl(fota_state.FileId);
        dlfile.Offset          = KING_htonl(fota_state.Offset);
        dlfile.FileVerifyCode  = KING_htonl(fota_state.FileVerifyCode);
        if (fota_state.FileLenth - fota_state.Offset >= CZ_FOTA_PACKAGE_DATALEN)
        {
            dlfile.GetLength   = KING_htonl(CZ_FOTA_PACKAGE_DATALEN);
        }
        else
        {
            dlfile.GetLength   = KING_htonl(fota_state.FileLenth - fota_state.Offset);
        }
        srcLen = sizeof(CZ_FOTA_FILE_DATA_REQ_S);
        s_cz_socket_trxbuff.SendBytes = CZ_FotaPacketReq(CZ_FOTA_CMD_DOWNLOADFILE_REQ, (uint8*)&dlfile, srcLen, \
                                                        s_cz_socket_trxbuff.SendDataBuf, sizeof(s_cz_socket_trxbuff.SendDataBuf));

        CZ_FotaSetState(CZ_FOTA_STATE_DOWNLOAD);
        
        msg.messageID = EV_CZ_FOTA_SOCKET_WRITE_REQ;
        msg.pData = (void*)s_cz_socket_trxbuff.SendDataBuf;
        msg.DataLen = s_cz_socket_trxbuff.SendBytes;
        ret = KING_MsgSend(&msg, s_cz_fota_task_info.FotaMsgHandle);
        if (ret == FAIL)
        {
            goto end;
            //Fota_ResendTimer_Reactive();
        }
    }
    else
    {
        URC_CmdFunc_CZUPDL(CZ_FOTA_DL_DONE, 100);
        isDownLoading = FALSE;
        s_cz_fota_dl_state.dl_state = CZ_FOTA_DL_DONE;
        ret = CZ_FotaSendReportReq(CZ_FOTA_STATUS_DL_OK);
    }
end:
    CZFOTA_LOG("%s: ret = %d",__FUNCTION__, ret);
    return ret;
}


/*************************************************************
函数名称:CZ_FotaSendStatusReport
函数描述:向fota服务器发送状态报告
*************************************************************/
static int32 CZ_FotaSendReportReq(CZ_FOTA_FILE_STATUS_E status)
{
    int32 ret = FAIL;
    MSG_S msg = {0};
    CZ_FOTA_FILE_STATUS_IND_S FileStatusInd ={0};
    uint32 srcLen = 0;
    FOTA_NV_S fota_state = {0};
    uint16 ps_ver = 0;
    uint16 bootapp_ver = 0;
    uint8 fota_index = CZ_FOTA_INDEX_PS;
    CZ_Get_FotaNv(&fota_state);
    CZ_Get_PS_Svn_Ver(&ps_ver);
    CZ_Get_BApp_Svn_Ver(&bootapp_ver);
    memset(s_cz_socket_trxbuff.SendDataBuf, 0x00, CZ_FOTA_PACKAGE_MAXLEN);

    switch(status)
    {
        case CZ_FOTA_STATUS_UPDATE_OK:
            fota_index = CZ_Fota_Get_SpecNvStateIndex(&fota_state, CZ_FOTA_NEEDDOWNLOAD);
            break;
        case CZ_FOTA_STATUS_UPDATE_ERROR:
            fota_index = CZ_Fota_Get_SpecNvStateIndex(&fota_state, CZ_FOTA_NEEDDOWNLOAD);
            break;
        case CZ_FOTA_STATUS_DL_OK:
            fota_index = CZ_Fota_Get_SpecNvStateIndex(&fota_state, CZ_FOTA_NEEDDOWNLOAD);
            break;
        case CZ_FOTA_STATUS_DL_ERROR:
            fota_index = CZ_Fota_Get_SpecNvStateIndex(&fota_state, CZ_FOTA_NEEDDOWNLOAD);
            CZ_FotaSetState(CZ_FOTA_STATE_CHECK);
            break;
    }
    CZ_Set_FotaNv(&fota_state);
    
    FileStatusInd.Status = KING_htonl(status);
    FileStatusInd.FileId = KING_htonl(fota_state.FileId);
    switch(fota_index + 1)
    {
        case CZ_FOTA_FILE_TYPE_PS:
            FileStatusInd.FileVerMajor = ps_ver >> 8;
            FileStatusInd.FileVerMinor = ps_ver & 0xFF;
            break;
        case CZ_FOTA_FILE_TYPE_KINGAPP:
            FileStatusInd.FileVerMajor = CZ_SVN_REV >> 8;
            FileStatusInd.FileVerMinor = CZ_SVN_REV & 0xFF;
            break;
        case CZ_FOTA_FILE_TYPE_BOOTAPP:
            FileStatusInd.FileVerMajor = bootapp_ver >> 8;
            FileStatusInd.FileVerMinor = bootapp_ver & 0xFF;
            break;
        default:
            FileStatusInd.FileVerMajor = 0;
            FileStatusInd.FileVerMinor = 0;
            break;
    }


    srcLen = sizeof(CZ_FOTA_FILE_STATUS_IND_S);
    s_cz_socket_trxbuff.SendBytes = CZ_FotaPacketReq(CZ_FOTA_CMD_REPORTRESULT_REQ, (uint8 *)&FileStatusInd, srcLen, \
                                    s_cz_socket_trxbuff.SendDataBuf, sizeof(s_cz_socket_trxbuff.SendDataBuf));

    msg.messageID = EV_CZ_FOTA_SOCKET_WRITE_REQ;
    msg.pData = (void*)s_cz_socket_trxbuff.SendDataBuf;
    msg.DataLen = s_cz_socket_trxbuff.SendBytes;
    ret = KING_MsgSend(&msg, s_cz_fota_task_info.FotaMsgHandle);
    
    return ret;
}

/*************************************************************
函数名称:CZ_FotaPreParseData
函数描述:按照报文格式预解析收到的socket数据并做对应处理
*************************************************************/
static int32 CZ_FotaPreParseData(void)
{
    int32 ret = FAIL;
    CZ_FOTA_RSP_PKG_HEAD_S * pRspPgkHead = NULL;
    s_cz_socket_trxbuff.RecvBytes = KING_SocketRecv(s_cz_FotaSocket, s_cz_socket_trxbuff.RecvDataBuf, CZ_FOTA_PACKAGE_MAXLEN, 0);
    if (s_cz_socket_trxbuff.RecvBytes <= 0)
    {
        ret = CZ_FOTA_SOCKET_RECV_FAIL;
        if(CZ_CheckNetState() != 0)
        {
            CZ_FotaSetErrorCode(ERR_AT_CME_CZFOTA_NET_ERROR);
        }
        if(CZ_FotaGetState() == CZ_FOTA_STATE_DOWNLOAD)
        {
            URC_CmdFunc_CZUPDL(CZ_FOTA_DL_FAIL, ERR_AT_CME_CZFOTA_READ_FILE);
            isDownLoading = FALSE;
        }
        CZ_Fota_ConnectClose();
        goto end;
    }

    pRspPgkHead = (CZ_FOTA_RSP_PKG_HEAD_S *) s_cz_socket_trxbuff.RecvDataBuf;
    CZFOTA_LOG("CZ_FotaRecvData nLen=%d",KING_ntohs(pRspPgkHead->PkgLength));

    if (s_cz_socket_trxbuff.RecvBytes >= sizeof(CZ_FOTA_RSP_PKG_HEAD_S))
    {
        uint32 cmdid = KING_htonl(pRspPgkHead->CmdId);
        uint32 offSetOfMd5 = sizeof(CZ_FOTA_RSP_PKG_HEAD_S);
        uint8 md5Out[CZ_MD5_LEN] = {0};
        KING_Md5(s_cz_socket_trxbuff.RecvDataBuf + offSetOfMd5, s_cz_socket_trxbuff.RecvBytes - offSetOfMd5,md5Out);
        if (pRspPgkHead->Magic != CZ_FOTA_PACKAGE_MAGIC)
        {
            CZFOTA_LOG("%s: ERROR! Receive data Magic[0x%x] error!", __FUNCTION__, pRspPgkHead->Magic);
            ret = CZ_FOTA_MAGIC_ERROR;
            goto end;
        }
        else if (pRspPgkHead->ProtocolVer != CZ_FotaGetProtocolVer())
        {
            CZFOTA_LOG("%s: Recv ProtocolVer[0x%x] error, not equal to [0x%x]",
                       __FUNCTION__, pRspPgkHead->ProtocolVer,CZ_FotaGetProtocolVer());
            ret = CZ_FOTA_PROTOCOL_ERROR;
            goto end;
        }
        else if (cmdid == CZ_FOTA_CMD_ABNORAML_RSP)
        {
            //收到8100表示有异常，直接清空FotaNv
            FOTA_NV_S fota_state = {0};
            memset((void*)&fota_state, 0x00, sizeof(FOTA_NV_S));
            CZ_Set_FotaNv(&fota_state);
            ret = CZ_FOTA_CMDID_ERROR;
            goto end;
        }
        else if (0 != memcmp(pRspPgkHead->MD5, md5Out,CZ_MD5_LEN))
        {
            ret = CZ_FOTA_MD5_ERROR;
            goto end;
        }
        else
        {
            MSG_S msg = {0};
            msg.messageID = EV_CZ_FOTA_PARSE_DATA_RSP;
            if (s_cz_fota_task_info.FotaMsgHandle == NULL)
            {
                CZFOTA_LOG("Fota thread is not ready!");
            } 
            else 
            {
                KING_MsgSend(&msg, s_cz_fota_task_info.FotaMsgHandle);
                ret = CZ_FOTA_NO_ERROR;
                goto end;
            }
        }
    }
    else
    {
        ret = CZ_FOTA_DATALEN_ERROR;
        goto end;
    }

end:
    if((ret != CZ_FOTA_NO_ERROR)  && (CZ_FotaGetState() == CZ_FOTA_STATE_CHECK))
    {
        CZ_Fota_ConnectClose();
        KING_MutexLock(s_fota_check_mutex, WAIT_OPT_INFINITE);
        s_cz_fota_chk_flag = CZ_AT_FOTA_CHKDONE;
        KING_MutexUnLock(s_fota_check_mutex);
        URC_CmdFunc_CZUPCHK(0);
    }
    CZFOTA_LOG("%s ret = %d", __FUNCTION__, ret);
    return ret;
}


/*************************************************************
函数名称:CZ_FotaParseData
函数描述:将数据TEA解密并做对应处理
*************************************************************/
static int32 CZ_FotaParseData(void)
{
    int32 ret = FAIL;
    uint32 ulEnc = 0;
    uint32 key[5];
    uint8  pdu[CZ_FOTA_PACKAGE_MAXLEN] = {0};
    uint16 headlen = sizeof(CZ_FOTA_RSP_PKG_HEAD_S);
    CZ_TEACTX ctx;
    CZ_FOTA_RSP_PKG_HEAD_S * pRspPgkHead = NULL;
    pRspPgkHead = (CZ_FOTA_RSP_PKG_HEAD_S *) s_cz_socket_trxbuff.RecvDataBuf;
    uint32 cmdid = KING_htonl(pRspPgkHead->CmdId);
    //这里需要根据cmdid来决定ulEnc的长度。
    //每个阶段的ulEnc长度不一样，所以每个阶段的tea解密所需要的ulEnc也不一样
    CZFOTA_LOG("%s cmdid:0x%08X", __FUNCTION__, cmdid);
    switch(cmdid)
    {
        case CZ_FOTA_CMD_CHECKUPDATE_RSP:
            ulEnc = CZ_FOTA_NUM * sizeof(CZ_FOTA_FILE_UPDATEFLAG_S);
            break;
        case CZ_FOTA_CMD_GETFILEINFO_RSP:
            ulEnc = sizeof(CZ_FOTA_FILE_GETINFO_S);
            break;
        case CZ_FOTA_CMD_DOWNLOADFILE_RSP:
            ulEnc = sizeof(CZ_FOTA_FILE_DATA_RSP_S);
            break;
        case CZ_FOTA_CMD_REPORTRESULT_RSP:
            ulEnc = sizeof(CZ_FOTA_FILE_STATUS_CNF_S);
            break;
    }

    memset(key,0x00,sizeof(key));
    if (CZ_FotaGetPrivateKey((uint8*)key)!= 0)
    {
        ret = CZ_FOTA_ENCRYPT_ERROR;
        goto end;
    }

    memset(&ctx, 0x00, sizeof(ctx));

    
    if (0 == CZ_Tea_Decrypt(&ctx, (const uint8 *)(s_cz_socket_trxbuff.RecvDataBuf + headlen), KING_ntohs(pRspPgkHead->PkgLength)-headlen,
        (const uint8 *)key, pdu, (uint32 *)&ulEnc))
    {
        ret = CZ_FOTA_DECRYPT_ERROR;
        goto end;
    }

    //CZ_FotaPreParseData中已经对cmdid的取值范围进行验证了
    //此处只需要将cmdid转换成数组下标即可，无需再验证有效性
    uint8 tmp_id = (uint8)cmdid/2;
    ret = Fota_Cmd_Parse[tmp_id](pdu, ulEnc);
    
end:
    CZFOTA_LOG("%s:ret = %d", __FUNCTION__, ret);
    return ret;
}
/*************************************************************
函数名称:CZ_FotaParse_Null
函数描述:空函数，返回错误
*************************************************************/
static int32 CZ_FotaParse_Null(uint8* input, uint32 inputlen)
{
    return CZ_FOTA_CMDID_ERROR;
}

/*******************************************************************************
函数名称:CZ_FotaCheckUpdateParse
函数描述:对检查更新的请求的响应报文解密后，进行下一步操作(请求文件信息或退出线程)
********************************************************************************/
static int32 CZ_FotaCheckUpdateParse(uint8* input, uint32 inputlen)
{
    int32 ret = CZ_FOTA_NO_ERROR;
    uint8 fota_index;
    uint16 is_need_dl;
    FOTA_NV_S fota_state = {0};
    uint32 size = sizeof(FOTA_NV_S);
    CZ_FOTA_FILE_UPDATEFLAG_S update_flag[CZ_FOTA_NUM] = {0};
    memcpy((void*)update_flag, (void*)input, inputlen);
    memset((void*)&fota_state, 0x00, size);
    CZ_Get_FotaNv(&fota_state);
    CZ_FOTA_STATE_E state = CZ_FotaGetState();
    if (state != CZ_FOTA_STATE_CHECK)
    {
        ret = FAIL;
        goto exit;
    }
    
    for (fota_index = CZ_FOTA_INDEX_PS; fota_index < CZ_FOTA_NUM; fota_index++)
    {
        if ((update_flag[fota_index].AppId > 0) && (update_flag[fota_index].UpdateFlag >= '0'))
        {
            update_flag[fota_index].UpdateFlag -= '0';
            fota_state.State[fota_index] = (CZ_FOTA_NV_STATE_E)update_flag[fota_index].UpdateFlag;
            CZFOTA_LOG("AppId=%d,UpdateFlag=%d",update_flag[fota_index].AppId, update_flag[fota_index].UpdateFlag);
        }
    }

    CZ_Fota_ConnectClose();
    CZ_Set_FotaNv(&fota_state);
    KING_MutexLock(s_fota_check_mutex, WAIT_OPT_INFINITE);
    s_cz_fota_chk_flag = CZ_AT_FOTA_CHKDONE;
    KING_MutexUnLock(s_fota_check_mutex);
    
    fota_index = CZ_FOTA_INDEX_PS;
    while (fota_index < CZ_FOTA_NUM)
    {
        if (update_flag[fota_index].UpdateFlag)
        {
            goto end;
        }
        else
        {
            fota_index ++ ;
        }
    }
    CZFOTA_LOG("%s:No partition need update", __FUNCTION__);
    //CZ_Fota_Deactive();

end:
    is_need_dl = (fota_index == CZ_FOTA_NUM) ? 0 : 1;    //能执行到这里说明DNS解析没问题，所以只有0或1两种情况
    URC_CmdFunc_CZUPCHK(is_need_dl);

exit:
    CZFOTA_LOG("%s:ret = %d", __FUNCTION__, ret);
    return ret;
}


/*******************************************************************************
函数名称:CZ_FotaGetFileInfoParse
函数描述:对获取文件信息请求的响应报文解密后，进行下一步操作(请求下载)
********************************************************************************/
static int32 CZ_FotaGetFileInfoParse(uint8* input, uint32 inputlen)
{
    int32 ret = FAIL;
    uint8 fota_index = CZ_FOTA_INDEX_PS;
    FOTA_NV_S fota_state = {0};
    uint32 size = sizeof(FOTA_NV_S);
    CZ_FOTA_FILE_GETINFO_S fota_file_info = {0};
    
    memcpy((void*)&fota_file_info, input, inputlen);
    memset((void*)&fota_state, 0x00, size);
    CZ_Get_FotaNv(&fota_state);
    CZ_FOTA_STATE_E state = CZ_FotaGetState();
    
    if (state != CZ_FOTA_STATE_GETFILEINFO)
    {
        goto end;
    }
    
    fota_index = CZ_Fota_Get_SpecNvStateIndex(&fota_state, CZ_FOTA_NEEDDOWNLOAD);
    if ((fota_index + 1) == KING_ntohl(fota_file_info.AppId))
    {
        fota_state.FileId            =  KING_ntohl(fota_file_info.FileId);
        fota_state.FileLenth         =  KING_ntohl(fota_file_info.FileLength);
        fota_state.FileVerifyCode    =  KING_ntohl(fota_file_info.FileVerifyCode);
        fota_state.Offset            =  0;
        CZ_Set_FotaNv(&fota_state);
        ret = CZ_FOTA_NO_ERROR;
    }
    else
    {
        ret = CZ_FOTA_APPID_ERROR;
        goto end;
    }

    if (ret == CZ_FOTA_NO_ERROR)
    {
        KING_FotaFlagSet(FOTA_CLEAR);
        ret = CZ_FotaSendDownloadReq();
    }
end:
    CZFOTA_LOG("%s:ret = %d", __FUNCTION__, ret);
    return ret;
}


/****************************************************************************************
函数名称:CZ_FotaDownloadParse
函数描述:对请求下载文件的响应报文解密后，进行下一步操作(报告下载状态或继续请求下载下一包)
****************************************************************************************/
static int32 CZ_FotaDownloadParse(uint8* input, uint32 inputlen)
{
    int32 ret = FAIL;
    uint32 written_bytes = 0;
    FOTA_NV_S fota_state = {0};
    uint32 size = sizeof(FOTA_NV_S);
    uint32 FileId = 0;
    uint32 Offset = 0;
    uint32 Datalen = 0;
    CZ_FOTA_FILE_DATA_RSP_S dlfilersp = {0};
    
    memcpy((void*)&dlfilersp, input, inputlen);
    memset((void*)&fota_state, 0x00, size);
    CZ_Get_FotaNv(&fota_state);
    CZ_FOTA_STATE_E state = CZ_FotaGetState();
    
    if (state != CZ_FOTA_STATE_DOWNLOAD)
    {
        goto end;
    }

    FileId = KING_ntohl(dlfilersp.FileId);
    Offset = KING_ntohl(dlfilersp.Offset);
    Datalen = KING_ntohl(dlfilersp.DataLength);

    if (fota_state.FileId != FileId)
    {
        ret = CZ_FOTA_FILEID_ERROR;
        goto end;
    }
    else if (fota_state.Offset != Offset)
    {
        ret = CZ_FOTA_OFFSET_ERROR;
        goto end;
    }

    ret = KING_FotaFlashWrite(fota_state.Offset, (uint8*)dlfilersp.FileData, Datalen, &written_bytes);
    if (ret != FAIL && written_bytes == Datalen)
    {
        fota_state.Offset += Datalen;
        CZ_Set_FotaNv(&fota_state);
    }
    
end:
    if (ret == CZ_FOTA_NO_ERROR)
    {
        s_cz_fota_dl_state.dl_state = CZ_FOTA_DL_DOING;
        ret = CZ_FotaSendDownloadReq();
    }
    else
    {
        URC_CmdFunc_CZUPDL(CZ_FOTA_DL_FAIL, ERR_AT_CME_CZFOTA_READ_FILE);
        isDownLoading = FALSE;
        s_cz_fota_dl_state.dl_state = CZ_FOTA_DL_FAIL;
        ret = CZ_FotaSendReportReq(CZ_FOTA_STATUS_DL_ERROR);
    }
    
    CZFOTA_LOG("%s:ret = %d", __FUNCTION__, ret);
    return ret;
}


/****************************************************************************************
函数名称:CZ_FotaReportParse
函数描述:收到发送状态报告的响应，通过本地保存的一些状态做下一步操作(退出线程/发送报告/重启)
****************************************************************************************/
static int32 CZ_FotaReportParse(uint8* input, uint32 inputlen)
{
    int32   ret = FAIL;
    uint32  size = sizeof(FOTA_NV_S);
    FOTA_NV_S fota_state = {0};
    CZ_FOTA_FILE_STATUS_CNF_S report_result = {0};
    CZ_FOTA_STATE_E state = CZ_FotaGetState();
    
    memcpy((void*)&report_result, input, inputlen);
    memset((void*)&fota_state, 0x00, size);
    CZ_Get_FotaNv(&fota_state);
    
    switch(state)
    {
        
        case CZ_FOTA_STATE_UPDATE:
            {
                //上报得到响应
                memset((void*)&fota_state, 0x00, size);
                CZ_Set_FotaNv(&fota_state);
                if(s_cz_fota_dl_state.dl_file_vaild == CZ_FOTA_NO_ERROR)
                {
                    KING_Sleep(500);
                    KING_PowerReboot();
                }
            }
            break;

        case CZ_FOTA_STATE_CHECK:
            {
                //收到下载异常的上报响应，置位NV状态为CZ_FOTA_COMPLETE。此时用户应检查fota服务器端是否存在问题
                memset((void*)&fota_state, 0x00, size);
                CZ_Set_FotaNv(&fota_state);
                ret = CZ_FOTA_NO_ERROR;
            }
            break;
        
        default:
            {
                //下载完成的报告
                ret = FAIL;
            }
            break;
    }
    CZ_Fota_ConnectClose();

    CZFOTA_LOG("%s:ret = %d", __FUNCTION__, ret);
    return ret;
}

/*************************************************************
函数名称:CZ_Fota_CheckImage
函数描述:校验写入fotaflash的文件包头及差分版本是否有效
*************************************************************/
static int32 CZ_Fota_CheckImage(void)
{
    int32 ret = FAIL;
    ret = KING_FotaImageCheck(NULL);
    if (ret == FAIL)
    {
        ret = CZ_FOTA_IMAGECHECK_FAIL;
        goto end;
    }
    else
    {
        ret = KING_FotaFlagSet(FOTA_IMAGE_IS_READY);
        {
            if (ret == FAIL)
            {
                ret = CZ_FOTA_FLAGSET_FAIL;
                goto end;
            }
        }
    }
end:
    if(ret != CZ_FOTA_NO_ERROR)
    {
        CZ_FotaSetErrorCode(ERR_AT_CME_CZFOTA_IMG_ERROR);
    }
    return ret;
}

/*************************************************************
函数名称:URC_CmdFunc_CZUPCHK
函数描述:发送+CZUPCHK相关的URC
*************************************************************/
static void URC_CmdFunc_CZUPCHK(uint16 result)
{
    memset(g_rspStr, 0x00, 1024);
    sprintf((char *)g_rspStr, "+CZUPCHKIND: 2,%d",result);
    at_CmdRespUrcText(gFotaEngine, g_rspStr);
}

/*************************************************************
函数名称:URC_CmdFunc_CZUPDL
函数描述:发送+CZUPDL相关的URC
*************************************************************/
static void URC_CmdFunc_CZUPDL(uint8 result, uint32 status)
{
    memset(g_rspStr, 0x00, 1024);
    CZ_FotaSetErrorCode(status);
    sprintf((char *)g_rspStr, "+CZUPDL: %d,%d",result,CZ_FotaGetErrorCode());
    at_CmdRespUrcText(gFotaEngine, g_rspStr);
}

/*************************************************************
函数名称:AT_CmdFunc_CZUPCHK
函数描述:AT+CZUPCHK
*************************************************************/
void AT_CmdFunc_CZUPCHK(AT_CMD_PARA *pParam)
{
    gFotaEngine = pParam->engine;
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            {
                char strRsp[50] = {0};
                memset(strRsp, 0x00, sizeof(strRsp));
                sprintf(strRsp, "+CZUPCHK: \"ip/domain\",<port>");
                at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
                at_CmdRespOK(pParam->engine);
            }
            break;

        case AT_CMD_SET:
            {
                char strRsp[50] = {0};
                bool paramok = true;
                memset(strRsp, 0x00, sizeof(strRsp));

                if (pParam->paramCount != 2)
                {
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                
                const uint8_t *ip = at_ParamStr(pParam->params[0], &paramok);
                if (!paramok)
                {
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                
                const uint16 port = at_ParamUintInRange(pParam->params[1],1,65535,&paramok);
                if (!paramok)
                {
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                KING_MutexLock(s_fota_check_mutex, WAIT_OPT_INFINITE);
                CZ_AT_FOTA_STATE_E state = s_cz_fota_chk_flag;
                KING_MutexUnLock(s_fota_check_mutex);
                if (state != CZ_AT_FOTA_CHKING)
                {
                    KING_MutexLock(s_fota_check_mutex, WAIT_OPT_INFINITE);
                    s_cz_fota_chk_flag = CZ_AT_FOTA_CHKING;
                    KING_MutexUnLock(s_fota_check_mutex);
                    CZ_FotaSetHost((char*)ip);
                    CZ_FotaSetPort(port);
                    at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
                    at_CmdRespOK(pParam->engine);
                    CZ_FotaSetState(CZ_FOTA_STATE_CHECK);
                    CZ_Fota_Connect();
                }
                else
                {
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_CZFOTA_OPERATE_NOT_ALLOWED));
                }
            }
            break;

        case AT_CMD_READ:
            {
                char strRsp[20] = {0};
                FOTA_NV_S fota_nv = {0};
                memset(strRsp, 0x00, sizeof(strRsp));
                CZ_Get_FotaNv(&fota_nv);

                if(CZ_Fota_Check_NeedUpdate(&fota_nv) == TRUE)
                {
                    KING_MutexLock(s_fota_check_mutex, WAIT_OPT_INFINITE);
                    s_cz_fota_chk_flag = CZ_AT_FOTA_CHKDONE;
                    KING_MutexUnLock(s_fota_check_mutex);
                    sprintf(strRsp,"+CZUPCHK: 2,1");              //fotaNV不处于初始状态，说明有未完成的任务
                }
                else
                {
                    KING_MutexLock(s_fota_check_mutex, WAIT_OPT_INFINITE);
                    CZ_AT_FOTA_STATE_E state = s_cz_fota_chk_flag;
                    KING_MutexUnLock(s_fota_check_mutex);
                    switch(state)
                    {
                        case CZ_AT_FOTA_CHKDONE:
                            sprintf(strRsp,"+CZUPCHK: 2,0"); //已查询完，无需更新
                            break;
                        
                        case CZ_AT_FOTA_CHKING:
                            sprintf(strRsp,"+CZUPCHK: 1,0"); //查询中
                            break;

                        case CZ_AT_FOTA_CHKFAIL:
                            sprintf(strRsp,"+CZUPCHK: 2,%d", g_czErrCode);   //已查询，但因为网络原因失败
                            break;
                        
                        default:
                            sprintf(strRsp,"+CZUPCHK: 0,0"); //开机至今没查询过
                            break;
                    }
                }
                at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
                at_CmdRespOK(pParam->engine);
            }
            break;

        default:
            {
                at_CmdRespError(pParam->engine);
            }
            break;
    }

    return;
}

/*************************************************************
函数名称:AT_CmdFunc_CZUPDL
函数描述:AT+CZUPDL
*************************************************************/
void AT_CmdFunc_CZUPDL(AT_CMD_PARA *pParam)
{
    gFotaEngine = pParam->engine;
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            {
                at_CmdRespOK(pParam->engine);
            }
            break;

        case AT_CMD_EXE:
            {
                FOTA_NV_S fota_nv = {0};

                if(isDownLoading == TRUE)
                {
                    CZFOTA_LOG("fota is downloading");
                    AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                }
                CZ_Get_FotaNv(&fota_nv);
                if(CZ_Fota_Check_NeedUpdate(&fota_nv) == TRUE)
                {
                    at_CmdRespOK(pParam->engine);
                    //执行未完成的任务，包括从获取文件信息开始下载流程、已获取文件信息未下载、已下载未完成
                    CZ_FotaSetState(CZ_FOTA_STATE_CONTINUE);
                    CZ_Fota_Connect();
                    isDownLoading = TRUE;
                }
                else
                {
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_CZFOTA_NOT_NEED_UPDATE));
                }
            }
            break;

        case AT_CMD_READ:
            {
                char strRsp[20] = {0};
                FOTA_NV_S fota_nv = {0};
                CZ_Get_FotaNv(&fota_nv);
                memset(strRsp, 0x00, sizeof(strRsp));
                switch(s_cz_fota_dl_state.dl_state)
                {
                    case CZ_FOTA_DL_IDLE:
                        {
                            //fotaNV中有固件信息
                            if(fota_nv.FileLenth != 0)
                            {
                                if(fota_nv.Offset == fota_nv.FileLenth)
                                {
                                    //已下载完成
                                    sprintf(strRsp,"+CZUPDL: 3,100");
                                }
                                else
                                {

                                    //下载中
                                    sprintf(strRsp,"+CZUPDL: 1,%d", (fota_nv.Offset * 100)/ fota_nv.FileLenth);
                                }
                            }
                            else
                            {
                                //无下载任务
                                sprintf(strRsp,"+CZUPDL: 0,0");
                            }
                        }
                        break;

                    case CZ_FOTA_DL_DOING:
                        {
                            sprintf(strRsp,"+CZUPDL: 1,%d", s_cz_fota_dl_state.dl_progress);
                        }
                        break;

                    case CZ_FOTA_DL_FAIL:
                        {
                            sprintf(strRsp,"+CZUPDL: 2,%d", CZ_FotaGetErrorCode());
                        }
                        break;

                    case CZ_FOTA_DL_DONE:
                        {
                            sprintf(strRsp,"+CZUPDL: 3,100");
                        }
                        break;

                    default:
                        at_CmdRespError(pParam->engine);
                        return;
                }
                
                at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
                at_CmdRespOK(pParam->engine);
            }
            break;

        default:
            {
                at_CmdRespError(pParam->engine);
            }
            break;
    }

    
    return;
}

/*************************************************************
函数名称:AT_CmdFunc_CZUPDATE
函数描述:AT+CZUPDATE
*************************************************************/
void AT_CmdFunc_CZUPDATE(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            {
                at_CmdRespOK(pParam->engine);
            }
            break;

        case AT_CMD_EXE:
            {
            	if(CZ_CheckNetState() == 0)
            	{
	                FOTA_NV_S fota_nv = {0};
	                char strRsp[20] = {0};
	                memset(strRsp, 0x00, sizeof(strRsp));
	                CZ_Get_FotaNv(&fota_nv);
					if(fota_nv.Offset == 0) //此处处理用户的异常操作:查询到更新后不下载直接升级，导致fotaNV被清空。
					{
						//没有要升级的image
	                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_CZFOTA_NOT_NEED_UPDATE);
					}
	                else if(CZ_Fota_Check_NeedUpdate(&fota_nv) == true || (fota_nv.Offset == fota_nv.FileLenth))//Offset非0且等于FileLenth
	                {
	                    s_cz_fota_dl_state.dl_file_vaild = CZ_Fota_CheckImage();
	                    if(s_cz_fota_dl_state.dl_file_vaild == CZ_FOTA_NO_ERROR)
	                    {
	                        at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
	                        at_CmdRespOK(pParam->engine);
	                    }
	                    else
	                    {
	                        //image错误
	                        at_CmdRespCmeError(pParam->engine, CZ_FotaGetErrorCode());
	                    }
	                    CZ_FotaSetState(CZ_FOTA_STATE_UPDATE);
	                    CZ_Fota_Connect();
	                }
	                else
	                {
	                    //没有要升级的image
	                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_CZFOTA_NOT_NEED_UPDATE);
	                }
            	}
				else
				{
					CZ_FotaSetErrorCode(ERR_AT_CME_CZFOTA_NET_ERROR);
					at_CmdRespCmeError(pParam->engine, CZ_FotaGetErrorCode());
				}
            }
            break;

        default:
            {
                at_CmdRespError(pParam->engine);
            }
            break;
    }

    return;

}

