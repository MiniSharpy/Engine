#include "Events.h"
#include "Renderer.h"
#include "Window.h"
#include "../Maths/Vector2.h"
#include "SceneManagement/SceneManager.h"
#include <algorithm>
#include <SDL.h>
#include <imgui_impl_sdl.h>
#include <map>
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

	bool Events::Process()
	{
		float deltaTime = 1;
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
				UpdateInputIfMappingFound(SDL_GetScancodeName(event.key.keysym.scancode), Continuous, true);
				break;
			}
			case SDL_KEYUP:
				UpdateInputIfMappingFound(SDL_GetScancodeName(event.key.keysym.scancode), Stop, false);
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
				UpdateInputIfMappingFound("Mouse Wheel Y", Once, (float)MouseWheelY);
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
						SDL_GameControllerClose(Controllers[i]); // Might want to RAIIfy this at some point so erasing will handle this.
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
					}
				}
				break;
			case SDL_CONTROLLERAXISMOTION:
				for (auto& controller : Controllers)
				{
					if (event.cbutton.which == GetControllerJoystickInstanceID(controller))
					{
						// Normalise axis to a value between -1 and 1.
						float value = event.caxis.value < 0
						? -static_cast<float>(event.caxis.value) / std::numeric_limits<Sint16>::min()
						: static_cast<float>(event.caxis.value) / std::numeric_limits<Sint16>::max();
						UpdateInputIfMappingFound(
							SDL_GameControllerGetStringForAxis(static_cast<SDL_GameControllerAxis>(event.caxis.axis)),
							Continuous, 
							value);
					}
				}
				break;
			default: break;
			}
		}

		for (Action& action : SceneManager::Instance().GetCurrentScene().Actions)
		{
			action.Process();
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

	void Events::UpdateInputIfMappingFound(const std::string& name, const ProcessState processState, bool value)
	{
		std::vector<Action>& actions = SceneManager::Instance().GetCurrentScene().Actions;
		for (auto& action : actions)
		{
			if (action.HasInput(name))
			{
				Input& input = action.GetInput(name);
				input.CurrentProcessState = processState;

				switch (action.Type)
				{
				case Trigger:
					input.Value = std::monostate();
					break;
				case Float:
					input.Value = 1.0f;
					break;
				case Vector2Float:
					input.Value = Vector2<float>::Right();
					break;
				}
			}
		}
	}

	void Events::UpdateInputIfMappingFound(const std::string& name, const ProcessState processState, float value)
	{
		std::vector<Action>& actions = SceneManager::Instance().GetCurrentScene().Actions;
		for (auto& action : actions)
		{
			if (action.HasInput(name))
			{
				Input& input = action.GetInput(name);
				input.CurrentProcessState = processState;

				switch (action.Type)
				{
				case Trigger:
					input.Value = std::monostate();
					break;
				case Float:
					input.Value = value;
					break;
				case Vector2Float:
					input.Value = Vector2<float>::Right() * value;
					break;
				}
			}
		}
	}

	void Events::UpdateInputIfMappingFound(const std::string& name, const ProcessState processState, Vector2<float> value)
	{
		std::vector<Action>& actions = SceneManager::Instance().GetCurrentScene().Actions;
		for (auto& action : actions)
		{
			if (action.HasInput(name))
			{
				Input& input = action.GetInput(name);
				input.CurrentProcessState = processState;

				switch (action.Type)
				{
				case Trigger:
					input.Value = std::monostate();
					break;
				case Float:
					input.Value = value.Length(); // TODO: Is this a reasonable expectation?
					break;
				case Vector2Float:
					input.Value = value;
					break;
				}
			}
		}
	}
}
