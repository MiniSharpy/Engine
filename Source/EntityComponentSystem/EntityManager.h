#pragma once
#include "Entity.h"
#include "EntityMemoryPool.h"
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

namespace Engine
{
	struct SerialisedData
	{
		std::bitset<MAX_COMPONENTS> EnabledComponents;
		char Tag[64];
		ComponentSlice Slice;
	};

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

		void Save(const std::string& path)
		{
			auto saveEntity = [](Entity entity, std::ofstream& out, EntityManager& manager)
			{
				size_t id = entity.GetID();

				SerialisedData data
				{
					manager.GetEnabledComponents(id),
					"\0",
					manager.GetPoolSlice(id)
				};
				strcpy(data.Tag, entity.GetTag().c_str());

				out.write(reinterpret_cast<char*>(&data), sizeof(data));
			};

			// Each line is an entity.
			std::ofstream out{ path };

			// Want to avoid new line at the end file so that loading is simpler.
			saveEntity(Entities[0], out, *this);
			for (int i = 1; i < Entities.size(); ++i)
			{
				out << "\n";
				saveEntity(Entities[i], out, *this);
			}
		}

		void Load(const std::string& path)
		{
			std::ifstream in{ path };

			std::string line;
			while (std::getline(in, line))
			{
				SerialisedData data;
				std::istringstream string(line);
				string.read(reinterpret_cast<char*>(&data), sizeof(data));

				Entity entity = AddEntity(data.Tag);
				SetEnabledComponents(entity.GetID(), data.EnabledComponents);
				SetPoolSlice(entity.GetID(), data.Slice);
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

		std::bitset<MAX_COMPONENTS> GetEnabledComponents(size_t id) const
		{
			return EntityMemoryPool::Instance().GetEnabledComponents(id);
		}

		std::bitset<MAX_COMPONENTS>& GetEnabledComponents(size_t id)
		{
			return EntityMemoryPool::Instance().GetEnabledComponents(id);
		}

		void SetEnabledComponents(size_t id, std::bitset<MAX_COMPONENTS> enabledComponents)
		{
			EntityMemoryPool::Instance().SetEnabledComponents(id, enabledComponents);
		}

		ComponentSlice GetPoolSlice(size_t id)
		{
			return EntityMemoryPool::Instance().CreateSlice(id);
		}

		void SetPoolSlice(size_t id, ComponentSlice& componentSlice)
		{
			EntityMemoryPool::Instance().UpdatePoolWithSlice(id, componentSlice);
		}

		ComponentReferenceSlice GetReferenceSlice(size_t id)
		{
			return EntityMemoryPool::Instance().GetReferenceSlice(id);
		}
	};
}