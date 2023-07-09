#pragma once

#include <engine/ecs/systems/system.hpp>

class PlayerSystem : public Engine::System
{
public:
	PlayerSystem(Engine::Scene& scene);

	virtual void on_update(double delta) override;
};
