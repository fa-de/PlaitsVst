#pragma once
#include "pluginterfaces/vst/vsttypes.h"

namespace MyCompanyName
{
    enum ParamIDs : Steinberg::Vst::ParamID
    {
        HARMONICS = 0,
        TIMBRE = 1,
        MORPH = 2,
        ENGINE = 3,
        LPG_DECAY = 4,
        LPG_COLOUR = 5,
        SIMULATE_TRIGGER = 6,
        LEGATO_TRIGGER = 7,
        /*  */
        NUM_PARAMETERS = 8,
    };
}
