
SET(OUTPUT_FILENAME  nrfpp)

SET(PROJECT_SOURCES  wrappers.c
                     syscalls.c
                     pwm.cpp
                     buzzer.cpp
                     uart.cpp
                     ble_peripheral_device.cpp
                     ble_application.cpp
                     ble_service.cpp
                     ble_characteristic.cpp)
SET(NRF_SOURCES      ${TEMPLATE_PATH}/system_${DEVICESERIES}.c
                     ${SDK_SOURCE_PATH}/nrf_delay/nrf_delay.c
                     ${SDK_SOURCE_PATH}/app_common/app_timer.c
                     ${SDK_SOURCE_PATH}/app_common/app_scheduler.c
                     ${SDK_SOURCE_PATH}/ble/ble_sensorsim.c
                     ${SDK_SOURCE_PATH}/ble/ble_advdata.c
                     ${SDK_SOURCE_PATH}/ble/ble_conn_params.c
                     ${SDK_SOURCE_PATH}/ble/ble_services/ble_bas.c
                     ${SDK_SOURCE_PATH}/ble/ble_services/ble_srv_common.c
                     ${SDK_SOURCE_PATH}/sd_common/softdevice_handler.c
                    )


ADD_LIBRARY(${OUTPUT_FILENAME} STATIC ${NRF_SOURCES} ${PROJECT_SOURCES} ${NRF_ASM_SOURCES} )
