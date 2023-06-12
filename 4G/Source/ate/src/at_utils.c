/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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
#include "at_sa.h"
#include "at_list.h"
#include "at_utils.h"
#include "cs_types.h"
#include "kingplat.h"
#include "kingrtos.h"
#include "kingcstd.h"

#define DEBUG 0
#if DEBUG
#define DBG(fmt, ...) do { KING_SysLog(fmt, ##__VA_ARGS__) }while(0)
#else
#define DBG(fmt, ...)
#endif

typedef struct {
    uint32 time_out;
    AT_CALLBACK_FUNC_T func;
    void *ctx;
    TIMER_HANDLE handle;
}AT_TIMER_PARAM_T;

extern THREAD_HANDLE g_hAtTask;
extern MSG_HANDLE g_hAtMsg;
static linkList* g_atTimerList = NULL;

static const uint8_t r_crctable[256] = {
    0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75,
    0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
    0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69,
    0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
    0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D,
    0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
    0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51,
    0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,
    0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05,
    0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
    0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19,
    0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
    0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D,
    0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
    0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21,
    0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,
    0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95,
    0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
    0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89,
    0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
    0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD,
    0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
    0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1,
    0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,
    0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5,
    0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
    0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9,
    0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
    0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD,
    0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
    0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1,
    0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF};

bool at_CmuxCheckFcs(const uint8_t *data, unsigned length, uint8_t crc)
{
    uint8_t fcs = 0xff;
    while (length--)
    {
        fcs = r_crctable[fcs ^ (uint8_t)*data++];
    }
    fcs = r_crctable[fcs ^ crc];
    return (fcs == 0xcf) ? true : false;
}


uint8_t at_CmuxCalcFcs(const uint8_t *data, unsigned length)
{
    uint8_t fcs = 0xff;
    while (length--)
    {
        fcs = r_crctable[fcs ^ (uint8_t)*data++];
    }
    return 0xff - fcs;
}


void at_TaskCallback(AT_CALLBACK_FUNC_T func, void *param)
{
    MSG_S msg = {0};
    AT_TASK_MSG_PARAM_T* data = NULL;

    DBG("%s++", __FUNCTION__);
    KING_MemAlloc((void**)&data, sizeof(AT_TASK_MSG_PARAM_T));
    if(data != NULL)
    {
        memset(data,0,sizeof(AT_TASK_MSG_PARAM_T));
        data->func = func;
        data->param = param;
        msg.messageID = EV_AT_CALLBACK;
        msg.pData = data;
        msg.DataLen = sizeof(AT_TASK_MSG_PARAM_T);
        KING_MsgSend(&msg,g_hAtMsg);
    }
    DBG("%s--", __FUNCTION__);
}

void at_TaskCallbackNotif(AT_CALLBACK_FUNC_T func, void *param)
{
    DBG("%s++", __FUNCTION__);
    at_TaskCallback(func, param);
    DBG("%s--", __FUNCTION__);
}

int at_StartCallbackTimer(unsigned ms, AT_CALLBACK_FUNC_T callback, void *ctx)
{
    int ret = FAIL;
    AT_TIMER_PARAM_T* pTimerParam = NULL;
    TIMER_ATTR_S timerAtt = {0};
    bool isMatch =FALSE;
    int list_size = 0;

    DBG("%s++", __FUNCTION__);
    if(g_atTimerList == NULL)
    {
        g_atTimerList = create_linklist();
        if(!g_atTimerList)
        {
            KING_SysLog("create_linklist fail\n");
            DBG("%s-- fail 000", __FUNCTION__);
            return ret;
        }
        KING_SysLog("create_linklist\n");
    }
    
    list_size = linklist_size(g_atTimerList);
    KING_SysLog("create_linklist list_size:%d\n",list_size);
    for(uint8 i=0;i<list_size;i++)
    {
        pTimerParam = (AT_TIMER_PARAM_T*)linklist_get(g_atTimerList,i);
        if(pTimerParam != NULL && pTimerParam->time_out == ms && pTimerParam->func == callback)
        {
            KING_SysLog("linklist match %d, time out:%d\n",i,pTimerParam->time_out);
            pTimerParam->ctx = ctx;
            //reactive timer
            timerAtt.timer_fun = (TIMER_FUN)callback;
            timerAtt.timeout = ms;
            timerAtt.isPeriod = FALSE;
            KING_TimerDeactive(pTimerParam->handle);
            KING_TimerActive(pTimerParam->handle, &timerAtt);
            isMatch = TRUE;
            break;
        }
    }
    if(!isMatch)
    {
        KING_SysLog("linklist dismatch");
        //new timer
        pTimerParam = (AT_TIMER_PARAM_T*)malloc(sizeof(AT_TIMER_PARAM_T));  //free in linklist_delete
        pTimerParam->ctx = ctx;
        pTimerParam->func = callback;
        pTimerParam->time_out = ms;
        ret = KING_TimerCreate(&pTimerParam->handle);
        if(ret != SUCCESS)
        {
            DBG("%s-- fail", __FUNCTION__);
            return ret;
        }
        linklist_insert(g_atTimerList,list_size,pTimerParam);
        
        timerAtt.timer_fun = (TIMER_FUN)callback;
        timerAtt.timeout = ms;
        timerAtt.isPeriod = FALSE;
        KING_TimerDeactive(pTimerParam->handle);
        KING_TimerActive(pTimerParam->handle, &timerAtt);
    }
    DBG("%s--", __FUNCTION__);
    return SUCCESS;
}

void at_StopCallbackTimer(AT_CALLBACK_FUNC_T callback, void *ctx)
{
    AT_TIMER_PARAM_T* pTimerParam = NULL;
    int list_size = linklist_size(g_atTimerList);

    DBG("%s++", __FUNCTION__);
    for(uint8 i=0;i<list_size;i++)
    {
        pTimerParam = (AT_TIMER_PARAM_T*)linklist_get(g_atTimerList,i);
        if(pTimerParam != NULL &&  pTimerParam->func == callback)
        {
            KING_TimerDeactive(pTimerParam->handle);
            KING_TimerDelete(pTimerParam->handle);
            linklist_delete(g_atTimerList,i);
            break;
        }
    }
    DBG("%s--", __FUNCTION__);
}

void *at_GetTimerCtx(TIMER_HANDLE handle)
{
    int list_size = 0;    
    AT_TIMER_PARAM_T* pTimerParam = NULL;

    DBG("%s++", __FUNCTION__);
    list_size = linklist_size(g_atTimerList);

    for(uint8 i=0;i<list_size;i++)
    {
        pTimerParam = (AT_TIMER_PARAM_T*)linklist_get(g_atTimerList,i);
        if(handle == pTimerParam->handle)
        {
            KING_SysLog("linklist match %d, time out:%d\n",i,pTimerParam->time_out);
            DBG("%s--", __FUNCTION__);
            return pTimerParam->ctx;
        }
    }

    DBG("%s-- NULL", __FUNCTION__);
    return NULL;
}

