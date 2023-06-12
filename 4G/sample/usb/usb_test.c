/*******************************************************************************
 ** File Name:   usb_test.c
 ** Copyright:   Copyright 2020-2020 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: usb api test code
 *******************************************************************************
 
 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-04-27     suzhiliang         Create.
 ******************************************************************************/

/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingCSTD.h"
#include "KingPlat.h"
#include "KingRtos.h"
#include "Kingusb.h" 
#include "app_main.h"

/*******************************************************************************
 ** MACROS
 ******************************************************************************/
#define USB_0   5 
#define USB_1   6

#define LogPrintf(fmt, args...)    do { KING_SysLog("usb: "fmt, ##args); } while(0)

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef enum
{
    MSG_EVT_USB_BASE = 0,
    MSG_EVT_USB0_READ,
    MSG_EVT_USB1_READ,
    MSG_EVT_USB_END,
} MSG_EVT_USB_E; 

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static MSG_HANDLE usbMsgHandle = NULL;

/*******************************************************************************
 ** External Function Declerations
 ******************************************************************************/

/*******************************************************************************
 ** Local Function Declerations
 ******************************************************************************/

/*******************************************************************************
 ** Functions
 ******************************************************************************/
//注:如果在对端的USB未打开接收数据write会返回失败
static void usb0EventCb(uint32 eventID, void* pData, uint32 len)
{
    KING_SysLog("usb_0 event id is %d", eventID);
    if (eventID == USB_EVT_DATA_TO_READ)
    {
        MSG_S msg;
        memset(&msg, 0x00, sizeof(MSG_S));
        msg.messageID = MSG_EVT_USB0_READ;
        KING_MsgSend(&msg, usbMsgHandle);
    }
}

static void usb1EventCb(uint32 eventID, void* pData, uint32 len)
{
    KING_SysLog("usb_1 event id is %d", eventID);
    if (eventID == USB_EVT_DATA_TO_READ)
    {
        MSG_S msg;
        memset(&msg, 0x00, sizeof(MSG_S));
        msg.messageID = MSG_EVT_USB1_READ;
        KING_MsgSend(&msg, usbMsgHandle);
    }
}

static int Usb0_init(void)
{
    int ret = 0;

    ret = KING_RegNotifyFun(DEV_CLASS_USB, USB_0, usb0EventCb); 
    if (-1 == ret)
    {
        LogPrintf("KING_RegNotifyFun() USB_0 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return -1;
    }
    
    //USB_0 init
    ret = KING_UsbInit(USB_0);
    if(-1 == ret)
    {
        LogPrintf("KING_UsbInit() USB_0 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return -1;
    }

    return 0;
}

static int Usb1_init(void)
{
    int ret = 0;

    ret = KING_RegNotifyFun(DEV_CLASS_USB, USB_1, usb1EventCb); 
    if (-1 == ret)
    {
        LogPrintf("KING_RegNotifyFun() USB_1 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return -1;
    }
    
    //USB_1 init
    ret = KING_UsbInit(USB_1);
    if(-1 == ret)
    {
        LogPrintf("KING_UsbInit() USB_1 Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return -1;
    }
    return 0;
}

static void Usb_Init(void)
{
    int ret = 0;

    ret = Usb0_init();
    if (ret == -1)
    {
        LogPrintf("usb_0 Init Fail\r\n");
    }
    
    ret = Usb1_init();
    if (ret == -1)
    {
        LogPrintf("usb_1 Init Fail\r\n");
    }
}
 
static void Usb_dataReadAndWrite(uint32 usbId)
{
    int ret = SUCCESS;
    uint8 targetBuf[50] = {0};
    uint32 bytesRead = 0;
    uint32 bytesWrite = 0;
    
    memset(targetBuf, 0x00, 50);
    ret = KING_UsbRead(usbId, targetBuf, 48, &bytesRead);
    if (FAIL == ret)
    {
        LogPrintf("KING_UsbRead() usbId=%d Failed! errcode=0x%x\r\n", usbId, KING_GetLastErrCode());
        return;
    }
    LogPrintf("USB_%d bytesRead: %u read data: %s \r\n", usbId, bytesRead, targetBuf);

    ret = KING_UsbWrite(usbId, targetBuf, bytesRead, &bytesWrite);
    if (FAIL == ret)
    {
        LogPrintf("KING_UsbWrite() usbId=%d Failed! errcode=0x%x\r\n", usbId, KING_GetLastErrCode());
        return;
    }    
    LogPrintf("USB_%d bytesWrite: %u \r\n", usbId, bytesWrite);

    return;
}

void Usb_Test(void)
{
    int ret = 0;
    MSG_S msg;
    
    LogPrintf("\r\n-----usb test start------\r\n");

    Usb_Init();
    
    //create msg
    ret = KING_MsgCreate(&usbMsgHandle);
    if(FAIL == ret)
    {
        LogPrintf("KING_MsgCreate() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        KING_MsgRcv(&msg, usbMsgHandle, WAIT_OPT_INFINITE);
        switch (msg.messageID)
        {
            case MSG_EVT_USB0_READ:
            {
                Usb_dataReadAndWrite(USB_0);
                break;
            }
            
            case MSG_EVT_USB1_READ:
            {
                Usb_dataReadAndWrite(USB_1);
                break;
            }
            
            default:
            {
                LogPrintf("msg no know\r\n");
                break;
            }
        }
    }
}


