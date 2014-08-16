#include "pwm.hpp"

namespace nrfpp
{

template<>
PWMBase* PWM<2>::instance() 
{
    static PWMBase* s_instance = new PWM<2>(NRF_TIMER2, TIMER2_IRQn, 3, PSC0);
    return s_instance;
}

template<>
PWMBase* PWM<1>::instance() 
{
    static PWMBase* s_instance = new PWM<1>(NRF_TIMER1, TIMER1_IRQn, 3, PSC0);
    return s_instance;
}

} // namespace nrfpp

extern "C" {
void TIMER2_IRQHandler(void) 
{
    nrfpp::PWM<2>::instance()->handler();
}
void TIMER1_IRQHandler(void) 
{
    nrfpp::PWM<1>::instance()->handler();
}
}

