#pragma once
#include "../Maths/Vector2.h"
#include "InputMisc.h"
#include "Modifiers/Modifier.h"
#include "Conditions/Condition.h"
#include <memory>

namespace Engine
{
	class Action;

	class Input
	{
	public:
		/// <summary>
		/// A collection of modifiers that process the input's value.
		///	These do not alter the object's value itself, this is so that modifiers don't just undo what they did the
		///	following frame, E.G. Negate, and negate again to end up with the original value.
		///	Instead, the action object creates a copy that is passed as the actual intended type to overloaded methods
		///	on the modifier.
		/// </summary>
		std::vector<std::unique_ptr<Modifier>> Modifiers;

		/// <summary>
		/// A collection of conditions that determine whether the input is able to be processed.
		/// </summary>
		std::vector<std::unique_ptr<Condition>> Conditions;

		/// <summary>
		/// Raw value set by the event loop before any modification. 
		/// </summary>
		ActionValue RawValue;

		/// <summary>
		/// SDL's event loop only handles events for that last key pressed, so to support multiple keys
		///	in use simultaneously they need to be processed elsewhere when input is received. This means
		///	a key press will only alter an Input on press and release.
		/// </summary>
		ProcessState CurrentProcessState = Stop;

		/// <summary>
		/// Some actions outputs are always multiplied by delta time elsewhere, while others depends on
		/// the input influencing the value. E.G. Camera zoom with mouse wheel doesn't need delta time
		/// as it occurs once, but camera zoom with axis or key occurs until dead zone or release.
		/// TODO: Do as modifier. This would require changes to how modifiers are initialised, 
		/// or use of singletones.
		/// </summary>
		bool handleDeltaTime;

		Input() : handleDeltaTime{false} {};
		Input(bool handleDeltaTime) : handleDeltaTime{handleDeltaTime} {};
		~Input() = default;

		// As we're dealing with unique pointers we don't want any copying as it results in a very unclear compiler error.
		Input(const Input& other) = delete; // Copy Constructor
		Input& operator=(const Input& other) = delete; // Copy assignment
		Input(Input&& other) noexcept = default; // Move Constructor
		Input& operator=(Input&& other) noexcept = default; // Move Assignment

		template<typename T, typename... Args>
		void AddModifier(Args&&... args)
		{
			Modifiers.emplace_back(std::make_unique<T>(std::forward<T>(args)...));
		}

		template<typename T, typename... Args>
		void AddCondition(Args&&... args)
		{
			Conditions.emplace_back(std::make_unique<T>(std::forward<T>(args)...));
		}
	};
}
