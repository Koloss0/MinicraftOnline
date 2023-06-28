#pragma once

#include "window.h"
#include "layer_stack.h"

int main(int argc, char** argv);

class Application
{
public:
	Application();
	virtual ~Application();
	
	inline static Application& get() { return *s_instance; }

	virtual void on_event(Event& e);

	void shutdown();

protected:
	void push_layer(Layer* layer);
	virtual void on_update(double delta);

	LayerStack m_layer_stack;
private:
	void run();

	bool m_running;
	
	static Application* s_instance;
	
	friend int ::main(int argc, char** argv);
};

Application* create_application();
