#pragma once

#include "window.h"
#include "layer_stack.h"

int main(int argc, char** argv);

class Application
{
public:
	Application();
	virtual ~Application() = default;
	
	static Application& get() { return *s_instance; }

	void push_layer(Layer* layer);
private:
	// TODO: make these functions non-virtual
	virtual void run();
	void on_window_resize(unsigned int width, unsigned int height);
	virtual void on_key_input(int key);
protected:
	std::unique_ptr<Window> m_window;

private:
	LayerStack m_layer_stack;
	
	static Application* s_instance;
	
	friend int ::main(int argc, char** argv);
};
