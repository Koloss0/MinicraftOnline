#include <base/systems/player_system.hpp>

#include <engine/ecs/components.hpp>
#include <engine/ecs/scene.hpp>
#include <engine/ecs/scene_view.hpp>

PlayerSystem::PlayerSystem(Engine::Scene& scene)
	: System(scene)
{}

void PlayerSystem::on_update(double delta)
{
	for (Engine::EntityID player : Engine::SceneView<Engine::PlayerComponent>(m_scene))
	{
		Engine::PlayerComponent* player_comp =
			m_scene.get_component<Engine::PlayerComponent>(player);
	}
}
