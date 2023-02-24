// scene_view.h
#pragma once

#include "scene.h"

template<typename... ComponentTypes>
struct SceneView
{
	struct Iterator
	{
		EntityIndex index;
		Scene* p_scene;
		ComponentMask mask;
		bool all = false;

		Iterator(Scene* p_scene_, EntityIndex index_, ComponentMask mask_, bool all_)
	       		: index(index_), p_scene(p_scene_),  mask(mask_), all(all_)
		{}

		EntityID operator*() const
		{
			return p_scene->entities[index].id;
		}

		bool operator==(const Iterator& other) const
		{
			return index == other.index || index == p_scene->entities.size();
		}

		bool operator!=(const Iterator& other) const
		{
			return index != other.index && index != p_scene->entities.size();
		}

		bool valid_index()
		{
			return
				// is a valid entity ID
				is_entity_valid(p_scene->entities[index].id) &&
				// it has the correct component mask
				(all || mask == (mask & p_scene->entities[index].mask));
		}

		Iterator& operator++()
		{
			do {
				index++;
			} while (index < p_scene->entities.size() && !valid_index());
			return *this;
		}
	};
	
	Scene* p_scene = nullptr;
	ComponentMask component_mask{};
	bool all = false;

	SceneView(Scene& scene)
		: p_scene(&scene)
	{
		if (sizeof...(ComponentTypes) == 0)
		{
			all = true; // iterate all entities in the scene
		}
		else
		{
			// unpack the template parameters into an initializer list
			int component_ids[] = {0, get_id<ComponentTypes>() ... };
			for (long unsigned int i = 1ul; i < (sizeof...(ComponentTypes) + 1); i++)
				component_mask.set(static_cast<std::size_t>(component_ids[i]));
		}
	}

	const Iterator begin() const
	{
		EntityIndex first_index = 0;
		while (first_index < p_scene->entities.size() &&
				(component_mask != (component_mask &
						   p_scene->entities[first_index].mask)
				 || !is_entity_valid(p_scene->entities[first_index].id)))
		{
			first_index++;
		}
		return Iterator(p_scene, first_index, component_mask, all);
	}

	const Iterator end() const
	{
		return Iterator(p_scene, EntityIndex(p_scene->entities.size()), component_mask, all);
	}
};
