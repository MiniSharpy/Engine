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
		static const char* GetMouseButtonName(Uint8 button);
		// Instance
	private:
		Events(SDL_Window* window);
		~Events();

	public:
		Events(const Events& other) = delete; // Copy Constructor
		Events& operator=(const Events& other) = delete; // Copy Assignment

		bool Process(float deltaTime);
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
		Vector2<int> WindowSize; // TODO: This should probably be on the window.

		/// <summary>
		/// Update input mapping if one exists, with new values when triggered.
		/// </summary>
		/// <param name="name">The name of the input.</param>
		/// <param name="processState">The state of the Input object, e.g. process once, continuously, release.</param>
		static void UpdateMappedInputs(const std::string& name, ProcessState processState);
		/// <summary>
		/// Update input mapping if one exists, with new values when triggered.
		/// </summary>
		/// <param name="name">The name of the input.</param>
		/// <param name="processState">The state of the Input object, e.g. process once, continuously, release.</param>
		/// <param name="value">The value of the the raw input. Converted depending on action type.</param>
		static void UpdateMappedInputs(const std::string& name, ProcessState processState, float value);
		/// <summary>
		/// Update input mapping if one exists, with new values when triggered.
		/// </summary>
		/// <param name="name">The name of the input.</param>
		/// <param name="processState">The state of the Input object, e.g. process once, continuously, release.</param>
		/// <param name="value">The value of the the raw input. Converted depending on action type.</param>
		static void UpdateMappedInputs(const std::string& name, ProcessState processState, Vector2<float> value);
	};
}
