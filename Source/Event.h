#pragma once

#include <vector>
#include <functional>

#include "Globals.h"

template<typename... ARGS> class EventListener
{
private:
	std::function<void(ARGS... args)> callback;

public:
	EventListener();
	EventListener(std::function<void(ARGS... args)> new_callback);

public:
	void operator() (ARGS... args)
	{
		callback(args...);
	}
};

template<typename... ARGS>
inline EventListener<ARGS...>::EventListener()
{
}

template<typename... ARGS>
inline EventListener<ARGS...>::EventListener(std::function<void(ARGS... args)> new_callback)
{
	callback = new_callback;
}

// NOTE: Right now this Event template class only accepts functions with 
// any type/number of parameters but only with return type of void.
// Support for other return types may be added in the future.

template<typename... ARGS> class Event
{
private:
	std::vector<EventListener<ARGS...>*> listeners;

public:
	void AddListener(EventListener<ARGS...>* listener);
	void RemoveListener(EventListener<ARGS...>* listener);
	void RemoveAllListeners();
	void Invoke(ARGS... args);
};

template<typename... ARGS>
inline void Event<ARGS...>::AddListener(EventListener<ARGS...>* listener)
{
	LOG("Added Listener");

	listeners.push_back(listener);
}

template<typename... ARGS>
inline void Event<ARGS...>::RemoveListener(EventListener<ARGS...>* listener)
{
	LOG("Removed Listener");

	std::vector<EventListener<ARGS...>*>::template iterator index_to_erase;

	// NOTE: Right now this determines index to erase by comparing the memory addresses,
	// making each instance of EventListener hold a unique ID would be a better solution.
	// TODO: Address this issue.

	index_to_erase = std::find(listeners.begin(), listeners.end(), listener);

	if (index_to_erase != listeners.end())
	{
		listeners.erase(index_to_erase);
	}
}

template<typename... ARGS>
inline void Event<ARGS...>::RemoveAllListeners()
{
	listeners.clear();

	// NOTE: If we decide to add a reference to the registered Event on EventListeners, 
	// this RemoveAllListener method might call a function that deregisters the listener 
	// from event.
}

template<typename... ARGS>
inline void Event<ARGS...>::Invoke(ARGS... args)
{
	LOG("Invoked event");

	for (EventListener<ARGS...>* listener : listeners)
	{
		(*listener)(args...);
	}
}