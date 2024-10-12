//------------------------------------------------------------------------
// Copyright(c) 2024 My Plug-in Company.
//------------------------------------------------------------------------

#include "myplugincontroller.h"
#include "myplugincids.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "mypluginparameters.h"

using namespace Steinberg;

namespace MyCompanyName {

//------------------------------------------------------------------------
// PlaitsVstController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API PlaitsVstController::initialize (FUnknown* context)
{
	// Here the Plug-in will be instantiated

	//---do not forget to call parent ------
	tresult result = EditControllerEx1::initialize (context);
	if (result != kResultOk)
	{
		return result;
	}

	// Here you could register some parameters
	auto paramHarmonics = parameters.addParameter(STR16("Harmonics"), nullptr, 0, 0.5f, 1, ParamIDs::HARMONICS, 0, STR16("HARM"));
	paramHarmonics->setPrecision(2);
	auto paramTimbre = parameters.addParameter(STR16("Timbre"), nullptr, 0, 0.5f, 1, ParamIDs::TIMBRE, 0, STR16("TIMB"));
	paramTimbre->setPrecision(2);
	auto paramMorph = parameters.addParameter(STR16("Morph"), nullptr, 0, 0.5f, 1, ParamIDs::MORPH, 0, STR16("MORP"));
	paramMorph->setPrecision(2);
	auto paramEngine = parameters.addParameter(STR16("Engine"), nullptr, 0 /*23*/ /* 24 engines */, 0.0f, Steinberg::Vst::ParameterInfo::kIsList | Steinberg::Vst::ParameterInfo::kCanAutomate, ParamIDs::ENGINE, 0, STR16("ENG"));
	//auto paramEngine = parameters.addParameter(STR16("Engine"), nullptr, 23 /* 24 engines */, 0.0f, Steinberg::Vst::ParameterInfo::kIsList | Steinberg::Vst::ParameterInfo::kCanAutomate, ParamIDs::ENGINE, 0, STR16("ENG"));
	auto paramLpgDecay = parameters.addParameter(STR16("LPG decay"), nullptr, 0, 0.5f, 1, ParamIDs::LPG_DECAY, 0, STR16("DECAY"));
	paramLpgDecay->setPrecision(2);
	auto paramLpgFlavor = parameters.addParameter(STR16("LPG colour"), nullptr, 0, 1.0f, 1, ParamIDs::LPG_COLOUR, 0, STR16("LPG"));
	paramLpgFlavor->setPrecision(2);
	auto paramSimulateTrigger = parameters.addParameter(STR16("Simulate Trigger"), nullptr, 1, 1.0f, 1, ParamIDs::SIMULATE_TRIGGER, 0, STR16("SIM_T"));
	auto paramLegatoTrigger = parameters.addParameter(STR16("Trigger on Legato"), nullptr, 1, 1.0f, 1, ParamIDs::LEGATO_TRIGGER, 0, STR16("TRIGL"));
	

	return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlaitsVstController::terminate ()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!

	//---do not forget to call parent ------
	return EditControllerEx1::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlaitsVstController::setComponentState (IBStream* state)
{
	// Here you get the state of the component (Processor part)
	if (!state)
		return kResultFalse;

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlaitsVstController::setState (IBStream* state)
{
	// Here you get the state of the controller

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlaitsVstController::getState (IBStream* state)
{
	// Here you are asked to deliver the state of the controller (if needed)
	// Note: the real state of your plug-in is saved in the processor

	return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API PlaitsVstController::createView (FIDString name)
{
	// Here the Host wants to open your editor (if you have one)
	if (FIDStringsEqual (name, Vst::ViewType::kEditor))
	{
		// create your editor here and return a IPlugView ptr of it
		auto* view = new VSTGUI::VST3Editor (this, "view", "myplugineditor.uidesc");
		//VSTGUI::UIDescription desc("myplugineditor.uidesc");
		//VSTGUI::CView *cview = desc.createView("view", 0);
		return view;
	}
	return nullptr;
}


//------------------------------------------------------------------------
} // namespace MyCompanyName
