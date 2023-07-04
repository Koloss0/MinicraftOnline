#include <engine/core/layer.h>
#include <engine/events/event.h>
#include <engine/events/window_event.h>
#include <engine/events/network_event.h>

#include <functional>

void Layer::on_event(Event& event)
{
	EventDispatcher dispatcher(event);

	dispatcher.dispatch<KeyPressEvent>(std::bind(&Layer::on_key_press,
				this, std::placeholders::_1));
	/*
	dispatcher.dispatch<engine::ClientMessageEvent<int>>(std::bind(&Layer::on_client_message_recieved,
				this, std::placeholders::_1)); */ // TODO: Fix!
}

bool Layer::on_key_press(KeyPressEvent& event)
{
	return false;
}

void Layer::on_client_message_recieved(engine::ClientMessageEvent<int>& event)
{}
