#pragma once

#include "system.hpp"

class SpriteSystem : public System
{
public:
	SpriteSystem(Scene& scene);
	
	virtual void on_update(double delta) override;
};
