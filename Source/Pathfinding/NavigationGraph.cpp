#include "NavigationGraph.h"
#include "../Maths/Vector2.h"
#include "../Collision/Intersections.h"
#include "../SceneManagement/IsometricScene.h"
#include <vector>
#include <array>
#include <algorithm>
#include <queue>

namespace Engine
{
	NavigationGraph::NavigationGraph(IsometricScene& scene) : Scene(scene)
	{
	}

	std::vector<Vector2<int>> Engine::NavigationGraph::GetNeighbours(Vector2<int> centralNode)
	{
		std::vector adjacentNodes = Directions;
		for (auto& position : adjacentNodes)
		{
			position = (Vector2<int>)Scene.GridToWorldSpace((Vector2<float>)position) + centralNode;
		}

		for (auto& entity : Scene.GetEntityManager().GetEntities())
		{
			if (!entity.HasComponents<Position, Collider, Sprite>()) { continue; } // Entities with a collider should have a position, but best check anyway.
			// TODO: Restrict to only nearby entities.

			// Create a copy of the points to account for position and offset.
			std::vector<Vector2<float>> collisionEdges = entity.GetComponent<Collider>().Points;
			for (auto& edge : collisionEdges)
			{
				edge += entity.GetComponent<Position>();
				edge -= entity.GetComponent<Sprite>().PivotOffset;
			}

			// For each adjacent node see if the connection intersects the entity's colliders.
			auto isColliding = std::ranges::remove_if(adjacentNodes, [centralNode, &collisionEdges](Vector2<int> adjacentNode) 
			{
				if (!collisionEdges.empty())
				{
					Edge<float> connection = { (Vector2<float>)(adjacentNode), (Vector2<float>)centralNode };
					for (auto it = collisionEdges.cbegin(); it != collisionEdges.cend() - 1; ++it)
					{
						if (Collision::LineSegmentIntersection({ *it, *(it + 1) }, connection))
						{
							return true;
						}
					}
				}

				return false;
			}).begin();

			adjacentNodes.erase(isColliding, adjacentNodes.end());
			if (adjacentNodes.empty())
			{
				return adjacentNodes;
			}
		}

		return adjacentNodes;
	}

	void NavigationGraph::CacheConnections(Vector2<int> start)
	{
		Connections = {};

		std::queue frontier = std::queue<Vector2<int>>();
		frontier.emplace(start);

		while (!frontier.empty())
		{
			Vector2<int> current = frontier.front();
			frontier.pop();

			const auto& neighbours = GetNeighbours(current);
			for (auto& neighbour : neighbours)
			{
				Edge<float> edge((Vector2<float>)current, (Vector2<float>)neighbour);
				// If the connection hasn't already been added..
				if (std::find(Connections.begin(), Connections.end(), edge) == Connections.end())
				{
					frontier.push(neighbour);
					Connections.push_back(edge);
				}
			}
		}
	}

	const std::vector<Edge<float>>& NavigationGraph::GetConnections() const
	{
		return Connections;
	}

	std::unordered_map<Vector2<int>, Vector2<int>> NavigationGraph::BreadthFirstSearch(Vector2<int> start)
	{
		// https://www.redblobgames.com/pathfinding/a-star/introduction.html
		std::queue frontier = std::queue<Vector2<int>>();
		frontier.emplace(start);
		std::unordered_map cameFrom = std::unordered_map<Vector2<int>, Vector2<int>>();


		while (!frontier.empty())
		{
			Vector2<int> current = frontier.front();
			frontier.pop();

			const auto& neighbours = GetNeighbours(current);
			for (auto& neighbour : neighbours)
			{
				// If neighbour hasn't already been reached.
				if (cameFrom.count(neighbour) == 0)
				{
					frontier.push(neighbour);
					cameFrom[neighbour] = current;
				}
			}
		}

		// Start shouldn't be reachable from anywhere, but because it's not set to anything in the map it means when
		// the first node that is reached from start begins to explore it'll "find" start and add it to the map, 
		// necessitating its removal.
		cameFrom.erase(start);

		return cameFrom;
	}
}
