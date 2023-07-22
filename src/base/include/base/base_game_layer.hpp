#pragma once

#include "systems/player_system.hpp"

#include "base/network.hpp"

#include "engine/net/player_uid.hpp"
#include <engine/core/layer.hpp>
#include "engine/ecs/entity.hpp"
#include <engine/ecs/systems/tilemap_system.hpp>
#include <engine/ecs/scene.hpp>

#include <memory>
#include <unordered_map>

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
	virtual void register_player(const PlayerInfo& info);
	virtual void deregister_player(Engine::PlayerUID uid);

	static constexpr glm::ivec2 SPAWN_POS{128, 72};
	static constexpr int PLAYER_SPEED = 1;
	
	std::shared_ptr<Engine::Scene> m_scene;
	PlayerSystem m_player_system;
	Engine::TilemapSystem m_tilemap_system;

	Engine::Entity m_tilemap;
	std::unordered_map<Engine::PlayerUID, Engine::Entity> m_players;
	std::unordered_map<Engine::PlayerUID, PlayerInfo> m_player_infos;
};
