#pragma once

#include <base/base_game_layer.h>

class ServerGameLayer : public GameLayer
{
public:
	ServerGameLayer();
	virtual ~ServerGameLayer() = default;

	virtual void on_attach() override;
	virtual void on_detach() override;
	virtual void on_update(double delta) override;
	virtual void on_draw(double delta) override;
};
