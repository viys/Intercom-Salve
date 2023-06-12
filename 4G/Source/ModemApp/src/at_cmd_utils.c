/*******************************************************************************
 ** File Name:   at_cmd_utils.c
 ** Author:      Allance.Chen
 ** Date:        2020-06-10
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains ds functions about common interface.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-06-10     Allance.Chen         Create.
 ******************************************************************************/
/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "kingdef.h"

/*******************************************************************************
 ** Functions
 ******************************************************************************/
bool atUtils_ConvertHexToBin
(    // FALSE: There is invalid char
    char   *hex_ptr,     // in: the hexadecimal format string
    uint16 length,       // in: the length of hexadecimal string
    char   *bin_ptr      // out: pointer to the binary format string
)
{
    char  *dest_ptr = bin_ptr;
    int32 i = 0;
    uint8 ch;

    if (NULL == hex_ptr || bin_ptr == NULL || (length % 2) != 0)
    {
        return FALSE;
    }

    for(i = 0; i < length; i += 2)
    {
        // the bit 8,7,6,5
        ch = hex_ptr[i];

        // digital 0 - 9
        if(ch >= '0' && ch <= '9')
        {
            *dest_ptr = (uint8)((ch - '0') << 4);
        }
        // a - f
        else if(ch >= 'a' && ch <= 'f')
        {
            *dest_ptr = (uint8)((ch - 'a' + 10) << 4);
        }
        // A - F
        else if(ch >= 'A' && ch <= 'F')
        {
            *dest_ptr = (uint8)((ch - 'A' + 10) << 4);
        }
        else
        {
            return FALSE;
        }

        // the bit 1,2,3,4
        ch = hex_ptr[i+1];

        // digtial 0 - 9
        if(ch >= '0' && ch <= '9')
        {
            *dest_ptr |= (uint8)(ch - '0');
        }
        // a - f
        else if(ch >= 'a' && ch <= 'f')
        {
            *dest_ptr |= (uint8)(ch - 'a' + 10);
        }
        // A - F
        else if(ch >= 'A' && ch <= 'F')
        {
            *dest_ptr |= (uint8)(ch - 'A' + 10);
        }
        else
        {
            return FALSE;
        }

        dest_ptr++;
    }

    return TRUE;
}

void atUtils_ConvertBinToHex
(
    uint8 *bin_ptr,     // in: the binary format string
    uint16 length,       // in: the length of binary string
    uint8 *hex_ptr,      // out: pointer to the hexadecimal format string
    uint8 charcase      // in: char case  0 -- big case; 1 -- little case.
)
{
    uint8 semi_octet;
    int32 i;

    if (NULL == hex_ptr || bin_ptr == NULL || (charcase != 0 && charcase != 1))
    {
        return;
    }

    for(i = 0; i < length; i++)
    {
        // get the high 4 bits
        semi_octet = (uint8)((bin_ptr[i] & 0xF0) >> 4);

        if(semi_octet <= 9)  //semi_octet >= 0
        {
            *hex_ptr = (uint8)(semi_octet + '0');
        }
        else
        {
            if (0 == charcase)
            {
                *hex_ptr = (uint8)(semi_octet + 'A' - 10);
            }
            else
            {
                *hex_ptr = (uint8)(semi_octet + 'a' - 10);
            }
        }

        hex_ptr++;

        // get the low 4 bits
        semi_octet = (uint8)(bin_ptr[i] & 0x0f);

        if(semi_octet <= 9)  // semi_octet >= 0
        {
            *hex_ptr = (uint8)(semi_octet + '0');
        }
        else
        {
            if (0 == charcase)
            {
                *hex_ptr = (uint8)(semi_octet + 'A' - 10);
            }
            else
            {
                *hex_ptr = (uint8)(semi_octet + 'a' - 10);
            }
        }

        hex_ptr++;
    }
}
