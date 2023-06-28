#include "player_system.h"
#include <ecs/components.h>
#include <ecs/scene_view.h>

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
