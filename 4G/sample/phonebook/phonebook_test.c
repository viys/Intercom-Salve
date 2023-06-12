/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "kingplat.h"
#include "kingcstd.h"
#include "kingUsim.h"
#include "kingphonebook.h"
#include "kingnet.h"
#include "kingRtos.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("phonebook: "fmt, ##args); } while(0)

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
static void pbk_statusGet(void)
{
    int ret = -1;
    uint32 total = 0, used = 0;
    SIM_PHONE_BOOK_LOC location = SIM_NUMPBSTORAGES;

    location = SIM_PBSTORAGE_EMERGENCY;
    ret = KING_SimPhonebookStatusGet(SIM_1, location, &used, &total);
    LogPrintf("%s SIM_1 SIM_PBSTORAGE_EMERGENCY ret=%d", __FUNCTION__, ret);
    if (ret == 0)
    {
        LogPrintf("%s SIM_1 SIM_PBSTORAGE_EMERGENCY total=%u, used=%u", __FUNCTION__, total, used);
    }

    location = SIM_PBSTORAGE_FIXEDDIALING;
    total = used = 0;
    ret = KING_SimPhonebookStatusGet(SIM_1, location, &used, &total);
    LogPrintf("%s SIM_1 SIM_PBSTORAGE_FIXEDDIALING ret=%d", __FUNCTION__, ret);
    if (ret == 0)
    {
        LogPrintf("%s SIM_1 SIM_PBSTORAGE_FIXEDDIALING total=%u, used=%u", __FUNCTION__, total, used);
    }
    
    location = SIM_PBSTORAGE_LASTDIALING;
    total = used = 0;
    ret = KING_SimPhonebookStatusGet(SIM_1, location, &used, &total);
    LogPrintf("%s SIM_1 SIM_PBSTORAGE_LASTDIALING ret=%d", __FUNCTION__, ret);
    if (ret == 0)
    {
        LogPrintf("%s SIM_1 SIM_PBSTORAGE_LASTDIALING total=%u, used=%u", __FUNCTION__, total, used);
    }

    location = SIM_PBSTORAGE_OWNNUMBERS;
    total = used = 0;
    ret = KING_SimPhonebookStatusGet(SIM_1, location, &used, &total);
    LogPrintf("%s SIM_1 SIM_PBSTORAGE_OWNNUMBERS ret=%d", __FUNCTION__, ret);
    if (ret == 0)
    {
        LogPrintf("%s SIM_1 SIM_PBSTORAGE_OWNNUMBERS total=%u, used=%u", __FUNCTION__, total, used);
    }

    location = SIM_PBSTORAGE_SIM;
    total = used = 0;
    ret = KING_SimPhonebookStatusGet(SIM_1, location, &used, &total);
    LogPrintf("%s SIM_1 SIM_PBSTORAGE_SIM ret=%d", __FUNCTION__, ret);
    if (ret == 0)
    {
        LogPrintf("%s SIM_1 SIM_PBSTORAGE_SIM total=%u, used=%u", __FUNCTION__, total, used);
    }

    location = SIM_PBSTORAGE_EMERGENCY;
    total = used = 0;
    ret = KING_SimPhonebookStatusGet(SIM_2, location, &used, &total);
    LogPrintf("%s SIM_2 SIM_PBSTORAGE_EMERGENCY ret=%d", __FUNCTION__, ret);
    if (ret == 0)
    {
        LogPrintf("%s SIM_2 SIM_PBSTORAGE_EMERGENCY total=%u, used=%u", __FUNCTION__, total, used);
    }

    location = SIM_PBSTORAGE_FIXEDDIALING;
    total = used = 0;
    ret = KING_SimPhonebookStatusGet(SIM_2, location, &used, &total);
    LogPrintf("%s SIM_2 SIM_PBSTORAGE_FIXEDDIALING ret=%d", __FUNCTION__, ret);
    if (ret == 0)
    {
        LogPrintf("%s SIM_2 SIM_PBSTORAGE_FIXEDDIALING total=%u, used=%u", __FUNCTION__, total, used);
    }

    location = SIM_PBSTORAGE_LASTDIALING;
    total = used = 0;
    ret = KING_SimPhonebookStatusGet(SIM_2, location, &used, &total);
    LogPrintf("%s SIM_2 SIM_PBSTORAGE_LASTDIALING ret=%d", __FUNCTION__, ret);
    if (ret == 0)
    {
        LogPrintf("%s SIM_2 SIM_PBSTORAGE_LASTDIALING total=%u, used=%u", __FUNCTION__, total, used);
    }

    location = SIM_PBSTORAGE_OWNNUMBERS;
    total = used = 0;
    ret = KING_SimPhonebookStatusGet(SIM_2, location, &used, &total);
    LogPrintf("%s SIM_2 SIM_PBSTORAGE_OWNNUMBERS ret=%d", __FUNCTION__, ret);
    if (ret == 0)
    {
        LogPrintf("%s SIM_2 SIM_PBSTORAGE_OWNNUMBERS total=%u, used=%u", __FUNCTION__, total, used);
    }

    location = SIM_PBSTORAGE_SIM;
    total = used = 0;
    ret = KING_SimPhonebookStatusGet(SIM_2, location, &used, &total);
    LogPrintf("%s SIM_2 SIM_PBSTORAGE_SIM ret=%d", __FUNCTION__, ret);
    if (ret == 0)
    {
        LogPrintf("%s SIM_2 SIM_PBSTORAGE_SIM total=%u, used=%u", __FUNCTION__, total, used);
    }

    location = SIM_PBSTORAGE_EMERGENCY;
    total = used = 0;
    ret = KING_SimPhonebookStatusGet(SIM_0, location, &used, &total);
    LogPrintf("%s SIM_0 SIM_PBSTORAGE_EMERGENCY ret=%d", __FUNCTION__, ret);
    if (ret == 0)
    {
        LogPrintf("%s SIM_0 SIM_PBSTORAGE_EMERGENCY total=%u, used=%u", __FUNCTION__, total, used);
    }

    location = SIM_PBSTORAGE_FIXEDDIALING;
    total = used = 0;
    ret = KING_SimPhonebookStatusGet(SIM_0, location, &used, &total);
    LogPrintf("%s SIM_0 SIM_PBSTORAGE_FIXEDDIALING ret=%d", __FUNCTION__, ret);
    if (ret == 0)
    {
        LogPrintf("%s SIM_0 SIM_PBSTORAGE_FIXEDDIALING total=%u, used=%u", __FUNCTION__, total, used);
    }

    location = SIM_PBSTORAGE_LASTDIALING;
    total = used = 0;
    ret = KING_SimPhonebookStatusGet(SIM_0, location, &used, &total);
    LogPrintf("%s SIM_0 SIM_PBSTORAGE_LASTDIALING ret=%d", __FUNCTION__, ret);
    if (ret == 0)
    {
        LogPrintf("%s SIM_0 SIM_PBSTORAGE_LASTDIALING total=%u, used=%u", __FUNCTION__, total, used);
    }

    location = SIM_PBSTORAGE_OWNNUMBERS;
    total = used = 0;
    ret = KING_SimPhonebookStatusGet(SIM_0, location, &used, &total);
    LogPrintf("%s SIM_0 SIM_PBSTORAGE_OWNNUMBERS ret=%d", __FUNCTION__, ret);
    if (ret == 0)
    {
        LogPrintf("%s SIM_0 SIM_PBSTORAGE_OWNNUMBERS total=%u, used=%u", __FUNCTION__, total, used);
    }

    location = SIM_PBSTORAGE_SIM;
    total = used = 0;
    ret = KING_SimPhonebookStatusGet(SIM_0, location, &used, &total);
    LogPrintf("%s SIM_0 SIM_PBSTORAGE_SIM ret=%d", __FUNCTION__, ret);
    if (ret == 0)
    {
        LogPrintf("%s SIM_0 SIM_PBSTORAGE_SIM total=%u, used=%u", __FUNCTION__, total, used);
    }
}

