#pragma once

//#include "public.sdk/source/vst/pluginterfaces/vst/ivstparameterchanges.h"
//#include <pluginterfaces\vst\ivstparameterchanges.h>
#include <public.sdk/source/vst/hosting/parameterchanges.h>

namespace MyCompanyName {

	class VstParameterInterpolator
	{
	private:
		Steinberg::Vst::IParamValueQueue* queue = nullptr;
		float currentValue;

		int lastOffset;
		float lastValue;

		int blockWidth;

	public:
		void operator = (const float v) { currentValue = v; }
		float getCurrentValue() { return currentValue; }

		void SetQueue(Steinberg::Vst::IParamValueQueue* queue)
		{
			this->queue = queue;			
		}

		void FinishBlock(unsigned int nSamples)
		{
			unsigned int nPoints;
			if(!queue || (nPoints = this->queue->getPointCount()) == 0) // If there are no events this window, retain the constant value.
			{
				lastOffset = -1;
			}
			else //Store the last point and offset it for the next block
			{
				int o;
				double v;
				this->queue->getPoint(nPoints - 1, o, v);
				lastValue = float(v);
				lastOffset = o - nSamples;
			}

			this->queue = nullptr;
		}

		void AdvanceTimeTo(unsigned int sampleOffset)
		{
			if (!queue) return;
			int newOffset = int(sampleOffset);

			//Calculate value
			float left_value = lastValue;
			int left_offset = lastOffset;

			Steinberg::Vst::ParamValue value;
			Steinberg::int32 offset;

			for (int i = 0; i < queue->getPointCount(); i++)
			{
				queue->getPoint(i, offset, value);
				if (offset > newOffset)
				{
					//Interpolate
					float v = float(newOffset - left_offset) / float(offset - left_offset);
					currentValue = (1.f - v) * left_value + v * value;
					return;
				}
				else
				{
					left_offset = offset;
					left_value = value;
				}
			}
			
			//Nothing to interpolate
			currentValue = left_value;
		}
	};

}