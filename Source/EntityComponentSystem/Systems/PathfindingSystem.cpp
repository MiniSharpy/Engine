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

			Vector2<float> currentPosition = Vector2<float>{ position.X, position.Y };

			if (pathfinding.Current)
			{

				constexpr auto almostEquals = [](float lhs, float rhs)
				{
					constexpr float epsilon = 5.f;
					return std::abs(lhs - rhs) < epsilon ? true : false;
				};

				Vector2<float> targetPosition = Vector2<float>{ pathfinding.Current->X, pathfinding.Current->Y };


				SDL_Log("Current: %f, %f", currentPosition.X, currentPosition.Y);
				SDL_Log("Target: %f, %f", targetPosition.X, targetPosition.Y);

				if (almostEquals(targetPosition.X, currentPosition.X) &&
					almostEquals(targetPosition.Y, currentPosition.Y))
				{
					velocity.Speed = 0;
					pathfinding.Current = std::nullopt;

					// Just ensure it's exactly in the correct place. Should help prevent floating point errors adding up.
					position.X = targetPosition.X;
					position.Y = targetPosition.Y;
				}
			}
			else
			{
				const Vector2<int> start = static_cast<Vector2<int>>(scene->WorldSpaceToGrid({position.X, position.Y}));
				const Vector2<int> goal = static_cast<Vector2<int>>(pathfinding.Goal);

				if (start == goal) { continue; }

				auto edges = scene->ManagedNavigationGraph.AStar(start, goal);
				auto path = scene->ManagedNavigationGraph.ConstructPath(edges, start, goal);

				if (edges.size() > 1)
				{
					velocity.Speed = 256;
					velocity.Direction = static_cast<Vector2<float>>(path[1] - static_cast<Vector2<int>>(currentPosition));
					velocity.Direction.Normalise();
					pathfinding.Current = static_cast<Vector2<float>>(path[1]);
				}
			}
		}
	}
}
