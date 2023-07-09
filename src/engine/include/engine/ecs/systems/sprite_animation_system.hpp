#pragma once

#include "system.hpp"

namespace Engine
{
	class SpriteAnimationSystem : public System
	{
	public:
		SpriteAnimationSystem(Scene& scene);

		virtual void on_update(double delta) override;
	};
}
