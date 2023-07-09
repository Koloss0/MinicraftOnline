#include <engine/ecs/scene.hpp>

namespace Engine
{
	int s_component_counter = 0;

	EntityID Scene::new_entity()
	{
		if (!free_entities.empty())
		{
			EntityIndex new_index = free_entities.back();
			free_entities.pop_back();
			EntityID new_id = create_entity_id(new_index, get_entity_version(entities[new_index].id));
			entities[new_index].id = new_id;
			return entities[new_index].id;
		}
		entities.push_back({entities.size(), ComponentMask()});
		return entities.back().id;
	}

	void Scene::destroy_entity(EntityID id)
	{
		EntityID newID = create_entity_id(EntityIndex(-1), get_entity_version(id) + 1);
		entities[get_entity_index(id)].id = newID;
		entities[get_entity_index(id)].mask.reset();
		free_entities.push_back(get_entity_index(id));
	}
}
