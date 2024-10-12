#pragma once

#include "EventQueue.h"

namespace MyCompanyName {
	class MonoVoiceHandler
	{
	private:

	public:
		struct Note
		{
			int note;	//TODO: int16?
			float velocity;
		};
		EventQueue<Note> inputs;

		Note currentState;
		bool freshNoteOn = false;

		void AdvanceSamples(unsigned int nSamples)
		{
			bool newEventOccurred;
			inputs.ClearCurrentWindow(nSamples, newEventOccurred);
			if (!newEventOccurred) return;
			
			//Handle most recent event
			if (inputs.mostRecentEvent.velocity == 0.0f && inputs.mostRecentEvent.note != currentState.note)
			{
				//Ignore note off for "old" voices.
			}
			else
			{
				currentState = inputs.mostRecentEvent;
				if (currentState.velocity > 0.f) freshNoteOn = true;
			}
		}

		void InsertNoteOff(int offset)
		{
			//Get active pitch at offset
			Note lastNote;
			if (inputs.getLastEventWhere([](const Note& n){ return n.velocity > 0.f; }, lastNote))
			{
				inputs.QueueEvent(offset, Note{ lastNote.note, 0.f });
			}
			else
			{
				//current voice state is inactive and no NoteOn event in queue -> Don't need to do anything.
			}
		}
	};
}