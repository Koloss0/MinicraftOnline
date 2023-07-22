#include "server_game_layer.hpp"
#include "base/base_game_layer.hpp"
#include "base/components.hpp"
#include "engine/ecs/entity.hpp"
#include "engine/net/message.hpp"

#include <base/network.hpp>

#include <engine/events/network_event.hpp>
#include <engine/net/server.hpp>
#include <engine/core/application.hpp>
#include <engine/core/log.hpp>

static constexpr int PORT = 7777;

ServerGameLayer::ServerGameLayer()
	: GameLayer()
{}

void ServerGameLayer::on_attach()
{
	GameLayer::on_attach();

	LOG_INFO("Starting server...");
	
	Engine::Application& app = Engine::Application::get();
	app.create_server<MessageID>(PORT);
	Engine::Server<MessageID>& server = dynamic_cast<Engine::Server<MessageID>&>(app.get_network_device());

	ASSERT(server.start(), "Server failed to start");
}

void ServerGameLayer::on_detach()
{
	GameLayer::on_detach();
}

void ServerGameLayer::on_update(double delta)
{
	GameLayer::on_update(delta);
}

void ServerGameLayer::on_draw(double delta)
{
	GameLayer::on_draw(delta);
}

void ServerGameLayer::on_event(Engine::Event &event)
{
	GameLayer::on_event(event);

	Engine::EventDispatcher dispatcher(event);
	
	dispatcher.dispatch<Engine::ClientConnectedToServerEvent>(std::bind(
				&ServerGameLayer::on_client_connected_to_server,
				this, std::placeholders::_1));
	
	dispatcher.dispatch<Engine::ClientDisconnectedEvent>(std::bind(
				&ServerGameLayer::on_client_disconnected,
				this, std::placeholders::_1));
	
	dispatcher.dispatch<Engine::ClientMessageEvent>(std::bind(
				&ServerGameLayer::on_client_message,
				this, std::placeholders::_1));
}

bool ServerGameLayer::on_client_message(Engine::ClientMessageEvent &event)
{
	LOG_INFO("player {0} says: {1}", static_cast<uint32_t>(event.get_sender_uid()), event.to_string());

	Engine::Message msg = event.get_message();
	MessageID msg_id;
	msg >> msg_id;
	
	auto& server = dynamic_cast<Engine::Server<MessageID>&>(Engine::Application::get().get_network_device());

	switch (msg_id)
	{
		case MessageID::REGISTER_PLAYER:
		{
			if (msg.size() == sizeof(PlayerInfo))
			{

				PlayerInfo info;
				msg >> info;

				register_player(info);

				// send back information about other players
				for (const auto& [other_uid, other_info] : m_player_infos)
				{
					if (other_uid != info.uid)
					{
						Engine::Message response;
						response << other_info;
						response << MessageID::REGISTER_PLAYER;
						server.send(response, event.get_sender_uid());
					}
				}

				// re-send message to other clients
				msg << info;
				msg << MessageID::REGISTER_PLAYER;
				server.send_all(msg, event.get_sender_uid());
			}
			break;
		}
		case MessageID::UPDATE_PLAYER:
		{
			int x, y;
			msg >> y >> x;
			Engine::PlayerUID puid = event.get_sender_uid();
			
			// update info
			m_player_infos[puid].x = x;
			m_player_infos[puid].x = y;
			
			// update world position
			Engine::PositionComponent* pos = m_players[puid].get<Engine::PositionComponent>();
			pos->x = x;
			pos->y = y;

			// re-send message to other clients
			msg << puid << x << y;
			msg << MessageID::UPDATE_PLAYER;
			server.send_all(msg, puid);
			break;
		}
		default:
		{
			// intentionally left blank.
		}
	}

	return false;
}

bool ServerGameLayer::on_client_connected_to_server(Engine::ClientConnectedToServerEvent &event)
{
	LOG_INFO("{0} joined.", static_cast<uint32_t>(event.get_player_uid()));
	return false;
}

bool ServerGameLayer::on_client_disconnected(Engine::ClientDisconnectedEvent &event)
{
	LOG_INFO("{0} left.", static_cast<uint32_t>(event.get_player_uid()));
	
	deregister_player(event.get_player_uid());
	return false;
}
