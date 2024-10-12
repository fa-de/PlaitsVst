//------------------------------------------------------------------------
// Copyright(c) 2024 My Plug-in Company.
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace MyCompanyName {
//------------------------------------------------------------------------
static const Steinberg::FUID kFabOscillatorProcessorUID (0x6DC72358, 0xB5055198, 0x8A2ACCF0, 0x8FA9E65F);
static const Steinberg::FUID kFabOscillatorControllerUID (0x06858B0E, 0xEE255603, 0xA2510BE5, 0x24008F22);

#define PlaitsVstVST3Category "Instrument"

//------------------------------------------------------------------------
} // namespace MyCompanyName
