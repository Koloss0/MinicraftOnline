// sprite_component.h
#pragma once

#include "component.h"
#include <glm/vec2.hpp>

struct SpriteComponent: public Component
{
	glm::vec2 size{1.0f, 1.0f};
	glm::vec2 source_position{0.0f};
	glm::vec2 source_size{0.0f};
};
