#pragma once
#include "../Maths/Vector2.h"
#include "InputMisc.h"
#include "Modifiers/Modifier.h"
#include <string>
#include <variant>

namespace Engine
{
	class Action;


	class Input
	{
	public:
		/// <summary>
		/// The name of the input, and which corresponds to a map for the SDL event loop to access Inputs. 
		/// </summary>
		std::string Name;

		// TODO: Ideally want this to be a unique_ptr, but input copies seem to be messing with this.
		std::vector<std::shared_ptr<Modifier>> Modifiers;

		/// <summary>
		/// Value set by the event loop, accessed via a map from SDL converted strings
		/// to Inputs.
		/// </summary>
		ActionValue Value;

		/// <summary>
		/// SDL's event loop only handles events for that last key pressed, so to support multiple keys
		///	in use simultaneously they need to be processed elsewhere when input is received. This means
		///	a key press will only alter an Input on press and release.
		/// </summary>
		ProcessState CurrentProcessState = Stop;

		/// <summary>
		/// Setup the name for events loop to find the object, as well as defining the conditions and modifiers
		///	for the input.
		///	The action object handles setting the default value.
		/// </summary>
		/// <param name="owningAction"></param>
		/// <param name="inputName"></param>
		Input(ActionType type, std::string inputName);
	};
}
