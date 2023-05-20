#include "application.h"
#include <assert.h>
#include "log.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

Application* Application::s_instance = nullptr;

Application::Application()
	: m_window{}
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

Application::~Application()
{}

void Application::run()
{}

void Application::on_key_input(int key)
{}

void Application::on_window_resize(unsigned int width, unsigned int height)
{}
