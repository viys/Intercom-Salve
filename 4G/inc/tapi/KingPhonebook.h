#ifndef KH_PBK
#define KH_PBK

#include "KingUsim.h"

typedef enum{
    SIM_PBSTORAGE_EMERGENCY,
    SIM_PBSTORAGE_FIXEDDIALING,
    SIM_PBSTORAGE_LASTDIALING,
    SIM_PBSTORAGE_OWNNUMBERS,
    SIM_PBSTORAGE_SIM,
    SIM_NUMPBSTORAGES,
    SIM_PBSTORAGE_MAX = 0x7FFFFFFF
}SIM_PHONE_BOOK_LOC;

#define MAX_LENGTH_ADDRESS 20
#define MAX_LENGTH_PHONEBOOKENTRYTEXT 64

typedef enum{
    SIM_ADDRTYPE_INTERNATIONAL,
    SIM_ADDRTYPE_NATIONAL,
    SIM_ADDRTYPE_NETWKSPECIFIC,
    SIM_ADDRTYPE_SUBSCRIBER,
    SIM_ADDRTYPE_ALPHANUM,
    SIM_ADDRTYPE_ABBREV,
    SIM_ADDRTYPE_UNKNOWN = 0x7FFFFFFF
}SIM_ADDR_TYPE;

typedef enum{
    SIM_NUMPLAN_TELEPHONE, // ISDN/telephone numbering plan (E.164/E.163). 
    SIM_NUMPLAN_DATA, // Data numbering plan (X.121). 
    SIM_NUMPLAN_TELEX,// Telex numbering plan. 
    SIM_NUMPLAN_NATIONAL,// National numbering plan. 
    SIM_NUMPLAN_PRIVATE,// Private numbering plan. 
    SIM_NUMPLAN_ERMES,// ERMES numbering plan (ETSI DE/PS 3 01-3). 
    SIM_NUMPLAN_UNKNOWN = 0x7FFFFFFF// Unknown. 
}SIM_NUMPLAN;

typedef struct _tagSimPhoneBookEntry {
  DWORD dwParams;
  SIM_ADDR_TYPE dwAddressType;
  SIM_NUMPLAN dwNumPlan;
  uint8 lpszAddress[MAX_LENGTH_ADDRESS+2];
  uint8 lpszText[MAX_LENGTH_PHONEBOOKENTRYTEXT];
} SIMPHONEBOOKENTRY, *LPSIMPHONEBOOKENTRY;


typedef struct _tagPhoneBook{
    SIM_ID SimID;
    SIM_PHONE_BOOK_LOC LocType;
    DWORD  Index;
    SIMPHONEBOOKENTRY spbEntry;
}SIM_PHONE_BOOK;

/**
 *��ȡ�绰��״̬
 *
 * @param[in] sim  ��Ӧ�Ŀ�ID
 * @param[in] location �洢λ��
 * @param[out] Used �Ѵ洢������
 * @param[out] Total ���Դ洢��������
 * @return 0 success  -1 fail
 */
int KING_SimPhonebookStatusGet(SIM_ID sim, SIM_PHONE_BOOK_LOC location, uint32 *Used, uint32 *Total);

/**
 *��ȡָ���ĵ绰����Ŀ������Ϣ
 *
 * @param[in] sim  ��Ӧ�Ŀ�ID
 * @param[in] location �洢λ��
 * @param[in] Index  ��ȡ�ڼ���
 * @param[out] entry  ��ȡ������
 * @return 0 success  -1 fail
 */
int KING_SimPhonebookEntryRead(SIM_ID sim, SIM_PHONE_BOOK_LOC location, uint32 Index, SIMPHONEBOOKENTRY *entry);

/**
 *�޸�ָ���ĵ绰����Ŀ������Ϣ
 *
 * @param[in] sim  ��Ӧ�Ŀ�ID
 * @param[in] location �洢λ��
 * @param[in] Index  �޸ĵڼ���
 * @param[out] entry  �޸ĵ�����
 * @return 0 success  -1 fail
 */
int KING_SimPhonebookEntryWrite(SIM_ID sim, SIM_PHONE_BOOK_LOC location, uint32 Index, SIMPHONEBOOKENTRY *entry);

/**
 *ɾ��ָ���ĵ绰����Ŀ������Ϣ
 *
 * @param[in] sim  ��Ӧ�Ŀ�ID
 * @param[in] location �洢λ��
 * @param[in] Index  ɾ���ڼ���
 * @return 0 success  -1 fail
 */
int KING_SimPhonebookEntryDelete(SIM_ID sim, SIM_PHONE_BOOK_LOC location, uint32 Index);

#endif
