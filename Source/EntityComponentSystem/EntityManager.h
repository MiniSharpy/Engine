#pragma once
#include "Entity.h"
#include "EntityMemoryPool.h"
#include <vector>
#include <map>

namespace Engine
{
	class EntityManager
	{
		std::vector<Entity> Entities;
		/// <summary>
		/// Because of vector resizing there is the potential for iterator invalidation if directly altering <see cref="Entities"/>.
		/// </summary>
		std::vector<Entity>	EntitiesToAdd;
		std::map<std::string, std::vector<Entity>> EntitiesByTag;

	public:
		~EntityManager()
		{
			for (auto& entity : Entities)
			{
				EntityMemoryPool::Instance().Destroy(entity.ID);
			}
		}

		void Update()
		{
			Entities.insert(Entities.end(), EntitiesToAdd.begin(), EntitiesToAdd.end());
			EntitiesToAdd.clear();
		}

		std::vector<Entity>& GetEntities()
		{
			return Entities;
		}

		std::vector<Entity>& GetEntitiesByTag(std::string tag)
		{
			// This automatically creates an entry if it doesn't exist, which could result in unintended allocations
			// if there's a typo in the tag but this is neccesary to attempt to access entities before they might been created.
			// TODO: I suppose with an enum instead of a string this'll prevent compilation with invalid tags.
			return EntitiesByTag[tag];
		}

		Entity AddEntity(const std::string& tag)
		{
			size_t id = EntityMemoryPool::Instance().AddEntity(tag);
			Entity entity = Entity(id);
			EntitiesToAdd.push_back(entity); // Add to a seperate vector to prevent resizing that could cause iterator invalidation when called from within loop.
			EntitiesByTag[tag].push_back(entity);
			return entity;
		}

		// TOOD: Could this cause iterator invalidation if done in a loop?
		void Destroy(Entity entity)
		{
			// Remove from vectors.
			std::erase(Entities, entity);
			std::erase(EntitiesToAdd, entity);

			// Remove from tag map.
			const std::string& tag = EntityMemoryPool::Instance().GetTag(entity.ID);
			std::vector<Entity>& SameTagEntities = EntitiesByTag[tag];
			std::erase(SameTagEntities, entity);

			// Mark as destroyed.
			EntityMemoryPool::Instance().Destroy(entity.ID);
		}

		size_t GetEnabledComponents(size_t id)
		{
			return EntityMemoryPool::Instance().GetEnabledComponents(id);
		}

		void SetEnabledComponents(size_t id, size_t enabledComponents)
		{
			EntityMemoryPool::Instance().SetEnabledComponents(id, enabledComponents);
		}

		ComponentSlice GetPoolSlice(size_t id)
		{
			return EntityMemoryPool::Instance().GetPoolSlice(id);
		}

		void SetPoolSlice(size_t id, ComponentSlice& componentSlice)
		{
			EntityMemoryPool::Instance().SetPoolSlice(id, componentSlice);
		}
	};
}