#include "Action.h"
#include <SDL.h>
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
		float finalValue = 0;
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

	void Action::ProcessVector(auto inputsToProcess)
	{
		// TODO: Bool for inputs accumulate or taking highest value.
		// For something like moving then accumulate might work better as it ends up normalised anyway and it means
		// that up/down, etc. movement can cancel itself out.
		bool execute = false;
		Vector2<float> finalValue = Vector2<float>::Zero();
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

	bool Action::HasInput(const std::string& string) const { return BoundInputs.contains(string); }

	Input& Action::GetInput(const std::string& string) { return BoundInputs.find(string)->second; }

	ActionType Action::GetType() const { return Type; }
}
