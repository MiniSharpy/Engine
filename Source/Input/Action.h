#pragma once
#include "../Maths/Vector2.h"
#include "InputMisc.h"
#include "Input.h"
#include <map>
#include <variant>

namespace Engine
{
	/// <summary>
	///	A wrapper of some behaviour, e.g. entity movement, and the inputs that trigger it. The inputs are processed according
	///	to their specific modifiers to create a seamless abstraction upon raw inputs between potentially different devices.
	/// Inspired by Unreal Engine 5's Enhanced Input system.
	/// </summary>
	class Action
	{
	private:
		/// <summary>
		/// The type of the value held in the ActionValue union. This determines how the event loop will convert its value
		/// for an input, E.G. key press to float, or 2D vector.
		/// </summary>
		ActionType Type;

		/// <summary>
		/// Behaviour to execute when an input is triggered.
		/// TODO: As function pointers cannot be used with lambdas that capture objects std::function is instead used,
		/// but this is quite a heavy object so it might be worth looking into alternatives.
		/// </summary>
		std::function<void(ActionValue)> BoundFunction;

		/// <summary>
		/// A mapping from the name of an input to the input object itself. 
		/// </summary>
		std::map<std::string, Input> BoundInputs;
	public:

		/// <param name="type">The type of value that needs to be passed to the bound function.</param>
		/// <param name="boundFunction">The logic to be executed when bound inputs are triggered.</param>
		/// or when the input is released.</param>
		Action(ActionType type, std::function<void(ActionValue)> boundFunction);

		// No copies as Input objects contain unique pointers.
		Action(const Action& other) = delete; // Copy Constructor
		Action& operator=(const Action& other) = delete; // Copy assignment
		Action(Action&& other) noexcept = default; // Move Constructor
		Action& operator=(Action&& other) noexcept = default; // Move Assignment

		/// <summary>
		/// Creates and bind an input object, given its name, and modifiers, to the action.
		/// </summary>
		/// <typeparam name="...T">The modifiers to default construct.</typeparam>
		/// <param name="inputName">The name of the input that the underlying event loop updates.</param>
		template<typename ...T>
		void BindInput(const std::string& inputName)
		{
			Input input = Input();
			(input.Modifiers.emplace_back(std::make_unique<T>()), ...);

			BoundInputs.emplace(inputName, std::move(input));
		}

		/// <summary>
		///	Filters inputs that need to be processed and the conditionally calls the corresponding
		///	ProcessType() function depending on the action's value. Input's modifiers are then processed,
		///	the final value created, and the bound function called.
		/// </summary>
		void Process();

		bool HasInput(const std::string& string) const;
		Input& GetInput(const std::string& string);

		ActionType GetType() const;

	private:
		/// <summary>
		/// Iterates over all bound inputs, handling those that need to be processed, and then calls the bound
		/// function if one is found.
		/// </summary>
		void ProcessTrigger(auto inputsToProcess);

		/// <summary>
		///	Iterates over all bound inputs, handling those that need to be processed, getting the highest absolute value,
		///	and then calls the bound function with the final value as the parameter.
		///	By using only the highest value it prevents multiple inputs compounding to create an unexpectedly high value.
		/// </summary>
		void ProcessFloat(auto inputsToProcess);

		/// <summary>
		///	Iterates over all bound inputs, handling those that need to be processed, getting the highest absolute X and Y
		///	separately, and then calls the bound function with the final value as the parameter.
		///	By using only the highest value it prevents multiple inputs compounding to create an unexpectedly high value.
		/// </summary>
		void ProcessVector(auto inputsToProcess);
	};
}
