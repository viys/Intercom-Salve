#ifndef __KING_USIM_H__
#define __KING_USIM_H__

#include "KingDef.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define SIM_CAPSTYPE_PBENTRYLENGTH          BIT0 //Phone book entry lengths 
#define SIM_CAPSTYPE_PBSTORELOCATIONS       BIT1 //Phone book storage locations 
#define SIM_CAPSTYPE_LOCKFACILITIES         BIT2 //Lock facilities 
#define SIM_CAPSTYPE_PBINDEXRANGE           BIT3 //Valid phone book entry indexes 
#define SIM_CAPSTYPE_LOCKINGPWDLENGTHS      BIT4 //Locking password lengths 
#define SIM_CAPSTYPE_MSGMEMORYLOCATIONS     BIT5 //Message memory locations 
#define SIM_CAPSTYPE_ALL                    0xFFFFFFFF //All of the above 

#define PHONE_NUM_MAX_BYTES     21
//--------------------------------------------------------------------------------------------------
#define SIM_ICCID_MAX_LEN       20
//--------------------------------------------------------------------------------------------------
#define SIM_IMSI_MAX_LEN        20
//--------------------------------------------------------------------------------------------------
#define SIM_PIN_MAX_LEN         8
//--------------------------------------------------------------------------------------------------
#define SIM_PUK_MAX_LEN         8
//--------------------------------------------------------------------------------------------------
#define SIM_EID_MAX_LEN         32
//--------------------------------------------------------------------------------------------------
#define SIM_HOME_NET_OPT_NAME_LEN   16
#define SIM_HOME_MCC_MNC_LEN        3


//usim event
#define SIM_EVENT_STATE              0x0001
#define SIM_EVENT_PIN_PUK_BLOCK      0x0002
#define SIM_EVENT_PIN_CHANGE         0x0003

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef enum{
    SIM_0,
    SIM_1,
    SIM_2,
    SIM_MAX = 0x7FFFFFFF
}SIM_ID;

typedef enum {
    SIM_PIN_UNLOCK,
    SIM_PIN_LOCK,
    SIM_PIN_MAX = 0x7FFFFFFF
} SIM_PIN_LOCK_STATE;

typedef enum{
    SIM_ABSENT, //SIM card is absent
    SIM_INSERTED, //SIM card is inserted and locked
    SIM_READY, //SIM card is inserted and unlocked
    SIM_BLOCKED,//SIM card is blocked
    SIM_BUSY, //SIM card is busy
    SIM_POWER_DOWN, //SIM card is powered down
    SIM_STATE_UNKNOWN = 0x7FFFFFFF
}SIM_STATES;

typedef struct _tagSimRecordInfo {
  DWORD dwRecordType;
  DWORD dwItemCount;
  DWORD dwSize;
} SIMRECORDINFO, *PSIMRECORDINFO;

typedef enum{
    SIM_RECORDTYPE_TRANSPARENT, //A single variable-length record. 
    SIM_RECORDTYPE_CYCLIC, //A cyclic set of records, each of the same length. 
    SIM_RECORDTYPE_LINEAR, //A linear set of records, each of the same length. 
    SIM_RECORDTYPE_MASTER, //Every SIM has a single master record, effectively the head node. 
    SIM_RECORDTYPE_DEDICATED, //Effectively a "directory" file that is a parent of other records. 
    SIM_RECORDTYPE_UNKNOWN = 0x7FFFFFFF// An unknown file type. 
}SIM_RECORDTYPE;

