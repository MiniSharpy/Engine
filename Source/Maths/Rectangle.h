#pragma once
#include "Vector2.h"
namespace Engine
{
	/// <summary>
	/// An axis-aligned rectangle with an origin at the top-left corner of the rectangle.
	/// </summary>
	/// <typeparam name="T">The numerical representation of the points in the rectangle. </typeparam>
	template <typename T>
	struct Rectangle
	{
		Vector2<T> Position;
		Vector2<T> Size;

		Rectangle() : Position(), Size() {}
		Rectangle(Vector2<T> position, Vector2<T> size) : Position(position), Size(size) {}
		Rectangle(T x, T y, T w, T h) : Position({ x,y }), Size({ w,h }) {}

		Vector2<T> Centre() // Probably want to return this as a float, or T. Whichever is largest because of division.
		{
			return Position + (Size / 2);
		}

		// TODO:? Implicit reinterpret_cast to SDL rects?
	};
}