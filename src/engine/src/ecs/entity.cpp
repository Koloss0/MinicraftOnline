#include "engine/ecs/entity.hpp"
#include "engine/ecs/scene.hpp"

#include <utility>

namespace Engine
{
	Entity::Entity(EntityID id, const std::shared_ptr<Scene>& scene)
		: m_id(id), m_scene(scene) 
	{}

	bool Entity::operator==(const Entity& other) const
	{
		return m_id == other.m_id && !m_scene.expired() && m_scene.lock() == other.m_scene.lock();
	}

	bool Entity::operator!=(const Entity& other) const
	{
		return !(*this == other);
	}
}
