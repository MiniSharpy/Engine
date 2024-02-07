#include "Events.h"
#include "Renderer.h"
#include "Window.h"
#include "../Maths/Vector2.h"
#include "SceneManagement/SceneManager.h"
#include <algorithm>
#include <SDL.h>
#include <imgui_impl_sdl2.h>
#include "../Input/Input.h"

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

	Events::~Events()
	{
		for (SDL_GameController* controller : Controllers)
		{
			SDL_GameControllerClose(controller);
		}
	}

	int Events::GetControllerJoystickInstanceID(SDL_GameController* controller)
	{
		return SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
	}

	const char* Events::GetMouseButtonName(Uint8 button)
	{
		switch (button)
		{
		case SDL_BUTTON_LEFT:
			return "Mouse Button Left";
		case SDL_BUTTON_MIDDLE:
			return "Mouse Button Middle";
		case SDL_BUTTON_RIGHT:
			return "Mouse Button Right";
		case SDL_BUTTON_X1:
			return "Mouse Button Extra1";
		case SDL_BUTTON_X2:
			return "Mouse Button Extra2";
		default:
			return "";
		}
	}


	bool Events::Process()
	{
		BaseScene& currentScene = SceneManager::Instance().GetCurrentScene();

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
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					WindowSize.X = event.window.data1;
					WindowSize.Y = event.window.data2;
				}
				break;
			case SDL_KEYDOWN:
			{
				if (event.key.keysym.scancode == SDL_SCANCODE_1) // TODO: Remove. Temp VSYNC testing.
				{
					Renderer::Instance().SetVSync(0);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_2)
				{
					Renderer::Instance().SetVSync(1);
				}
				currentScene.Input.UpdateMappedInputs(SDL_GetScancodeName(event.key.keysym.scancode), Continuous);
				break;
			}
			case SDL_KEYUP:
				currentScene.Input.UpdateMappedInputs(SDL_GetScancodeName(event.key.keysym.scancode), Release);
				break;
			case SDL_MOUSEMOTION:
				MousePosition.X = event.motion.x;
				MousePosition.Y = event.motion.y;
				currentScene.Input.UpdateMappedInputs("Mouse Axis X", Once, (float)event.motion.xrel);
				currentScene.Input.UpdateMappedInputs("Mouse Axis Y", Once, (float)event.motion.yrel);
				break;
			case SDL_MOUSEBUTTONDOWN:
				MouseButtonBitField |= SDL_BUTTON(event.button.button);
				currentScene.Input.UpdateMappedInputs(
					GetMouseButtonName(event.button.button),
					Continuous,
					Vector2<float>(event.button.x, event.button.y));
				break;
			case SDL_MOUSEBUTTONUP:
				MouseButtonBitField &= ~SDL_BUTTON(event.button.button);
				currentScene.Input.UpdateMappedInputs(
					GetMouseButtonName(event.button.button),
					Release,
					Vector2<float>(event.button.x, event.button.y));
				break;
			case SDL_MOUSEWHEEL:
				MouseWheelY = event.wheel.y;
				currentScene.Input.UpdateMappedInputs("Mouse Wheel Y", Once, event.wheel.preciseY);
				break;
			case SDL_CONTROLLERDEVICEADDED:
				// This seems to handle initial start up, meaning no need to manually iterate over IDs in constructor.
				if (SDL_GameController* controller = SDL_GameControllerOpen(event.cdevice.which))
				{
					Controllers.push_back(controller);
				}
				break;
			case SDL_CONTROLLERDEVICEREMOVED:
				// Could optimise, potentially at the expense of memory, by making sure the vector indices match
				// joystick instance id. Will need to investigate how instance ID is affected by hot plugging.
				for (int i = 0; i < Controllers.size(); ++i)
				{
					if (event.cdevice.which == GetControllerJoystickInstanceID(Controllers[i]))
					{
						SDL_GameControllerClose(Controllers[i]); // Want to RAIIfy this at some point so erasing will handle this.
						Controllers.erase(Controllers.begin() + i);
						break;
					}
				}
				break;
			case SDL_CONTROLLERBUTTONDOWN:
				for (auto& controller : Controllers)
				{
					if (event.cbutton.which == GetControllerJoystickInstanceID(controller))
					{
						currentScene.Input.UpdateMappedInputs(
							SDL_GameControllerGetStringForButton((SDL_GameControllerButton)event.cbutton.button),
							Continuous);
					}
				}
				break;
			case SDL_CONTROLLERBUTTONUP:
				for (auto& controller : Controllers)
				{
					if (event.cbutton.which == GetControllerJoystickInstanceID(controller))
					{
						currentScene.Input.UpdateMappedInputs(
							SDL_GameControllerGetStringForButton((SDL_GameControllerButton)event.cbutton.button),
							Release);
					}
				}
				break;
			case SDL_CONTROLLERAXISMOTION:
				for (auto& controller : Controllers)
				{
					if (event.caxis.which == GetControllerJoystickInstanceID(controller))
					{
						// Normalise axis to a value between -1 and 1.
						float value = event.caxis.value < 0
							? -static_cast<float>(event.caxis.value) / std::numeric_limits<Sint16>::min()
							: static_cast<float>(event.caxis.value) / std::numeric_limits<Sint16>::max();
						currentScene.Input.UpdateMappedInputs(
							SDL_GameControllerGetStringForAxis(static_cast<SDL_GameControllerAxis>(event.caxis.axis)),
							Continuous,
							value);
					}
				}
				break;
			default: break;
			}
		}

		currentScene.Input.Process();

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
