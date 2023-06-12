#include "KingDef.h"
#include "KingCSTD.h"
#include "king_lwm2m.h"


bool get_system_imei(uint8 *imei)
{
    if ( imei == NULL )
        return FALSE;

    return KING_GetSysImei(imei);
}

char* get_end_of_arg(char* buffer)
{
    while (buffer[0] != 0 && !isspace(buffer[0]&0xFF))
    {
        buffer++;
    }
    return buffer;
}


// =============================================================================
// paramHex2Num ([0-9a-f] -> number)
// =============================================================================
static int paramHex2Num(uint8 c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return 0xa + (c - 'A');
    if (c >= 'a' && c <= 'f')
        return 0xa + (c - 'a');
    return -1;
}

// =============================================================================
// at_ParamHexDataParse
// =============================================================================
bool paramHexDataParse(char *param, int length)
{
    const uint8 *pv = param;
    uint32 n = 0;

    for (n = 0; n < length; n++)
    {
        if (paramHex2Num(*pv++) < 0)
            return FALSE;
    }

    uint8 *s = param;
    uint8 *d = param;
    length /= 2;
    for (n = 0; n < length; n++)
    {
        int num0 = paramHex2Num(s[0]);
        int num1 = paramHex2Num(s[1]);
        *d++ = (num0 << 4) | num1;
        s += 2;
    }
    return TRUE;
}

