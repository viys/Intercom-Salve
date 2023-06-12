
ifeq "${DEVICE}" "bootApp"
PACK_JSON_PATH = ${TOPDIR}prebuilts/${PLATFORM_NAME}/bootapp_image_flash.json
KING_SIGN_NAME = BOOTAPP
BUILD_VERSION_STRING ?= $(shell perl ${TOPDIR}make/make_cmd/cz_getlabel.pl ${TOPDIR}version/bootapp_version.h)
else
PACK_JSON_PATH = ${TOPDIR}prebuilts/${PLATFORM_NAME}/appimg_flash.json
KING_SIGN_NAME = WSOPAPP
BUILD_VERSION_STRING ?= $(shell perl ${TOPDIR}make/make_cmd/cz_getlabel.pl ${TOPDIR}version/app_version.h)
endif
PACGEN := ${TOPDIR}prebuilts/win32/common/pacgen.py

#$(warning $(BUILD_VERSION_STRING))

export KING_PRODUCT_VENDOR ?= $(word 1, $(BUILD_VERSION_STRING))
#$(warning KING_PRODUCT_VENDOR = $(KING_PRODUCT_VENDOR))

export KING_PRODUCT_MODEL ?= $(word 2, $(BUILD_VERSION_STRING))
#$(warning KING_PRODUCT_MODEL = $(KING_PRODUCT_MODEL))

export KING_MAJOR_VER ?= $(word 3, $(BUILD_VERSION_STRING))
#$(warning KING_MAJOR_VER = $(KING_MAJOR_VER))

export KING_MINOR_VER ?= $(word 4, $(BUILD_VERSION_STRING))
#$(warning KING_MINOR_VER = $(KING_MINOR_VER))

export KING_BUILD_ID ?= $(word 5, $(BUILD_VERSION_STRING))
#$(warning KING_BUILD_ID = $(KING_BUILD_ID))


COMMON_GLOBAL_AFLAGS += -mcpu=cortex-m33 -mtune=cortex-m33 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard
COMMON_GLOBAL_CFLAGS += -std=gnu11 -mcpu=cortex-m33 -mtune=cortex-m33 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mno-unaligned-access -g -Os -Wall -ffunction-sections -fdata-sections
COMMON_GLOBAL_CFLAGS += -nostdlib
COMMON_GLOBAL_CFLAGS += -DCPL_GCC
ifneq (,$(findstring nostdlib,${COMMON_GLOBAL_CFLAGS}))
COMMON_GLOBAL_CFLAGS += -DUSE_KING_SDK_STD_API
endif

COMMON_GLOBAL_CFLAGS += -DPLATFORM_UIS8811CD

MAKEFLAGS += -s --no-print-directory

COMMON_LINKER_LFLAGS += $(foreach modlib,$(MODULES),-l$(modlib))
COMMON_LINKER_LFLAGS += -lgcc
ifeq "${BUILD_OPERATOR}" "CUSTOMER_BUILD"
COMMON_LINKER_LFLAGS += -lsystem
endif

LINKER_FLAGS += -L${TOPDIR}Build/$(PLATFORM_NAME)/${DEVICE}/${TARGET_BUILD_TYPE}/lib
LINKER_FLAGS += -L${TOPDIR}SysLib/$(PLATFORM_NAME)/${TARGET_BUILD_TYPE}
LINKER_FLAGS += -L${TOPDIR}prebuilts/win32/UIS8811CD/gcc-arm-none-eabi/lib/gcc/arm-none-eabi/9.2.1/thumb/v8-m.main+fp/hard
LINKER_FLAGS += -L${TOPDIR}Third-party/lib
LINKER_FLAGS += --start-group ${COMMON_LINKER_LFLAGS} --end-group

COMMON_LIB_PACKER_FLAGS := cru

OFORMAT 	:= --oformat=elf32-littlearm
BINARY		:= --input-target=elf32-littlearm --output-target=binary

export COMMON_GLOBAL_AFLAGS COMMON_GLOBAL_CFLAGS COMMON_LINKER_LFLAGS COMMON_LIB_PACKER_FLAGS LINKER_FLAGS



