//------------------------------------------------------------------------
// Copyright(c) 2024 My Plug-in Company.
//------------------------------------------------------------------------

#include "mypluginprocessor.h"
#include "myplugincids.h"
#include "mypluginparameters.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstevents.h"

#include "PlaitsWrapper.h"

using namespace Steinberg;

namespace MyCompanyName {
//------------------------------------------------------------------------
// PlaitsVstProcessor
//------------------------------------------------------------------------
PlaitsVstProcessor::PlaitsVstProcessor()
{
	//--- set the wanted controller for our processor
	setControllerClass(kFabOscillatorControllerUID);
}

//------------------------------------------------------------------------
PlaitsVstProcessor::~PlaitsVstProcessor()
{}

//------------------------------------------------------------------------
tresult PLUGIN_API PlaitsVstProcessor::initialize(FUnknown* context)
{
	// Here the Plug-in will be instantiated

	//---always initialize the parent-------
	tresult result = AudioEffect::initialize(context);
	// if everything Ok, continue
	if (result != kResultOk)
	{
		return result;
	}

	//--- create Audio IO ------
	// addAudioInput (STR16 ("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
	addAudioOutput(STR16("Main Out"), Steinberg::Vst::SpeakerArr::kMono, Steinberg::Vst::BusTypes::kMain, 1);
	addAudioOutput(STR16("AUX out"), Steinberg::Vst::SpeakerArr::kMono, Steinberg::Vst::BusTypes::kAux, 0);

	/* If you don't need an event bus, you can remove the next line */
	addEventInput(STR16("MIDI In"), 1);

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlaitsVstProcessor::terminate()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!

	//---do not forget to call parent ------
	return AudioEffect::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlaitsVstProcessor::setActive(TBool state)
{
	//--- called when the Plug-in is enable/disable (On/Off) -----
	return AudioEffect::setActive(state);
}

//-----------------------------------------------------------------------
tresult PLUGIN_API PlaitsVstProcessor::setBusArrangements(Steinberg::Vst::SpeakerArrangement* inputs, int32 numIns, Steinberg::Vst::SpeakerArrangement* outputs, int32 numOuts)
{
	// the first input is the Main Input and the second is the SideChain Input
	// be sure that we have 2 inputs and 1 output
	if (numIns == 0 && numOuts == 2)
	{
		// we support only when Main input has the same number of channel than the output
		if (Vst::SpeakerArr::getChannelCount(outputs[0]) != 1) return kResultFalse;
		if (Vst::SpeakerArr::getChannelCount(outputs[1]) != 1) return kResultFalse;

		// we are agree with all arrangement for Main Input and output
		getAudioOutput(0)->setArrangement(outputs[0]);
		getAudioOutput(1)->setArrangement(outputs[1]);

		// OK the Side-chain is mono, we accept this by returning kResultTrue
		return kResultTrue;
	}

	// we do not accept what the host wants: return kResultFalse !
	return kResultFalse;
}

PlaitsWrapper plaits;

void fillBuffer(Steinberg::Vst::Sample32 *buffer, Steinberg::Vst::Sample32 *aux, int32 numSamples)
{
	plaits.FillBuffer(buffer, aux, numSamples);
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlaitsVstProcessor::process (Vst::ProcessData& data)
{
	//--- First : Read inputs parameter changes-----------

	if (data.inputParameterChanges)
	{
		int32 numParamsChanged = data.inputParameterChanges->getParameterCount ();
		for (int32 index = 0; index < numParamsChanged; index++)
		{
			if (auto* paramQueue = data.inputParameterChanges->getParameterData (index))
			{
				Vst::ParamID paramID = paramQueue->getParameterId();
				switch (paramID)
				{
				case ParamIDs::MORPH:
				case ParamIDs::HARMONICS:
				case ParamIDs::TIMBRE:
				case ParamIDs::LPG_COLOUR:
				case ParamIDs::ENGINE:
				case ParamIDs::SIMULATE_TRIGGER:
				{
					plaits.parameter[paramID].SetQueue(paramQueue);
					break;
				}
				case ParamIDs::LEGATO_TRIGGER:	//TODO: Create a real voice handler and let it process this parameter correctly
				{
					Vst::ParamValue v;
					int32 offset;
					paramQueue->getPoint(paramQueue->getPointCount() - 1, offset, v);
					plaits.triggerOnLegato = v >= 0.5f;
				}
				}
			}
		}
	}

	//MIDI points
	if (data.inputEvents)	//TODO: Create a real (polyphonic) voice handler
	{
		for (int i = 0; i < data.inputEvents->getEventCount(); i++)
		{
			Steinberg::Vst::Event e;
			data.inputEvents->getEvent(i, e);
			if (e.type == Vst::Event::EventTypes::kNoteOnEvent)
			{
				if (plaits.triggerOnLegato)	//Emulate trigger flank (insert inactive trigger for `silenceLength` samples and delay note on.
				{
					const int silenceLength = plaits.internalWindowLength;
					plaits.voiceHandler.InsertNoteOff(e.sampleOffset);
					plaits.voiceHandler.inputs.QueueEvent(e.sampleOffset + silenceLength, MonoVoiceHandler::Note({ e.noteOn.pitch, e.noteOn.velocity }));
				}
				else
				{
					plaits.voiceHandler.inputs.QueueEvent(e.sampleOffset, MonoVoiceHandler::Note({ e.noteOn.pitch, e.noteOn.velocity }));
				}
			}
			else if (e.type == Vst::Event::EventTypes::kNoteOffEvent)
			{
				plaits.voiceHandler.inputs.QueueEvent(e.sampleOffset, MonoVoiceHandler::Note({ e.noteOff.pitch, 0.0f }));
			}
		}
	}

	//Generate the audio signal
	if (data.numSamples > 0)
	{
		fillBuffer(data.outputs[0].channelBuffers32[0], data.outputs[1].channelBuffers32[0], data.numSamples);
	}

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlaitsVstProcessor::setupProcessing (Vst::ProcessSetup& newSetup)
{
	plaits.Init(newSetup.sampleRate);
	//--- called before any processing ----
	return AudioEffect::setupProcessing (newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlaitsVstProcessor::canProcessSampleSize (int32 symbolicSampleSize)
{
	// by default kSample32 is supported
	if (symbolicSampleSize == Vst::kSample32)
		return kResultTrue;

	// disable the following comment if your processing support kSample64
	/* if (symbolicSampleSize == Vst::kSample64)
		return kResultTrue; */

	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlaitsVstProcessor::setState (IBStream* state)
{
	// called when we load a preset, the model has to be reloaded
	IBStreamer streamer (state, kLittleEndian);
		
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlaitsVstProcessor::getState (IBStream* state)
{
	// here we need to save the model
	IBStreamer streamer (state, kLittleEndian);

	return kResultOk;
}

//------------------------------------------------------------------------
} // namespace MyCompanyName
