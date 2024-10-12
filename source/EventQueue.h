#pragma once

#include <vector>
#include <algorithm>

template<typename T> class EventQueue
{
private:
	std::vector<std::pair<unsigned int, T>> points;

public:
	EventQueue(size_t initialSize = 4) { points.reserve(initialSize); };
	~EventQueue() {};

	T mostRecentEvent;

	bool getLastEventWhere(bool (*predicate)(const T&), T& out)	const //Get the last event either in the queue or the stored event
	{
		if (points.empty())
		{
			if (predicate(mostRecentEvent))
			{
				out = mostRecentEvent;
				return true;
			}
			else return false;
		}
		for (auto i = points.rbegin(); i != points.rend(); i++)
		{
			if (predicate(i->second))
			{
				out = i->second;
				return true;
			}
		}
		return false;
	}

	void QueueEvent(int sampleOffset, const T& event)
	{
		points.push_back(std::pair(sampleOffset, event));
	}

	void ClearCurrentWindow(unsigned int nSamples)
	{
		//Find the last expired event (prev) and the latest unexpired one (it)
		auto prev = points.end();
		auto it = points.begin();
		for (; it != points.end(); it++ )
		{
			if (it->first >= nSamples)
			{
				break;
			}
			prev = it;
		}

		if (prev != points.end())
		{
			mostRecentEvent = prev->second;	//Store the latest expired event
		}

		//Remove all expired points
		std::move(it, points.end(), points.begin());
		points.resize(points.end() - it);

		//Shift the offset of all unexpired points
		for (it = points.begin(); it != points.end(); it++)
		{
			it->first -= nSamples;
		}
	}
};

