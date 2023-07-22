#pragma once

#include "system.hpp"
#include <engine/ecs/components.hpp>

#include <memory>

namespace Engine
{
	class TilemapSystem : public System
	{
	public:
		TilemapSystem(const std::shared_ptr<Scene>& scene);
		
		virtual void on_update(double delta) override;
		void generate_chunks(TilemapComponent& tilemap,
				unsigned short start_x_chks, unsigned short start_y_chks,
				unsigned short end_x_chks, unsigned short end_y_chks);
	};
}
