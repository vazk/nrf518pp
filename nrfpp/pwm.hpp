#ifndef NRFPP_PWM_HPP
#define NRFPP_PWM_HPP

extern "C" {
#include "nrf.h"
#include "nrf51.h"
//#include "nrf_sdm.h"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "wrappers.h"
}
#include <string.h>

namespace nrfpp {

class PWMBase
{
public:
    enum { MAX_CHANNELS = 3 };
    enum Prescaler { PSC0, PSC1, PSC2, PSC3, PSC4, 
                     PSC5, PSC6, PSC7, PSC8, PSC9 };
    struct Channel
    {
        Channel() : running(0) {}
        uint32_t gpio_pin;
        uint8_t  gpiote_channel;
        uint8_t  ppi_channels[2];
        uint32_t target_value;
        bool     running;
    };


protected:
    PWMBase(NRF_TIMER_Type* t, IRQn_Type tirq, uint8_t tprio, Prescaler tpsc)
     : PWM_timer_(t),
       PWM_timer_IRQ_(tirq),
       PWM_timer_IRQ_priority_(tprio),
       PWM_timer_prescaler_(tpsc),
       num_channels_(0),
       initialized_(false),
       pwm_max_value_(0)
    {
        memset(channels_, 0, sizeof channels_);
        static bool is_hfclk_initialized = false;
        if(!is_hfclk_initialized) {
            // Start 16 MHz crystal oscillator
            //NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;  // [review]
            NRF_CLOCK->TASKS_HFCLKSTART    = 1;
            // wait for the external oscillator to start up 
           // while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {} // [review]
            is_hfclk_initialized = true;
        }
    }

public:
    bool initialize()
    {
        if(initialized_) {
            return false;
        }
        initialized_ = true;
        PWM_timer_->PRESCALER = (uint32_t)PWM_timer_prescaler_;
        PWM_timer_->TASKS_CLEAR = 1;
        PWM_timer_->BITMODE = TIMER_BITMODE_BITMODE_16Bit;
        PWM_timer_->CC[3] = pwm_max_value_;
        PWM_timer_->MODE = TIMER_MODE_MODE_Timer;
        PWM_timer_->SHORTS = TIMER_SHORTS_COMPARE3_CLEAR_Msk;
        PWM_timer_->EVENTS_COMPARE[0] = PWM_timer_->EVENTS_COMPARE[1] = 
        PWM_timer_->EVENTS_COMPARE[2] = PWM_timer_->EVENTS_COMPARE[3] = 0;     

        for(int i = 0; i < num_channels_; i++) {
            nrf_gpio_cfg_output(channels_[i]->gpio_pin);

            NRF_PPI->CH[channels_[i]->ppi_channels[0]].EEP = (uint32_t)&PWM_timer_->EVENTS_COMPARE[i];
            NRF_PPI->CH[channels_[i]->ppi_channels[0]].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[channels_[i]->gpiote_channel];    
            NRF_PPI->CHENSET = (1 << channels_[i]->ppi_channels[0]);   

            NRF_PPI->CH[channels_[i]->ppi_channels[1]].EEP = (uint32_t)&PWM_timer_->EVENTS_COMPARE[3];
            NRF_PPI->CH[channels_[i]->ppi_channels[1]].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[channels_[i]->gpiote_channel];    
            NRF_PPI->CHENSET = (1 << channels_[i]->ppi_channels[1]);   


            //nrfpp_sd_ppi_channel_assign(channels_[i]->ppi_channels[0], 
            //                            &PWM_timer_->EVENTS_COMPARE[i], 
            //                            &NRF_GPIOTE->TASKS_OUT[channels_[i]->gpiote_channel]);
            //nrfpp_sd_ppi_channel_enable_set(1 << channels_[i]->ppi_channels[0]);

            //nrfpp_sd_ppi_channel_assign(channels_[i]->ppi_channels[1], 
            //                            &PWM_timer_->EVENTS_COMPARE[3], 
            //                            &NRF_GPIOTE->TASKS_OUT[channels_[i]->gpiote_channel]);
            //nrfpp_sd_ppi_channel_enable_set(1 << channels_[i]->ppi_channels[1]);
        }
        //nrfpp_sd_nvic_SetPriority(PWM_timer_IRQ_, PWM_timer_IRQ_priority_| 0x01);
        //nrfpp_sd_nvic_EnableIRQ(PWM_timer_IRQ_);
        NVIC_SetPriority(PWM_timer_IRQ_, PWM_timer_IRQ_priority_);
        NVIC_EnableIRQ(PWM_timer_IRQ_);
        PWM_timer_->TASKS_START = 1;

        return true;
    }

