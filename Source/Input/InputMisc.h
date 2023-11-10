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
		/// <summary>
		/// The default inactive state, occurring when an input is in a relaxed state.
		/// </summary>
		Stop,
		/// <summary>
		/// Some inputs do not have release events or ranges that cause them to stop processing, such as the mouse
		/// scrolling, and so should only occur once.
		/// </summary>
		Once,
		/// <summary>
		/// Most inputs will use this as it's not guaranteed SDL events will occur multiple times,
		/// e.g. multiple key presses, or constant axis values.
		///	A method of nullifying these inputs is required, like setting to Once on key up, or
		///	in setting the value to 0 in processing, such as an an axis within the dead zone.
		/// </summary>
		Continuous,
		/// <summary>
		/// Certain inputs like Buttons/Keys also have release events that might cause processing to be done at the end.
		/// </summary>
		Release
	};
}
