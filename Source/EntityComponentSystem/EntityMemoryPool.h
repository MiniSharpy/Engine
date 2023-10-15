#pragma once
#include "Components.h"
#include "TupleHelper.h"
#include <vector>
#include <string>
#include <tuple>
#include <stack>
#include <bitset>

namespace Engine
{
	/// <summary>
	/// Largely based on videos from https://www.youtube.com/@DaveChurchill
	/// </summary>
	class EntityMemoryPool
	{
		#define MAX_ENTITIES 16384
		#define MAX_COMPONENTS 64

		size_t AliveCount = 0;
		ComponentPool Pool; // Includes entity ID, and all its components.
		std::vector<std::bitset<MAX_COMPONENTS>> EnabledComponents;
		std::vector<std::string> Tags; // A category of entities, e.g. enemies. For maximum performance could use enum.
		std::stack<size_t> AvailableIDs;

		EntityMemoryPool(size_t maxEntities)
		{
			std::apply([maxEntities](auto&&... args) {((args.resize(maxEntities)), ...); }, Pool);
			Tags = std::vector<std::string>(maxEntities);
			EnabledComponents = std::vector<std::bitset<MAX_COMPONENTS>>(maxEntities);

			for (size_t i = maxEntities - 1; i > 0; --i) { AvailableIDs.push(i); }
			AvailableIDs.push(0); // Start from 0 for consistency.
		}

		size_t GetNextEntityIndex()
		{
			// Order of popping is important for undo/redo system, it must use a stack.
			// As entity ID is stored by delete/create commands if an entity is deleted and then a new entity
			// created the expected ID for the delete command undo would invalid as the ID has been moved to the end of the queue.
			// With a queue it does:
			// Create 1, Delete 1, Undo (which creates 2), Undo (which tries to delete 1), leaving entity 2 incorrectly still in existence.
			// When it should be:
			// Create 1, Delete 1, Undo (which creates 1), Undo (which deletes 1),.
			size_t id = AvailableIDs.top();
			AvailableIDs.pop();
			return id;
		}

	public:
		static EntityMemoryPool& Instance()
		{
			static EntityMemoryPool pool(MAX_ENTITIES);
			return pool;
		}

		size_t GetEntityAliveCount() const { return AliveCount; }

		size_t AddEntity(const std::string& tag)
		{
			size_t index = GetNextEntityIndex();

			std::apply([index](auto&&... args) {((args[index] = {}), ...); }, Pool); // Reset values of each component that belongs to this entity.
			Tags[index] = tag;
			AliveCount++;
			return index;
		}

		template <typename T>
		void AddComponent(size_t id)
		{
			constexpr std::size_t index = tuple_element_index_v<std::vector<T>, ComponentPool>;
			EnabledComponents[id] |= 1 << index;
		}

		template <typename T>
		T& GetComponent(size_t id)
		{
			return std::get<std::vector<T>>(Pool)[id];
		}

		template <typename T>
		bool HasComponent(size_t id)
		{
			const std::bitset<MAX_COMPONENTS> requiredComponents = (1 << tuple_element_index_v<std::vector<T>, ComponentPool>);

			auto componentsInBoth = requiredComponents & EnabledComponents[id];
			if (componentsInBoth == requiredComponents) { return true; }
			return false;
		}

		template<typename... T>
		bool HasComponents(size_t id)
		{
			std::bitset<MAX_COMPONENTS> requiredComponents = (... + []()
			{
				return (1 << tuple_element_index_v<std::vector<T>, ComponentPool>);
			}());

			auto componentsInBoth = requiredComponents & EnabledComponents[id];
			if (componentsInBoth == requiredComponents) { return true; }
			return false;
		}

		const std::string& GetTag(size_t id) const
		{
			return Tags[id];
		}

		void Destroy(size_t id)
		{
			AliveCount--;
			EnabledComponents[id] = 0;
			AvailableIDs.push(id);
		}

		std::bitset<MAX_COMPONENTS> GetEnabledComponents(size_t id) const
		{
			return EnabledComponents[id];
		}

		std::bitset<MAX_COMPONENTS>& GetEnabledComponents(size_t id)
		{
			return EnabledComponents[id];
		}

		void SetEnabledComponents(size_t id, std::bitset<MAX_COMPONENTS> enabledComponents)
		{
			EnabledComponents[id] = enabledComponents;
		}

		ComponentSlice CreateSlice(size_t id)
		{
			return ComponentHelper<Components>::CreateSlice(Pool, id);
		}

		void UpdatePoolWithSlice(size_t id, ComponentSlice& componentSlice)
		{
			auto copyComponentToPool = []<typename T>(T& component, size_t id, ComponentPool& pool) { std::get<std::vector<T>>(pool)[id] = component; };
			auto copyComponentsToPool = [&copyComponentToPool, id, this]<typename... T>(T&... component) { (copyComponentToPool(component, id, Pool), ...); };
			std::apply(copyComponentsToPool, componentSlice);
		}

		ComponentReferenceSlice GetReferenceSlice(size_t id)
		{
			return ComponentHelper<Components>::CreateReferenceSlice(Pool, id);
		}
	};
}