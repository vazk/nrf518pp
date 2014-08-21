#include "nrfpp/uart.hpp"
#include "nrfpp/ble_app_timer.hpp"
#include "nrfpp/ble_application.hpp"
#include "nrf_gpio.h"
#include <stdint.h>
#include <stdio.h>

#define PIN_3 3
#define PIN_5 5


using nrfpp::UART;
using nrfpp::BLEAppTimer;
using nrfpp::BLEApplication;

class UARTTester
{
public:
    UARTTester() {}
    static void on_timer(void* context) {
        static int counter = 0;
        printf("message #: %d\n", counter++);
    }
};

int main(void)
{

    nrf_gpio_cfg_output(PIN_3);
    nrf_gpio_cfg_output(PIN_5);

    BLEApplication& app = BLEApplication::instance();
    app.initialize(BLEApplication::Config());


    UART& uart = UART::instance();
    UART::Config config = {UART::BR_230400,3,7,0,0,false};
    uart.initialize(config);

    BLEAppTimer<UARTTester> timer(nrfpp::TIMER_REPEATED);
    bool status = timer.is_good();
    timer.start(50);
    status = timer.is_good();

    int i = 0;
    while(true) {
        app.idle();
    }


    return 0;
}
