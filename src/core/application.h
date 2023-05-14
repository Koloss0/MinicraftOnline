#pragma once

#include "window.h"

int main(int argc, char** argv);

class Application
{
public:
	Application();
	virtual ~Application();
	
	static Application& get() { return *s_instance; }

	// TODO: remove function
	inline bool should_close() { return m_window->should_close(); };
private:
	// TODO: make these functions non-virtual
	virtual void run();
	void on_window_resize(unsigned int width, unsigned int height);
	virtual void on_key_input(int key);

protected:
	std::unique_ptr<Window> m_window;

private:
	static Application* s_instance;
	
	friend int ::main(int argc, char** argv);
};
