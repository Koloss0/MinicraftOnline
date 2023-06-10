#pragma once

#include <ecs/systems/sprite_animation_system.h>
#include <src/core/layer.h>
#include <src/events/window_event.h>
#include <src/renderer/texture.h>
#include <src/ecs/scene.h>
#include <src/core/common.h>
#include <ecs/systems/player_system.h>
#include <ecs/systems/tilemap_system.h>
#include <ecs/systems/sprite_system.h>

class GameLayer : public Layer
{
public:
	GameLayer();
	~GameLayer() = default;

	void on_attach() override;
	void on_detach() override;
	void on_update(double delta_time) override;

private:
	std::shared_ptr<Texture> m_player_palette;
	
	Scene m_scene;
	PlayerSystem m_player_system;
	TilemapSystem m_tilemap_system;
	SpriteAnimationSystem m_sprite_animation_system;
	SpriteSystem m_sprite_system;

	EntityID m_tilemap;
	EntityID m_player;
};
