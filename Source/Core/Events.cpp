#include "Events.h"
#include "Renderer.h"
#include "../Maths/Vector2.h"
#include <imgui_impl_sdl.h>
#include <SDL.h>
#include <algorithm>
#include "Window.h"

namespace Engine
{
	Events& Events::Instance()
	{
		static Events instance(Window::Instance());
		return instance;
	}

	Events::Events(SDL_Window* window) :
		KeyboardState(SDL_GetKeyboardState(nullptr)),
		PreviousKeyboardState{ 0 }
	{
		SDL_GetWindowSize(window, &WindowSize.X, &WindowSize.Y);
	}

	bool Events::Process()
	{
		// Update previous keyboard state.
		std::copy(&KeyboardState[0], &KeyboardState[SDL_NUM_SCANCODES], PreviousKeyboardState);

		ResetStates();

		// Poll and handle events (inputs, window resize, etc.)
		SDL_Event event; // Prefer scan code, keycode will mean it'll be in a different place on non-WASD/normal keyboards.
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			switch (event.type)
			{
			case SDL_QUIT:
				return false;
			case SDL_KEYDOWN:
				if (event.key.keysym.scancode == SDL_SCANCODE_1) // TODO: Remove. Temp VSYNC testing.
				{
					Renderer::Instance().SetVSync(0);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_2)
				{
					Renderer::Instance().SetVSync(1);
				}
				break;
			case SDL_MOUSEMOTION:
				MousePosition.X = event.motion.x;
				MousePosition.Y = event.motion.y;
				break;
			case SDL_MOUSEBUTTONDOWN:
				MouseButtonBitField |= 1 << (event.button.button - 1);
				break;
			case SDL_MOUSEBUTTONUP:
				MouseButtonBitField &= ~(1 << (event.button.button - 1));
				break;
			case SDL_MOUSEWHEEL:
				MouseWheelY = event.wheel.y;
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					WindowSize.X = event.window.data1;
					WindowSize.Y = event.window.data2;
				}
				break;
			}
		}

		return true;
	}

	bool Events::IsKeyDown(SDL_Scancode scancode) const
	{
		return KeyboardState[scancode];
	}

	bool Events::IsKeyPressed(SDL_Scancode scancode) const
	{
		return KeyboardState[scancode] == 1 && PreviousKeyboardState[scancode] == 0;
	}

	bool Events::IsLeftMouseButtonDown() const
	{
		return MouseButtonBitField & (1 << (SDL_BUTTON_LEFT - 1));
	}

	Vector2<int> Events::GetMousePosition() const
	{
		return MousePosition;
	}

	int Events::GetMouseWheelY() const
	{
		return MouseWheelY;
	}

	Vector2<int> Events::GetWindowSize() const
	{
		return WindowSize;
	}

	void Events::ResetStates()
	{
		MouseWheelY = 0;
	}
}