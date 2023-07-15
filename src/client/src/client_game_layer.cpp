#include "client_game_layer.hpp"
#include "base/base_game_layer.hpp"

#include <base/network.hpp>

#include <engine/core/log.hpp>
#include <engine/renderer/renderer.hpp>
#include <engine/core/application.hpp>
#include <engine/core/gui_application.hpp>
#include <engine/ecs/components.hpp>
#include <engine/events/event.hpp>
#include <engine/events/window_event.hpp>
#include <engine/events/network_event.hpp>
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
	std::shared_ptr<Engine::Texture> tile_palette_atlas = std::make_shared<Engine::Texture>();
	tile_palette_atlas->load(Engine::ImageLoader::load(
				"assets/images/maps/palettes/tiles.png", false));
	Engine::TilemapRendererComponent* tilemap_renderer_component =
		m_scene.assign_component<Engine::TilemapRendererComponent>(m_tilemap);
	tilemap_renderer_component->palette_atlas = tile_palette_atlas;

	// CREATE PLAYER SPRITE
	Engine::SpriteComponent* player_sprite =
		m_scene.assign_component<Engine::SpriteComponent>(m_player);
	player_sprite->rect.x = -Engine::TILE_SIZE_PX / 2; // TODO: Engine::TILE_SIZE_PX should be decided by user. Make it extern in engine.
	player_sprite->rect.y = -Engine::TILE_SIZE_PX / 2;
	player_sprite->rect.width = Engine::TILE_SIZE_PX;
	player_sprite->rect.height = Engine::TILE_SIZE_PX;
	player_sprite->source_rect.width = 16;
	player_sprite->source_rect.height = 16;
	player_sprite->source_rect.x = 0;
	player_sprite->source_rect.y = -16;
	player_sprite->palette_index = 0;

	m_player_palette = Engine::Image::create_palette({0xff010101,0xff004179,0xffa2dbff,0x0});
	
	std::shared_ptr<Engine::Texture> player_palette_tex = std::make_shared<Engine::Texture>();
	player_palette_tex->load(m_player_palette);
	player_sprite->palette_atlas = player_palette_tex;
	
	const std::vector<Engine::IntRect> WALK_ANIMATION{
		{0,  -16, 16,  16},
		{16, -16, -16, 16}
	};

	Engine::SpriteAnimatorComponent* player_animator =
		m_scene.assign_component<Engine::SpriteAnimatorComponent>(m_player);
	player_animator->frames = WALK_ANIMATION;
	player_animator->frame_duration = 0.25;

	LOG_INFO("Connecting to server...");
	Engine::Application& app = Engine::Application::get();
	app.create_client<MESSAGES>();
	Engine::Client<MESSAGES>& client = dynamic_cast<Engine::Client<MESSAGES>&>(app.get_network_device());
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
	Engine::Window& window = static_cast<Engine::GUIApplication&>(Engine::Application::get()).get_window();

	Engine::PositionComponent* pos =
		m_scene.get_component<Engine::PositionComponent>(m_player);

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

	Engine::GUIApplication& app = dynamic_cast<Engine::GUIApplication&>(Engine::Application::get());
	app.set_camera({pos->x - Engine::Renderer::VIEWPORT_WIDTH/2, pos->y - Engine::Renderer::VIEWPORT_HEIGHT/2});
}

void ClientGameLayer::on_draw(double delta)
{
	GameLayer::on_draw(delta);

	m_sprite_animation_system.on_update(delta);
	m_sprite_system.on_update(delta);
}

void ClientGameLayer::on_event(Engine::Event& event)
{
	GameLayer::on_event(event);

	Engine::EventDispatcher dispatcher(event);
	
	dispatcher.dispatch<Engine::ConnectedToServerEvent>(std::bind(
				&ClientGameLayer::on_connected_to_server,
				this, std::placeholders::_1));

	dispatcher.dispatch<Engine::ClientConnectedToServerEvent>(std::bind(
				&ClientGameLayer::on_client_connected_to_server,
				this, std::placeholders::_1));
	
	dispatcher.dispatch<Engine::ClientDisconnectedEvent>(std::bind(
				&ClientGameLayer::on_client_disconnected,
				this, std::placeholders::_1));
	
	dispatcher.dispatch<Engine::FailedToConnectEvent>(std::bind(
				&ClientGameLayer::on_failed_to_connect,
				this, std::placeholders::_1));
	
	dispatcher.dispatch<Engine::ServerMessageEvent>(std::bind(
				&ClientGameLayer::on_server_message,
				this, std::placeholders::_1));

	dispatcher.dispatch<Engine::ClientMessageEvent>(std::bind(
				&ClientGameLayer::on_client_message,
				this, std::placeholders::_1));
	
	dispatcher.dispatch<Engine::ServerDisconnectedEvent>(std::bind(
				&ClientGameLayer::on_server_disconnected,
				this, std::placeholders::_1));
	
}

bool ClientGameLayer::on_connected_to_server(Engine::ConnectedToServerEvent& event)
{
	LOG_INFO("connected to server");
	return false;
}

bool ClientGameLayer::on_client_connected_to_server(Engine::ClientConnectedToServerEvent& event)
{
	LOG_INFO("client joined");
	return false;
}

bool ClientGameLayer::on_client_disconnected(Engine::ClientDisconnectedEvent& event)
{
	LOG_INFO("client left");
	return false;
}

bool ClientGameLayer::on_failed_to_connect(Engine::FailedToConnectEvent& event)
{
	LOG_INFO("failed to connect to server. Reason: {0}", event.get_info());
	return false;
}

bool ClientGameLayer::on_server_message(Engine::ServerMessageEvent& event)
{
	LOG_INFO("server says: {0}", event.to_string());
	return false;
}

bool ClientGameLayer::on_client_message(Engine::ClientMessageEvent& event)
{
	LOG_INFO("client says: {0}", event.to_string());
	return false;
}

bool ClientGameLayer::on_server_disconnected(Engine::ServerDisconnectedEvent& event)
{
	LOG_INFO("server disconnected. Reason: {0}", event.get_info());
	return false;
}