static void pbk_opsTestByLocation(SIM_PHONE_BOOK_LOC location, SIM_ID sim)
{
    int ret = -1;
    SIMPHONEBOOKENTRY entry;
    uint32 index = 1;
    uint32 total = 0, used = 0;

    LogPrintf("%s sim = %d location=%d, index=%u", __FUNCTION__, sim, location, index);
    memset(&entry, 0x00, sizeof(SIMPHONEBOOKENTRY));
    strncpy((char*)(entry.lpszAddress), "13678924567", strlen("13678924567"));
    strncpy((char*)(entry.lpszText), "abc", strlen("abc"));
    ret = KING_SimPhonebookEntryWrite(sim, location, index, &entry);
    if (ret == 0)
    {
        LogPrintf("%s write %d ph succ", __FUNCTION__, index);
    }

    index++;
    memset(&entry, 0x00, sizeof(SIMPHONEBOOKENTRY));
    strncpy((char*)(entry.lpszAddress), "13678924523", strlen("13678924523"));
    strncpy((char*)(entry.lpszText), "abcd", strlen("abcd"));
    ret = KING_SimPhonebookEntryWrite(sim, location, index, &entry);
    if (ret == 0)
    {
        LogPrintf("%s write %d ph succ", __FUNCTION__, index);
    }

    index++;
    memset(&entry, 0x00, sizeof(SIMPHONEBOOKENTRY));
    strncpy((char*)(entry.lpszAddress), "13878924523", strlen("13878924523"));
    strncpy((char*)(entry.lpszText), "bcd", strlen("bcd"));
    ret = KING_SimPhonebookEntryWrite(sim, location, index, &entry);
    if (ret == 0)
    {
        LogPrintf("%s write %d ph succ", __FUNCTION__, index);
    }
    
    total = used = 0;
    ret = KING_SimPhonebookStatusGet(sim, location, &used, &total);
    if (ret == 0)
    {
        LogPrintf("%s get sim = %d location=%d, total=%u, used=%u", __FUNCTION__, sim, location, total, used);
    }
    
    memset(&entry, 0x00, sizeof(SIMPHONEBOOKENTRY));
    ret = KING_SimPhonebookEntryRead(sim, location, index, &entry);
    if (ret == 0)
    {
        LogPrintf("%s Read %u pb succ, lpszAddress=%s, lpszText=%s", __FUNCTION__, index, entry.lpszAddress, entry.lpszText);
    } 

    ret = KING_SimPhonebookEntryDelete(sim, location, index);
    if (ret == 0)
    {
        LogPrintf("%s delete %u pb succ", __FUNCTION__, index);
    }
    
    total = used = 0;
    ret = KING_SimPhonebookStatusGet(sim, location, &used, &total);
    if (ret == 0)
    {
        LogPrintf("%s get sim = %d location=%d, total=%u, used=%u", __FUNCTION__, sim, location, total, used);
    }
}

static void pbk_startTest(void)
{
    int ret = -1;
    NET_STATE_E state;
    
    while (1)
    {
        ret = KING_NetStateGet(&state);
        LogPrintf("%s: Get net state %d, ret : %d\r\n", __FUNCTION__, state, ret);
        if (ret == 0 && state > NET_NO_SERVICE && state < NET_EMERGENCY)
        {
            NET_CESQ_S Cesq;
            ret = KING_NetSignalQualityGet(&Cesq);
            LogPrintf("%s: Get sinagal ret is %d. rxlev:%d, ber:%d, rscp:%d, ecno:%d, rsrq:%d, rsrp:%d\r\n", __FUNCTION__,
                ret,Cesq.rxlev,Cesq.ber,Cesq.rscp,Cesq.ecno,Cesq.rsrq,Cesq.rsrp);

            KING_Sleep(2000);
            pbk_statusGet();
            pbk_opsTestByLocation(SIM_PBSTORAGE_SIM, SIM_0);

            LogPrintf("%s: complete\r\n", __FUNCTION__);
            break;
        }
        
        KING_Sleep(1000);
    }
}

void pbk_test(void)
{
    pbk_startTest();
}

