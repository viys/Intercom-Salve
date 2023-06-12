
ifeq "${DEVICE}" "bootApp"
PACK_JSON_PATH = ${TOPDIR}prebuilts/$(PLATFORM_NAME)/bootapp_image_flash.json
KING_SIGN_NAME = BOOTAPP
BUILD_VERSION_STRING ?= $(shell perl ${TOPDIR}make/make_cmd/cz_getlabel.pl ${TOPDIR}version/bootapp_version.h)
else ifeq "${DEVICE}" "bootAppVolte"
PACK_JSON_PATH = ${TOPDIR}prebuilts/$(PLATFORM_NAME)/bootapp_image_flash_volte.json
KING_SIGN_NAME = BOOTAPP
BUILD_VERSION_STRING ?= $(shell perl ${TOPDIR}make/make_cmd/cz_getlabel.pl ${TOPDIR}version/bootapp_version.h)
else ifeq ($(strip $(DEVICE)),$(filter $(DEVICE),ModemAppVolte AppVolte))
PACK_JSON_PATH = ${TOPDIR}prebuilts/$(PLATFORM_NAME)/appimg_flash_volte.json
KING_SIGN_NAME = WSOPAPP
BUILD_VERSION_STRING ?= $(shell perl ${TOPDIR}make/make_cmd/cz_getlabel.pl ${TOPDIR}version/app_version.h)
else
PACK_JSON_PATH = ${TOPDIR}prebuilts/$(PLATFORM_NAME)/appimg_flash.json
KING_SIGN_NAME = WSOPAPP
BUILD_VERSION_STRING ?= $(shell perl ${TOPDIR}make/make_cmd/cz_getlabel.pl ${TOPDIR}version/app_version.h)
endif
PACGEN := ${TOPDIR}prebuilts/win32/common/pacgen.py
PRESETGEN := ${TOPDIR}prebuilts/win32/common/preset.py
PRESETCFG := ${TOPDIR}preset/preset.json

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

COMMON_GLOBAL_AFLAGS += -march=armv7-a -mcpu=cortex-a5 -mtune=generic-armv7-a -mthumb -mfpu=neon-vfpv4 -mfloat-abi=hard
COMMON_GLOBAL_CFLAGS += -march=armv7-a -std=gnu11 -mcpu=cortex-a5 -mtune=generic-armv7-a -mthumb -mfpu=neon-vfpv4 -mfloat-abi=hard -mno-unaligned-access -g -Os -Wall -ffunction-sections -fdata-sections
COMMON_GLOBAL_CFLAGS += -nostdlib
COMMON_GLOBAL_CFLAGS += -DCPL_GCC
ifneq (,$(findstring nostdlib,${COMMON_GLOBAL_CFLAGS}))
COMMON_GLOBAL_CFLAGS += -DUSE_KING_SDK_STD_API
endif
MAKEFLAGS += -s --no-print-directory

COMMON_LINKER_LFLAGS += $(foreach modlib,$(MODULES),-l$(modlib))
COMMON_LINKER_LFLAGS += -lgcc
COMMON_LINKER_LFLAGS += -livstts
ifeq "${BUILD_OPERATOR}" "CUSTOMER_BUILD"
COMMON_LINKER_LFLAGS += -lsystem
COMMON_LINKER_LFLAGS += -ltts
endif

LINKER_FLAGS += -L${TOPDIR}Build/$(PLATFORM_NAME)/${DEVICE}/${TARGET_BUILD_TYPE}/lib
LINKER_FLAGS += -L${TOPDIR}SysLib/$(PLATFORM_NAME)/${TARGET_BUILD_TYPE}
LINKER_FLAGS += -L${TOPDIR}prebuilts/win32/UIS8910DM/gcc-arm-none-eabi/lib/gcc/arm-none-eabi/7.2.1/thumb/v7-ar/fpv3/hard
LINKER_FLAGS += -L${TOPDIR}Third-party/lib
LINKER_FLAGS += --start-group ${COMMON_LINKER_LFLAGS} --end-group

COMMON_LIB_PACKER_FLAGS := cru

OFORMAT 	:= --oformat=elf32-littlearm
BINARY		:= --input-target=elf32-littlearm --output-target=binary

export COMMON_GLOBAL_AFLAGS COMMON_GLOBAL_CFLAGS COMMON_LINKER_LFLAGS COMMON_LIB_PACKER_FLAGS LINKER_FLAGS



