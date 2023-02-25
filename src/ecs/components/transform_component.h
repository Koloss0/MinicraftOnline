// transform_component.h
#pragma once

#include "component.h"
#include <glm/mat3x3.hpp>

struct TransformComponent: public Component
{
	glm::mat3 transform;
};
