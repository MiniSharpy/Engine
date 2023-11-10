#pragma once
#include "../InputMisc.h"
#include "../Input.h"

namespace Engine
{
	class ReleasedCondition : public Condition
	{
	public:
		bool operator()(const Input& input) override
		{
			return input.CurrentProcessState == Release;
		}
	};
}