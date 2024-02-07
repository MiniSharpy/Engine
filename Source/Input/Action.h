#pragma once
#include "../Maths/Vector2.h"
#include "Input.h"
#include <map>
#include <variant>
#include <ranges>

namespace Engine
{
	class IAction
	{
	public:
		virtual ~IAction() = default;

		virtual void Process() = 0;

		virtual void Update(const std::string& name, const ProcessState processState) = 0;
		virtual void Update(const std::string& name, const ProcessState processState, float value) = 0;
		virtual void Update(const std::string& name, const ProcessState processState, Vector2<float> value) = 0;

		virtual bool HasInput(const std::string& string) const = 0;
		virtual Input& GetInput(const std::string& string) = 0;
	};

	/// <summary>
	///	A wrapper of some behaviour, e.g. entity movement, and the inputs that trigger it. The inputs are processed according
	///	to their specific modifiers to create a seamless abstraction upon raw inputs between potentially different devices.
	/// Inspired by Unreal Engine 5's Enhanced Input system. 
	/// </summary>
	/// <typeparam name="...T">The types of the arguments in the callback function.</typeparam>
	template<typename ...T>
	class Action final : public IAction
	{
	private:
		/// <summary>
		/// Behaviour to execute when an input is triggered.
		/// </summary>
		std::function<void(T...)> BoundFunction;
		/// <summary>
		/// A mapping from the name of an input to the input object itself. 
		/// </summary>
		std::map<std::string, Input> BoundInputs;

		/// <summary>
		/// Determines how inputs are combined for this action.
		/// </summary>
		bool CumulateInputs;
	public:

		/// <param name="boundFunction">The logic to be executed when bound inputs are triggered.</param>
		/// or when the input is released.</param>
		/// <param name="cumulateInputs">When true inputs will add upon each other, when false the highest absolute value will be used.</param>
		Action(std::function<void(T...)> boundFunction, bool cumulateInputs = false)
			: BoundFunction(std::move(boundFunction)), CumulateInputs(cumulateInputs) {}
		~Action() override = default;

	private:
		/// <summary>
		/// Filters inputs according to their process state and attached conditions.
		/// </summary>
		/// <returns>The filtered values of the BoundInputs map.</returns>
		auto FilterInputs();

	public:
		/// <summary>
		///	Processes all inputs attached to this action by determining each input's actual value.
		///	The bound function is called using either the highest value or cumulative value of each input.
		/// </summary>
		void Process() override;

		/// <summary>
		/// Updates the raw value of an input, converting values to match the expected value of the
		/// bound function. E.G. trigger -> float (1.0f).
		/// </summary>
		void Update(const std::string& name, const ProcessState processState) override;
		/// <summary>
		/// Updates the raw value of an input, converting values to match the expected value of the
		/// bound function. E.G. float (0.85f) -> vector2 (right vector * 0.85).
		/// </summary>
		void Update(const std::string& name, const ProcessState processState, float value) override;
		/// <summary>
		/// Updates the raw value of an input, converting values to match the expected value of the
		/// bound function. E.G. vector2 (8, 4) -> float ((8,4).length).
		/// </summary>
		void Update(const std::string& name, const ProcessState processState, Vector2<float> value) override;

		bool HasInput(const std::string& string) const override { return BoundInputs.contains(string); }
		Input& GetInput(const std::string& string) override { return BoundInputs.find(string)->second; }

		/// <summary>
		/// Creates and bind an input object, given its name, and modifiers, to the action.
		/// </summary>
		/// <typeparam name="...T">The modifiers to default construct.</typeparam>
		/// <param name="inputName">The name of the input that the underlying event loop updates.</param>
		template<typename ...Modifiers>
		void BindInput(const std::string& inputName)
		{
			Input input;
			(input.Modifiers.emplace_back(std::make_unique<Modifiers>()), ...);

			BoundInputs.emplace(inputName, std::move(input));
		}
	};
}
