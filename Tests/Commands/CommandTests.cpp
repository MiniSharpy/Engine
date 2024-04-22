#include "../../Source/Commands/CreateEntityCommand.h"
#include "../../Source/Commands/DeleteEntityCommand.h"
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

	TEST(CommandTests, DeleteEntityCommandExecute)
	{
		UndoManager undoManager;
		EntityManager entityManager;

		ComponentSlice componentData = {};
		std::get<Position>(componentData) = { 5, 10 };

		std::bitset<64> enabledComponents;
		constexpr std::size_t index = tuple_element_index_v<Position, ComponentSlice>;
		enabledComponents[index] = true;

		ASSERT_EQ(entityManager.GetEntities().size(), 0);

		undoManager.AddCommandAndExecute<CreateEntityCommand>(componentData, enabledComponents, entityManager);
		entityManager.Update();

		ASSERT_EQ(entityManager.GetEntities().size(), 1);

		Entity entity = entityManager.GetEntities()[0];
		undoManager.AddCommandAndExecute<DeleteEntityCommand>(entity, entityManager);
		entityManager.Update();

		ASSERT_EQ(entityManager.GetEntities().size(), 0);
	}

	TEST(CommandTests, Undo)
	{
		UndoManager undoManager;
		EntityManager entityManager;

		ComponentSlice componentData = {};
		std::get<Position>(componentData) = { 5, 10 };

		std::bitset<64> enabledComponents;
		constexpr std::size_t index = tuple_element_index_v<Position, ComponentSlice>;
		enabledComponents[index] = true;

		undoManager.AddCommandAndExecute<CreateEntityCommand>(componentData, enabledComponents, entityManager);

		entityManager.Update();
		ASSERT_EQ(entityManager.GetEntities().size(), 1);

		undoManager.Undo();
		entityManager.Update();
		ASSERT_EQ(entityManager.GetEntities().size(), 0);
	}

	TEST(CommandTests, Redo)
	{
		UndoManager undoManager;
		EntityManager entityManager;

		ComponentSlice componentData = {};
		std::get<Position>(componentData) = { 5, 10 };

		std::bitset<64> enabledComponents;
		constexpr std::size_t index = tuple_element_index_v<Position, ComponentSlice>;
		enabledComponents[index] = true;

		undoManager.AddCommandAndExecute<CreateEntityCommand>(componentData, enabledComponents, entityManager);

		entityManager.Update();
		ASSERT_EQ(entityManager.GetEntities().size(), 1);

		undoManager.Undo();
		entityManager.Update();
		ASSERT_EQ(entityManager.GetEntities().size(), 0);

		undoManager.Redo();
		entityManager.Update();
		ASSERT_EQ(entityManager.GetEntities().size(), 1);
	}

	TEST(CommandTests, CreateDeleteUndoID)
	{
		UndoManager undoManager;
		EntityManager entityManager;

		ComponentSlice componentData = {};
		std::get<Position>(componentData) = { 5, 10 };

		std::bitset<64> enabledComponents;
		constexpr std::size_t index = tuple_element_index_v<Position, ComponentSlice>;
		enabledComponents[index] = true;

		ASSERT_EQ(entityManager.GetEntities().size(), 0);

		undoManager.AddCommandAndExecute<CreateEntityCommand>(componentData, enabledComponents, entityManager);
		entityManager.Update();
		ASSERT_EQ(entityManager.GetEntities().size(), 1);

		int expectedID = entityManager.GetEntities()[0].GetID();

		Entity entity = entityManager.GetEntities()[0];
		undoManager.AddCommandAndExecute<DeleteEntityCommand>(entity, entityManager);
		entityManager.Update();
		ASSERT_EQ(entityManager.GetEntities().size(), 0);

		undoManager.Undo();
		entityManager.Update();
		ASSERT_EQ(entityManager.GetEntities().size(), 1);

		int actualID = entityManager.GetEntities()[0].GetID();
		ASSERT_EQ(actualID, expectedID);
	}

	TEST(CommandTests, CreateUndoUndo)
	{
		UndoManager undoManager;
		EntityManager entityManager;

		ComponentSlice componentData = {};
		std::get<Position>(componentData) = { 5, 10 };

		std::bitset<64> enabledComponents;
		constexpr std::size_t index = tuple_element_index_v<Position, ComponentSlice>;
		enabledComponents[index] = true;

		undoManager.AddCommandAndExecute<CreateEntityCommand>(componentData, enabledComponents, entityManager);

		entityManager.Update();
		ASSERT_EQ(entityManager.GetEntities().size(), 1);

		undoManager.Undo();
		entityManager.Update();
		ASSERT_EQ(entityManager.GetEntities().size(), 0);

		undoManager.Undo();
		entityManager.Update();
		ASSERT_EQ(entityManager.GetEntities().size(), 0);
	}

	TEST(CommandTests, CreateUnnecesaryRedo)
	{
		UndoManager undoManager;
		EntityManager entityManager;

		ComponentSlice componentData = {};
		std::get<Position>(componentData) = { 5, 10 };

		std::bitset<64> enabledComponents;
		constexpr std::size_t index = tuple_element_index_v<Position, ComponentSlice>;
		enabledComponents[index] = true;

		undoManager.AddCommandAndExecute<CreateEntityCommand>(componentData, enabledComponents, entityManager);

		entityManager.Update();
		ASSERT_EQ(entityManager.GetEntities().size(), 1);

		undoManager.Redo();
		entityManager.Update();
		ASSERT_EQ(entityManager.GetEntities().size(), 1);
	}
}
