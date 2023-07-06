#pragma once

#include "event.hpp"
#include <engine/core/window.hpp>

#include <sstream>

class WindowEvent: public Event
{};

class WindowResizeEvent: public WindowEvent
{
public:
	WindowResizeEvent(unsigned int w, unsigned int h)
		: m_width(w), m_height(h)
	{}

	inline unsigned int get_width() { return m_width; }
	inline unsigned int get_height() { return m_height; }
	
	EVENT_CLASS_TYPE(WindowResize)
	
	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "[WindowResize: " << m_width << ", " << m_height << "]";

		return ss.str();
	}
private:
	unsigned int m_width, m_height;
};

class WindowCloseEvent: public WindowEvent
{
public:
	EVENT_CLASS_TYPE(WindowClose)
};

class WindowFocusEvent: public WindowEvent
{
public:
	EVENT_CLASS_TYPE(WindowFocus)
};

class WindowLoseFocusEvent: public WindowEvent
{
public:
	EVENT_CLASS_TYPE(WindowLoseFocus)
};

class KeyEvent: public WindowEvent
{
protected:
	KeyEvent(int k)
		: m_keycode(k)
	{}
public:
	inline int get_keycode() const { return m_keycode; }
private:
	int m_keycode;
};

class KeyPressEvent: public KeyEvent
{
public:	
	KeyPressEvent(int k, bool repeat)
		: KeyEvent(k), m_repeat(repeat)
	{}

	inline bool is_repeat() const { return m_repeat; }
	
	EVENT_CLASS_TYPE(KeyPress)
	
	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "[KeyPress: " << get_keycode() <<
			" (repeat: " << m_repeat << ")]";

		return ss.str();
	}
private:
	bool m_repeat;
};

class KeyReleaseEvent: public KeyEvent
{
public:
	KeyReleaseEvent(int k)
		: KeyEvent(k)
	{}

	EVENT_CLASS_TYPE(KeyRelease)
	
	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "[KeyRelease: " << get_keycode() << "]";

		return ss.str();
	}
};

class MouseButtonEvent: public WindowEvent
{
protected:
	MouseButtonEvent(int b)
		: m_button(b)
	{}
public:
	int get_button() const { return m_button; }
private:
	int m_button;
};

class MouseButtonPressEvent: public MouseButtonEvent
{
public:
	MouseButtonPressEvent(int b)
		: MouseButtonEvent(b)
	{}

	EVENT_CLASS_TYPE(MouseButtonPress)

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "[MouseButtonPress: " << get_button() << "]";

		return ss.str();
	}
};

class MouseButtonReleaseEvent: public MouseButtonEvent
{
public:
	MouseButtonReleaseEvent(int b)
		: MouseButtonEvent(b)
	{}

	EVENT_CLASS_TYPE(MouseButtonRelease)
	
	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "[MouseButtonRelease: " << get_button() << "]";

		return ss.str();
	}
};

class MouseMoveEvent: public WindowEvent
{
public:
	// TODO: Implement relative getters and Input namespace
	MouseMoveEvent(double x, double y)
		: m_x(x), m_y(y)
	//	m_rx(x - s_prev_x), m_ry(y - s_prev_y)
	{
	//	s_prev_x = x;
	//	s_prev_y = y;
	}

	double get_x() const { return m_x; }
	double get_y() const { return m_y; }
	
	// double get_relative_x() const { return m_rx; }
	// double get_relative_y() const { return m_ry; }

	EVENT_CLASS_TYPE(MouseMove)
	
	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "[MouseMove: " << m_x << ", " << m_y << "]";

		return ss.str();
	}
private:
	double m_x, m_y;
	// double m_rx, m_ry; // relative position
	// static double s_prev_x, s_prev_y;
};

class MouseScrollEvent: public WindowEvent
{
public:
	MouseScrollEvent(double x_offset, double y_offset)
		: m_x_offset(x_offset), m_y_offset(y_offset)
	{}

	double get_x_offset() const { return m_x_offset; }
	double get_y_offset() const { return m_y_offset; }

	EVENT_CLASS_TYPE(MouseScroll)
	
	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "[MouseScroll: " << m_x_offset << ", " << m_y_offset << "]";

		return ss.str();
	}
private:
	double m_x_offset, m_y_offset;
};
