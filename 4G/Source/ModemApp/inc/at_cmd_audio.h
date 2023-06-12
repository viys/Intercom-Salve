#ifndef __AT_CMD_AUDIO_H__
#define __AT_CMD_AUDIO_H__


/*******************************************************************************
 ** Macro Definition
 ******************************************************************************/

/*******************************************************************************
 ** Extern Variables
 ******************************************************************************/

/*******************************************************************************
 ** Struct
 ******************************************************************************/
typedef struct 
{
    UINT8 filenum;
    char **file;
} AudPlayParserMsg;

typedef struct
{
    char Name[16];
    UINT32 Offset;
    UINT32 Size;
} INFO_TABLE;

typedef enum
{
    MSG_ID_PLAY_START,
    MSG_ID_PLAY_STOP
}PLAY_MSG_ID;

typedef struct
{
  UINT32 size;              // Total Size of the buffer.
  UINT32 inPoint;    // First data in the buffer.
  UINT32 outPoint;   // Last data in the buffer.
  CHAR *bufPtr;            // buffer address.
  UINT32 count;              // Data Size in the buffer.  
  MUTEX_HANDLE mutex;
} CiAtRingBuf;

#endif
