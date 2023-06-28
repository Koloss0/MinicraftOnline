#pragma once

#include <core/log.h>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <events/event.h>

class Window
{
public:
	using event_callback_fn = std::function<void(Event&)>;

	Window(unsigned int width, unsigned int height, const char* title) noexcept;
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window();

	inline void set_event_callback(const event_callback_fn& callback) {
		m_event_callback = callback; };

	inline bool is_key_pressed(int k) const { return
		glfwGetKey(m_window, k); }

	inline bool is_focused() const { return
		glfwGetWindowAttrib(m_window, GLFW_FOCUSED); }

	void on_update();
private:
	GLFWwindow* m_window;
	event_callback_fn m_event_callback;
	bool m_focused;
};
