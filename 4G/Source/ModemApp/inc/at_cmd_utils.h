/*******************************************************************************
 ** File Name:   at_cmd_utils.h
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
 ** 2020-06-10    Allance.Chen         Create.
 ******************************************************************************/
#ifndef __AT_CMD_UTILS_H__
#define __AT_CMD_UTILS_H__

/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "kingdef.h"

/*******************************************************************************
  ** Function Delcarations
 ******************************************************************************/
bool atUtils_ConvertHexToBin
(    // FALSE: There is invalid char
    char   *hex_ptr,     // in: the hexadecimal format string
    uint16 length,       // in: the length of hexadecimal string
    char   *bin_ptr      // out: pointer to the binary format string
);


void atUtils_ConvertBinToHex
(
    uint8 *bin_ptr,     // in: the binary format string
    uint16 length,       // in: the length of binary string
    uint8 *hex_ptr,      // out: pointer to the hexadecimal format string
    uint8 charcase      // in: char case  0 -- big case; 1 -- little case.
);

#endif /*__AT_CMD_UTILS_H__*/