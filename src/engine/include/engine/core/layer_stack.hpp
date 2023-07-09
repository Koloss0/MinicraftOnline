#pragma once

#include "layer.hpp"

#include <vector>

namespace Engine
{
	class LayerStack
	{
		std::vector<Layer*> m_layers;
	public:
		LayerStack();
		~LayerStack();

		void push_layer(Layer* layer);

		std::vector<Layer*>::iterator begin() { return m_layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_layers.end(); }
	};
}
