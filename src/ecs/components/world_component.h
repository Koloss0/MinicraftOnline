// world_component.h
#pragma once

#include "component.h"
#include <src/ecs/scene.h>
#include <cstddef>

#include <array>

struct WorldComponent: public Component
{
	static constexpr unsigned short WORLD_SIZE = 4;
	std::array<std::array<EntityID, WORLD_SIZE>, WORLD_SIZE> chunks;
};
