#!/usr/bin/perl

#******************************************************************************
#** File Name:      cz_getlabel.pl                                    *
#** Author:         duam                                               *
#** DATE:           08/19/2015                                                *
#** Copyright:      2015 Xiamen Cheerzing IOT Technology Co., Ltd.            *
#**                 All Rights Reserved.                                      *
#******************************************************************************

$NUM_ARGV = @ARGV;

if($NUM_ARGV == 1)
{
	$input_file = $ARGV[0];
}
else
{
	return 1;
}

#ifndef __VERSION_CHEERZING_H__
#define __VERSION_CHEERZING_H__
#define CZ_VENDOR   "CHEERZING"
#define CZ_PRODUCT_MODEL "ModemApp"
#define CZ_PROJECT_NAME "ModemApp"
#define CZ_SW_MAJOR 1
#define CZ_SW_MINOR 0
#define CZ_SW_BUILDNUMBER 14
#define CZ_SVN_REV 633
#define CZ_BUILD_YEAR 20
#define CZ_BUILD_MONTH 7
#define CZ_BUILD_DAY 27
#define CZ_BUILD_HOUR 9
#define CZ_HW_MAJOR 1
#define CZ_HW_MINOR 0
#define CZ_BUILD_FLAG "R"
#endif //__VERSION_CHEERZING_H__

$MANUFACTURE_IND = 0;
$PRODUCT_MODEL_IND = 1;
$PRODUCT_PROJECT_IND = 2;
$SW_MAJOR_IND = 3;
$SW_MINOR_IND = 4;
$SW_BUILDID_IND = 5;

$MAX_IND = $SW_BUILDID_IND + 1;
$VERSION_STRING[$MAX_IND] = 0;

open (inf,$input_file) || die ("file error!!!");
@alllines = <inf>;
close (inf);
$cnt = 0;
foreach $myline (@alllines)
{
	$_=$myline;
	chomp;
	$_=~s/^\s+|\s+$//g;

	if($_ =~ m/(\#define) (.*) (.*)/)
	{	
		$tmp=$3;
		#print $tmp."\r\n";
		$tmp=~s/"//g;
		$VERSION_STRING[$cnt] = $tmp;
		$cnt++;
		if ($cnt == $MAX_IND)
		{
			last;
		}
	} 
}

$ver_str = "".$VERSION_STRING[$MANUFACTURE_IND]." ".$VERSION_STRING[$PRODUCT_MODEL_IND]." ".$VERSION_STRING[$SW_MAJOR_IND]." ".$VERSION_STRING[$SW_MINOR_IND]." ".$VERSION_STRING[$SW_BUILDID_IND];

print $ver_str;
