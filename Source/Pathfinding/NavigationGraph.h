#pragma once
#include "../Maths/Vector2.h"
#include <optional>
#include <vector>
namespace Engine
{
	class IsometricScene;

	/// <summary>
	/// Navigation graph to perform pathfinding. Most algorithm adapted from:
	/// https://www.redblobgames.com/pathfinding/a-star/introduction.html
	/// </summary>
	class NavigationGraph
	{
	private:
		IsometricScene& Scene;

		std::vector <Vector2<int>> Directions = 
		{
			{1, 1}, {1, -1}, {-1, -1}, {-1, 1}, // Up, Down, Left, Right.
			{0, -1}, {0, 1}, {-1, 0}, {1, 0} // Top Right, Bottom Left, Top Left, Bottom Right.
		};

	public:
		NavigationGraph(IsometricScene& scene);

		std::vector<Vector2<int>> GetNeighbours(Vector2<int> centralNode) const;
		int GetCost(Vector2<int> current, Vector2<int> neighbour) const;

		/// <summary>
		/// Explore the graph evenly from the starting node outwards.
		/// </summary>
		/// <param name="start">The starting node to explore from. This should be in the centre of a grid cell, not its corner, for collision accuracy. </param>
		/// <param name="goal">The optional end node for an early exit, if found.</param>
		/// <returns>A mapping of edges between nodes.</returns>
		std::unordered_map<Vector2<int>, Vector2<int>> BreadthFirstSearch(Vector2<int> start, std::optional<Vector2<int>> goal = std::nullopt) const;

		/// <summary>
		/// Explore the graph weighted towards the goal node from the starting node.
		/// </summary>
		/// <param name="start">The starting node to explore from. This should be in the centre of a grid cell, not its corner, for collision accuracy.</param>
		/// <param name="goal">The goal node to search for.</param>
		/// <returns>A mapping of edges between nodes, empty if the goal node was not reached.</returns>
		std::unordered_map<Vector2<int>, Vector2<int>> AStar(Vector2<int> start, Vector2<int> goal) const;

		/// <summary>
		/// Constructs a path backwards from a goal node to the start node using a provided node sequence.
		/// </summary>
		/// <param name="edges">A mapping of a node to the node that leads to it.</param>
		/// <param name="start">The starting node to path to. This should be in the centre of a grid cell, not its corner, for collision accuracy.</param>
		/// <param name="goal">The goal node to path from.</param>
		/// <returns>A sequence of nodes that form a path from the start node to the goal node. <br>
		/// If a path is not possible an empty collection will be returned.</returns>
		static std::vector<Vector2<int>> ConstructPath(const std::unordered_map<Vector2<int>, Vector2<int>>& edges, Vector2<int> start, Vector2<int> end);

	};
}