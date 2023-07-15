#include "engine/ecs/entity.hpp"
#include "engine/ecs/scene.hpp"

namespace Engine
{
	Entity::Entity(Scene& scene)
		: m_id(0), m_scene(scene) 
	{
		m_id = scene.new_entity();
	}

	Entity::~Entity()
	{
		m_scene.destroy_entity(m_id);
	}
}
