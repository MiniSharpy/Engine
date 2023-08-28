#pragma once
#include "Command.h"
#include "../EntityComponentSystem/Components.h"
#include "../EntityComponentSystem/Entity.h"
#include <optional>
#include <bitset>

namespace Engine
{
	class EntityManager;

	// TODO: Unit test!
	class DeleteEntityCommand : public Command
	{
	private:
		DeleteEntityCommand(Entity entity, EntityManager& entityManager);
		friend class UndoManager;

	public:
		virtual void Execute();
		virtual void Undo();
	private:
		std::optional<Entity> CreatedEntity;
		EntityManager& OwningEntityManger;
		ComponentSlice ComponentData;
		size_t EnabledComponents;
	};
}