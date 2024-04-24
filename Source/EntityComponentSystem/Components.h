#pragma once
#include "ComponentHelper.h"
#include "../Maths/Vector2.h"
#include "../Maths/Rectangle.h"
#include <optional>
#include <array>

// TODO: Constructors that are forwarded to AddComponent() in the EntityMemoryPool so that I don't need to manually do
// setup.
namespace Engine
{
	class Texture;

	// By inheriting and having a blank constructor it allows for multiple components of essentially the same type.
	// This means it is no longer an aggregate type (e.g. POD) preventing normal brace initialisation.
	struct Position : Vector2<float> 
	{ 
		int Z; // Okay the addition of Z makes it confusing with the inherited operators and constructors.
	};

	struct Velocity
	{
		float Speed;
		Vector2<float> Direction;
	};

	struct Zoom
	{
		float Value;
	};

	/// <summary>
	/// Holds mostly static render information about an object. Render position and size is not stored as it can be calculated from the 
	/// Position component when needed and changes essentially every frame negating any benefit from caching.
	///	This ends up duplicated quite a bit, in the interest of saving memory it might be worth pointing duplicates
	///	to a shared version if it proves too much.
	/// </summary>
	struct Sprite
	{
		char TextureName[64] = "\0";

		/// <summary>
		/// Sub-rectangle of texture to render.
		/// SDL_RenderCopyF requires ints for the source rectangle.
		/// </summary>
		Rectangle<int> SourceRectangle;

		/// <summary>
		/// For offsetting position component, essentially setting an entities origin in different places, e.g. Isometric sprite at the tip of the 
		/// floor diamond, player sprite at bottom-centre.
		/// </summary>
		Vector2<float> PivotOffset;
	};

	struct Animation
	{
		/// <summary>
		/// Time in milliseconds that the current loop of the animation has been playing.
		/// </summary>
		int Time;
	};

	struct Collider
	{
		int NumberOfPoints = 0;
		std::array<Vector2<float>, 32> Points;
	};

	struct Pathfinding
	{
		/// <summary>
		/// Grid coordinates.
		/// </summary>
		std::optional<Vector2<float>> Current;

		/// <summary>
		/// Grid coordinates.
		/// </summary>
		Vector2<float> Goal;
	};

	using Components = TypeList<Position, Velocity, Zoom, Sprite, Animation, Collider, Pathfinding>;
	using ComponentPool = ComponentHelper<Components>::Pool;
	using ComponentSlice = ComponentHelper<Components>::Slice;
	using ComponentReferenceSlice = ComponentHelper<Components>::ReferenceSlice;
}