// scene.h
#pragma once

#include "component_pool.hpp"

#include <bitset>
#include <vector>

namespace Engine
{
	// get IDs of components
	extern int s_component_counter;
	template <class T>
	int get_id()
	{
		static int s_component_id = s_component_counter++;
		return s_component_id;
	}

	typedef unsigned int EntityIndex;
	typedef unsigned int EntityVersion;
	typedef unsigned long long EntityID;
	constexpr int MAX_COMPONENTS = 32;
	typedef std::bitset<MAX_COMPONENTS> ComponentMask;

	inline EntityID create_entity_id(EntityIndex index, EntityVersion version)
	{
		// Shift the index up 32, and put the version in the bottom
		return (static_cast<EntityID>(index) << 32) | (static_cast<EntityID>(version));
	}
	inline EntityIndex get_entity_index(EntityID id)
	{
		// Shift down 32 so we lose the version and get our index
		return static_cast<EntityIndex>(id >> 32);
	}
	inline EntityVersion get_entity_version(EntityID id)
	{
		// Cast to a 32 bit int to get our version number (loosing the top 32 bits)
		return static_cast<EntityVersion>(id);
	}
	inline bool is_entity_valid(EntityID id)
	{
		// Check if the index is our invalid index
		return (id >> 32) != EntityIndex(-1);
	}

#define INVALID_ENTITY create_entity_id(EntityIndex(-1), 0)

	struct Scene
	{
		struct EntityData
		{
			EntityID id;
			ComponentMask mask;
		};
		std::vector<EntityData> entities;
		std::vector<EntityIndex> free_entities;
		std::vector<ComponentPool*> component_pools;

	public:
		Scene()
			: entities(), free_entities(), component_pools()
		{}

		EntityID new_entity();
		void destroy_entity(EntityID id);

		template <typename T>
		T* assign_component(EntityID entity_id)
		{
			auto component_id = static_cast<std::vector<ComponentPool>::size_type>(get_id<T>());
			
			// create a space for a new pool if one doesn't already exist
			if (component_pools.size() <= component_id)
				component_pools.resize(component_pools.size() + 1, nullptr);
			
			// create a new instance if pool doesn't exist
			if (component_pools[component_id] == nullptr)
				component_pools[component_id] = new ComponentPool(sizeof(T));

			// Looks up the component in the pool, and initializes it with placement new
			T* p_component = new (component_pools[component_id]->get(entity_id)) T();

			entities[entity_id].mask.set(component_id);
			return p_component;
		}
		
		template <typename T>
		void remove_component(EntityID entity_id)
		{
			// ensures you're not accessing an entity that has been deleted
			if (entities[get_entity_index(entity_id)].id != entity_id)
				return;

			entities[entity_id].mask.reset(get_id<T>());
		}

		template <typename T>
		T* get_component(EntityID entity_id)
		{
			auto component_id = static_cast<std::vector<ComponentPool>::size_type>(get_id<T>());
			
			if (!entities[entity_id].mask.test(component_id))
				return nullptr;

			T* p_component = static_cast<T*>(component_pools[component_id]->get(entity_id));
			return p_component;
		}
	};
}
