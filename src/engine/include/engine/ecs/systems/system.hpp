#pragma once

#include <engine/ecs/scene.hpp>
#include <memory>

namespace Engine
{
	class System
	{
	public:
		System(const std::shared_ptr<Scene>& scene)
			: m_scene(scene)
		{}

		virtual ~System() = default;

		virtual void on_update(double delta) = 0;
	protected:
		std::weak_ptr<Scene> m_scene;
	};
}
