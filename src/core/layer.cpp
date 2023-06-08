#include "layer.h"
#include <src/events/event.h>
#include <src/events/window_event.h>

void Layer::on_event(Event& event)
{
	EventDispatcher dispatcher(event);

	dispatcher.dispatch<KeyPressEvent>(std::bind(&Layer::on_key_press,
				this, std::placeholders::_1));
}

bool Layer::on_key_press(KeyPressEvent& event)
{
	return false;
}
