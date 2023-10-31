#include "Input.h"
#include "Action.h"
#include "InputMisc.h"

namespace Engine
{
	Input::Input(const ActionType type, std::string inputName): Name(std::move(inputName)), Modifiers({})
	{
		switch (type)
		{
		case Trigger:
			Value = std::monostate();
			break;
		case Float:
			Value = 0.0f;
			break;
		case Vector2Float:
			Value = Vector2<float>::Zero();
			break;
		}
	}
}
