Rem Set Build Envirement

set PLATFORM_NAME=UIS8910DM
set COMPILER=GCC
set COMPILER_DIR=%CD%

set CPP_COMPILER="%COMPILER_DIR%\prebuilts\win32\UIS8910DM\gcc-arm-none-eabi\bin\arm-none-eabi-gcc.exe"
set ASM_COMPILER="%COMPILER_DIR%\prebuilts\win32\UIS8910DM\gcc-arm-none-eabi\bin\arm-none-eabi-as.exe"
set LINKER="%COMPILER_DIR%\prebuilts\win32\UIS8910DM\gcc-arm-none-eabi\bin\arm-none-eabi-ld.exe"
set LIB_PACKER="%COMPILER_DIR%\prebuilts\win32\UIS8910DM\gcc-arm-none-eabi\bin\arm-none-eabi-ar.exe"
set OBJCOPY="%COMPILER_DIR%\prebuilts\win32\UIS8910DM\gcc-arm-none-eabi\bin\arm-none-eabi-objcopy.exe"
set LDPP="%COMPILER_DIR%\prebuilts\win32\UIS8910DM\gcc-arm-none-eabi\bin\arm-none-eabi-cpp.exe"
set PYTHON="%COMPILER_DIR%\prebuilts\win32\common\python3\python3.exe"

set PATH=.\make\make_cmd;%PATH%

