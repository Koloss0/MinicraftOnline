#pragma once

class Layer
{
public:
	Layer() = default;
	virtual ~Layer() = default;

	virtual void on_attach() = 0;
	virtual void on_detach() = 0;
	virtual void on_update(double delta_time) = 0;
};
