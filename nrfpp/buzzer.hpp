#ifndef NRFPP_BUZZER_HPP
#define NRFPP_BUZZER_HPP

#include <stdint.h>

namespace nrfpp {

namespace melody {

enum Note { 
    DO,
    DOD,
    RE,
    RED,
    MI,
    FA,
    FAD,
    SOL,
    SOLD,
    LA,
    LAD,
    SI,
    PAUSE,
};

const float NoteFrequencies[] = {
    523.2511,
    554.3653,
    587.3295,
    622.2540,
    659.2551,
    698.4565,
    739.9888,
    783.9909,
    830.6094,
    880,
    932.3276,
    987.7666,
    0
};

enum Octave {
    O1,
    O2,
    O3,
    O4,
    O5,
    O6,
    O7
};

const float OctaveMultipliers[] = {
    0.0625,
    0.125,
    0.25,
    0.5,
    1,
    2,
    3
};

}

class PWMBase;

class Buzzer
{
public:
    Buzzer(PWMBase* pwm, uint32_t chi, uint32_t duty = 50);
    void play(melody::Note note, melody::Octave octave = melody::O5);
private:
    PWMBase* pwm_;
    uint32_t chi_;
    uint32_t duty_; 
};

}
#endif //NRFPP_BUZZER_HPP
