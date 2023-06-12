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
LD_FILE := ${BAS_FINAL}.ld
KEY_FILE := ${TOPDIR}make/make_cmd/key.bin

########################## Directories #######################################

DOS_BINARY_PATH:=$(subst /,\,$(BINARY_PATH))
${BINARY_PATH}:
	@if not exist $(DOS_BINARY_PATH) mkdir $(DOS_BINARY_PATH)

########################## Default Target ###################################
.PHONY: force
force: ;

${BIN_FINAL}: ${BINARY_PATH}
	cp -f ${LD_SRC} ${LD_FILE}
	$(LINKER) -o ${BIN_FINAL} ${OFORMAT} ${FULL_SRC_OBJECTS} \
		${LDFLAG_USED_ELF_FILES}	\
		--script ${LD_FILE} \
		$(LINKER_FLAGS) \
		-Map ${MAP_FINAL}

############################ hex files #######################################
# Generate srec and disassembly files, same logic as bin
# Always build APPBIN
APPBIN: force
	${MAKE} ${BIN_FINAL}
	@${ECHO} ""
	$(OBJCOPY) ${OBJCPY_OPTS} ${BINARY} ${BIN_FINAL} ${BINF_FINAL}
	${SIGN} ${BINF_FINAL} ${KING_PRODUCT_MODEL} ${KING_SIGN_NAME} ${KING_MAJOR_VER} ${KING_MINOR_VER} ${KING_BUILD_ID}
	cp -f ${BAS_FINAL}.img ${TOPDIR}prebuilts/${PLATFORM_NAME}/UIS8811CD_release.img
	$(PYTHON) $(PACGEN) kingapp-gen --cfg ${PACK_JSON_PATH} ${BAS_FINAL}.pac
	del .\prebuilts\${PLATFORM_NAME}\UIS8811CD_release.img

	cp -f ${TOPDIR}prebuilts/delete.img ${TOPDIR}prebuilts/${PLATFORM_NAME}/UIS8811CD_release.img
	$(PYTHON) $(PACGEN) kingapp-gen --cfg ${PACK_JSON_PATH} ${BAS_FINAL}_delete.pac
	del .\prebuilts\${PLATFORM_NAME}\UIS8811CD_release.img
################### "all" target #############################################
# Needs to come after everything else has been defined...
.PHONY: all
all: APPBIN


