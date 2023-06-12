@echo off
if "" =="%1" goto usage
if "" =="%2" goto usage
if "" =="%3" goto usage

set base_pac=%1
set app_pac=%2
set merged_pac=%3
set temp_pac=temp.pac

echo merge application...
.\fota-win\dtools.exe pacmerge --id APPIMG,PS %base_pac% %app_pac% %temp_pac%

echo merge resource...
.\fota-win\dtools.exe pacmerge --id PRESET,PREPACK %temp_pac% %app_pac% %merged_pac%

echo finish!
del %temp_pac%

goto end

:usage
	echo usage: MergeAppAndBase.bat [base_pac] [app_pac] [merged_pac] 
	echo     base_pac - base package
	echo     app_pac - app package
	echo     merged_pac - merged packge
:end