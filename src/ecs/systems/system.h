#pragma once

#include <src/ecs/scene.h>

class System
{
public:
	System(Scene& scene)
		: m_scene(scene)
	{}

	virtual ~System() = default;

	virtual void on_update(double delta) = 0;
protected:
	Scene& m_scene;
};
