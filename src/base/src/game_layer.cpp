#include "game_layer.h"

#include "base/config.h"
#include <engine/core/log.h>
#include <engine/renderer/renderer.h>
#include <engine/core/application.h>
#include <engine/core/gui_application.h>
#include <engine/ecs/components.h>
#include <engine/events/event.h>
#include <engine/events/window_event.h>
#include <engine/math/math.h>

#include <glm/ext/vector_float2.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

static const glm::ivec2 SPAWN_POS( Renderer::VIEWPORT_WIDTH/2, Renderer::VIEWPORT_HEIGHT/2);

static constexpr int PLAYER_SPEED = 1;

GameLayer::GameLayer()
	: m_player_palette(),
	  m_scene(),
	  m_player_system(m_scene),
	  m_tilemap_system(m_scene),
	  m_sprite_animation_system(m_scene),
	  m_sprite_system(m_scene),
	  m_tilemap(0),
	  m_player(0)
{}

void GameLayer::on_attach()
{
	LOG_INFO("Starting Minicraft Online v{0}.{1}", MCO_VERSION_MAJOR, MCO_VERSION_MINOR);
	
	Math::randomise(); // randomise the RNG
#ifndef COMPILE_SERVER
	// CREATE PLAYER PALETTE
	m_player_palette = std::make_shared<Texture>();
	m_player_palette->load(Image::create_palette(
				{0xff010101,0xff004179,0xffa2dbff,0x0}));
	
	// LOAD TILE PALETTE
	std::shared_ptr<Texture> tile_palette_atlas = std::make_shared<Texture>();
	tile_palette_atlas->load(ImageLoader::load(
				"assets/images/maps/palettes/tiles.png", false));
#endif
	// CREATE TILEMAP
	EntityID tilemap = m_scene.new_entity();
	
	TilemapComponent* tilemap_component =
		m_scene.assign_component<TilemapComponent>(tilemap);
	
#ifndef COMPILE_SERVER
	TilemapRendererComponent* tilemap_renderer_component =
		m_scene.assign_component<TilemapRendererComponent>(tilemap);
	tilemap_renderer_component->palette_atlas = tile_palette_atlas;
#endif
	
	m_scene.assign_component<PositionComponent>(tilemap);

	m_tilemap_system.generate_chunks(*tilemap_component, 0, 0, 0, 0);

	// CREATE PLAYER ENTITY
	m_player = m_scene.new_entity();
	PositionComponent* player_pos =
		m_scene.assign_component<PositionComponent>(m_player);
	player_pos->x = SPAWN_POS.x;
	player_pos->y = SPAWN_POS.y;

#ifndef COMPILE_SERVER
	SpriteComponent* player_sprite =
		m_scene.assign_component<SpriteComponent>(m_player);
	player_sprite->rect.x = -TILE_SIZE_PX / 2;
	player_sprite->rect.y = -TILE_SIZE_PX / 2;
	player_sprite->rect.width = TILE_SIZE_PX;
	player_sprite->rect.height = TILE_SIZE_PX;
	player_sprite->source_rect.width = 16;
	player_sprite->source_rect.height = 16;
	player_sprite->source_rect.x = 0;
	player_sprite->source_rect.y = -16;
	player_sprite->palette_atlas = m_player_palette;
	player_sprite->palette_index = 0;
	
	const std::vector<IntRect> WALK_ANIMATION{
		{0,  -16, 16,  16},
		{16, -16, -16, 16}
	};

	SpriteAnimatorComponent* player_animator =
		m_scene.assign_component<SpriteAnimatorComponent>(m_player);
	player_animator->frames = WALK_ANIMATION;
	player_animator->frame_duration = 0.25;
#endif

	m_scene.assign_component<PlayerComponent>(m_player);
}

void GameLayer::on_detach()
{
	m_player_palette.reset();
}

void GameLayer::on_update(double delta)
{
#ifndef COMPILE_SERVER
	// process input
	Window& window = static_cast<GUIApplication&>(Application::get()).get_window();

	PositionComponent* pos =
		m_scene.get_component<PositionComponent>(m_player);

	glm::ivec2 dir(0);
	
	if (window.is_key_pressed(GLFW_KEY_W))
	{
		dir.y += PLAYER_SPEED;
	}
	if (window.is_key_pressed(GLFW_KEY_S))
	{
		dir.y -= PLAYER_SPEED;
	}
	if (window.is_key_pressed(GLFW_KEY_D))
	{
		dir.x += PLAYER_SPEED;
	}
	if (window.is_key_pressed(GLFW_KEY_A))
	{
		dir.x -= PLAYER_SPEED;
	}
	
	if (glm::length(glm::vec2(dir)) > 0)
	{
		pos->x += dir.x*PLAYER_SPEED;
		pos->y += dir.y*PLAYER_SPEED;
	}
#endif
	
	m_player_system.on_update(delta);
	
}

void GameLayer::on_draw(double delta)
{
	m_tilemap_system.on_update(delta);
	m_sprite_animation_system.on_update(delta);
	m_sprite_system.on_update(delta);
}

