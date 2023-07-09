#include <engine/tiles.hpp>

const Engine::TileData Engine::TILES[] = {
	{ // air
		{},
		{},
		0,
		false,
		false,
		false
	},
	{ // grass
		Engine::BASIC_FLOOR_SOURCES,
		Engine::BASIC_WALL_SOURCES,
		0,
		true,
		false,
		false
	},
	{ // stone
		Engine::BASIC_FLOOR_SOURCES,
		Engine::BASIC_WALL_SOURCES,
		1,
		true,
		false,
		false
	},
};
