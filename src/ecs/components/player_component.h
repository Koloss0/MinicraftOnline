// player_component.h
#pragma once

#include <cstdlib>

#include "component.h"
#include "../../common.h"

struct PlayerComponent: public Component
{
	static constexpr double FRAME_DURATION = 0.25;
	static const Rect WALK_FRAMES[];
	double animation_time = 0.0;
	std::size_t current_frame = 0;
};

const Rect PlayerComponent::WALK_FRAMES[] = {
	{0.0f, -16.0f, 16.0f, 16.0f},
	{16.0f, -16.0f, -16.0f, 16.0f}
};

