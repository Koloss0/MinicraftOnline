#pragma once

#include <engine/core/layer.hpp>

#include "systems/player_system.hpp"
#include <engine/ecs/systems/tilemap_system.hpp>
#include <engine/ecs/scene.hpp>

class GameLayer : public Engine::Layer
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
	
	std::shared_ptr<Engine::Image> m_player_palette;
	
	Engine::Scene m_scene;
	PlayerSystem m_player_system;
	Engine::TilemapSystem m_tilemap_system;

	Engine::EntityID m_tilemap;
	Engine::EntityID m_player;
};
