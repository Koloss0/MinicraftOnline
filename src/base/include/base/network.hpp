#pragma once

#include "engine/net/player_uid.hpp"
#include "engine/renderer/colour.hpp"
#include "engine/renderer/image.hpp"

#include <cstdint>
#include <string>

enum class MessageID : uint8_t
{
	REGISTER_PLAYER,
	UPDATE_PLAYER
};

struct PlayerInfo
{
	Engine::PlayerUID uid;
	//std::string username;
	Engine::colour outline_colour;
	Engine::colour fill_colour_1;
	Engine::colour fill_colour_2;
	int x, y;
};
