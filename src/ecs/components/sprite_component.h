// sprite_component.h
#pragma once

#include "component.h"
#include "../../common.h"

struct SpriteComponent: public Component
{
	Rect rect{0.0f, 0.0f, 0.0f, 0.0f};
	Rect source_rect{0.0f, 0.0f, 1.0f, 1.0f};
};
