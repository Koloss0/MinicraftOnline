#pragma once

#include "engine/core/log.hpp"
#include "engine/ecs/scene.hpp"

#include <memory>

namespace Engine
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(EntityID id, const std::shared_ptr<Scene>& scene);
		~Entity() = default;
		Entity(const Entity& other) = default;
		Entity& operator=(const Entity& entity) = default;
		bool operator==(const Entity& other) const;
		bool operator!=(const Entity& other) const;

		inline EntityID get_id() { return m_id; }

		template<typename T>
		T* assign() const
		{
			auto scene = m_scene.lock();
			ASSERT(scene, "Attempt to assign component to entity in freed scene");
			return scene->assign_component<T>(m_id);
		}

		template<typename T>
		void remove() const
		{
			auto scene = m_scene.lock();
			ASSERT(scene, "Attempt to remove component from entity in freed scene");
			scene->remove_component<T>(m_id);
		}

		template<typename T>
		T* get() const
		{
			auto scene = m_scene.lock();
			ASSERT(scene, "Attempt to get component from entity in freed scene");
			return scene->get_component<T>(m_id);
		}

	private:
		EntityID m_id = 0;
		std::weak_ptr<Scene> m_scene{};

		friend struct Scene;
	};
}
