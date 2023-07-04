#pragma once

#include <engine/events/event.h>
#include <engine/events/window_event.h>
#include <engine/events/network_event.h>

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
	virtual void on_client_message_recieved(engine::ClientMessageEvent<int>& event);
};
