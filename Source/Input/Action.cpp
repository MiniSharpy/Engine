#include "Action.h"
#include <ranges>

namespace Engine
{
	template <typename ... T>
	auto Action<T...>::FilterInputs()
	{
		auto enabled = [](const RawInput& input) { return input.CurrentProcessState != Stop; };
		auto conditions = [](const RawInput& input)
		{
			for (auto&& condition : input.Conditions)
			{
				if (!(*condition)(input))
				{
					return false;
				}
			}
			return true;
		};

		// This is lazily evaluated and the modifiers alter their own state, so it must not be accessed until
		// each element is acted upon otherwise a condition could change.
		// E.G. PressedCondition will set its previous process state which if called multiple times will
		// result in it treating an input as a hold when it's actually only been pressed.
		return BoundInputs
			| std::views::values
			| std::views::filter(enabled)
			| std::views::filter(conditions);
	}

	template <>
	void Action<>::Process()
	{
		bool execute = false;
		for (auto& input : FilterInputs())
		{
			execute = true;
			if (input.CurrentProcessState != Continuous)
			{
				input.CurrentProcessState = Stop;
			}
		}

		if (execute)
		{
			BoundFunction();
		}
	}

	template <>
	void Action<float>::Process()
	{
		bool execute = false;
		float finalValue = 0;
		for (RawInput& input : FilterInputs())
		{
			execute = true;
			float value = std::get<float>(input.RawValue);
			for (const auto& modifier : input.Modifiers)
			{
				(*modifier)(value);
			}

			if (CumulateInputs)
			{
				finalValue += value;
			}
			else
			{
				finalValue = abs(value) > abs(finalValue) ? value : finalValue;
			}

			if (input.CurrentProcessState != Continuous)
			{
				input.CurrentProcessState = Stop;
			}
		}

		if (execute)
		{
			BoundFunction(finalValue);
		}
	}

	template <>
	void Action<Vector2<float>>::Process()
	{
		bool execute = false;
		Vector2<float> finalValue = Vector2<float>::Zero();
		for (RawInput& input : FilterInputs())
		{
			execute = true;
			Vector2<float> value = std::get<Vector2<float>>(input.RawValue);
			for (const auto& modifier : input.Modifiers)
			{
				(*modifier)(value);
			}

			if (CumulateInputs)
			{
				finalValue += value;
			}
			else
			{
				finalValue.X = abs(value.X) > abs(finalValue.X) ? value.X : finalValue.X;
				finalValue.Y = abs(value.Y) > abs(finalValue.Y) ? value.Y : finalValue.Y;
			}

			if (input.CurrentProcessState != Continuous)
			{
				input.CurrentProcessState = Stop;
			}
		}

		if (execute)
		{
			BoundFunction(finalValue);
		}
	}

	template <>
	void Action<>::Update(const std::string& name, const ProcessState processState)
	{
		GetInput(name).CurrentProcessState = processState;
	}

	template <>
	void Action<>::Update(const std::string& name, const ProcessState processState, float value)
	{
		Update(name, processState);
	}

	template <>
	void Action<>::Update(const std::string& name, const ProcessState processState, Vector2<float> value)
	{
		Update(name, processState);
	}

	template <>
	void Action<float>::Update(const std::string& name, const ProcessState processState)
	{
		RawInput& input = GetInput(name);
		input.CurrentProcessState = processState;
		input.RawValue = 1.0f;
	}

	template <>
	void Action<float>::Update(const std::string& name, const ProcessState processState, float value)
	{
		RawInput& input = GetInput(name);
		input.CurrentProcessState = processState;
		input.RawValue = value;
	}

	template <>
	void Action<float>::Update(const std::string& name, const ProcessState processState, Vector2<float> value)
	{
		RawInput& input = GetInput(name);
		input.CurrentProcessState = processState;
		input.RawValue = value.Length();
	}

	template <>
	void Action<Vector2<float>>::Update(const std::string& name, const ProcessState processState)
	{
		RawInput& input = GetInput(name);
		input.CurrentProcessState = processState;
		input.RawValue = Vector2<float>::Right();
	}

	template <>
	void Action<Vector2<float>>::Update(const std::string& name, const ProcessState processState, float value)
	{
		RawInput& input = GetInput(name);
		input.CurrentProcessState = processState;
		input.RawValue = Vector2<float>::Right() * value;
	}

	template <>
	void Action<Vector2<float>>::Update(const std::string& name, const ProcessState processState, Vector2<float> value)
	{
		RawInput& input = GetInput(name);
		input.CurrentProcessState = processState;
		input.RawValue = value;
	}
}