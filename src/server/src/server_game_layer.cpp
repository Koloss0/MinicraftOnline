#include "server_game_layer.h"

ServerGameLayer::ServerGameLayer()
	: GameLayer()
{}

void ServerGameLayer::on_attach()
{
	GameLayer::on_attach();
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

