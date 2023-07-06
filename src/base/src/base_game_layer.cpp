#include <base/base_game_layer.hpp>

#include "base/config.hpp"
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

GameLayer::GameLayer()
	: m_player_palette(),
	  m_scene(),
	  m_player_system(m_scene),
	  m_tilemap_system(m_scene),
	  m_tilemap(0),
	  m_player(0)
{}

void GameLayer::on_attach()
{
	LOG_INFO("Starting Minicraft Online v{0}.{1}", MCO_VERSION_MAJOR, MCO_VERSION_MINOR);
	
	Math::randomise(); // randomise the RNG
	
	// CREATE TILEMAP
	m_tilemap = m_scene.new_entity();
	
	TilemapComponent* tilemap_component =
		m_scene.assign_component<TilemapComponent>(m_tilemap);
	m_scene.assign_component<PositionComponent>(m_tilemap);

	m_tilemap_system.generate_chunks(*tilemap_component, 0, 0, 0, 0);

	// CREATE PLAYER ENTITY
	m_player = m_scene.new_entity();
	m_scene.assign_component<PlayerComponent>(m_player);
	PositionComponent* player_pos =
		m_scene.assign_component<PositionComponent>(m_player);
	player_pos->x = SPAWN_POS.x;
	player_pos->y = SPAWN_POS.y;
}

void GameLayer::on_detach()
{
	m_player_palette.reset();
}

void GameLayer::on_update(double delta)
{
	m_player_system.on_update(delta);
}

void GameLayer::on_draw(double delta)
{
	m_tilemap_system.on_update(delta);
}

