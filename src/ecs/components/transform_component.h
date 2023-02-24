// transform_component.h
#pragma once

#include "component.h"
#include <glm/vec2.hpp>

struct TransformComponent: public Component
{
	glm::vec2 position{};
	glm::vec2 scale{1.0f, 1.0f};
	float rotation = 0.0f;
};
