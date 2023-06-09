#include "application.h"
#include "log.h"
#include <src/core/window.h>
#include <src/events/window_event.h>
#include <src/renderer/renderer.h>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr const char* WINDOW_TITLE = "Minicraft Online";

Application* Application::s_instance = nullptr;

Application::Application()
	: m_window{}, m_layer_stack{}, m_running{true}
{
	ASSERT(s_instance == nullptr, "Attempt to create more than one Application instance");

	s_instance = this;

	m_window = std::make_unique<Window>(WINDOW_WIDTH,
				WINDOW_HEIGHT, WINDOW_TITLE);

	m_window->set_event_callback(std::bind(&Application::on_event,
					this, std::placeholders::_1));
	
	Renderer::init(WINDOW_WIDTH, WINDOW_HEIGHT);
}

Application::~Application()
{
	Renderer::shutdown();
}

void Application::push_layer(Layer* layer)
{
	m_layer_stack.push_layer(layer);
}

void Application::run()
{
	// delta time variables
	double delta_time = 0.0;
	double last_frame_time = 0.0;

	while (m_running)
	{
		// calculate delta time
		double time = glfwGetTime();
		delta_time = time - last_frame_time;
		last_frame_time = time;

		for (Layer* layer : m_layer_stack)
		{
			layer->on_update(delta_time);
		}

		m_window->on_update();
	}
}

void Application::on_event(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.dispatch<WindowResizeEvent>(std::bind(&Application::on_window_resize,
				this, std::placeholders::_1));
	dispatcher.dispatch<WindowCloseEvent>(std::bind(&Application::on_window_close,
				this, std::placeholders::_1));

	for (Layer* layer : m_layer_stack)
	{
		if (event.handled)
			break;

		layer->on_event(event);
	}
}

bool Application::on_window_resize(WindowResizeEvent& event)
{
	Renderer::update_viewport_size(event.get_width(), event.get_height());
	return false;
}

bool Application::on_window_close(WindowCloseEvent& event)
{
	m_running = false;
	return false;
}
