#pragma once

#include "engine/net/message.hpp"
#include <engine/events/event.hpp>
#include <engine/events/window_event.hpp>
#include <engine/events/network_event.hpp>

namespace Engine
{
	class Layer
	{
	public:
		Layer() = default;
		virtual ~Layer() = default;

		virtual void on_attach() = 0;
		virtual void on_detach() = 0;
		virtual void on_update(double delta) = 0;
		virtual void on_draw(double delta) = 0;
		virtual void on_event(Event& event);
		virtual bool on_key_pressed(KeyPressEvent& event);
	};
}
