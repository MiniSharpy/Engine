#include "Action.h"
#include <ranges>
namespace Engine
{
	Action::Action(ActionType type, std::function<void(ActionValue)> boundFunction, bool cumulateInputs)
		: Type(type), BoundFunction(std::move(boundFunction)), CumulateInputs(cumulateInputs) {}

	void Action::Process()
	{
		auto enabled = [](const Input& input) { return input.CurrentProcessState != Stop; };
		auto conditions = [](const Input& input)
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

		// This is lazily evaluated and the modifiers alter their own state, so it must not be handled until
		// the for each loop otherwise a condition could change. E.G. PressedCondition will set its previous
		// process state which if called multiple times will result in it treating an input as a hold when
		// it's actually only been pressed.
		auto inputsToProcess =
			BoundInputs
			| std::views::values
			| std::views::filter(enabled)
			| std::views::filter(conditions);

		switch (Type)
		{
		case Trigger:
			ProcessTrigger(inputsToProcess);
			break;
		case Float:
			ProcessFloat(inputsToProcess);
			break;
		case Vector2Float:
			ProcessVector(inputsToProcess);
			break;
		}
	}

	void Action::ProcessTrigger(auto inputsToProcess)
	{
		bool execute = false;
		for (auto& input : inputsToProcess)
		{
			execute = true;
			if (input.CurrentProcessState != Continuous)
			{
				input.CurrentProcessState = Stop;
			}
		}

		if (execute)
		{
			BoundFunction(std::monostate());
		}
	}

	void Action::ProcessFloat(auto inputsToProcess)
	{
		bool execute = false;
		float highestValue = 0;
		for (Input& input : inputsToProcess)
		{
			execute = true;
			float value = std::get<float>(input.RawValue);
			for (const auto& modifier : input.Modifiers)
			{
				(*modifier)(value);
			}

			if (CumulateInputs)
			{
				highestValue += value;
			}
			else
			{
				highestValue = abs(value) > abs(highestValue) ? value : highestValue;
			}
			
			

			if (input.CurrentProcessState != Continuous)
			{
				input.CurrentProcessState = Stop;
			}
		}

		if (execute)
		{
			BoundFunction(highestValue);
		}
	}

	void Action::ProcessVector(auto inputsToProcess)
	{
		// TODO: Bool for inputs accumulate or taking highest value.
		// For something like moving then accumulate might work better as it ends up normalised anyway and it means
		// that up/down, etc. movement can cancel itself out.
		bool execute = false;
		Vector2<float> highestValue = Vector2<float>::Zero();
		for (Input& input : inputsToProcess)
		{
			execute = true;
			Vector2<float> value = std::get<Vector2<float>>(input.RawValue);
			for (const auto& modifier : input.Modifiers)
			{
				(*modifier)(value);
			}

			if (CumulateInputs)
			{
				highestValue += value;
			}
			else
			{
				highestValue.X = abs(value.X) > abs(highestValue.X) ? value.X : highestValue.X;
				highestValue.Y = abs(value.Y) > abs(highestValue.Y) ? value.Y : highestValue.Y;
			}

			if (input.CurrentProcessState != Continuous)
			{
				input.CurrentProcessState = Stop;
			}
		}

		if (execute)
		{
			BoundFunction(highestValue);
		}
	}

	bool Action::HasInput(const std::string& string) const { return BoundInputs.contains(string); }

	Input& Action::GetInput(const std::string& string) { return BoundInputs.find(string)->second; }

	ActionType Action::GetType() const { return Type; }
}
