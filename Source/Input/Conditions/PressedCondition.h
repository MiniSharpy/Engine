#pragma once
#include "../InputMisc.h"
#include "../Input.h"

namespace Engine
{
	class PressedCondition : public Condition
	{
	private:
		ProcessState PreviousProcessState = Stop;
	public:
		bool operator()(const Input& input) override
		{
			bool isHeld = input.CurrentProcessState == PreviousProcessState;
			bool isPressed = !isHeld && input.CurrentProcessState != Release;
			PreviousProcessState = input.CurrentProcessState;

			return isPressed;
		}
	};
}