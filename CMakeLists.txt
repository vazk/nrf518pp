CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(NRFTest)
SET(CMAKE_SYSTEM_NAME Generic)

###################################################
#### SETTING UP CMAKE FOR CROSS-COMPILATION... ####
###################################################

# which compilers to use for C and C++ and the rest
SET(CROSS_COMPILE /opt/gcc-arm-none-eabi-4_8-2014q2)
SET(GNU_PREFIX arm-none-eabi)
SET(CMAKE_C_COMPILER   ${CROSS_COMPILE}/bin/${GNU_PREFIX}-gcc)
SET(CMAKE_CXX_COMPILER ${CROSS_COMPILE}/bin/${GNU_PREFIX}-g++)
SET(CMAKE_AS           ${CROSS_COMPILE}/bin/${GNU_PREFIX}-as)
SET(CMAKE_AR           ${CROSS_COMPILE}/bin/${GNU_PREFIX}-ar)
SET(CMAKE_LD           ${CROSS_COMPILE}/bin/${GNU_PREFIX}-ld)
SET(CMAKE_NM           ${CROSS_COMPILE}/bin/${GNU_PREFIX}-nm)
SET(CMAKE_OBJDUMP      ${CROSS_COMPILE}/bin/${GNU_PREFIX}-objdump)
SET(CMAKE_OBJCOPY      ${CROSS_COMPILE}/bin/${GNU_PREFIX}-objcopy)
ENABLE_LANGUAGE(ASM)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH /opt/gcc-arm-none-eabi-4_8-2014q2)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(SDK_PATH /opt/nrf51sdk/nrf51822/)
SET(SDK_SOURCE_PATH ${SDK_PATH}/Source/)
SET(TEMPLATE_PATH   ${SDK_SOURCE_PATH}/templates/)

INCLUDE_DIRECTORIES(. .. 
                    ${SDK_PATH}/Include
                    ${SDK_PATH}/Include/gcc
                    ${SDK_PATH}/Include/ble
                    ${SDK_PATH}/Include/ble/rpc
                    ${SDK_PATH}/Include/ble/ble_services
                    ${SDK_PATH}/Include/s110
                    ${SDK_PATH}/Include/app_common
                    ${SDK_PATH}/Include/sd_common
                    ${SDK_PATH}/Include/ext_sensors)

# need these to avoid getting -rdynamic error during build...
SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

# Assembler flags
#ASMFLAGS += -x assembler-with-cpp

SET(CPU cortex-m0)
SET(DEVICE NRF51)
SET(DEVICESERIES nrf51)
SET(TARGET_CHIP  NRF51822_QFAA_CA)
SET(DEVICE_VARIANT xxaa)
SET(LINKER_SCRIPT gcc_${DEVICESERIES}_s110_${DEVICE_VARIANT}.ld)

SET(CMAKE_C_FLAGS "-mfloat-abi=soft -mcpu=${CPU} -mthumb -mabi=aapcs -D${DEVICE} -D${TARGET_CHIP} -DDEBUG_NRF_USER -DBLE_STACK_SUPPORT_REQD --std=gnu99")
SET(CMAKE_CXX_FLAGS "-mfloat-abi=soft -mcpu=${CPU} -mthumb -mabi=aapcs -D${DEVICE} -D${TARGET_CHIP} -fno-rtti -fno-exceptions -DDEBUG_NRF_USER -DBLE_STACK_SUPPORT_REQD -DSVCALL_AS_NORMAL_FUNCTION")

LINK_DIRECTORIES(${CROSS_COMPILE}/arm-none-eabi/lib/armv6-m
                 ${CROSS_COMPILE}/lib/gcc/arm-none-eabi/armv6-m
                 ${TEMPLATE_PATH}
                 ${TEMPLATE_PATH}/gcc)


SET(ASM_OPTIONS "-x assembler-with-cpp")

SET(CMAKE_EXE_LINKER_FLAGS "-Xlinker -Map=binary.map -mcpu=${CPU} -mthumb -mabi=aapcs -T${TEMPLATE_PATH}/gcc/${LINKER_SCRIPT} --specs=nano.specs ")







###################################################
#### PREPARING THE LIBRARY                     ####
###################################################
ADD_SUBDIRECTORY(nrfpp)

###################################################
#### SETTING UP THE PROJECT                    ####
###################################################

SET(OUTPUT_FILENAME  nrfp_test)

SET(PROJECT_SOURCES  main.cpp)
SET(NRF_ASM_SOURCES  ${TEMPLATE_PATH}/gcc/gcc_startup_${DEVICESERIES}.s)



ADD_EXECUTABLE(${OUTPUT_FILENAME}.elf ${PROJECT_SOURCES} ${NRF_SOURCES} ${NRF_ASM_SOURCES} )
TARGET_LINK_LIBRARIES(${OUTPUT_FILENAME}.elf nrfpp)

ADD_CUSTOM_COMMAND(OUTPUT ${OUTPUT_FILENAME}.bin
    COMMAND ${CMAKE_OBJCOPY} --output-format=binary ${OUTPUT_FILENAME}.elf ${OUTPUT_FILENAME}.bin
    DEPENDS ${OUTPUT_FILENAME}.elf
    COMMENT "generating ${OUTPUT_FILENAME}.bin")
ADD_CUSTOM_TARGET(bin ALL DEPENDS ${OUTPUT_FILENAME}.bin)

SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${OUTPUT_FILENAME}.bin)

