ifeq "${DEVICE}" "bootApp"
PACK_JSON_PATH = ${TOPDIR}prebuilts/${PLATFORM_NAME}/bootapp_image_flash.json
KING_SIGN_NAME = BOOTAPP
BUILD_VERSION_STRING ?= $(shell perl ${TOPDIR}make/make_cmd/cz_getlabel.pl ${TOPDIR}version/bootapp_version.h)
else
PACK_JSON_PATH = ${TOPDIR}prebuilts/${PLATFORM_NAME}/appimg_flash.json
KING_SIGN_NAME = WSOPAPP
BUILD_VERSION_STRING ?= $(shell perl ${TOPDIR}make/make_cmd/cz_getlabel.pl ${TOPDIR}version/app_version.h)
endif
PACGEN := ${TOPDIR}prebuilts/win32/HI2120/packet_create.py

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


COMMON_GLOBAL_CFLAGS = -nostdlib
ifneq (,$(findstring nostdlib,${COMMON_GLOBAL_CFLAGS}))
COMMON_GLOBAL_CFLAGS += -DUSE_KING_SDK_STD_API
endif


COMMON_GLOBAL_CFLAGS += -std=gnu99
COMMON_GLOBAL_CFLAGS += -Wall
COMMON_GLOBAL_CFLAGS += -Werror
COMMON_GLOBAL_CFLAGS += -Wextra
COMMON_GLOBAL_CFLAGS += -Winit-self
COMMON_GLOBAL_CFLAGS += -Wmissing-include-dirs
COMMON_GLOBAL_CFLAGS += -Wtrampolines
COMMON_GLOBAL_CFLAGS += -Wundef
COMMON_GLOBAL_CFLAGS += -Wpointer-arith
COMMON_GLOBAL_CFLAGS += -Wlogical-op
COMMON_GLOBAL_CFLAGS += -Wstrict-prototypes
COMMON_GLOBAL_CFLAGS += -Wjump-misses-init
COMMON_GLOBAL_CFLAGS += -Wno-missing-include-dirs
COMMON_GLOBAL_CFLAGS += -fno-strict-aliasing
COMMON_GLOBAL_CFLAGS += -ffreestanding
COMMON_GLOBAL_CFLAGS += -fdata-sections
COMMON_GLOBAL_CFLAGS += -ffunction-sections
COMMON_GLOBAL_CFLAGS += --short-enums
COMMON_GLOBAL_CFLAGS += -nostdinc
COMMON_GLOBAL_CFLAGS += -nostdlib
COMMON_GLOBAL_CFLAGS += -fno-exceptions
COMMON_GLOBAL_CFLAGS += -Wpointer-arith
COMMON_GLOBAL_CFLAGS += -Wstrict-prototypes
COMMON_GLOBAL_CFLAGS += -Wno-write-strings
COMMON_GLOBAL_CFLAGS += -Wimplicit-fallthrough=2
COMMON_GLOBAL_CFLAGS += -Wno-unused-parameter
COMMON_GLOBAL_CFLAGS += -Wno-attributes
COMMON_GLOBAL_CFLAGS += -Wno-cast-function-type
COMMON_GLOBAL_CFLAGS += -fno-common
COMMON_GLOBAL_CFLAGS += -fno-stack-protector
COMMON_GLOBAL_CFLAGS += -Wa,-enable-c-lbu-sb
COMMON_GLOBAL_CFLAGS += -Wa,-enable-c-lhu-sh
COMMON_GLOBAL_CFLAGS += -mpush-pop
COMMON_GLOBAL_CFLAGS += -msmall-data-limit=0
COMMON_GLOBAL_CFLAGS += -fimm-compare
COMMON_GLOBAL_CFLAGS += -femit-muliadd
COMMON_GLOBAL_CFLAGS += -fmerge-immshf
COMMON_GLOBAL_CFLAGS += -femit-uxtb-uxth
COMMON_GLOBAL_CFLAGS += -femit-lli
COMMON_GLOBAL_CFLAGS += -flto
COMMON_GLOBAL_CFLAGS += -mabi=ilp32
COMMON_GLOBAL_CFLAGS += -march=rv32imc
COMMON_GLOBAL_CFLAGS += -falign-functions=2
COMMON_GLOBAL_CFLAGS += -DCPL_GCC
ifneq (,$(findstring nostdlib,${COMMON_GLOBAL_CFLAGS}))
COMMON_GLOBAL_CFLAGS += -DUSE_KING_SDK_STD_API
endif
COMMON_GLOBAL_CFLAGS += -DPLATFORM_HI2120

MAKEFLAGS = -s --no-print-directory

COMMON_LIB_PACKER_FLAGS = rcT

COMMON_LINKER_LFLAGS = -Wl,--enjal16
COMMON_LINKER_LFLAGS += -nostartfiles
COMMON_LINKER_LFLAGS += -flto
COMMON_LINKER_LFLAGS += -Wl,--gc-section,--cref,--relax,-M
COMMON_LINKER_LFLAGS += -Wl,-Map=${MAP_FINAL}
COMMON_LINKER_LFLAGS += -Wl,-T${LD_FILE}
COMMON_LINKER_LFLAGS += -mabi=ilp32
COMMON_LINKER_LFLAGS += -march=rv32imc
COMMON_LINKER_LFLAGS += -falign-functions=2

COMMON_LINKER_LFLAGS += $(foreach modlib,$(MODULES),-l$(modlib))
COMMON_LINKER_LFLAGS += -lgcc
ifeq "${BUILD_OPERATOR}" "CUSTOMER_BUILD"
COMMON_LINKER_LFLAGS += -lsystem
endif

LINKER_FLAGS = -L${TOPDIR}Build/$(PLATFORM_NAME)/${DEVICE}/${TARGET_BUILD_TYPE}/lib
LINKER_FLAGS += -L${TOPDIR}SysLib/$(PLATFORM_NAME)/${TARGET_BUILD_TYPE}
LINKER_FLAGS += -L${TOPDIR}prebuilts/win32/HI2120/hcc_riscv32_win/lib/gcc/riscv32-unknown-elf/7.3.0
LINKER_FLAGS += -L${TOPDIR}Third-party/lib

LINKER_FLAGS += -Wl,--start-group ${COMMON_LINKER_LFLAGS} -Wl,--end-group

export COMMON_GLOBAL_AFLAGS COMMON_GLOBAL_CFLAGS MAKEFLAGS COMMON_LIB_PACKER_FLAGS COMMON_LINKER_LFLAGS   



