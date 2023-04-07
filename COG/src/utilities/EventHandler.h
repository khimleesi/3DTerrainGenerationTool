#pragma once
#include <pretty_sdl/sdl.h>
#include <map>
#include <functional>
#include <iostream>
#include <algorithm>

class EventHandler {

	typedef std::function<void(SDL_Event const&)>	EventCallback;
	typedef std::multimap<SDL_EventType, EventCallback> RegisteredCallbacks;

public:
	//Use for free functions or lambdas!
	void Register(const SDL_EventType& type, const EventCallback& callback)
	{
		m_RegisteredCallbacks.emplace(type, callback);
	}
	
	//Use for class member functions!
	template <class T>
	void Register(const SDL_EventType& type, T* const object, void(T::* const callback)(SDL_Event const&))
	{
		using namespace std::placeholders;
		m_RegisteredCallbacks.emplace(type, std::bind(callback, object, _1));
	}
	
	void Register(const SDL_EventType& first, const SDL_EventType& second, const EventCallback& callback)
	{
		/* Register a pair of callbacks e.g. Button Down & Button Up */
		/* Note that they will both point to the same & of the function */
		Register(first, callback);
		Register(second, callback);
	}

	template <class T>
	void Register(const SDL_EventType& first, const SDL_EventType& second, T* const object, void(T::* const callback)(SDL_Event const&))
	{
		Register(first, object, callback);
		Register(second, object, callback);
	}
	
	bool IsRegistered(const SDL_EventType& type) const
	{
		return (m_RegisteredCallbacks.count(type) > 0);
	}


	void PushEvents() {

		SDL_Event event = {};

		while (SDL_PollEvent(&event)) {

			/* If event type isn't registered this will just return (end, end) */
			const auto& registered = m_RegisteredCallbacks.equal_range((SDL_EventType)event.type);

			/* Both values will be equal if event.type wasn't found in map, in which case this will be skipped */
			if (registered.first != registered.second) {

				std::for_each(
					registered.first,
					registered.second,
					[&event](auto const& callback) {
					callback.second(event); /* Call the callback function, passing in the current event */
				});
			}
		}
	}


private:
	RegisteredCallbacks m_RegisteredCallbacks;
};