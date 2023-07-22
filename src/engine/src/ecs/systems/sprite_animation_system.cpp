#include <engine/ecs/systems/sprite_animation_system.hpp>
#include <engine/ecs/components.hpp>
#include <engine/ecs/scene.hpp>
#include <engine/ecs/scene_view.hpp>

namespace Engine
{
	SpriteAnimationSystem::SpriteAnimationSystem(const std::shared_ptr<Scene>& scene)
		: System(scene)
	{}

	void SpriteAnimationSystem::on_update(double delta)
	{
		if (auto scene = m_scene.lock())
		{
			for (Entity entity : scene->get_view<SpriteComponent, SpriteAnimatorComponent>())
			{
				auto* sprite = entity.get<SpriteComponent>();
				auto* animator = entity.get<SpriteAnimatorComponent>();

				animator->time += delta;

				if (animator->time > animator->frame_duration)
				{
					animator->time = 0.0;
					animator->frame++;
					animator->frame %= animator->frames.size();

					// update sprite source Rect
					sprite->source_rect = animator->frames[animator->frame];
				}
			}
		}
	}
}
