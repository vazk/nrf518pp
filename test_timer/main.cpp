#include "nrfpp/ble_app_timer.hpp"
#include "nrfpp/ble_application.hpp"
#include "nrf_gpio.h"
#include <stdint.h>

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

using nrfpp::BLEAppTimer;
using nrfpp::BLEApplication;

int main(void)
{

    nrf_gpio_cfg_output(PIN_3);
    nrf_gpio_cfg_output(PIN_5);

    BLEApplication& app = BLEApplication::instance();
    app.initialize(BLEApplication::Config());

    BLEAppTimer<LEDSwitcher> timer(nrfpp::TIMER_REPEATED);
    bool status = timer.is_good();
    timer.start(300);
    status = timer.is_good();

    while(true) {
        app.idle();
    }

    return 0;
}
