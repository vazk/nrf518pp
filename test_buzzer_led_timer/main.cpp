#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nrfpp/pwm.hpp"
#include "nrfpp/buzzer.hpp"
#include "nrfpp/ble_app_timer.hpp"
#include "nrfpp/ble_peripheral_device.hpp"
extern "C" {
#include "nrf_delay.h"
}


using nrfpp::PWMBase;
using nrfpp::PWM;
using nrfpp::Buzzer;
using nrfpp::BLEAppTimer;
using nrfpp::BLEPeripheralDevice;
using namespace nrfpp::melody;

struct OneTick
{
    Note n;
    Octave o;
    int32_t ms;
};


// Maximum size of scheduler events. Note that scheduler BLE stack events do not contain 
// any data, as the events are being pulled from the stack in the event handler. 
#define SCHED_MAX_EVENT_DATA_SIZE       sizeof(app_timer_event_t) 
// Maximum number of events in the scheduler queue. *
#define SCHED_QUEUE_SIZE                10

class LEDSwitcher 
{
public:
    LEDSwitcher() {}
    static void on_timer(void* context) {
        static int duty = 0;
        static int change = 10;

        PWMBase* p = (PWMBase*)context;

        duty += change;
        if(duty == 100) change = -10;
        else
        if(duty == 0) change = 10;
        p->set_channel_dutycycle(1, duty);
        p->set_channel_dutycycle(2, 100-duty);
    }
};

class MelodyPlayer 
{
public:
    MelodyPlayer() {}
    static void on_timer(void* context) {
        PWMBase* p = (PWMBase*)context;
        static Buzzer bz(p, 0);

        static Octave O = O5;
        static Octave OP = O6;

        static OneTick music[] = {
            {DO,O,200}, {DO,O,200}, {RE,O,500}, {DO,O,500}, {FA,O,500}, {MI,O,500},
            {PAUSE,O,400},
            {DO,O,200}, {DO,O,200}, {RE,O,500}, {DO,O,500}, {SOL,O,500}, {FA,O,500},
            {PAUSE,O,400},
            {DO,O,200}, {DO,O,200}, {DO,OP,500}, {LA,O,500}, {FA,O,500}, {MI,O,500}, {RE,O,500},
            {PAUSE,O,400},
            {LAD,O,200}, {LAD,O,200}, {LA,O,500}, {FA,O,500}, {SOL,O,500}, {FA,O,500},
            {PAUSE,O,800},
        };

        static int i = 0;
        static bool should_pause = false;
        if(!should_pause) {
            OneTick& ot = music[i++%(sizeof(music)/sizeof(OneTick))];
            bz.play(ot.n, ot.o);
        } else {
            bz.play(PAUSE, O);
        }
        should_pause = !should_pause;
    }
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



    PWMBase::Channel* ch0 = new PWMBase::Channel();
    ch0->gpio_pin = 3;
    ch0->gpiote_channel = 1;
    ch0->ppi_channels[0] = 2;
    ch0->ppi_channels[1] = 3;
    ch0->target_value = 100;

    PWMBase::Channel* ch1 = new PWMBase::Channel();
    ch1->gpio_pin = 5;
    ch1->gpiote_channel = 0;
    ch1->ppi_channels[0] = 4;
    ch1->ppi_channels[1] = 5;
    ch1->target_value = 100;

    p->add_channel(ch0);
    p->add_channel(ch1);

    p->initialize();



    bool status;
    BLEAppTimer<LEDSwitcher> timer(nrfpp::TIMER_REPEATED, p);
    status = timer.is_good();
    timer.start(50);
    status = timer.is_good();

    BLEAppTimer<MelodyPlayer> mtimer(nrfpp::TIMER_REPEATED, p);
    status = mtimer.is_good();
    mtimer.start(200);
    status = mtimer.is_good();

    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);


    BLEPeripheralDevice::start();

    int i = 0;
    while(true) {
        BLEPeripheralDevice::idle();
    }

    return 0;
}
