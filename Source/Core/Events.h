#pragma once
struct SDL_Window;
#include "../Maths/Vector2.h"
#include <SDL.h>

namespace Engine
{
	class Renderer;

	class Events
	{
	private:
		Events(SDL_Window* window);

	public:
		static Events& Instance();

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
		Vector2<int> WindowSize;
	};
}