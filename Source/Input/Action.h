#pragma once
#include "../Maths/Vector2.h"
#include "InputMisc.h"
#include "Input.h"
#include <map>
#include <variant>

namespace Engine
{
	/// <summary>
	///	A wrapper of some behaviour, e.g. camera movement, and the inputs that trigger it. The inputs are processed according
	///	to their specific modifiers to create a seamless abstraction upon raw inputs between potentially different devices.
	/// Inspired by Unreal Engine 5's Enhanced Input system.
	/// </summary>
	class Action
	{
	public:
		ActionType Type;
		std::function<void(ActionValue)> BoundFunction;
		// std::vector<Input> BoundInputs;
		std::map<std::string, Input> BoundInputs;
		Action(ActionType type, std::function<void(ActionValue)> boundFunction);

		/// <summary>
		/// Create and bind an input object to the action given the input's name, conditions, and modifiers.
		/// </summary>
		/// <param name="inputName">The name of the input that the underlying event loop updates.</param>
		void BindInput(const std::string& inputName);

		/// <summary>
		///	Conditionally calls the corresponding ProcessType() function which handles the inputs conditions, modifiers,
		///	and creating its final value.
		/// </summary>
		void Process();

		/// <summary>
		/// Iterates over all bound inputs, handling those that need to be processed, and then calls the bound
		/// function if one is found.
		/// </summary>
		void ProcessTrigger();

		/// <summary>
		///	Iterates over all bound inputs, handling those that need to be processed, getting the highest absolute value,
		///	and then calls the bound function with the final value as the parameter.
		///	By using only the highest value it prevents multiple inputs compounding to create an unexpectedly high value.
		/// </summary>
		void ProcessFloat();

		/// <summary>
		///	Iterates over all bound inputs, handling those that need to be processed, getting the highest absolute X and Y
		///	separately, and then calls the bound function with the final value as the parameter.
		///	By using only the highest value it prevents multiple inputs compounding to create an unexpectedly high value.
		/// </summary>
		void ProcessVector();

		bool HasInput(const std::string& string) const;
		Input& GetInput(const std::string& string);
	};
}
