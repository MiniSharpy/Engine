#pragma once
#include "../Maths/Vector2.h"
#include "../Maths/Rectangle.h"
#include <optional>

namespace Engine
{
	namespace Collision
	{
		/// <summary>
		/// Calculates the point of intersection between two lines.
		/// </summary>
		/// <returns>The found point of intersection, if it exists.</returns>
		std::optional<Vector2<float>> LineSegmentIntersection(Edge<float> edgeA, Edge<float> edgeB);
		/// <summary>
		/// Calculates the size of the intersection between two rectangles.
		/// </summary>
		/// <returns>The size of the intersection, if it exists.</returns>
		std::optional<Vector2<float>> RectangleIntersection(Rectangle<float> rectangleA, Rectangle<float> rectangleB);
	}
}