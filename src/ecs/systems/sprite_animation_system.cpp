#include "sprite_animation_system.h"
#include <ecs/components.h>
#include <ecs/scene_view.h>

SpriteAnimationSystem::SpriteAnimationSystem(Scene& scene)
	: System(scene)
{}

void SpriteAnimationSystem::on_update(double delta)
{
	for (EntityID entity : SceneView<SpriteComponent, SpriteAnimatorComponent>(m_scene))
	{
		SpriteComponent* sprite =
			m_scene.get_component<SpriteComponent>(entity);
		SpriteAnimatorComponent* animator =
			m_scene.get_component<SpriteAnimatorComponent>(entity);

		animator->time += delta;

		if (animator->time >= animator->frame_duration)
		{
			animator->time = 0.0;
			animator->frame++;
			animator->frame %= animator->frames.size();

			// update sprite source Rect
			sprite->source_rect = animator->frames[animator->frame];
		}
	}
}
