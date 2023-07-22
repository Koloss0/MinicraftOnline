#include <base/systems/player_system.hpp>

#include "base/components.hpp"

#include <engine/ecs/scene.hpp>
#include <engine/ecs/scene_view.hpp>
#include <memory>

PlayerSystem::PlayerSystem(const std::shared_ptr<Engine::Scene>& scene)
	: System(scene)
{}

void PlayerSystem::on_update(double delta)
{
	if (auto scene = m_scene.lock())
	{
		for (Engine::Entity player : scene->get_view<PlayerComponent>())
		{
			auto* player_comp = player.get<PlayerComponent>();
		}
	}
}
