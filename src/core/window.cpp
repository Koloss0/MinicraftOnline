#include "window.h"
#include "log.h"
#include <src/renderer/renderer.h>
#include <GLFW/glfw3.h>
#include <exception>

static uint8_t s_window_count = 0;

Window::Window(unsigned int width, unsigned int height, const char* title)
	: m_window(nullptr), m_event_callback()
{
	if (s_window_count == 0)
	{
		LOG_INFO("Initialising GLFW");

		// INIT GLFW
		if (!glfwInit())
		{
			LOG_ERROR("Failed to initialise GLFW");
			throw std::runtime_error("Failed to initialise GLFW");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	}

	// CREATE A GLFW WINDOW
	m_window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title, nullptr, nullptr);
	if (m_window == nullptr)
	{
		LOG_ERROR("Failed to create window");
		glfwTerminate();
		throw std::runtime_error("Failed to create window");
	}

	if (s_window_count == 0)
	{
		glfwMakeContextCurrent(m_window);
		LOG_INFO("Initialising GLAD");
		// LOAD GLAD
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		{
			LOG_ERROR("Failed to load GLAD");
			glfwTerminate();
			throw std::runtime_error("Failed to load GLAD");
		}
	}

	glfwSetWindowUserPointer(m_window, this);

	glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int w, int h)
	{
		Window& win = *static_cast<Window*>(glfwGetWindowUserPointer(window));

		WindowResizeEvent e(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
	
		Renderer::update_viewport_size(
				static_cast<unsigned int>(w),
				static_cast<unsigned int>(h)
		);

		win.m_event_callback(e);
	});

	glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Window& win = *static_cast<Window*>(glfwGetWindowUserPointer(window));

		WindowKeyEvent e(key);
		win.m_event_callback(e);
	});
	

	if (s_window_count == 0)
	{
		Renderer::init(width, height);
	}

	s_window_count++;
}

Window::~Window()
{
	glfwDestroyWindow(m_window);
	s_window_count--;

	if (s_window_count == 0)
	{
		Renderer::shutdown();
		glfwTerminate();
	}
}

std::unique_ptr<Window> Window::create(unsigned int width, unsigned int height, const char* title)
{
	return std::make_unique<Window>(width, height, title);
}
