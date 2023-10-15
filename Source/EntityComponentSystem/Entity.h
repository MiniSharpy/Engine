#pragma once
#include "EntityMemoryPool.h"

namespace Engine
{
	/// <summary>
	/// Entity is essentially just a wrapper around an integer ID that has helper functions to get components.
	/// </summary>
	class Entity
	{
		size_t ID;
		Entity(size_t id) : ID(id) {}
		friend class EntityManager;

	public:
		size_t GetID() { return ID; }

		std::string GetTag() { return EntityMemoryPool::Instance().GetTag(ID); }

		template<typename T>
		T& AddComponent()
		{
			EntityMemoryPool::Instance().AddComponent<T>(ID);
			return EntityMemoryPool::Instance().GetComponent<T>(ID);
		}

		template<typename T>
		T& GetComponent()
		{
			return EntityMemoryPool::Instance().GetComponent<T>(ID);
		}

		template<typename T>
		const T& GetComponent() const
		{
			return EntityMemoryPool::Instance().GetComponent<T>(ID);
		}

		template<typename T>
		bool HasComponent()
		{
			return EntityMemoryPool::Instance().HasComponent<T>(ID);
		}

		template<typename... T>
		bool HasComponents()
		{
			return EntityMemoryPool::Instance().HasComponents<T...>(ID);
		}

		bool operator== (const Entity& right) const { return ID == right.ID; };

	};
}