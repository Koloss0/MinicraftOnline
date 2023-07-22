#include <engine/ecs/systems/sprite_system.hpp>
#include <engine/ecs/scene.hpp>
#include <engine/ecs/scene_view.hpp>
#include <engine/ecs/components.hpp>
#include <engine/renderer/renderer.hpp>

namespace Engine
{
	SpriteSystem::SpriteSystem(const std::shared_ptr<Scene>& scene)
		: System(scene)
	{}

	void SpriteSystem::on_update(double delta)
	{
		if (auto scene = m_scene.lock())
		{
			for (Entity entity : scene->get_view<PositionComponent, SpriteComponent>())
			{
				auto* pos = entity.get<PositionComponent>();
				auto* sprite = entity.get<SpriteComponent>();

				Renderer::draw_rect(
					pos->x + sprite->rect.x,
					pos->y + sprite->rect.y,
					sprite->rect.width,
					sprite->rect.height,
					sprite->source_rect.x,
					sprite->source_rect.y,
					sprite->source_rect.width,
					sprite->source_rect.height,
					sprite->palette_atlas,
					sprite->palette_index
				);
			}
		}
	}
}
