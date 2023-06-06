#include "application.h"
#include <assert.h>
#include "log.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

Application* Application::s_instance = nullptr;

Application::Application()
	: m_window{}, m_layer_stack{}
{
	assert(s_instance == nullptr);

	s_instance = this;

	try {
		m_window = Window::create(WINDOW_WIDTH, WINDOW_HEIGHT, "Minicraft Online");
		m_window->set_event_callback([this](WindowEvent& e) 
		{
			// TODO: put this code somewhere else
			WindowResizeEvent* wre = dynamic_cast<WindowResizeEvent*>(&e);
			if (wre)
				this->on_window_resize(static_cast<unsigned int>(wre->width), static_cast<unsigned int>(wre->height));

			WindowKeyEvent* wke = dynamic_cast<WindowKeyEvent*>(&e);
			if (wke)
				this->on_key_input(wke->key);
		});
	} catch (std::exception& e) {
		throw std::runtime_error("Failed to start application");
	}
	
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

	while (!m_window->should_close())
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

void Application::on_key_input(int key)
{}

void Application::on_window_resize(unsigned int width, unsigned int height)
{}
