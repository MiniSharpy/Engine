#include "../../Source/Commands/CreateEntityCommand.h"
#include "../../Source/Commands/UndoManager.h"
#include "../../Source/EntityComponentSystem/EntityManager.h"
#include "../../Source/EntityComponentSystem/Components.h"
#include "../../Source/EntityComponentSystem/TupleHelper.h"
#include <gtest/gtest.h>

namespace Engine
{
	TEST(CommandTests, CreateEntityCommandExecute)
	{
		UndoManager undoManager;
		EntityManager entityManager;

		ComponentSlice componentData = {};
		std::get<Position>(componentData) = {5, 10};

		std::bitset<64> enabledComponents;
		constexpr std::size_t index = tuple_element_index_v<Position, ComponentSlice>;
		enabledComponents[index] = true;

		undoManager.AddCommandAndExecute<CreateEntityCommand>(componentData, enabledComponents, entityManager);
		entityManager.Update();

		ASSERT_EQ(entityManager.GetEntities().size(), 1);
		ASSERT_TRUE(entityManager.GetEntities()[0].HasComponent<Position>());
		ASSERT_FALSE(entityManager.GetEntities()[0].HasComponent<Sprite>());
	}

}
