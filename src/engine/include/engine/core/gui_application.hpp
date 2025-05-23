#pragma once

#include "application.hpp"
#include "engine/renderer/camera.hpp"

#include <memory>

namespace Engine
{
	class GUIApplication : public Application
	{
	public:
		GUIApplication(unsigned int width, unsigned int height, const char* title = "My Game");
		virtual ~GUIApplication() override;

		inline Window& get_window() { return *m_window; }
		inline void set_camera(Camera camera) { m_camera = camera; }
		
		virtual void on_event(Event& e) override;

	protected:
		virtual void on_update(double delta) override;

	private:
		bool on_window_resize(WindowResizeEvent& event);
		bool on_window_close(WindowCloseEvent& event);

		std::unique_ptr<Window> m_window;
		Camera m_camera;
	};
}
