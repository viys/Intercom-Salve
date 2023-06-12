##############################################################################
## General Targets
##############################################################################

###################################################################################
# Linker script generation
###################################################################################
LD_SRC := ${TOPDIR}Devices/${DEVICE}/${PLATFORM_NAME}/${DEVICE}.ld

NBR :=
PRODUCT :=
HEX_ROOT := ${TOPDIR}Build/${PLATFORM_NAME}/${DEVICE}/${TARGET_BUILD_TYPE}/out
# Build the base pathname of the generated lodfile.
LODBASE_NO_PATH := ${NBR}${PLATFORM_NAME}_${PRODUCT}${TARGET_BUILD_TYPE}
LODBASE_NO_PATH := $(notdir ${DEVICE})_${LODBASE_NO_PATH}

# Path for storing all the generated files for one test (elf, dis, lod...).
BINARY_PATH := ${HEX_ROOT}
BAS_FINAL := ${BINARY_PATH}/${LODBASE_NO_PATH}
BIN_FINAL := ${BAS_FINAL}.elf
MAP_FINAL := ${BAS_FINAL}.map
BINF_FINAL :=${BAS_FINAL}.bin
IMG_FINAL :=${BAS_FINAL}.img
LD_FILE := ${BAS_FINAL}.ld
KEY_FILE := ${TOPDIR}make/make_cmd/key.bin
SELOADER := ${TOPDIR}prebuilts/HI2120/seloader_hisi_sign_enc.bin

########################## Directories #######################################

DOS_BINARY_PATH:=$(subst /,\,$(BINARY_PATH))
${BINARY_PATH}:
	@if not exist $(DOS_BINARY_PATH) mkdir $(DOS_BINARY_PATH)

########################## Default Target ###################################
.PHONY: force
force: ;

${BIN_FINAL}: ${BINARY_PATH}
	cp -f ${LD_SRC} ${LD_FILE}
	@${ECHO} "Linking ..."
	$(LINKER) -o ${BIN_FINAL} ${LINKER_FLAGS}
############################ hex files #######################################
# Generate srec and disassembly files, same logic as bin
# Always build APPBIN
APPBIN: force
	${MAKE} ${BIN_FINAL}
	@${ECHO} ""
	$(OBJCOPY) --gap-fill 0xFF -O binary ${BIN_FINAL} ${BINF_FINAL}
	${SIGN} ${BINF_FINAL} ${KING_PRODUCT_MODEL} ${KING_SIGN_NAME} ${KING_MAJOR_VER} ${KING_MINOR_VER} ${KING_BUILD_ID}
	cp -f ${BAS_FINAL}.img ${TOPDIR}prebuilts/${PLATFORM_NAME}/HI2120_release.img
	$(PYTHON) $(PACGEN) -packet "${BAS_FINAL}.fwpkg" "${SELOADER}|0|0|0" "${IMG_FINAL}|1675264|131072|12" -version:UNKNOWN
	del .\prebuilts\${PLATFORM_NAME}\HI2120_release.img
################### "all" target #############################################
# Needs to come after everything else has been defined...
.PHONY: all
all: APPBIN


