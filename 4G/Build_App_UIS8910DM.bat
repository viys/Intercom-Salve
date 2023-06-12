@Echo Off
set CZ_START_TIME=%time%

set PLATFORM_NAME=UIS8910DM
set DEVICE=App
set BUILD_OPERATOR=CUSTOMER_BUILD
set TARGET_BUILD_TYPE=release

if "%1" == "release" (
    set TARGET_BUILD_TYPE=release
) else if "%1" == "debug" (
    set TARGET_BUILD_TYPE=debug
)

RD /S /Q .\build\%PLATFORM_NAME%\%DEVICE%\%TARGET_BUILD_TYPE%

call SetEnv_%PLATFORM_NAME%.bat

make 2>&1 | tee %DEVICE%_%PLATFORM_NAME%_make.log

set CZ_END_TIME=%time%
echo.
echo "Start building at: %CZ_START_TIME%"
echo "End building at: %CZ_END_TIME%"

