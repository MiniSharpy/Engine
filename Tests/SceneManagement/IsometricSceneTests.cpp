#include "../../Source/EntityComponentSystem/Components.h"
#include "../../Source/SceneManagement/IsometricScene.h"
#include "../../Source/EntityComponentSystem/EntityManager.h"
#include <vector>
#include <gtest/gtest.h>

namespace Engine
{
	//          0         1         2         3         4
	//	  Y:Z{ 1,0 }, { 2,1 }, { 0, 0 }, { 1, 1 }, { 0, 2 }
	//          2         0         3         1         4
	// output{ 0,0 }, { 1,0 }, {1, 1},   {2, 1},   {0, 2}	
	TEST(VectorTests, ZSorting)
	{
		const std::vector<Position> input = { {{-64, 32}, 0 }, {{-128, 64}, 1}, {{0, 0}, 0}, {{-64, 32}, 1}, {{0, 0}, 2} };
		const std::vector<Position> output = { input[2], input[0], input[3], input[1], input[4]};
		
		EntityManager entityManager;
		for (auto& position : input)
		{
			Entity entity = entityManager.AddEntity("ZSorting");
			entity.AddComponent<Position>() = position;
		}

		std::vector<Entity>& entities = entityManager.GetEntitiesByTag("ZSorting");

		IsometricScene::SortEntities(entities);

		for (size_t i = 0; i < entities.size(); i++)
		{
			Position expected = output[i];
			Position actual = entities[i].GetComponent<Position>();

			ASSERT_EQ(expected.Y, actual.Y);
			ASSERT_EQ(expected.Z, actual.Z);
		}
	}

	TEST(VectorTests, YSorting)
	{
		std::vector<Position> input = { {{0,64}}, {{64, 32 }} };
		std::vector<Position> output = { {{64, 32 }}, {{0,64}} };
	
		EntityManager entityManager;
		for (auto& position : input)
		{
			Entity entity = entityManager.AddEntity("YSorting");
			entity.AddComponent<Position>() = position;
		}

		std::vector<Entity>& entities = entityManager.GetEntitiesByTag("YSorting");

		IsometricScene::SortEntities(entities);

		for (size_t i = 0; i < entities.size(); i++)
		{
			Position expected = output[i];
			Position actual = entities[i].GetComponent<Position>();

			ASSERT_EQ(expected.Y, actual.Y);
			ASSERT_EQ(expected.Z, actual.Z);
		}
	}

	TEST(VectorTests, XSorting)
	{
		std::vector<Position> input = { {{0, 64}}, {{-64, 32}} };
		std::vector<Position> output = { {{-64, 32}}, {{0, 64}}};
	
		EntityManager entityManager;
		for (auto& position : input)
		{
			Entity entity = entityManager.AddEntity("YSorting");
			entity.AddComponent<Position>() = position;
		}

		std::vector<Entity>& entities = entityManager.GetEntitiesByTag("YSorting");

		IsometricScene::SortEntities(entities);

		for (size_t i = 0; i < entities.size(); i++)
		{
			Position expected = output[i];
			Position actual = entities[i].GetComponent<Position>();

			ASSERT_EQ(expected.Y, actual.Y);
			ASSERT_EQ(expected.Z, actual.Z);
		}
	}
}