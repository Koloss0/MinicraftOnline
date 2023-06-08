#pragma once

#include "window.h"
#include "layer_stack.h"

int main(int argc, char** argv);

class Application
{
public:
	Application();
	~Application();
	
	static Application& get() { return *s_instance; }

	Window& get_window() { return *m_window; }

	void push_layer(Layer* layer);

	void on_event(Event& e);
private:
	void run();
	bool on_window_resize(WindowResizeEvent& event);
	bool on_window_close(WindowCloseEvent& event);
	
	std::unique_ptr<Window> m_window;
	LayerStack m_layer_stack;
	bool m_running;
	
	static Application* s_instance;
	
	friend int ::main(int argc, char** argv);
};

Application* create_application();
