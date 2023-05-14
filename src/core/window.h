#pragma once

#include <src/core/log.h>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// TODO: make this prettier
struct WindowEvent
{
	WindowEvent() {};
	virtual ~WindowEvent() = default;
};

struct WindowResizeEvent: public WindowEvent
{
	unsigned int width, height;
	
	WindowResizeEvent(unsigned int w, unsigned int h)
		: width(w), height(h)
	{}
	
	~WindowResizeEvent()
	{}
};

struct WindowKeyEvent: public WindowEvent
{
	int key;
		
	WindowKeyEvent(int k)
		: key(k)
	{}
	
	~WindowKeyEvent()
	{}
};

class Window
{
public:
	using event_callback_fn = std::function<void(WindowEvent&)>;
	Window(unsigned int width, unsigned int height, const char* title);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window();

	inline void set_event_callback(const event_callback_fn& callback)
	{
		LOG_INFO("set event callback");
		m_event_callback = callback;
	};

	// TODO: consider deleting these functions
	inline bool should_close() { return glfwWindowShouldClose(m_window); };
	inline bool is_key_pressed(int k) { return glfwGetKey(m_window, k); }
	void swap_buffers()
	{
		glfwSwapBuffers(m_window);
	}
	
	static std::unique_ptr<Window> create(unsigned int width, unsigned int height, const char* title);
private:
	GLFWwindow* m_window;
	event_callback_fn m_event_callback;
};
