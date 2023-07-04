#pragma once

#include <engine/events/event.h>
#include <functional>

namespace engine
{
	class INetworkDevice
	{
	public:
		using event_callback_fn = std::function<void(Event&)>;

		INetworkDevice()
			: m_event_callback{}
		{}

		virtual ~INetworkDevice() = default;

		virtual void on_update() = 0;
		
		inline void set_event_callback(const event_callback_fn& callback)
		{
			m_event_callback = callback;
		}
	
	protected:
		event_callback_fn m_event_callback;
	};
}
