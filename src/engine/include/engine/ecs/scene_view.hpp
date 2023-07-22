#pragma once

#include "scene.hpp"
#include "entity.hpp"
#include <memory>

namespace Engine
{
	template<typename... ComponentTypes>
	struct SceneView
	{
		struct Iterator
		{
			EntityIndex index;
			std::shared_ptr<Scene> scene;
			ComponentMask mask;
			bool all = false;

			Iterator(const std::shared_ptr<Scene>& scene_, EntityIndex index_, ComponentMask mask_, bool all_)
				: index(index_), scene(scene_),  mask(mask_), all(all_)
			{}

			Entity operator*() const
			{
				return { scene->entities[index].id, scene };
			}

			bool operator==(const Iterator& other) const
			{
                                return index == other.index ||
                                       index == scene->entities.size();
                        }

                        bool operator!=(const Iterator &other) const {
                                return index != other.index &&
                                       index != scene->entities.size();
                        }

                        bool valid_index() {
                                return
                                    // is a valid entity ID
                                    is_entity_valid(
                                        scene->entities[index].id) &&
                                    // it has the correct component mask
                                    (all || mask == (mask & scene->entities[index].mask));
                        }

                        Iterator& operator++()
			{
				do {
					index++;
				} while (index < scene->entities.size() && !valid_index());
				return *this;
			}
		};
		
		std::shared_ptr<Scene> scene = nullptr;
		ComponentMask component_mask{};
		bool all = false;

		SceneView(const std::shared_ptr<Scene>& scene_)
			: scene(scene_)
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
			while (first_index < scene->entities.size() &&
				(component_mask != (component_mask &
				scene->entities[first_index].mask)
				|| !is_entity_valid(scene->entities[first_index].id)))
			{
				first_index++;
			}
			return Iterator(scene, first_index, component_mask, all);
		}

		const Iterator end() const
		{
			return Iterator(scene, EntityIndex(scene->entities.size()), component_mask, all);
		}
	};
}
