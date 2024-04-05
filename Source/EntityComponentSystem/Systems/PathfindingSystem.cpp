#include "PathfindingSystem.h"
#include "../../SceneManagement/BaseScene.h"
#include "../../SceneManagement/IsometricScene.h"

namespace Engine
{
	PathfindingSystem::PathfindingSystem(BaseScene& scene) : OwningScene(scene)
	{
		
	}

	void PathfindingSystem::Update(const float& deltaTime)
	{
		IsometricScene* scene = dynamic_cast<IsometricScene*>(&OwningScene);
		if (!scene) { return; }

		for (Entity entity : OwningScene.GetEntityManager().GetEntities())
		{
			if (!entity.HasComponents<Position, Velocity, Pathfinding>()) { continue; }

			Pathfinding& pathfinding = entity.GetComponent<Pathfinding>();
			Position& position = entity.GetComponent<Position>();
			Velocity& velocity = entity.GetComponent<Velocity>();

			if (pathfinding.Current)
			{
				Vector2<float> currentPosition = { position.X, position.Y };

				constexpr auto almostEquals = [](float lhs, float rhs)
				{
					constexpr float epsilon = 1.f;
					return std::abs(lhs - rhs) < epsilon ? true : false;
				};


				SDL_Log("Target: %f, %f", pathfinding.Current->X, pathfinding.Current->Y);
				SDL_Log("Current: %f, %f", currentPosition.X, currentPosition.Y);

				if (almostEquals(pathfinding.Current->X, currentPosition.X) &&
					almostEquals(pathfinding.Current->Y, currentPosition.Y))
				{
					velocity.Speed = 0;
					pathfinding.Current = std::nullopt;
				}
			}
			else
			{
				Vector2 grid = scene->WorldSpaceToGrid({ position.X, position.Y });
				Vector2 clampedWorld = scene->GridToWorldSpace(grid);
				const Vector2<int> start = static_cast<Vector2<int>>(clampedWorld) + Vector2<int>{ 0, scene->TileSize.Y / 4 };
				const Vector2<int> goal = static_cast<Vector2<int>>(pathfinding.Goal);

				auto edges = scene->ManagedNavigationGraph.AStar(start, goal);
				auto path = scene->ManagedNavigationGraph.ConstructPath(edges, start, goal);

				if (edges.size() > 1)
				{
					velocity.Speed = 256;
					velocity.Direction = static_cast<Vector2<float>>(path[1] - path[0]);
					pathfinding.Current = static_cast<Vector2<float>>(path[1]);
				}
			}
		}
	}
}
