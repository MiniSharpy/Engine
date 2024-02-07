#pragma once
#include "../RawInput.h"

namespace Engine
{
	class ReleasedCondition : public Condition
	{
	public:
		bool operator()(const RawInput& input) override
		{
			return input.CurrentProcessState == Release;
		}
	};
}