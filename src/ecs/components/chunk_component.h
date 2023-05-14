// chunk_component.h
#pragma once

#include "component.h"
#include <src/ecs/scene.h>

#include <array>

struct ChunkComponent: public Component
{
	static constexpr unsigned short CHUNK_SIZE = 16;
	std::array<std::array<EntityID, CHUNK_SIZE>, CHUNK_SIZE> tiles;
};
