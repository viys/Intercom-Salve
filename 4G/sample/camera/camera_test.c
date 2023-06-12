/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "KingCSTD.h"
#include "KingPlat.h"
#include "KingRtos.h"
#include "KingCamera.h"
#include "KingCbData.h"
#include "KingDisplay.h"
#include "app_main.h"

/*******************************************************************************
 ** MACROS
 ******************************************************************************/
#define LogPrintf(fmt, args...)    do { KING_SysLog("camera: "fmt, ##args); } while(0) 


/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/

/*******************************************************************************
 ** Variables
 ******************************************************************************/

/*******************************************************************************
 ** External Function Declerations
 ******************************************************************************/

/*******************************************************************************
 ** Local Function Declerations
 ******************************************************************************/

/*******************************************************************************
 ** Functions
 ******************************************************************************/
void camEvtCallback(void *data)
{
    CAMERA_DATA_S *pdata = (CAMERA_DATA_S *)data;
    LogPrintf("camEvtCallback:eventID=%d", pdata->eventID);
    static uint8 count = 0;
    switch(pdata->eventID)
    {
        case CAMERA_EVENT_SCANCODE_START:
            LogPrintf("CAMERA_EVENT_SCANCODE_START");
            break;

        case CAMERA_EVENT_SCANCODE_RESULT:
        {
            count++;
            LogPrintf("count: %d",count);
            LogPrintf("%s",pdata->data);
            if(count >= 5)
            {
                KING_CameraStop();
                KING_LcdDeinit(0);
                KING_LcdScreenOff(0);
                KING_CameraDeinit();
            }
        }
            break;

        case CAMERA_EVENT_SCANCODE_STOP:
            LogPrintf("CAMERA_EVENT_SCANCODE_STOP");
            break;

        case CAMERA_EVENT_CAPTURE:
            LogPrintf("CAMERA_EVENT_CAPTURE");
            break;

        default:
            break;
    }
}

/**
 *  Camera ≤‚ ‘
 *  …®√Ë‘§¿¿
 */  
void Camera_Test(void)
{
    int ret;
    uint32 value;
    ret = KING_CameraInit(NULL,0);
    if(ret < 0)
    {
        LogPrintf("KING_CameraInit() errcode=0x%x", KING_GetLastErrCode());
        return;
    }
    
    KING_Camera_ParameterGet(CAMERA_PARA_PREVIEW_ON_LCD,&value);
    LogPrintf("KING_Camera_ParameterGet LCD value= %d", value);
    ret = KING_Camera_ParameterSet(CAMERA_PARA_PREVIEW_ON_LCD,1);
    if(ret < 0)
    {
        LogPrintf("KING_Camera_ParameterSet() errcode=0x%x", KING_GetLastErrCode());
        return;
    }
    KING_Camera_ParameterGet(CAMERA_PARA_PREVIEW_ON_LCD,&value);
    LogPrintf("KING_Camera_ParameterGet LCD value= %d", value);

    ret = KING_LcdInit(0, NULL);
    if(ret < 0)
    {
        LogPrintf("KING_LcdInit() errcode=0x%x", KING_GetLastErrCode());
        return;
    }
    KING_LcdScreenOn(0);

    //…®√Ë“ª¥Œµ◊≤„◊‘∂ØæÕÕ£÷π:CAMERA_ONE_SHOT                 “ª÷±…®√Ë:CAMERA_CONSECUTIVELY
    ret = KING_CameraStart(CAMERA_CONSECUTIVELY,CAMERA_CODE_AUTO,camEvtCallback);
    if(ret < 0)
    {
        LogPrintf("KING_CameraStart() errcode=0x%x", KING_GetLastErrCode());
        return;
    }
    KING_Sleep(10000);
    KING_CameraStop();

    ret = KING_CameraCapture(camEvtCallback);
    if (ret < 0)
    {
        LogPrintf("KING_CameraCapture() errcode=0x%x", KING_GetLastErrCode());
        return;
    }

}


