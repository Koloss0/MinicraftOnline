#include <engine/core/gui_application.hpp>
#include <engine/core/application.hpp>
#include <engine/core/window.hpp>
#include <engine/events/event.hpp>
#include <engine/events/window_event.hpp>
#include <engine/renderer/renderer.hpp>

#include <memory>

namespace Engine
{
	GUIApplication::GUIApplication(unsigned int width, unsigned int height, const char* title)
		: Application(), m_window{}
	{
		ASSERT(m_window == nullptr, "Application may only have one window instance");

		m_window = std::make_unique<Window>(width, height, title);

		m_window->set_event_callback(std::bind(&Application::on_event,
						this, std::placeholders::_1));
		
		Renderer::init(width, height);
	}

	GUIApplication::~GUIApplication()
	{
		Renderer::shutdown();
	}

	void GUIApplication::on_update(double delta)
	{
		Application::on_update(delta);

		Renderer::begin();

		for (Layer* layer : m_layer_stack)
		{
			layer->on_draw(delta);
		}

		Renderer::end();


		if (m_window)
			m_window->on_update();
	}

	void GUIApplication::on_event(Event& event)
	{
		Application::on_event(event);

		EventDispatcher dispatcher(event);

		dispatcher.dispatch<WindowResizeEvent>(std::bind(&GUIApplication::on_window_resize,
					this, std::placeholders::_1));
		dispatcher.dispatch<WindowCloseEvent>(std::bind(&GUIApplication::on_window_close,
					this, std::placeholders::_1));
	}

	bool GUIApplication::on_window_resize(WindowResizeEvent& event)
	{
		if (m_window)
			Renderer::update_viewport_size(event.get_width(), event.get_height());
		return false;
	}

	bool GUIApplication::on_window_close(WindowCloseEvent& event)
	{
		shutdown();
		return false;
	}
}
