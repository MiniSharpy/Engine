#pragma once
#include <variant>

namespace Engine
{
	using ActionValue = std::variant<std::monostate, float, Engine::Vector2<float>>;

	enum ActionType
	{
		Trigger,
		Float,
		Vector2Float
	};

	enum ProcessState
	{
		Stop,
		Once,
		Continuous
	};
}
