#pragma once

#include <base/base_game_layer.hpp>
#include "engine/events/network_event.hpp"

class ServerGameLayer : public GameLayer
{
public:
	ServerGameLayer();
	virtual ~ServerGameLayer() = default;

	virtual void on_attach() override;
	virtual void on_detach() override;
	virtual void on_update(double delta) override;
	virtual void on_draw(double delta) override;
	virtual void on_event(Engine::Event& event) override;

	bool on_client_connected_to_server(Engine::ClientConnectedToServerEvent& event);
	bool on_client_disconnected(Engine::ClientDisconnectedEvent& event);
	bool on_client_message(Engine::ClientMessageEvent& event);
};
