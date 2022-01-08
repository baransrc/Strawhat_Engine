#pragma once

#include <vector>
#include "Globals.h"

// NOTE: Right now this Event template class only accepts functions with 
// any type/number of parameters but only with return type of void.
// Support for other return types may be added in the future.

template<typename... ARGS> class Event
{
private:
	std::vector<void(*)(ARGS... args)> listeners;
public:
	void AddListener(void(*listener)(ARGS... args));
	void RemoveListener(void(*listener)(ARGS... args));
	void Invoke(ARGS... args);
};

template<typename... ARGS>
void Event<ARGS...>::AddListener(void(*listener)(ARGS... args))
{
	LOG("Added Listener");

	listeners.push_back(listener);
}

template<typename... ARGS>
void Event<ARGS...>::RemoveListener(void(*listener)(ARGS... args))
{
	LOG("Removed Listener");

	std::vector<void(*)(ARGS... args)>::template iterator index_to_erase;

	index_to_erase = std::find(listeners.begin(), listeners.end(), listener);

	if (index_to_erase != listeners.end())
	{
		listeners.erase(index_to_erase);
	}
}

template<typename... ARGS>
void Event<ARGS...>::Invoke(ARGS... args)
{
	LOG("Invoked event");

	for (void(*listener)(ARGS... args) : listeners)
	{
		listener(args...);
	}
}