#include <engine/tiles.hpp>

const TileData TILES[] = {
	{ // air
		{},
		{},
		0,
		false,
		false,
		false
	},
	{ // grass
		BASIC_FLOOR_SOURCES,
		BASIC_WALL_SOURCES,
		0,
		true,
		false,
		false
	},
	{ // stone
		BASIC_FLOOR_SOURCES,
		BASIC_WALL_SOURCES,
		1,
		true,
		false,
		false
	},
};
