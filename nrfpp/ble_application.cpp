#include "ble_application.hpp"
#include "app_scheduler.h"
#include <assert.h>

namespace nrfpp 
{

BLEApplication::Config::Config()
 : sched_max_event_size(sizeof(app_timer_event_t)),
   sched_queue_size(10),
   timer_max_size(6),
   timer_queue_size(4),
   timer_prescaler(0)
{}

BLEApplication::BLEApplication()
 : sched_buff_(NULL),
   timer_buff_(NULL),
   is_initialized_(false)
{}

bool
BLEApplication::initialize(const Config& config)
{
    if(is_initialized_) {
        return false;
    }
    // setup the scheduler
    if(sched_buff_ == NULL) {
        uint32_t sched_buff_size = CEIL_DIV(APP_SCHED_BUF_SIZE((config.sched_max_event_size), 
                                                               (config.sched_queue_size)),
                                            sizeof(uint32_t));
        sched_buff_ = new uint32_t[sched_buff_size];
        uint32_t status = app_sched_init(config.sched_max_event_size, 
                                         config.sched_queue_size, 
                                         sched_buff_);
        if(status != NRF_SUCCESS) {
            delete[] sched_buff_;
            return false;
        }
    }
    // setup the timers
    if(timer_buff_ == NULL) {
        uint32_t timer_buff_size = CEIL_DIV(APP_TIMER_BUF_SIZE((config.timer_max_size),
                                                               (config.timer_queue_size) + 1),
                                            sizeof(uint32_t));
        timer_buff_ = new uint32_t[timer_buff_size];
        uint32_t status = app_timer_init(config.timer_prescaler,
                                         config.timer_max_size,
                                         config.timer_queue_size + 1,
                                         timer_buff_, 
                                         app_timer_evt_schedule);
        if(status != NRF_SUCCESS) {
            delete[] sched_buff_;
            return false;
        }
    }

    // now start the RTC clock
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
    { }

    config_ = config;
    is_initialized_ = true;
    return true;
}

// Note: the (singleton) instance of BLEApplication has to be
//       initialized before calling idle.
void 
BLEApplication::idle()
{
    assert(sched_buff_);
    assert(timer_buff_);
    app_sched_execute();
}

BLEApplication&
BLEApplication::instance()
{
    static BLEApplication s_instance;
    return s_instance;
}

}
