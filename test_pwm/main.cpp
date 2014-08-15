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
    PWMBase::Channel* ch = new PWMBase::Channel();
    ch->gpio_pin = 7;
    ch->gpiote_channel = 2;
    ch->ppi_channels[0] = 0;
    ch->ppi_channels[1] = 1;
    ch->target_value = 100;


    p->add_channel(ch);
    p->initialize();

    const float FREQ_HALF_NOTE_FACTOR = 1.059463f;
    float frequency = 440.0f;

    while(true) {
        p->set_channel_value(0, (16000000+(frequency/2))/frequency, 50);
        frequency *= FREQ_HALF_NOTE_FACTOR;
        if(frequency > (440.0f * 4.0f)) {
            frequency = 440.0f;
        }
        nrf_delay_us(500000);
    }

    return 0;
}
