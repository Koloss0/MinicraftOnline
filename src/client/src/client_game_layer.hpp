#pragma once

#include <base/base_game_layer.hpp>

#include "base/network.hpp"

#include "engine/net/player_uid.hpp"
#include <engine/ecs/systems/sprite_animation_system.hpp>
#include <engine/ecs/systems/sprite_system.hpp>

class ClientGameLayer : public GameLayer
{
public:
	ClientGameLayer();
	virtual ~ClientGameLayer() = default;

	virtual void on_attach() override;
	virtual void on_detach() override;
	virtual void on_update(double delta) override;
	virtual void on_draw(double delta) override;
	virtual void on_event(Engine::Event& event) override;

private:
	bool on_connected_to_server(Engine::ConnectedToServerEvent& event);
	bool on_client_connected_to_server(Engine::ClientConnectedToServerEvent& event);
	bool on_client_disconnected(Engine::ClientDisconnectedEvent& event);
	bool on_failed_to_connect(Engine::FailedToConnectEvent& event);
	bool on_server_message(Engine::ServerMessageEvent& event);
	bool on_client_message(Engine::ClientMessageEvent& event);
	bool on_server_disconnected(Engine::ServerDisconnectedEvent& event);

	virtual void register_player(const PlayerInfo& info) override;

	Engine::PlayerUID m_my_uid;
	Engine::SpriteAnimationSystem m_sprite_animation_system;
	Engine::SpriteSystem m_sprite_system;
};
