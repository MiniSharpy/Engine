#pragma once
namespace Engine
{
	class IGrid
	{
	public:
		IGrid() = default;
		virtual ~IGrid() = default;

		/// <summary>
		/// Converts screen space coordinates(pixels) to grid coordinates(index), taking into account camera offset and zoom.
		/// </summary>
		virtual Vector2<float> ScreenSpaceToGrid(Vector2<float> screen, bool floor = true) const = 0;
		/// <summary>
		/// Converts grid coordinates(index) to world space coordinate(pixels), taking into account camera offset.
		/// </summary>
		virtual Vector2<float> GridToWorldSpace(Vector2<float> grid) const = 0;
	};



}