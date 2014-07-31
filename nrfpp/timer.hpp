#ifndef NRFX_TIMER_HPP
#define NRFX_TIMER_HPP

extern "C" {
#include "app_timer.h"
}

namespace nrfx {

enum TimerTypeEN {
    TIMER_SINGLESHOT = APP_TIMER_MODE_SINGLE_SHOT,
    TIMER_REPEATED = APP_TIMER_MODE_REPEATED
};


class TimerInitializer 
{
public:
    static const int PRESCALER = 0;
    static const int NUM_TIMERS = 6;
    static const int QUEUE_SIZE = 4;
protected:
    TimerInitializer() {
        static bool is_timer_module_initialized = false;
        if(!is_timer_module_initialized) {
            APP_TIMER_INIT(PRESCALER, NUM_TIMERS, QUEUE_SIZE, false);
            is_timer_module_initialized = true;
        }
    }
};

template <typename TimerEventHandler>
class Timer : public TimerInitializer
{
public:
    explicit Timer(TimerTypeEN t) 
     : good_(false) 
    {
        uint32_t result = app_timer_create(&timer_id_, (app_timer_mode_t)t, 
                                           TimerEventHandler::on_timer);
        good_ = (result == NRF_SUCCESS);
    }
    void start(uint32_t ms) 
    {
        uint32_t ticks = APP_TIMER_TICKS(ms, PRESCALER);
        uint32_t result = app_timer_start(timer_id_, ticks, NULL);
        good_ = (result == NRF_SUCCESS);
    }
    void stop() 
    {
        uint32_t result = app_timer_stop(timer_id_);
        good_ = (result == NRF_SUCCESS);
    }
    bool is_good() const {
        return good_;
    }

private:
    bool good_;
    app_timer_id_t  timer_id_;    
};

}

#endif //NRFX_TIMER_HPP
