#pragma once

#include <engine/ecs/systems/system.hpp>

class PlayerSystem : public System
{
public:
	PlayerSystem(Scene& scene);

	virtual void on_update(double delta) override;
};
