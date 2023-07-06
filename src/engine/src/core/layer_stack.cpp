#include <engine/core/layer_stack.hpp>

LayerStack::LayerStack()
	: m_layers()
{}

LayerStack::~LayerStack()
{
	for (Layer* layer : m_layers)
	{
		layer->on_detach();
		delete layer;
	}
}

void LayerStack::push_layer(Layer* layer)
{
	m_layers.emplace_back(layer);
	layer->on_attach();
}
