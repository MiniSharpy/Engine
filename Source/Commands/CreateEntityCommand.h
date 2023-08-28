#pragma once
#include "Command.h"
#include "../EntityComponentSystem/Components.h"
#include "../EntityComponentSystem/Entity.h"
#include "../EntityComponentSystem/TupleHelper.h"
#include <optional>
#include <bitset>

namespace Engine
{
	class EntityManager;

	class CreateEntityCommand : public Command
	{
		CreateEntityCommand(ComponentSlice& components, std::bitset<64>& enabledComponents, EntityManager& entityManager);
		friend class UndoManager;

	public:
		virtual void Execute();
		virtual void Undo();
	private:
		std::optional<Entity> CreatedEntity;
		EntityManager& OwningEntityManger;
		ComponentSlice ComponentData;
		std::bitset<64> EnabledComponents;

		template <typename T>
		void AddAndSetEnabledComponents(Entity entity)
		{
			constexpr std::size_t index = tuple_element_index_v<T, ComponentSlice>;

			if (EnabledComponents[index])
			{
				T& component = entity.AddComponent<T>();
				component = std::get<T>(ComponentData);
			}
		}
	};
}