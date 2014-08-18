#include "buzzer.hpp"
#include "pwm.hpp"

namespace nrfpp {

using namespace melody;

Buzzer::Buzzer(PWMBase* pwm, uint32_t chi, uint32_t duty)
 : pwm_(pwm),
   chi_(chi),
   duty_(duty)
{
}

void Buzzer::play(Note note, Octave octave)
{
    switch (note) {
    case PAUSE: 
        pwm_->set_channel_dutycycle(chi_, 0);
        break;
    default: 
        float freq = OctaveMultipliers[octave] * NoteFrequencies[note];
        pwm_->set_max_value((16000000+(freq/2))/freq);
        pwm_->set_channel_dutycycle(chi_, duty_);
    }
}

}
