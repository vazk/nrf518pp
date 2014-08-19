#ifndef NRFPP_TIMER_HPP
#define NRFPP_TIMER_HPP

#include "ble_application.hpp"
#include "app_timer.h"
#include <assert.h>

namespace nrfpp {

enum TimerTypeEN {
    TIMER_SINGLESHOT = APP_TIMER_MODE_SINGLE_SHOT,
    TIMER_REPEATED = APP_TIMER_MODE_REPEATED
};

// Note: BLEApplication has to be initialized prior to 
//       instantiating BLEAppTimer objects.
template <typename TimerEventHandler>
class BLEAppTimer
{
public:
    explicit BLEAppTimer(TimerTypeEN t, void* context = NULL) 
     : good_(false),
       context_(context)
    {
        uint32_t result = app_timer_create(&timer_id_, (app_timer_mode_t)t, 
                                           TimerEventHandler::on_timer);
        good_ = (result == NRF_SUCCESS);
    }
    void start(uint32_t ms) 
    {
        BLEApplication& app = BLEApplication::instance();
        assert(app.is_initialized()); 
        uint32_t ticks = APP_TIMER_TICKS(ms, app.config().timer_prescaler);
        uint32_t result = app_timer_start(timer_id_, ticks, context_);
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
    bool   good_;
    void*  context_;
    app_timer_id_t  timer_id_;    
};

}

#endif //NRFX_TIMER_HPP
