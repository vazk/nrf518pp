SET(OUTPUT_FILENAME  test_buzzer)

SET(PROJECT_SOURCES  main.cpp)
SET(NRF_ASM_SOURCES  ${TEMPLATE_PATH}/gcc/gcc_startup_${DEVICESERIES}.s)



ADD_EXECUTABLE(${OUTPUT_FILENAME}.elf ${PROJECT_SOURCES} ${NRF_SOURCES} ${NRF_ASM_SOURCES} )
TARGET_LINK_LIBRARIES(${OUTPUT_FILENAME}.elf nrfpp)

ADD_CUSTOM_COMMAND(OUTPUT ${OUTPUT_FILENAME}.bin
    COMMAND ${CMAKE_OBJCOPY} --output-format=binary ${OUTPUT_FILENAME}.elf ${OUTPUT_FILENAME}.bin
    DEPENDS ${OUTPUT_FILENAME}.elf
    COMMENT "generating ${OUTPUT_FILENAME}.bin")
ADD_CUSTOM_TARGET(${OUTPUT_FILENAME}-bin ALL DEPENDS ${OUTPUT_FILENAME}.bin)

SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${OUTPUT_FILENAME}.bin)

