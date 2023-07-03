#pragma once

#include <engine/core/layer.h>

#include "systems/player_system.h"
#include <engine/ecs/systems/tilemap_system.h>
#include <engine/ecs/scene.h>

class GameLayer : public Layer
{
public:
	GameLayer();
	virtual ~GameLayer() = default;

	virtual void on_attach() override;
	virtual void on_detach() override;
	virtual void on_update(double delta) override;
	virtual void on_draw(double delta) override;

protected:
	static constexpr glm::ivec2 SPAWN_POS{128, 72};
	static constexpr int PLAYER_SPEED = 1;
	
	std::shared_ptr<Image> m_player_palette;
	
	Scene m_scene;
	PlayerSystem m_player_system;
	TilemapSystem m_tilemap_system;

	EntityID m_tilemap;
	EntityID m_player;
};
