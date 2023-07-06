#include <base/systems/player_system.hpp>

#include <engine/ecs/components.hpp>
#include <engine/ecs/scene.hpp>
#include <engine/ecs/scene_view.hpp>

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
