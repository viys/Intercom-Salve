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

#ifndef _BOOT_RTC_H_
#define _BOOT_RTC_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 自定义的时间结构体 */
typedef struct
{
    unsigned short nYear;
    unsigned char nMonth;
    unsigned char nDay;
    unsigned char nHour;
    unsigned char nMin;
    unsigned char nSec;
    unsigned char DayIndex; /* 0 = Sunday */
} mytime_t;


/*
 * 功能：
 *     获取系统本地时间/0时区
 * 参数：
 *     mytime: 本地时间
 *     daylightSaving：是否是夏令时
 *
 * 返回值：
 *     无
 */
void bootReadRtcTime(mytime_t* mytime,bool daylightSaving);


#ifdef __cplusplus
}
#endif
#endif
