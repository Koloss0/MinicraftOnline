#pragma once

#include <string>
#include <sstream>

#include <spdlog/spdlog.h>

enum class EventType
{
	None = 0,
	WindowResize, WindowClose, WindowFocus, WindowLoseFocus,
	KeyPress, KeyRelease,
	MouseButtonPress, MouseButtonRelease, MouseMove, MouseScroll,
	ClientConnection, ClientDisconnection, ClientMessage, ClientValidation,
	ServerMessage
};

#define EVENT_CLASS_TYPE(type) static EventType get_static_type() { return EventType::type; } \
		virtual EventType get_type() const override { return get_static_type(); }\
		virtual const char* get_name() const override { return #type; }

class Event
{
public:
	Event() = default;
	virtual ~Event() = default;

	virtual EventType get_type() const = 0;
	virtual const char* get_name() const = 0;

	virtual std::string to_string() const
	{
		std::stringstream ss;
		ss << "[" << get_name() << "]";

	       	return ss.str();
	}

	bool handled = false;
};

inline std::ostream& operator<<(std::ostream& os, const Event& e)
{
	return os << e.to_string();
}

class EventDispatcher
{
public:
	EventDispatcher(Event& event)
		: m_event(event)
	{
	}
	
	// F will be deduced by the compiler
	template<typename T, typename F>
	bool dispatch(const F& func)
	{
		if (m_event.get_type() == T::get_static_type())
		{
			m_event.handled |= func(static_cast<T&>(m_event));
			return true;
		}
		return false;
	}
private:
	Event& m_event;
};
