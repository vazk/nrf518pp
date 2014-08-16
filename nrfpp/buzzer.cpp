#include "buzzer.hpp"
#include "pwm.hpp"

namespace nrfpp {

using namespace melody;

Buzzer::Buzzer(PWMBase* pwm, uint32_t chi)
 : pwm_(pwm),
   chi_(chi)
{
}

void Buzzer::play(Note note, Octave octave)
{
    float value;
    switch (note) {
    case PAUSE: 
        value = 0;
        break;
    default: 
        float freq = OctaveMultipliers[octave] * NoteFrequencies[note];
        value = (16000000+(freq/2))/freq;
    }
    pwm_->set_channel_value(chi_, value, 50);
}

}
