@echo off
if "" =="%1" goto usage
if "" =="%2" goto usage
if "" =="%3" goto usage
if "" =="%4" goto usage
if "" =="%5" goto usage
if "" =="%6" goto usage
if "" =="%7" goto usage

set modal=%1
set sw_major=%2
set sw_minor=%3
set sw_buildid=%4
set old=%5
set new=%6
set pack=%7

.\fota-win\dtools.exe fotacreate2 --pac %old%,%new%,fota8910_king.xml %pack%

..\..\make\make_cmd\KingSign.exe %pack% %modal% 8910DIFF %sw_major% %sw_minor% %sw_buildid%

goto end

:usage
	echo usage: CreateKingFotaPack.bat [modal] [sw_major] [sw_minor] [sw_buildid] [pac_old] [pack_new] [diff_pack_name]
	echo     modal - modal name
	echo     sw_major - major version of old firmware
	echo     sw_minor - minor version of old firmware
	echo     sw_buildid - build id of old firmware
	echo     pac_old - old firmware
	echo     pac_new - new firmware
	echo     diff_pack_name - different package file name

:end