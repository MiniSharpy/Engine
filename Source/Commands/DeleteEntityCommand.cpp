#include "DeleteEntityCommand.h"
#include "../EntityComponentSystem/EntityManager.h"
#include <SDL.h>

// Need to pass reference to entity manager to encapsulate construction logic within the class.
Engine::DeleteEntityCommand::DeleteEntityCommand(Entity entity, EntityManager& entityManager) :
	OwningEntityManger{ entityManager }
{
	CreatedEntity = entity;
	ComponentData = entityManager.GetPoolSlice(entity.GetID());
	EnabledComponents = entityManager.GetEnabledComponents(entity.GetID());
}

void Engine::DeleteEntityCommand::Execute()
{
	SDL_Log("+Deleting entity %d", CreatedEntity->GetID());
	OwningEntityManger.Destroy(CreatedEntity.value());
	CreatedEntity.reset();
}

void Engine::DeleteEntityCommand::Undo()
{
	CreatedEntity = OwningEntityManger.AddEntity("TileSet");
	OwningEntityManger.SetPoolSlice(CreatedEntity->GetID(), ComponentData);
	OwningEntityManger.SetEnabledComponents(CreatedEntity->GetID(), EnabledComponents);
	SDL_Log("+Creating entity %d", CreatedEntity->GetID());
}
