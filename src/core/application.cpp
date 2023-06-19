#include "application.h"
#include "log.h"

Application* Application::s_instance = nullptr;

Application::Application()
	: m_layer_stack{}, m_running{true}
{
	ASSERT(s_instance == nullptr, "Attempt to create more than one Application instance");

	s_instance = this;
}

Application::~Application()
{
	shutdown();
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

		on_update(delta_time);

	}
}

void Application::on_update(double delta)
{
	for (Layer* layer : m_layer_stack)
	{
		layer->on_update(delta);
	}
}

void Application::shutdown()
{
	m_running = false;
}

void Application::on_event(Event& event)
{
	EventDispatcher dispatcher(event);

	for (Layer* layer : m_layer_stack)
	{
		if (event.handled)
			break;

		layer->on_event(event);
	}
}
