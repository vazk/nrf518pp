#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <vector>
#include <set>
#include <map>

extern "C" {
#include "simple_uart.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "ble_sensorsim.h"
}


#include "nrfpp/ble_peripheral_device.hpp"
#include "nrfpp/ble_service.hpp"
#include "nrfpp/ble_characteristic.hpp"
#include "nrfpp/timer.hpp"

#define PIN_0 0
#define PIN_3 3
#define PIN_5 5
#define PIN_30 30

class LEDSwitcher 
{
public:
    LEDSwitcher() {}
    static void on_timer(void* context) {
        static bool off = true;
        if(off) {
            nrf_gpio_pin_set(PIN_3);
            nrf_gpio_pin_set(PIN_5);
        } else {
            nrf_gpio_pin_clear(PIN_3);
            nrf_gpio_pin_clear(PIN_5);
        }
        off = !off;
    }
};

#define SCHED_MAX_EVENT_DATA_SIZE       sizeof(app_timer_event_t) /**< Maximum size of scheduler events. Note that scheduler BLE stack events do not contain any data, as the events are being pulled from the stack in the event handler. */
#define SCHED_QUEUE_SIZE                10 /**< Maximum number of events in the scheduler queue. */


int main(void)
{

    nrf_gpio_cfg_output(PIN_3);
    nrf_gpio_cfg_output(PIN_5);

    nrfpp::Timer<LEDSwitcher> timer(nrfpp::TIMER_REPEATED);
    bool status = timer.is_good();
    timer.start(800);
    status = timer.is_good();


    //nrf_delay_ms(300);
    //nrfpp::Timer<BASHandler> bas_handler(nrfpp::TIMER_REPEATED);
    //status = bas_handler.is_good();

    nrfpp::BLEPeripheralDevice::Params dp;
    dp.name = "poxosBLE";
    dp.cconf = nrfpp::SYNTH_250;
    dp.adv_interval_ms = 25;
    dp.adv_timeout_s = 180;

    dp.min_conn_interval_ms = 500;
    dp.max_conn_interval_ms = 1000;
    dp.slave_latency_ms = 0;
    dp.conn_supervisory_timeout_ms = 4000;

    nrfpp::BLEPeripheralDevice::SecurityParams sp;
    sp.sec_timeout_s = 30;
    sp.sec_bond = true;
    sp.sec_mitm = false;
    sp.sec_io_capabilities = BLE_GAP_IO_CAPS_NONE;
    sp.sec_oob = false;
    sp.sec_min_key_size = 7;
    sp.sec_max_key_size = 16;

    nrfpp::BLEPeripheralDevice::ConnectionParams cp;
    cp.conn_max_negotiation_attempts = 3;
    cp.conn_first_params_update_delay_ms = 5000;
    cp.conn_next_params_update_delay_ms = 10000;

    nrfpp::BLEPeripheralDevice::init(dp, sp, cp);

    nrfpp::BLEService* bs = new nrfpp::BLEService(0x1523, BLE_UUID_TYPE_BLE);
    nrfpp::BLECharacteristic* ch = new nrfpp::BLECharacteristic(0x1524, "bla",
                                                              false, false, 
                                                              true, false, 
                                                              1, nrfpp::LOC_STACK);
    bs->add_characteristic(ch);
    
    nrfpp::BLEPeripheralDevice::add_service(bs);

    //bas_handler.start(500);
    //status = bas_handler.is_good();

    nrfpp::BLEPeripheralDevice::start_advertising();

    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);

    while(true) {
        nrfpp::BLEPeripheralDevice::idle();
    }

    return 0;
}
