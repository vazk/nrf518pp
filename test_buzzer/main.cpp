#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nrfpp/pwm.hpp"
#include "nrfpp/buzzer.hpp"
extern "C" {
#include "nrf_delay.h"
}


using nrfpp::PWMBase;
using nrfpp::PWM;
using nrfpp::Buzzer;
using namespace nrfpp::melody;

struct OneTick
{
    Note n;
    Octave o;
    int32_t ms;
};

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


    Buzzer bz(p, 0);

    Octave O = O5;
    Octave OP = O6;

    OneTick music[] = {
        {DO,O,200}, {DO,O,200}, {RE,O,500}, {DO,O,500}, {FA,O,500}, {MI,O,500},
        {PAUSE,O,400},
        {DO,O,200}, {DO,O,200}, {RE,O,500}, {DO,O,500}, {SOL,O,500}, {FA,O,500},
        {PAUSE,O,400},
        {DO,O,200}, {DO,O,200}, {DO,OP,500}, {LA,O,500}, {FA,O,500}, {MI,O,500}, {RE,O,500},
        {PAUSE,O,400},
        {LAD,O,200}, {LAD,O,200}, {LA,O,500}, {FA,O,500}, {SOL,O,500}, {FA,O,500},
        {PAUSE,O,800},
    };
    int i = 0;
    while(true) {
        OneTick& ot = music[i++%(sizeof(music)/sizeof(OneTick))];
        bz.play(ot.n, ot.o);
        nrf_delay_ms(ot.ms);
        bz.play(PAUSE, ot.o);
        nrf_delay_ms(10);
    }

    return 0;
}
