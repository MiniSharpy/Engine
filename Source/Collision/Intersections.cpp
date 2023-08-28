#include "Intersections.h"
#include "../Maths/Vector2.h"
#include "../Maths/Rectangle.h"

namespace Engine
{
	std::optional<Vector2<float>> Collision::LineSegmentIntersection(Edge<float> edgeA, Edge<float> edgeB)
	{
		Vector2<float> r = edgeA.second - edgeA.first; // Direction.
		Vector2<float> s = edgeB.second - edgeB.first;
		float rxs = Vector2<float>::CrossProduct(r, s);
		Vector2<float> cma = edgeB.first - edgeA.first;
		float t = Vector2<float>::CrossProduct(cma, s) / rxs;
		float u = Vector2<float>::CrossProduct(cma, r) / rxs;

		if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
		{
			return Vector2{ edgeA.first.X + t * r.X, edgeA.first.Y + t * r.Y };
		}

		return {};
	}

	std::optional<Vector2<float>> Collision::RectangleIntersection(Rectangle<float> rectangleA, Rectangle<float> rectangleB)
	{
		// Intersection is done of the centre point of the rectangle.
		rectangleA.Position = rectangleA.Centre();
		rectangleB.Position = rectangleB.Centre();

		// Absolute means the position of each rectangle relative to each other doesn't matter.
		float dx = abs(rectangleA.Position.X - rectangleB.Position.X);
		float overlapX = (rectangleA.Size.X / 2) + (rectangleB.Size.X / 2) - dx;
		float dy = abs(rectangleA.Position.Y - rectangleB.Position.Y);
		float overlapY = (rectangleA.Size.Y / 2) + (rectangleB.Size.Y / 2) - dy;

		if (overlapX > 0 && overlapY > 0)
		{
			return Vector2<float>{ overlapX, overlapY };
		}

		return {};
	}
}