typedef enum{
    SIM_LOCKEDSTATE_READY, //Not awaiting a password (unlocked). 
    SIM_LOCKEDSTATE_SIM_PIN, //Awaiting the SIM PIN. 
    SIM_LOCKEDSTATE_SIM_PUK, //Awaiting the SIM pointer to an unknown (PUK). 
    SIM_LOCKEDSTATE_PH_SIM_PIN, //Awaiting the Phone to SIM Personalization PIN. 
    SIM_LOCKEDSTATE_PH_FSIM_PIN, //Awaiting the Phone to first SIM Personalization PIN. 
    SIM_LOCKEDSTATE_PH_FSIM_PUK, //Awaiting the Phone to first SIM Personalization PUK. 
    SIM_LOCKEDSTATE_SIM_PIN2, //Awaiting the SIM PIN2. 
    SIM_LOCKEDSTATE_SIM_PUK2, //Awaiting the SIM PUK2. 
    SIM_LOCKEDSTATE_PH_NET_PIN, //Awaiting the Network Personalization PIN. 
    SIM_LOCKEDSTATE_PH_NET_PUK, //Awaiting the Network Personalization PUK. 
    SIM_LOCKEDSTATE_PH_NETSUB_PIN, //Awaiting the Network Subset Personalization PIN. 
    SIM_LOCKEDSTATE_PH_NETSUB_PUK, //Awaiting the Network Subset Personalization PUK. 
    SIM_LOCKEDSTATE_PH_SP_PIN, //Awaiting the Service Provider Personalization PIN. 
    SIM_LOCKEDSTATE_PH_SP_PUK, //Awaiting the Service Provider Personalization PUK. 
    SIM_LOCKEDSTATE_PH_CORP_PIN, //Awaiting the Corporate Personalization PIN. 
    SIM_LOCKEDSTATE_PH_CORP_PUK, //Awaiting the Corporate Personalization PUK. 
    SIM_LOCKEDSTATE_UNKNOWN = 0x7FFFFFFF //The locking state is unknown. 
}SIM_LOCK_STATE;

typedef enum{
    SIM_LOCKFACILITY_CNTRL, // Lock control surface. 
    SIM_LOCKFACILITY_PH_SIM, // Lock phone to SIM card. 
    SIM_LOCKFACILITY_PH_FSIM, // Lock phone to first SIM card. 
    SIM_LOCKFACILITY_SIM, // Lock SIM card. 
    SIM_LOCKFACILITY_SIM_PIN2, // Lock SIM card. 
    SIM_LOCKFACILITY_SIM_FIXEDIALING, // SIM fixed dialing memory. 
    SIM_LOCKFACILITY_NETWORKPERS, // Network personalization. 
    SIM_LOCKFACILITY_NETWORKSUBPERS, // Network subset personalization. 
    SIM_LOCKFACILITY_SERVICEPROVPERS, // Service provider personalization. 
    SIM_LOCKFACILITY_CORPPERS, // Corporate personalization. 
    SIM_LOCKFACILITIES_NUM
}SIM_LOCKFACILITY;


typedef struct simlockingpwdlength {
  DWORD dwFacility;
  DWORD dwPasswordLength;
} SIMLOCKINGPWDLENGTH, *PSIMLOCKINGPWDLENGTH;

typedef struct _tagSimCaps {
  DWORD dwPBStorages;
  DWORD dwMinPBIndex;
  DWORD dwMaxPBIndex;
  DWORD dwMaxPBEAddressLength;
  DWORD dwMaxPBETextLength;
  DWORD dwLockFacilities;
  DWORD dwReadMsgStorages;
  DWORD dwWriteMsgStorages;
  DWORD dwNumLockingPwdLengths;
  SIMLOCKINGPWDLENGTH rgLockingPwdLengths[SIM_LOCKFACILITIES_NUM];
} SIMCAPS,  *PSIMCAPS;

typedef enum
{
    SIM_PIN = 0,     ///< PIN id
    SIM_PIN2       ///< PIN2 id
}SIM_PIN_TYPE;

typedef enum
{
    SIM_PUK = 0,    ///< PUK id
    SIM_PUK2      ///< PUK2 id
}SIM_PUK_TYPE;

typedef struct
{
    uint8 imsi_len;
    uint8 imsi_val[SIM_IMSI_MAX_LEN];
} IMSI_S;

typedef struct
{
    uint8 id_num[SIM_ICCID_MAX_LEN + 1];
} ICCID_S;


