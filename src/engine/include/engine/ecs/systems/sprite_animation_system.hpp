#pragma once

#include "system.hpp"

namespace Engine
{
	class SpriteAnimationSystem : public System
	{
	public:
		SpriteAnimationSystem(const std::shared_ptr<Scene>& scene);

		virtual void on_update(double delta) override;
	};
}
