#pragma once

#include <src/events/event.h>
#include <src/events/window_event.h>

class Layer
{
public:
	Layer() = default;
	virtual ~Layer() = default;

	virtual void on_attach() = 0;
	virtual void on_detach() = 0;
	virtual void on_update(double delta) = 0;
	virtual void on_draw(double delta) = 0;
	void on_event(Event& event);
	virtual bool on_key_press(KeyPressEvent& event);
};
