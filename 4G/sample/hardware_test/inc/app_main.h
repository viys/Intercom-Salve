#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

#include "KingDef.h"
#include "KingPlat.h"
#include "KingCSTD.h"
#include "KingRtos.h"
#include "KingRTC.h"
#include "KingUart.h"
#include "KingPlat.h"
#include "KingFlash.h"
#include "KingIIC.h"
#include "KingUsb.h"
#include "KingNet.h"
#include "KingDisplay.h"
#include "KingKeypad.h"
#include "KingGpio.h"
#include "KingAudio.h"
#include "KingFileSystem.h"
#include "KingSpi.h"
#include "KingCharge.h"
#include "KingPowerManage.h"
#include "KingSink.h"

extern MSG_HANDLE msgHandle;

enum Msg_Evt
{
    MSG_EVT_BASE = 0,
    MSG_EVT_UART1_READ,
    MSG_EVT_UART2_READ,
    MSG_EVT_UART3_READ,
    MSG_EVT_RECORD,
    MSG_EVT_RECORD_STOP,
    MSG_EVT_GPIO_INT,
    MSG_EVT_END = 0x7FFFFFFF,
};


#endif
