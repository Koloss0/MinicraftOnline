#pragma once

#include <base/base_game_layer.h>

#include <engine/ecs/systems/sprite_animation_system.h>
#include <engine/ecs/systems/sprite_system.h>

class ClientGameLayer : public GameLayer
{
public:
	ClientGameLayer();
	virtual ~ClientGameLayer() = default;

	virtual void on_attach() override;
	virtual void on_detach() override;
	virtual void on_update(double delta) override;
	virtual void on_draw(double delta) override;

private:
	SpriteAnimationSystem m_sprite_animation_system;
	SpriteSystem m_sprite_system;
};
