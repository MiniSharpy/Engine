#pragma once
#include "ComponentHelper.h"
#include "../Maths/Vector2.h"
#include "../Maths/Rectangle.h"
#include <optional>
#include <vector>

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
		/// <summary>
		/// Pointer to texture rather than a reference so that default entities can be list initialised in the same vein as other components.
		/// Should never be a nullptr provided objects are constructed properly.
		///	TODO: It would be safer to just store the path to the texture/name in the map rather than the pointer and access it through the map.
		///	Though that would mean no access to helper functions which is a bit of a loss. Perhaps std::optional instead?
		/// </summary>
		Texture* SourceTexture = nullptr;

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
		std::vector<Vector2<float>> Points; // An array might be better here for maximising cache hits, but it would mean setting a maximum size.
		// TODO: Bitmask for affecting things like path finding, lighting, sound, etc.
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