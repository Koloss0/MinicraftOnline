#include <engine/ecs/systems/sprite_system.hpp>
#include <engine/ecs/scene.hpp>
#include <engine/ecs/scene_view.hpp>
#include <engine/ecs/components.hpp>
#include <engine/renderer/renderer.hpp>

namespace Engine
{
	SpriteSystem::SpriteSystem(Scene& scene)
		: System(scene)
	{}

	void SpriteSystem::on_update(double delta)
	{
		for (EntityID ent : SceneView<PositionComponent, SpriteComponent>(m_scene))
		{
			PositionComponent* pos = m_scene.get_component<PositionComponent>(ent);
			SpriteComponent* sprite = m_scene.get_component<SpriteComponent>(ent);

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
