#include "client_game_layer.hpp"

#include "client.hpp"

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

#include <string>
#include <stdint.h>

static const std::string SERVER_IP = "127.0.0.1";//"192.168.100.109";
static constexpr u_int16_t SERVER_PORT = 7777;

ClientGameLayer::ClientGameLayer()
	: GameLayer(),
	  m_sprite_animation_system(m_scene),
	  m_sprite_system(m_scene)
{}

void ClientGameLayer::on_attach()
{
	GameLayer::on_attach();

	// LOAD TILE PALETTE
	std::shared_ptr<Texture> tile_palette_atlas = std::make_shared<Texture>();
	tile_palette_atlas->load(ImageLoader::load(
				"assets/images/maps/palettes/tiles.png", false));
	TilemapRendererComponent* tilemap_renderer_component =
		m_scene.assign_component<TilemapRendererComponent>(m_tilemap);
	tilemap_renderer_component->palette_atlas = tile_palette_atlas;

	// CREATE PLAYER SPRITE
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
	player_sprite->palette_index = 0;

	m_player_palette = Image::create_palette({0xff010101,0xff004179,0xffa2dbff,0x0});
	
	std::shared_ptr<Texture> player_palette_tex = std::make_shared<Texture>();
	player_palette_tex->load(m_player_palette);
	player_sprite->palette_atlas = player_palette_tex;
	
	const std::vector<IntRect> WALK_ANIMATION{
		{0,  -16, 16,  16},
		{16, -16, -16, 16}
	};

	SpriteAnimatorComponent* player_animator =
		m_scene.assign_component<SpriteAnimatorComponent>(m_player);
	player_animator->frames = WALK_ANIMATION;
	player_animator->frame_duration = 0.25;

	LOG_INFO("Connecting to server...");
	Application& app = Application::get();
	app.create_client<MESSAGES>();
	Client& client = dynamic_cast<Client&>(app.get_network_device());
	ASSERT(client.connect_to_host(SERVER_IP, SERVER_PORT), "Failed to connect.");
}

void ClientGameLayer::on_detach()
{
	GameLayer::on_detach();
}

void ClientGameLayer::on_update(double delta)
{
	GameLayer::on_update(delta);

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
}

void ClientGameLayer::on_draw(double delta)
{
	GameLayer::on_draw(delta);

	m_sprite_animation_system.on_update(delta);
	m_sprite_system.on_update(delta);
}
