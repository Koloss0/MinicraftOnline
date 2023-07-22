#include <engine/ecs/scene.hpp>

#include "engine/core/log.hpp"
#include "engine/ecs/entity.hpp"

namespace Engine
{
	int s_component_counter = 0;

	Entity Scene::new_entity()
	{
		if (!free_entities.empty())
		{
			EntityIndex new_index = free_entities.back();
			free_entities.pop_back();
			EntityID new_id = create_entity_id(new_index, get_entity_version(entities[new_index].id));
			entities[new_index].id = new_id;

			return {new_id, shared_from_this()};
		}

		EntityID new_id = create_entity_id(static_cast<EntityIndex>(entities.size()), 0);
		entities.push_back({new_id, ComponentMask()});

		return {new_id, shared_from_this()};
	}

	void Scene::destroy_entity(Entity entity)
	{
		LOG_INFO("destroying entity: {0}", entity.m_id);
		EntityIndex entity_index = get_entity_index(entity.m_id);
		EntityID newID = create_entity_id(EntityIndex(-1), get_entity_version(entity.m_id) + 1);
		entities[entity_index].id = newID;
		entities[entity_index].mask.reset();
		free_entities.push_back(entity_index);
	}
}
