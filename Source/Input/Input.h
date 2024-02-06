#pragma once
#include "../Maths/Vector2.h"
#include "Modifiers/Modifier.h"
#include "Conditions/Condition.h"
#include <memory>
#include <variant>

namespace Engine
{
	class Action;

	using ActionValue = std::variant<std::monostate, float, Engine::Vector2<float>>;

	enum ProcessState
	{
		/// <summary>
		/// The default inactive state, occurring when an input is in a relaxed state.
		/// </summary>
		Stop,
		/// <summary>
		/// Some inputs do not have release events or ranges that cause them to stop processing, such as the mouse
		/// scrolling, and so should only occur once.
		/// </summary>
		Once,
		/// <summary>
		/// Most inputs will use this as it's not guaranteed SDL events will occur multiple times,
		/// e.g. multiple key presses, or constant axis values.
		///	A method of nullifying these inputs is required, like setting to Once on key up, or
		///	in setting the value to 0 in processing, such as an an axis within the dead zone.
		/// </summary>
		Continuous,
		/// <summary>
		/// Certain inputs like Buttons/Keys also have release events that might cause processing to be done at the end.
		/// </summary>
		Release
	};

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

		Input() = default;
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
