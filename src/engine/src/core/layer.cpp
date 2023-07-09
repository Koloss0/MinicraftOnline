#include "engine/events/window_event.hpp"
#include <engine/events/event.hpp>
#include <engine/core/layer.hpp>
#include <functional>

namespace Engine
{
	void Layer::on_event(Event &event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.dispatch<KeyPressEvent>(std::bind(&Layer::on_key_pressed,
					this, std::placeholders::_1));
	}

	bool Layer::on_key_pressed(KeyPressEvent &event)
	{
		return false;
	}
}
