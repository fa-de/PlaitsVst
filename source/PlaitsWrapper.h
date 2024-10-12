#pragma once

#define TEST
#include "../eurorack/plaits/dsp/voice.h"
#undef TEST

#include "EventQueue.h"
#include "VstParameterInterpolator.h"
#include "mypluginparameters.h"
#include "MonoVoiceHandler.h"

namespace MyCompanyName {

	class PlaitsWrapper
	{
	private:
		plaits::Voice* plaitsVoice;
		char shared_buffer[16384]; //RAM for the PLAITS engines

	public:
		PlaitsWrapper();
		~PlaitsWrapper();

		void Init(size_t sampleRate);
		void FillBuffer(float* out, float* aux, unsigned int nSamples);

		VstParameterInterpolator parameter[ParamIDs::NUM_PARAMETERS];

		MonoVoiceHandler voiceHandler;
		VstParameterInterpolator morph;
		bool triggerOnLegato = true;

		static const unsigned int internalWindowLength = 8; //6-op engine glitches with larger windows. I don't yet know why...
	};
}