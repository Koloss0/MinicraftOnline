#include "server_game_layer.hpp"
#include "base/base_game_layer.hpp"

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
	app.create_server<MESSAGES>(PORT);
	Engine::Server<MESSAGES>& server = dynamic_cast<Engine::Server<MESSAGES>&>(app.get_network_device());
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
	LOG_INFO("client says: {0}", event.to_string());
	return false;
}

bool ServerGameLayer::on_client_connected_to_server(Engine::ClientConnectedToServerEvent &event)
{
	LOG_INFO("client joined");
	return false;
}

bool ServerGameLayer::on_client_disconnected(Engine::ClientDisconnectedEvent &event)
{
	LOG_INFO("client left");
	return false;
}
