#ifndef __KH_CBDATA_H__
#define __KH_CBDATA_H__

#include "kingdef.h"
#include "kingwifi.h"
#include "kingnet.h"
#include "tapi/kingcallcontrol.h"

typedef struct _tagUartEventData { 
  uint32  classID;
  uint32  uartID;
  uint32  eventID;
} UART_EVENT_DATA_S;

typedef struct _tagUsbEventData { 
  uint32  classID;
  uint32  usbID;
  uint32  eventID;
} USB_EVENT_DATA_S;

typedef struct _tagNetEventData {
  uint32    classID;
  uint32    simID;
  uint32    eventID;
  union{
      uint32     value;
      RSP_INFO_S info;
  };
} NET_EVENT_DATA_S;

typedef struct _tagSocketEventData {
  uint32    classID;
  uint32    socketID;
  uint32    eventID;
  uint32    availSize;
} SOCKET_EVENT_DATA_S;

typedef struct _tagAlarmEventData {
  uint32    classID;
  uint32    alarmID;
  uint32    eventID;
} ALARM_EVENT_DATA_S;

typedef struct _tagWifiEventData {
  uint32    classID;
  uint32    wifiID;
  uint32    eventID;
  KH_ScanRequest_t result;
} WIFI_EVENT_DATA_S;

typedef struct _tagUsimEventData {
  uint32    classID;
  uint32    simID;
  uint32    eventID;
  uint32    lockState;
} USIM_EVENT_DATA_S;

typedef struct _tagUsimStateData {
  uint32    classID;
  uint32    simID;
  uint32    eventID;
  uint32    state;
} USIM_STATE_DATA_S;

typedef struct _tagCallControlNotifyEvent{
  uint32                classID;
  uint32                simID;  
  uint8                 callID;
  char*                 pNumber;
  CC_STATE_EVENT        event;
  CC_TERMINATION_REASON ReasonCode;
}CC_EVENT_DATA_S;

typedef struct _tagSmsEventData {
  uint32    classID;
  uint32    simID;
  uint32    eventID;
  uint32    nParam;
} SMS_EVENT_DATA_S;

typedef struct _tagDualSimRspData {
  uint32    classID;
  uint32    eventID;
  uint32    ret;
} DUAL_SIM_RSP_DATA_S;

typedef struct _tagDualSimIndData {
  uint32    classID;
  uint32    eventID;
  uint32    aim_sim;
  uint32    cause;
} DUAL_SIM_IND_DATA_S;


typedef union _tagKhCbData {
  UART_EVENT_DATA_S     uart;
  USB_EVENT_DATA_S      usb;
  NET_EVENT_DATA_S      net;
  SOCKET_EVENT_DATA_S   socket;
  ALARM_EVENT_DATA_S    alarm;
  WIFI_EVENT_DATA_S     wifi;
  USIM_EVENT_DATA_S     usimEvent;
  USIM_STATE_DATA_S     usimState;
  CC_EVENT_DATA_S       CCEvent;
  SMS_EVENT_DATA_S      smsEvent;
  DUAL_SIM_RSP_DATA_S   dualSimRsp;
  DUAL_SIM_IND_DATA_S   dualSimInd;
} KH_CB_DATA_U;

#endif
