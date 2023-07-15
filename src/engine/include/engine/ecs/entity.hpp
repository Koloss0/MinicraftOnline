#pragma once

#include "engine/ecs/scene.hpp"

namespace Engine
{
	class Entity
	{
	public:
		Entity(Scene& scene);
		~Entity();

		inline EntityID get_id() { return m_id; }

		template<typename T>
		T* assign() const
		{
			return m_scene.assign_component<T>(m_id);
		}

		template<typename T>
		void remove() const
		{
			m_scene.remove_component<T>(m_id);
		}

		template<typename T>
		T* get() const
		{
			return m_scene.get_component<T>(m_id);
		}

	private:
		EntityID m_id;
		Scene& m_scene;
	};
}
