#include "log.h"
#include "window.h"
#include <events/window_event.h>
#include <GLFW/glfw3.h>

static uint8_t s_window_count = 0;

static void GLFWErrorCallback(int error, const char* description)
{
	LOG_ERROR("GLFW Error ({0}): {1}", error, description);
}

Window::Window(unsigned int width, unsigned int height, const char* title) noexcept
	: m_window(nullptr), m_event_callback(), m_focused(true)
{
	if (s_window_count == 0)
	{
		LOG_INFO("Initialising GLFW");

		bool success = glfwInit();

		ASSERT(success, "Failed to initialise GLFW");

		glfwSetErrorCallback(GLFWErrorCallback);
	}

	// set hints for call to glfwCreateWindow
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	LOG_INFO("Creating GLFW window");

	m_window = glfwCreateWindow(static_cast<int>(width),
			static_cast<int>(height), title,
			nullptr, nullptr);

	ASSERT(m_window != nullptr, "Failed to create GLFW window");
	
	s_window_count++;

	if (s_window_count == 1)
	{
		glfwMakeContextCurrent(m_window);
		
		LOG_INFO("Loading GLAD");
		
		bool success =
			gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
		
		ASSERT(success, "Failed to load GLAD");
	}

	glfwSetWindowUserPointer(m_window, this);

	glfwSetFramebufferSizeCallback(m_window,
			[](GLFWwindow* window, int w, int h)
	{
		Window& win = *static_cast<Window*>(glfwGetWindowUserPointer(window));

		WindowResizeEvent e(static_cast<unsigned int>(w),
				static_cast<unsigned int>(h));
	
		win.m_event_callback(e);
	});

	glfwSetWindowCloseCallback(m_window,
			[](GLFWwindow* window) 
	{
		Window& win = *static_cast<Window*>(glfwGetWindowUserPointer(window));

		WindowCloseEvent e{};

		win.m_event_callback(e);
	});

	glfwSetKeyCallback(m_window, [](GLFWwindow* window,
				int key, int scancode, int action, int mods)
	{
		Window& win = *static_cast<Window*>(glfwGetWindowUserPointer(window));
		
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			KeyPressEvent e(key, action == GLFW_REPEAT);
			win.m_event_callback(e);
		}
		else if (action == GLFW_RELEASE)
		{
			KeyReleaseEvent e(key);
			win.m_event_callback(e);
		}
	});

	glfwSetMouseButtonCallback(m_window,
			[](GLFWwindow* window, int button, int action, int mods)
	{
		Window& win = *static_cast<Window*>(glfwGetWindowUserPointer(window));

		if (action == GLFW_PRESS)
		{
			MouseButtonPressEvent e(button);
			win.m_event_callback(e);
		}
		else
		{
			MouseButtonReleaseEvent e(button);
			win.m_event_callback(e);
		}
	});

	glfwSetCursorPosCallback(m_window,
			[](GLFWwindow* window, double x, double y)
	{
		Window& win = *static_cast<Window*>(glfwGetWindowUserPointer(window));
		
		MouseMoveEvent e(x, y);
		win.m_event_callback(e);
	});

	glfwSetScrollCallback(m_window,
			[](GLFWwindow* window, double x_offset, double y_offset)
	{
		Window& win = *static_cast<Window*>(glfwGetWindowUserPointer(window));
		
		MouseScrollEvent e(x_offset, y_offset);
		win.m_event_callback(e);
	});
}

Window::~Window()
{
	glfwDestroyWindow(m_window);
	s_window_count--;

	if (s_window_count == 0)
	{
		glfwTerminate();
	}
}

void Window::on_update()
{
	glfwSwapBuffers(m_window);
	glfwPollEvents();

	if (is_focused())
	{
		if (!m_focused)
		{
			m_focused = true;

			WindowFocusEvent e{};
			m_event_callback(e);
		}
	}
	else
	{
		if (m_focused)
		{
			m_focused = false;

			WindowLoseFocusEvent e{};
			m_event_callback(e);
		}
	}
	
	// check for GL errors
	GLenum error = glGetError();
	if (error)
		LOG_ERROR("GL ERROR: {0}", error);
}
