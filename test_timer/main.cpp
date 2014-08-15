//#include <stdbool.h>
#include <stdint.h>
//#include <string.h>

extern "C" {
#include "nrf_delay.h"
#include "nrf_gpio.h"
}


#include "nrfpp/ble_app_timer.hpp"
#include "nrfpp/ble_peripheral_device.hpp"

#define PIN_3 3
#define PIN_5 5

class LEDSwitcher 
{
public:
    LEDSwitcher() {}
    static void on_timer(void* context) {
        static bool off = true;
        if(off) {
            nrf_gpio_pin_set(PIN_3);
            nrf_gpio_pin_clear(PIN_5);
        } else {
            nrf_gpio_pin_clear(PIN_3);
            nrf_gpio_pin_set(PIN_5);
        }
        off = !off;
    }
};


// Maximum size of scheduler events. Note that scheduler BLE stack events do not contain 
// any data, as the events are being pulled from the stack in the event handler. 
#define SCHED_MAX_EVENT_DATA_SIZE       sizeof(app_timer_event_t) 
// Maximum number of events in the scheduler queue. *
#define SCHED_QUEUE_SIZE                10

using nrfpp::BLEAppTimer;
using nrfpp::BLEPeripheralDevice;

int main(void)
{

    nrf_gpio_cfg_output(PIN_3);
    nrf_gpio_cfg_output(PIN_5);

    BLEAppTimer<LEDSwitcher> timer(nrfpp::TIMER_REPEATED);
    bool status = timer.is_good();
    timer.start(300);
    status = timer.is_good();

    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);

    BLEPeripheralDevice::start();


    while(true) {
        BLEPeripheralDevice::idle();
    }

    return 0;
}
