#include "CreateEntityCommand.h"
#include "../EntityComponentSystem/EntityManager.h"
#include "../EntityComponentSystem/Components.h"
#include <SDL.h>

Engine::CreateEntityCommand::CreateEntityCommand(ComponentSlice& components, std::bitset<64>& enabledComponents, EntityManager& entityManager) :
	OwningEntityManger{entityManager},
	ComponentData{components},
	EnabledComponents{enabledComponents} {
}

void Engine::CreateEntityCommand::Execute()
{
	CreatedEntity = OwningEntityManger.AddEntity("TileSet");
	auto initialiseEntity = [this]<typename... T>(T&... args) { (AddAndSetEnabledComponents<T>(*CreatedEntity), ...); };
	std::apply(initialiseEntity, ComponentData);
	SDL_Log("+Creating entity %d", CreatedEntity->GetID());
}

void Engine::CreateEntityCommand::Undo()
{
	SDL_Log("-Deleting entity %d", CreatedEntity->GetID());
	OwningEntityManger.Destroy(*CreatedEntity);
	CreatedEntity.reset();
}