    bool add_channel(Channel* ch) 
    {
        if(initialized_ || num_channels_ == MAX_CHANNELS) {
            return false;
        }
        channels_[num_channels_++] = ch;
    }

    bool set_max_value(uint32_t value)
    {
        pwm_max_value_ = value;

        PWM_timer_->EVENTS_COMPARE[3] = 0;
        PWM_timer_->SHORTS = TIMER_SHORTS_COMPARE3_CLEAR_Msk | TIMER_SHORTS_COMPARE3_STOP_Msk;
        if((PWM_timer_->INTENSET & TIMER_INTENSET_COMPARE3_Msk) == 0) {
            PWM_timer_->TASKS_STOP = 1;
            PWM_timer_->INTENSET = TIMER_INTENSET_COMPARE3_Msk;  
        }
        PWM_timer_->TASKS_START = 1;  
    }


    bool set_channel_dutycycle(uint32_t chi, uint32_t duty_cycle)
    {
        if(chi < 0 || chi >= num_channels_) {
            return false;
        }

        channels_[chi]->target_value = pwm_max_value_ * duty_cycle / 100;
        PWM_timer_->EVENTS_COMPARE[3] = 0;
        PWM_timer_->SHORTS = TIMER_SHORTS_COMPARE3_CLEAR_Msk | TIMER_SHORTS_COMPARE3_STOP_Msk;
        if((PWM_timer_->INTENSET & TIMER_INTENSET_COMPARE3_Msk) == 0) {
            PWM_timer_->TASKS_STOP = 1;
            PWM_timer_->INTENSET = TIMER_INTENSET_COMPARE3_Msk;  
        }
        PWM_timer_->TASKS_START = 1;  
    }

    void enable()
    {
        PWM_timer_->INTENSET = TIMER_INTENSET_COMPARE3_Msk; // [review]
        PWM_timer_->TASKS_START = 1;
    }

    void disable()
    {
        PWM_timer_->TASKS_STOP = 1;
        for(uint32_t i = 0; i < num_channels_; i++) {
            nrf_gpiote_unconfig(channels_[i]->gpiote_channel);
            nrf_gpio_pin_write(channels_[i]->gpio_pin, 0); 
            channels_[i]->running = false;
        }       
    }

    void handler() 
    {
        uint32_t i; 
        PWM_timer_->EVENTS_COMPARE[3] = 0;
        PWM_timer_->INTENCLR = 0xFFFFFFFF;
        PWM_timer_->CC[3] = pwm_max_value_;
        for(i = 0; i < num_channels_; i++) {
            if(channels_[i]->target_value == 0) {
                nrf_gpiote_unconfig(channels_[i]->gpiote_channel);
                nrf_gpio_pin_write(channels_[i]->gpio_pin, 0);
                channels_[i]->running = false;
            } else
            if (channels_[i]->target_value >= pwm_max_value_) {
                nrf_gpiote_unconfig(channels_[i]->gpiote_channel);
                nrf_gpio_pin_write(channels_[i]->gpio_pin, 1); 
                channels_[i]->running = false;
            } else {
                PWM_timer_->CC[i] = channels_[i]->target_value;
                if(!channels_[i]->running)
                {
                    nrf_gpiote_task_config(channels_[i]->gpiote_channel, 
                                           channels_[i]->gpio_pin, 
                                           NRF_GPIOTE_POLARITY_TOGGLE, 
                                           NRF_GPIOTE_INITIAL_VALUE_HIGH);  
                    channels_[i]->running = true;
                }
            }
        }
        PWM_timer_->SHORTS = TIMER_SHORTS_COMPARE3_CLEAR_Msk;
        PWM_timer_->TASKS_START = 1;
    };

protected:
    NRF_TIMER_Type* PWM_timer_;
    IRQn_Type       PWM_timer_IRQ_;
    uint8_t         PWM_timer_IRQ_priority_;
    Prescaler       PWM_timer_prescaler_;
    Channel*        channels_[MAX_CHANNELS];
    uint32_t        num_channels_;
    bool            initialized_;
    // review these
    uint32_t pwm_max_value_;
};

template <int TIMER>
class PWM : public PWMBase
{
private:
    PWM(NRF_TIMER_Type* t, IRQn_Type tirq, uint8_t tprio, Prescaler tpsc)
     : PWMBase(t, tirq, tprio, tpsc)
    {}

public:
    static PWMBase* instance();
};

}

#endif //NRFX_PWM_HPP
