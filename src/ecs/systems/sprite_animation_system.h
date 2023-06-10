#pragma once

#include "system.h"

class SpriteAnimationSystem : public System
{
public:
	SpriteAnimationSystem(Scene& scene);

	virtual void on_update(double delta) override;
};
