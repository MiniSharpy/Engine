#pragma once
#include "../Maths/Vector2.h"
#include <optional>
#include <vector>
namespace Engine
{
	class IsometricScene;

	class NavigationGraph
	{
	private:
		IsometricScene& Scene;

		std::vector <Vector2<int>> Directions = 
		{
			{1, 1}, {1, -1}, {-1, -1}, {-1, 1}, // Up, Down, Left, Right.
			{0, -1}, {0, 1}, {-1, 0}, {1, 0} // Top Right, Bottom Left, Top Left, Bottom Right.
		};

		std::vector<Edge<float>> Connections;
	public:
		NavigationGraph(IsometricScene& scene);

		std::vector<Vector2<int>> GetNeighbours(Vector2<int> centralNode) const;

		void CacheConnections(Vector2<int> start);
		const std::vector<Edge<float>>& GetConnections() const;

		/// <summary>
		/// Explore a navigation graph and get the optimal route from all locations to the provided coordinate.
		/// </summary>
		/// <param name="start">Point to explore from as a Vector2 in world coordinates. This should be in the centre of a grid cell, not its corner, for collision accuracy. </param>
		/// <param name="goal">Optional end goal for an early exit.</param>
		/// <returns>A mapping of paths between two nodes.</returns>
		std::unordered_map<Vector2<int>, Vector2<int>> BreadthFirstSearch(Vector2<int> start, std::optional<Vector2<int>> goal = std::nullopt) const;

		static std::vector<Vector2<int>> ConstructPath(const std::unordered_map<Vector2<int>, Vector2<int>>& cameFrom, Vector2<int> start, Vector2<int> goal);

	};
}