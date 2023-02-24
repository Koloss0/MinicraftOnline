// scene.cpp

#include "scene.h"
#include "components/component.h"

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


