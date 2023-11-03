#pragma once
#include "../Input/Input.h"
#include "../Maths/Vector2.h"
#include <SDL.h>
#include <vector>

struct SDL_Window;

namespace Engine
{
	class Renderer;

	class Events
	{
		// Static
	public:
		static Events& Instance();

	private:
		static int GetControllerJoystickInstanceID(SDL_GameController* controller);

		// Instance
	private:
		Events(SDL_Window* window);
		~Events();

	public:
		Events(const Events& other) = delete; // Copy Constructor
		Events& operator=(const Events& other) = delete; // Copy Assignment

		bool Process();
		bool IsKeyDown(SDL_Scancode scancode) const;
		bool IsKeyPressed(SDL_Scancode scancode) const;
		bool IsLeftMouseButtonDown() const;
		Vector2<int> GetMousePosition() const;
		int GetMouseWheelY() const;
		Vector2<int> GetWindowSize() const;
	private:
		void ResetStates();

		/// <summary>
		/// A pointer to an array of key states managed by SDL. Gets updated after SDL_PumpEvents() is called.
		/// SDL_PollEvent() calls SDL_PumpEvents().
		/// </summary>
		const Uint8* KeyboardState;
		Uint8 PreviousKeyboardState[SDL_NUM_SCANCODES];
		Vector2<int> MousePosition;
		char MouseButtonBitField = 0;
		int MouseWheelY = 0;
		std::vector<SDL_GameController*> Controllers;
		Vector2<int> WindowSize; // This should probably be on the window.

		// TODO: Need to consider how to handle button press, button release.
		/// <summary>
		/// Update input mapping with new values when triggered.
		/// </summary>
		/// <param name="name">The name of the input in the map.</param>
		/// <param name="processState">The state of the Input object, e.g. process once or continuously.</param>
		/// <param name="value">The value of the the raw input. Converted depending on action type.</param>
		static void UpdateInputIfMappingFound(const std::string& name, ProcessState processState, bool value);
		static void UpdateInputIfMappingFound(const std::string& name, ProcessState processState, float value);
		static void UpdateInputIfMappingFound(const std::string& name, ProcessState processState, Vector2<float> value);
	};
}