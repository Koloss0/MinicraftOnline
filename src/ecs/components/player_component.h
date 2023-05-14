// player_component.h
#pragma once

#include <cstdlib>

#include "component.h"
#include <src/core/common.h>

struct PlayerComponent: public Component
{
	static constexpr double FRAME_DURATION = 0.25;
	static const IntRect WALK_FRAMES[];
	double animation_time = 0.0;
	std::size_t current_frame = 0;
};

const IntRect PlayerComponent::WALK_FRAMES[] = {
	{0, -16, 16, 16},
	{16, -16, -16, 16}
};