/**
 * ��ȡsim��״̬
 *
 * @param[in]  id SIM_0,SIM_1, SIM_2
 * @param[out]  State       SIM_ABSENT,SIM_INSERTED,SIM_READY,etc.
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimCardInformationGet(SIM_ID id, SIM_STATES *State);

/**
 * ��ȡsim��Lock״̬
 *
 * @param[in]  id SIM_0,SIM_1, SIM_2
 * @param[out]  LockState       SIM_LOCKEDSTATE_READY,SIM_LOCKEDSTATE_SIM_PIN,SIM_LOCKEDSTATE_SIM_PUK,etc.
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimCardLockStateGet(SIM_ID id, SIM_LOCK_STATE *LockState);

/**
 * ��ȡsim��iccid
 *
 * @param[in]  id SIM_0,SIM_1, SIM_2
 * @param[out]  pIccid
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimICCIDGet(SIM_ID id, ICCID_S* pIccid);

/**
 * ��ȡsim��IMSI
 *
 * @param[in]  id SIM_0,SIM_1, SIM_2
 * @param[out]  pImsi
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimImsiGet(SIM_ID id, IMSI_S* pImsi);

/**
 * ��ȡsim��EID
 *
 * @param[in]  id SIM_0,SIM_1, SIM_2
 * @param[out]  EID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimEidGet(SIM_ID id, uint8 *EID);

/**
 * ��ȡsim����������
 *
 * @param[in]  id SIM_0,SIM_1, SIM_2
 * @param[out]  PhoneNumber
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimPhoneNumberGet(SIM_ID id, uint8 *PhoneNumber);

/**
 * ������ر�sim��PIN��
 *
 * @param[in]  id SIM_0,SIM_1, SIM_2
 * @param[in]  pinPtr
 * @param[in]  state  SIM_PIN_UNLOCK, SIM_PIN_LOCK
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimPinLock(SIM_ID id, const char*  pinPtr, SIM_PIN_LOCK_STATE state);

/**
 * ����sim��PIN��
 *
 * @param[in]  id SIM_0,SIM_1, SIM_2
 * @param[in]  pinPtr
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimPinEnter(SIM_ID id, const char*  pinPtr);

/**
 * �޸�sim��PIN��
 *
 * @param[in]  id SIM_0,SIM_1, SIM_2
 * @param[in]  OldpinPtr
 * @param[in]  NewpinPtr
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimPinChange(SIM_ID id, const char*  OldpinPtr, const char*  NewpinPtr);//Null pinptr will disable PIN

/**
 * ��ȡPIN���ʣ�����
 *
 * @param[in]  id SIM_0,SIM_1, SIM_2
 * @param[out]  Tries
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimRemainingPinTriesGet(SIM_ID id, uint32 *Tries);

/**
 * ����PUK��
 *
 * @param[in]  id SIM_0,SIM_1, SIM_2
 * @param[in]  pukPtr
 * @param[in]  NewPinPtr
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimPukEnter(SIM_ID id, const char*  pukPtr, const char*  NewPinPtr);//Null pinptr will disable PIN

/**
 * ��ȡPUK���ʣ�����
 *
 * @param[in]  id SIM_0,SIM_1, SIM_2
 * @param[out]  Tries
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimRemainingPukTriesGet(SIM_ID id, uint32 *Tries);

/**
 * ��ȡ������Ӫ������
 *
 * @param[in]  id SIM_0,SIM_1, SIM_2
 * @param[out]  NameStr
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimHomeNetworkOperatorGet(SIM_ID id, uint8 *NameStr);

/**
 * ��ȡHPLMN
 *
 * @param[in]  id SIM_0,SIM_1, SIM_2
 * @param[out]  mccPtr
 * @param[out]  mncPtr
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimHomePLMNGet(SIM_ID id, uint8 *mccPtr, uint8 *mncPtr);

/**
 * �л�SIM���л���Ҫ����
 * �ù��ܵ�ͬ���л����� SIM ���ۣ�ѡ���ĸ���������Ϊ�����SIM_0��
 *
 * @param[in]  id SIM_0,SIM_1, SIM_2
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimIdSelect(SIM_ID id);

/**
 * ��ȡSIM��id
 *
 * @param[out]  id SIM_0,SIM_1
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimCurrentIdGet(SIM_ID *id);

/**
 * ��ȡsim��ready״̬
 *
 * @param[in]   id           which sim to check
 * @param[out]  pReady       0 - not ready  1- ready
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimReadyGetById(SIM_ID id, uint32* pReady);

#endif /*__KING_USIM_H__*/

