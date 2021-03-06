#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nrfpp/pwm.hpp"
extern "C" {
#include "nrf_delay.h"
}


using nrfpp::PWMBase;
using nrfpp::PWM;


int main(void)
{

    PWMBase* p = PWM<2>::instance();

    PWMBase::Channel* ch0 = new PWMBase::Channel();
    ch0->gpio_pin = 3;
    ch0->gpiote_channel = 2;
    ch0->ppi_channels[0] = 0;
    ch0->ppi_channels[1] = 1;
    ch0->target_value = 100;

    PWMBase::Channel* ch1 = new PWMBase::Channel();
    ch1->gpio_pin = 5;
    ch1->gpiote_channel = 1;
    ch1->ppi_channels[0] = 2;
    ch1->ppi_channels[1] = 3;
    ch1->target_value = 100;

    p->add_channel(ch0);
    p->add_channel(ch1);
    p->initialize();

    int duty = 0;
    int change = 10;

    p->set_max_value(100);

    while(true) {

        duty += change;
        if(duty == 100) change = -10;
        else
        if(duty == 0) change = 10;

        p->set_channel_dutycycle(0, duty);
        p->set_channel_dutycycle(1, 100-duty);

        nrf_delay_ms(40);
    }

    return 0;
}
