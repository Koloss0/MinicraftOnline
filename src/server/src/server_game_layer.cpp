#include "server_game_layer.h"

#include <engine/net/server.h>
#include <engine/core/application.h>
#include <engine/core/log.h>

static constexpr int PORT = 7777;

ServerGameLayer::ServerGameLayer()
	: GameLayer()
{}

void ServerGameLayer::on_attach()
{
	GameLayer::on_attach();
	LOG_INFO("Starting server...");
	
	Application& app = Application::get();
	app.create_server<MESSAGES>(PORT);
	Server& server = dynamic_cast<Server&>(app.get_network_device());
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

void ServerGameLayer::on_client_message_recieved(engine::ClientMessageEvent<int>& event)
{
	LOG_INFO("network event!: {0}", event.to_string());
}
