#include "player_system.h"

#include <engine/ecs/components.h>
#include <engine/ecs/scene.h>
#include <engine/ecs/scene_view.h>

PlayerSystem::PlayerSystem(Scene& scene)
	: System(scene)
{}

void PlayerSystem::on_update(double delta)
{
	for (EntityID player : SceneView<PlayerComponent>(m_scene))
	{
		PlayerComponent* player_comp =
			m_scene.get_component<PlayerComponent>(player);
	}
}
