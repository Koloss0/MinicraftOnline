#include "client_game_layer.hpp"

#include "base/components.hpp"
#include "base/network.hpp"
#include "engine/ecs/entity.hpp"
#include "engine/net/client.hpp"
#include "engine/net/message.hpp"
#include "engine/net/player_uid.hpp"
#include "engine/renderer/colour.hpp"
#include <base/network.hpp>

#include <cstdint>
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
#include <bits/types/time_t.h>

static const std::string SERVER_IP = "192.168.100.109";//"127.0.0.1";//
static constexpr u_int16_t SERVER_PORT = 7777;

ClientGameLayer::ClientGameLayer()
	: GameLayer(),
	  m_my_uid(0),
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
	auto* tilemap_renderer_component = m_tilemap.assign<Engine::TilemapRendererComponent>();
	tilemap_renderer_component->palette_atlas = tile_palette_atlas;
	
	LOG_INFO("Connecting to server...");
	Engine::Application& app = Engine::Application::get();
	app.create_client<MessageID>();
	Engine::Client<MessageID>& client = dynamic_cast<Engine::Client<MessageID>&>(app.get_network_device());
	ASSERT(client.connect_to_host(SERVER_IP, SERVER_PORT), "Failed to connect.");
	
	m_my_uid = client.get_network_unique_id();
	
	// SETUP MY INFO
	PlayerInfo my_info;
	my_info.uid = m_my_uid;
	my_info.outline_colour = 0xff000000 | static_cast<Engine::colour>(Engine::Math::randomi()); // 0xff010101;
	my_info.fill_colour_1 = 0xff000000 | static_cast<Engine::colour>(Engine::Math::randomi()); // 0xff004179;
	my_info.fill_colour_2 = 0xff000000 | static_cast<Engine::colour>(Engine::Math::randomi()); // 0xffa2dbff;
	my_info.x = SPAWN_POS.x;
	my_info.y = SPAWN_POS.y;

	register_player(my_info);
}

void ClientGameLayer::on_detach()
{
	GameLayer::on_detach();
}

void ClientGameLayer::on_update(double delta)
{
	GameLayer::on_update(delta);
	
	Engine::GUIApplication& app = dynamic_cast<Engine::GUIApplication&>(Engine::Application::get());

	// process input
	Engine::Window& window = app.get_window();

	auto* pos = m_players[m_my_uid].get<Engine::PositionComponent>();

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

		Engine::Client<MessageID>& client = dynamic_cast<Engine::Client<MessageID>&>(app.get_network_device());
		Engine::Message update_msg;
		update_msg << pos->x << pos->y;
		update_msg << MessageID::UPDATE_PLAYER;
		client.send(update_msg);
	}

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
	LOG_INFO("connected to server.");
	Engine::Client<MessageID>& client =
		dynamic_cast<Engine::Client<MessageID>&>(Engine::Application::get().get_network_device());

	Engine::Message msg;
	msg << m_player_infos[m_my_uid];
	msg << MessageID::REGISTER_PLAYER;

	client.send(msg);
	return false;
}

bool ClientGameLayer::on_client_connected_to_server(Engine::ClientConnectedToServerEvent& event)
{
	LOG_INFO("{0} joined.", static_cast<uint32_t>(event.get_player_uid()));
	return false;
}

bool ClientGameLayer::on_client_disconnected(Engine::ClientDisconnectedEvent& event)
{
	LOG_INFO("{0} left.", static_cast<uint32_t>(event.get_player_uid()));

	deregister_player(event.get_player_uid());
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

	Engine::Message msg = event.get_message();
	MessageID msg_id;
	msg >> msg_id;

	switch (msg_id)
	{
		case MessageID::REGISTER_PLAYER:
		{
			if (msg.size() == sizeof(PlayerInfo))
			{
				PlayerInfo info;
				msg >> info;

				register_player(info);
			}
			break;
		}
		case MessageID::UPDATE_PLAYER:
		{
			Engine::PlayerUID puid;
			int x, y;
			msg >> y >> x >> puid;
			Engine::PositionComponent* pos = m_players[puid].get<Engine::PositionComponent>();
			pos->x = x;
			pos->y = y;
			break;
		}
		default:
		{
			// intentionally left blank.
		}
	}

	return false;
}

bool ClientGameLayer::on_client_message(Engine::ClientMessageEvent& event)
{
	LOG_INFO("player {0} says: {1}", static_cast<uint32_t>(event.get_sender_uid()), event.to_string());
	return false;
}

bool ClientGameLayer::on_server_disconnected(Engine::ServerDisconnectedEvent& event)
{
	LOG_INFO("server disconnected. Reason: {0}", event.get_info());
	return false;
}

void ClientGameLayer::register_player(const PlayerInfo& info)
{
	if (m_players.find(info.uid) != m_players.end())
		return;

	GameLayer::register_player(info);

	Engine::Entity player = m_players[info.uid];
	
	// CREATE PLAYER SPRITE
	auto* sprite = player.assign<Engine::SpriteComponent>();
	sprite->rect.x = -Engine::TILE_SIZE_PX / 2; // TODO: Engine::TILE_SIZE_PX should be decided by user. Make it extern in engine.
	sprite->rect.y = -Engine::TILE_SIZE_PX / 2;
	sprite->rect.width = Engine::TILE_SIZE_PX;
	sprite->rect.height = Engine::TILE_SIZE_PX;
	sprite->source_rect.width = 16;
	sprite->source_rect.height = 16;
	sprite->source_rect.x = 0;
	sprite->source_rect.y = -16;
	sprite->palette_index = 0;

	auto palette = Engine::Image::create_palette({
		info.outline_colour,
		info.fill_colour_1,
		info.fill_colour_2,
		0x0
	});
	
	std::shared_ptr<Engine::Texture> palette_tex = std::make_shared<Engine::Texture>();
	palette_tex->load(palette);
	sprite->palette_atlas = palette_tex;
	
	const std::vector<Engine::IntRect> WALK_ANIMATION{
		{0,  -16, 16,  16},
		{16, -16, -16, 16}
	};

	auto* player_animator = player.assign<Engine::SpriteAnimatorComponent>();
	player_animator->frames = WALK_ANIMATION;
	player_animator->frame_duration = 0.25;
}
