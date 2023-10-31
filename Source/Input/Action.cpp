#include "Action.h"
#include <ranges>
namespace Engine
{
	Action::Action(ActionType type, std::function<void(ActionValue)> boundFunction)
		: Type(type), BoundFunction(std::move(boundFunction)), BoundInputs({})
	{

	}

	void Action::BindInput(const std::string& inputName)
	{
		BoundInputs.emplace(inputName, Input{ Type, inputName }); // Input has no blank constructor, need to use emplace.
	}

	void Action::Process()
	{
		switch (Type)
		{
		case Trigger:
			ProcessTrigger();
			break;
		case Float:
			ProcessFloat();
			break;
		case Vector2Float:
			ProcessVector();
			break;
		}
	}

	void Action::ProcessTrigger()
	{
		bool isCallingFunction = false;
		for (auto& input : BoundInputs | std::views::values)
		{
			if (input.CurrentProcessState != Stop)
			{
				isCallingFunction = true;
			}

			if (input.CurrentProcessState == Once)
			{
				input.CurrentProcessState = Stop;
			}
		}

		if (isCallingFunction)
		{
			BoundFunction(std::monostate());
		}
	}

	void Action::ProcessFloat()
	{
		auto inputsToProcess = 
			BoundInputs | 
			std::views::values |
			std::views::filter([](const Input& input) { return input.CurrentProcessState != ProcessState::Stop; });

		if (inputsToProcess.empty()) { return; }

		float highestValue = 0;
		for (Input& input : inputsToProcess)
		{
			float value = std::get<float>(input.Value);
			for (const auto& modifier : input.Modifiers)
			{
				modifier->Process(value);
			}

			highestValue = abs(value) > abs(highestValue) ? value : highestValue;

			if (input.CurrentProcessState == Once)
			{
				input.CurrentProcessState = Stop;
			}
		}

		BoundFunction(highestValue);
	}

	void Action::ProcessVector()
	{
		auto inputsToProcess = 
			BoundInputs | 
			std::views::values |
			std::views::filter([](const Input& input) { return input.CurrentProcessState != ProcessState::Stop; });

		if (inputsToProcess.empty()) { return; }

		Vector2<float> highestValue = Vector2<float>::Zero();
		for (Input& input : inputsToProcess)
		{
			Vector2<float> value = std::get<Vector2<float>>(input.Value);
			for (const auto& modifier : input.Modifiers)
			{
				modifier->Process(value);
			}

			highestValue.X = abs(value.X) > abs(highestValue.X) ? value.X : highestValue.X;
			highestValue.Y = abs(value.Y) > abs(highestValue.Y) ? value.Y : highestValue.Y;

			if (input.CurrentProcessState == Once)
			{
				input.CurrentProcessState = Stop;
			}
		}

		BoundFunction(highestValue);
	}

	bool Action::HasInput(const std::string& string) const { return BoundInputs.contains(string); }

	Input& Action::GetInput(const std::string& string) { return BoundInputs.find(string)->second; }
}
