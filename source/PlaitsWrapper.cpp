#include "PlaitsWrapper.h"
#include "stmlib/utils/buffer_allocator.h"

namespace plaits
{
	float a0, kCorrectedSampleRate, kSampleRate;
}

namespace MyCompanyName
{

PlaitsWrapper::PlaitsWrapper()
{
	this->Init(48000);
	plaitsVoice = new plaits::Voice();
	
	stmlib::BufferAllocator allocator(shared_buffer, 16384);
	plaitsVoice->Init(&allocator);	
}

PlaitsWrapper::~PlaitsWrapper()
{
	if (plaitsVoice)
	{
		delete plaitsVoice;
		plaitsVoice = nullptr;
	}
}

void PlaitsWrapper::Init(size_t sampleRate)
{
	plaits::kSampleRate = float(sampleRate);
	plaits::kCorrectedSampleRate = plaits::kSampleRate;
	plaits::a0 = (440.0f / 8.0f) / plaits::kCorrectedSampleRate;

	stmlib::BufferAllocator allocator(shared_buffer, 16384);

	//Reinit in case the sample rate has changed -> six op needs to update
	if(plaitsVoice) plaitsVoice->Init(&allocator);

	//Set defaults
	parameter[ParamIDs::ENGINE] = 0.0f;
	parameter[ParamIDs::MORPH] = 0.5f;
	parameter[ParamIDs::HARMONICS] = 0.5f;
	parameter[ParamIDs::TIMBRE] = 0.5f;
	parameter[ParamIDs::LPG_DECAY] = 0.5f;
	parameter[ParamIDs::LPG_COLOUR] = 0.5f;
	parameter[ParamIDs::SIMULATE_TRIGGER] = 1.0f;
	parameter[ParamIDs::LEGATO_TRIGGER] = 0.0f;
}

void PlaitsWrapper::FillBuffer(float* out, float* aux, unsigned int nSamples)
{
	//Plaits only supports 24 samples per Render() call. We might have to call it multiple times
	float* pOut = out;
	float* pAux = aux;

	unsigned int currentOffset = 0;
	unsigned int UnprocessedSamples = nSamples;

	plaits::Voice::Frame frames[plaits::kMaxBlockSize];
	while (UnprocessedSamples > 0)
	{
		//unsigned int window = std::min(UnprocessedSamples, (unsigned int) plaits::kMaxBlockSize);
		static_assert(PlaitsWrapper::internalWindowLength <= plaits::kMaxBlockSize);
		int window = std::min(UnprocessedSamples, PlaitsWrapper::internalWindowLength);
		
		//Get latest MIDI note and velocity
		voiceHandler.AdvanceSamples(window);

		//Get current value of all parameter automations
		for (int i = 0; i < ParamIDs::NUM_PARAMETERS; i++) parameter[i].AdvanceTimeTo(currentOffset);

		plaits::Patch patch = {
			//noteQueue.mostRecentEvent.pitch, //note
			float(voiceHandler.currentState.note),
			parameter[ParamIDs::HARMONICS].getCurrentValue(), //harmonics
			parameter[ParamIDs::TIMBRE].getCurrentValue(), //timbre
			parameter[ParamIDs::MORPH].getCurrentValue(), //morph
			0.f, //frequency_modulation_amount
			0.f, //timbre_modulation_amount
			0.f, //morph_modulation_amount
			std::min(23, int(parameter[ParamIDs::ENGINE].getCurrentValue() * 24.0f)), //engine
			parameter[ParamIDs::LPG_DECAY].getCurrentValue(), //decay
			parameter[ParamIDs::LPG_COLOUR].getCurrentValue(), //lpg_colour
		};

		plaits::Modulations modulations =
		{
			0.f, 0.f, 0.f, 0.f, 0.f, 0.f, (voiceHandler.currentState.velocity > 0.01f) ? 1.0f : 0.0f, voiceHandler.currentState.velocity,
			false, false, false, parameter[ParamIDs::SIMULATE_TRIGGER].getCurrentValue() >= 0.5f, false
		};

		plaitsVoice->Render(patch, modulations, frames, window);

		//Convert signed 16bit to float
		for (int i = 0; i < window; i++)
		{
			*pOut = float(frames[i].out) / 32767.0f;
			pOut++;

			*pAux = float(frames[i].aux) / 32767.0f;
			pAux++;
		}

		//morph.AdvanceTime(window);
		currentOffset += window;
		UnprocessedSamples -= window;
	}

	for (int i = 0; i < ParamIDs::NUM_PARAMETERS; i++) parameter[i].FinishBlock(nSamples);
}

}