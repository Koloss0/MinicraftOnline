// component_pool.h
#pragma once

#include <iostream>

const int MAX_ENTITIES = 100000;

namespace Engine
{
	struct ComponentPool
	{
		char* p_data = nullptr;
		size_t component_size = 0;
		
		ComponentPool(size_t component_size_)
			// allocate enough space for maximum number of entities
			: p_data(new char[component_size_* MAX_ENTITIES]), component_size(component_size_)
		{}

		~ComponentPool()
		{
			delete[] p_data;
		}

		ComponentPool(const ComponentPool&) = delete;
		ComponentPool& operator=(const ComponentPool&) = delete;

		inline void* get(size_t index)
		{
			// look up component at desired index
			return p_data + index * component_size;
		}
	};
}
