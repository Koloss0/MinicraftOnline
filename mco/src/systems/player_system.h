#pragma once

#include <ecs/systems/system.h>

class PlayerSystem : public System
{
public:
	PlayerSystem(Scene& scene);

	virtual void on_update(double delta) override;
};
