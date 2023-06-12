#ifndef __AT_TEST_H__
#define __AT_TEST_H__

extern FS_HANDLE AudioFileHandle;

void At_CmdHandle(void);

int AtAdcValue(char* data, uint32 dataLen);
int AudioChannelGet(char* data, uint32 dataLen);
int AudioChannelSet(char* data, uint32 dataLen);
int AtAudioVolume(char* data, uint32 dataLen);
int AtRecord(char* data, uint32 dataLen);
int AtAudioPlay(char* data, uint32 dataLen);
int AtChargeCmd(char* data, uint32 dataLen);
int AtChargeCfg(char* data, uint32 dataLen);
int AtSpiFlash(char* data, uint32 dataLen);
int AtSpiFs(char* data, uint32 dataLen);
int AtGpioCfgSet(char* data, uint32 dataLen);
int AtGpioCfgGet(char* data, uint32 dataLen);
int AtGpioSet(char* data, uint32 dataLen);
int AtIic(char* data, uint32 dataLen);
int AtKeypadCmd(char* data, uint32 dataLen);
int AtDisplay(char* data, uint32 dataLen);
int AtBackCmd(char* data, uint32 dataLen);
int AtLcdBack(char* data, uint32 dataLen);
int AtSink(char* data, uint32 dataLen);
int AtAppVersions(char* data, uint32 dataLen);
int AtCamera(char* data, uint32 dataLen);
int AtSd(char* data, uint32 dataLen);
int AtSpi(char* data, uint32 dataLen);
int AtUartCfg(char* data, uint32 dataLen);
int AtEchoCmd(char* data, uint32 dataLen);
int AtGpioGet(char* data, uint32 dataLen);
int AtImeiGet(char* data, uint32 dataLen);
int AtSuspend(char* data, uint32 dataLen);
int AtCmdList(char* data, uint32 dataLen);

#endif
