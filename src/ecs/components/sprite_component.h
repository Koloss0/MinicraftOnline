// sprite_component.h
#pragma once

#include "component.h"
#include <src/core/common.h>
#include <src/renderer/texture.h>

struct SpriteComponent: public Component
{
	Rect rect{0.0f, 0.0f, 0.0f, 0.0f};
	IntRect source_rect{0, 0, 0, 0};
	Texture* palette_atlas = nullptr;
	unsigned char palette_index = 0;
};
