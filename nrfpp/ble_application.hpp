#ifndef NRFPP_BLE_APPLICATION_HPP
#define NRFPP_BLE_APPLICATION_HPP

extern "C" {
#include "app_timer.h"
}
#include <stdint.h>


namespace nrfpp {

class BLEApplication
{
public:
    struct Config
    {
        Config();
        uint32_t sched_max_event_size;
        uint32_t sched_queue_size;
        uint32_t timer_max_size;
        uint32_t timer_queue_size;
        uint32_t timer_prescaler;
    };
public:
    static BLEApplication& instance();

public:
    bool initialize(const Config& config);
    void idle();
    const Config& config() const 
    { return config_; }
    const bool is_initialized() const 
    { return is_initialized_; }
    
private:
    BLEApplication();

private:
    uint32_t* sched_buff_;
    uint32_t* timer_buff_;
    bool      is_initialized_;
    Config    config_;
};

}

#endif //NRFPP_BLE_APPLICATION_HPP
