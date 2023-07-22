#include <base/base_game_layer.hpp>

#include "base/config.hpp"
#include "base/components.hpp"

#include "engine/ecs/entity.hpp"
#include "engine/ecs/scene.hpp"
#include <engine/core/log.hpp>
#include <engine/renderer/renderer.hpp>
#include <engine/core/application.hpp>
#include <engine/core/gui_application.hpp>
#include <engine/ecs/components.hpp>
#include <engine/events/event.hpp>
#include <engine/events/window_event.hpp>
#include <engine/math/math.hpp>

#include <glm/ext/vector_float2.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include <memory>

GameLayer::GameLayer()
	: m_scene(std::make_shared<Engine::Scene>()),
	  m_player_system(m_scene),
	  m_tilemap_system(m_scene),
	  m_tilemap(m_scene->new_entity()),
	  m_players(),
	  m_player_infos()
{}

void GameLayer::on_attach()
{
	LOG_INFO("Starting Minicraft Online v{0}.{1}", MCO_VERSION_MAJOR, MCO_VERSION_MINOR);
	
	Engine::Math::randomise();
	
	// CREATE TILEMAP COMPONENTS
	auto* tilemap_component = m_tilemap.assign<Engine::TilemapComponent>();
	m_tilemap.assign<Engine::PositionComponent>();

	m_tilemap_system.generate_chunks(*tilemap_component, 0, 0, 0, 0);
}

void GameLayer::on_detach()
{}

void GameLayer::on_update(double delta)
{
	m_player_system.on_update(delta);
}

void GameLayer::on_draw(double delta)
{
	m_tilemap_system.on_update(delta);
}

void GameLayer::register_player(const PlayerInfo& info)
{
	if (m_players.find(info.uid) != m_players.end())
		return;

	Engine::Entity player = m_scene->new_entity();
	
	auto* player_component = player.assign<PlayerComponent>();

	auto* player_pos = player.assign<Engine::PositionComponent>();
	player_pos->x = info.x;
	player_pos->y = info.y;

	m_players[info.uid] = player;
	m_player_infos[info.uid] = info;
}

void GameLayer::deregister_player(Engine::PlayerUID uid)
{
	if (m_players.find(uid) == m_players.end())
		return;

	m_scene->destroy_entity(m_players[uid]);
	m_players.erase(uid);
	m_player_infos.erase(uid);
}
