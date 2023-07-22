#pragma once

#include "system.hpp"

namespace Engine
{
	class SpriteSystem : public System
	{
	public:
		SpriteSystem(const std::shared_ptr<Scene>& scene);
		
		virtual void on_update(double delta) override;
	};
}
