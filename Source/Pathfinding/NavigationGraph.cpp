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
	using PriorityNode = std::pair<int, Vector2<int>>; // If the first elements are equal then the second element will be compared, making it more performant for priority to be first.

	NavigationGraph::NavigationGraph(IsometricScene& scene) : Scene(scene) {}

	std::vector<Vector2<int>> NavigationGraph::GetNeighbours(Vector2<int> centralNode) const // TODO: Make lambda and pass in via constructor? That way individual scenes could handle this function and it would avoid inheritence.
	{
		// Early exit to prevent never ending search. TODO: Probably better way to handle this.
		if (centralNode.Length() > 1000) { return {}; }

		// Start by getting the adjacent nodes to the passed node.
		std::vector adjacentNodes = Directions;
		for (auto& position : adjacentNodes)
		{
			position = (Vector2<int>)Scene.GridToWorldSpace((Vector2<float>)position) + centralNode;
		}

		// Get the length between adjacent nodes by using the world 0 as basis with a little buffer to account for corners of a grid cell.
		const int maxNodeDistanceSquared = Scene.GridToWorldSpace({ 0, 2 }).LengthSquared();

		// Remove inaccessible nodes.
		for (auto& entity : Scene.GetEntityManager().GetEntities())
		{
			if (!entity.HasComponents<Position, Collider, Sprite>()) { continue; } // Entities with a collider should have a position, but best check anyway.
			const Position& position = entity.GetComponent<Position>();

			// If not an adjacent node skip.
			const int distanceFromCentralNode = (centralNode - static_cast<Vector2<int>>(Vector2{ position.X, position.Y })).LengthSquared();
			if (distanceFromCentralNode > maxNodeDistanceSquared) { continue; }

			// Create a copy of the collision points to account for position and offset of sprite.
			std::vector<Vector2<float>> collisionNodes = entity.GetComponent<Collider>().Points; // Sequence of nodes to form edge of collider.
			for (auto& edge : collisionNodes)
			{
				edge += position;
				edge -= entity.GetComponent<Sprite>().PivotOffset;
			}

			// For each adjacent node see if the connection intersects the entity's colliders.
			std::erase_if(adjacentNodes, [centralNode, &collisionNodes](const Vector2<int> adjacentNode)
			{
				const Edge<float> pathEdge = { static_cast<Vector2<float>>(adjacentNode), static_cast<Vector2<float>>(centralNode) };
				for (size_t i = 1; i < collisionNodes.size(); ++i)
				{
					const Edge<float> collisionEdge = { collisionNodes[i - 1], collisionNodes[i] };
					if (Collision::LineSegmentIntersection(pathEdge, collisionEdge))
					{
						return true;
					}
				}

				return false;
			});

			if (adjacentNodes.empty()) { return adjacentNodes; } // Early exit if all nodes have been erased.
		}

		return adjacentNodes;
	}

	int NavigationGraph::GetCost(Vector2<int> current, Vector2<int> neighbour) const
	{
		return 1; // TODO: Query entity at positions for the relevant data to determine cost.
	}

	std::unordered_map<Vector2<int>, Vector2<int>> NavigationGraph::BreadthFirstSearch(
		Vector2<int> start, std::optional<Vector2<int>> goal) const
	{
		std::queue<Vector2<int>> toExplore;
		toExplore.push(start);
		std::unordered_map<Vector2<int>, Vector2<int>> edges;
		edges[start] = start; // Don't want start to be added as a neighbour of another node, and don't want to use pointers or optional.


		while (!toExplore.empty())
		{
			const Vector2<int> current = toExplore.front();
			toExplore.pop();

			// Early exit.
			if (current == goal) { break; }

			// Create a mapping from the current node to the neighbouring nodes if hasn't been found before,
			// storing frontier nodes for later exploration.
			const auto& neighbours = GetNeighbours(current);
			for (auto& neighbour : neighbours)
			{
				// If the neighbour hasn't already been reached, explore from its position later.
				if (!edges.contains(neighbour))
				{
					toExplore.push(neighbour);
					edges[neighbour] = current;
				}
			}
		}

		return edges;
	}

	std::unordered_map<Vector2<int>, Vector2<int>> NavigationGraph::AStar(Vector2<int> start,
		Vector2<int> goal) const
	{
		// Heuristic to expand search towards goal rather than equally in all directions.
		auto manhattanDistance = [](const Vector2<int> lhs, const Vector2<int> rhs) { return std::abs(lhs.X - rhs.X) + std::abs(lhs.Y - rhs.Y); };

		std::priority_queue<PriorityNode, std::vector<PriorityNode>, std::greater<>> toExplore;
		toExplore.emplace(0, start);
		std::unordered_map<Vector2<int>, Vector2<int>> edges;
		edges[start] = start; // Don't want start to be added as a neighbour of another node, and don't want to use pointers or optional.
		std::unordered_map<Vector2<int>, int> costSoFar;
		costSoFar[start] = 0;


		while (!toExplore.empty())
		{
			const Vector2<int> current = toExplore.top().second;
			toExplore.pop();

			// Early exit.
			if (current == goal) { break; }

			// Create a mapping from the current node to the neighbouring nodes if hasn't been found before,
			// storing frontier nodes for later exploration.
			const auto& neighbours = GetNeighbours(current);
			for (auto& neighbour : neighbours)
			{
				int newCost = costSoFar[current] + GetCost(current, neighbour);
				// If the neighbour hasn't already been reached or is cheaper to reach from this path, explore from its position later.
				if (!costSoFar.contains(neighbour) || newCost < costSoFar[neighbour])
				{
					costSoFar[neighbour] = newCost;
					toExplore.emplace(newCost + manhattanDistance(goal, neighbour), neighbour);
					edges[neighbour] = current;
				}
			}
		}

		if (!edges.contains(goal)) { return {}; }

		return edges;
	}

	std::vector<Vector2<int>> NavigationGraph::ConstructPath(
		const std::unordered_map<Vector2<int>, Vector2<int>>& edges, Vector2<int> start, Vector2<int> end)
	{
		// TODO: Unit test: No start, no end, no path from start to end, start came from start (Not sure if there's some edge case that could cause problems here).
		std::vector<Vector2<int>> path;

		Vector2<int> current = end;
		while (current != start)
		{
			// It's possible that the start and end node are in the map but not connected, e.g. an isolated island.
			// This also handles a situation where the start or end nodes aren't in the map.
			// It might be faster to do an early exit for those cases in some circumstances.
			if (!edges.contains(current)) { return {}; }

			path.push_back(current);
			current = edges.at(current);
		}
		path.push_back(start);
		std::ranges::reverse(path);

		return path;
	}
}
