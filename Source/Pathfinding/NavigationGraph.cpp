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

	std::vector<Vector2<int>> NavigationGraph::GetNeighbours(Vector2<int> centralNode) const
	{
		// Start by getting the adjacent nodes to the passed node.
		std::vector adjacentNodes = Directions;
		for (auto& position : adjacentNodes)
		{
			position = position + centralNode;
		}

		// Remove inaccessible nodes.
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
				edge = Scene.ScreenSpaceToGrid(edge);
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
				Edge<float> edge(static_cast<Vector2<float>>(current), static_cast<Vector2<float>>(neighbour));
				// If the connection hasn't already been added..
				if (std::ranges::find(Connections, edge) == Connections.end())
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

	std::unordered_map<Vector2<int>, Vector2<int>> NavigationGraph::BreadthFirstSearch(
		Vector2<int> start, std::optional<Vector2<int>> goal) const
	{
		// https://www.redblobgames.com/pathfinding/a-star/introduction.html
		std::queue<Vector2<int>> frontier;
		frontier.emplace(start);
		std::unordered_map<Vector2<int>, Vector2<int>> cameFrom;
		cameFrom[start] = start; // Don't want start to be added as a neighbour of another node.


		while (!frontier.empty())
		{
			const Vector2<int> current = frontier.front();
			frontier.pop();

			if (current == goal) { break; }

			// Create a mapping from the current node to the neighbouring nodes if hasn't been found before,
			// storing frontier nodes for later exploration.
			const auto& neighbours = GetNeighbours(current);
			for (auto& neighbour : neighbours)
			{
				// If neighbour hasn't already been reached.
				if (!cameFrom.contains(neighbour))
				{
					frontier.push(neighbour);
					cameFrom[neighbour] = current;
				}
			}
		}

		return cameFrom;
	}

	std::vector<Vector2<int>> NavigationGraph::ConstructPath(
		const std::unordered_map<Vector2<int>, Vector2<int>>& cameFrom, Vector2<int> start, Vector2<int> goal)
	{
		Vector2<int> current = goal;

		std::vector<Vector2<int>> path;
		while (current != start)
		{
			path.push_back(current);
			current = cameFrom.at(current); // TODO: Handle possible thrown exception.
		}
		path.push_back(start);
		std::ranges::reverse(path);

		return path;
	}
}
