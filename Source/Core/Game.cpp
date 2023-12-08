#include "Game.h"
#include "Settings.h"
#include "Events.h"
#include "../SceneManagement/BaseScene.h"
#include "../SceneManagement/IGrid.h"
#include "../SceneManagement/IsometricScene.h"
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace Engine
{
	Game& Game::Instance()
	{
		static Game instance;
		return instance;
	}

	Game::Game()
	{
		SDL_Log("Engine Initialisation!");

		// Initialise SDL.
		// SDL_INIT_VIDEO will automatically intialise the events subsystem.
		// SDL_INIT_GAMECONTROLLER will automatically initialise the joystick subsystems.
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) < 0)
		{
			SDL_Log("Error: %s\n", SDL_GetError());
			exit(-1); // Cleaning up an early exit is the OS's problem. Only reason to call a clean up function would be if there were any destructors that needed to finish some logic. Maybe throw an exception and do the clean up in that?
		}

		// Initialise SDL_images.
		if (IMG_Init(IMG_INIT_JPG) == 0) // VCPKG requires extra libraries depending on the image formats used. libpng (repo seems to be outdated, maybe it comes with one of the other features?), libjpeg-turbo, etc. vcpkg search sdl2-image --featurepackages to find more.
		{
			SDL_Log("Error: %s", SDL_GetError());
			SDL_Quit();
			exit(-1);
		}

		// Initialise SDL_tff.
		if (TTF_Init() == -1)
		{
			SDL_Log("Error: %s", SDL_GetError());
			SDL_Quit();
			IMG_Quit();
			exit(-1);
		}


		// Setup Dear ImGui context.
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

		// Setup Dear ImGui style.
		ImGui::StyleColorsDark();
	}

	Game::~Game()
	{
		SDL_Log("Cleaning up Dear ImGui and SDL!");
		// Clean up ImGui.
		ImGui::DestroyContext();

		// Clean up SDL.
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
	}

	void Game::Update(const float& deltaTime, Settings& settings, BaseScene& scene, Events& events)
	{
		// Start the Dear ImGui frame, this is needed to provide UI commands.
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// ImGui commands goes here!
		{
			int maxFPS = settings.GetTargetFrameRate();
			ImGui::Begin("Debug Info", 0, ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::SliderInt("Max FPS", &maxFPS, 0, 400);

			Vector2<float> screenPosition = Vector2<float>(events.GetMousePosition());
			ImGui::Text("Mouse Screen Position: (%f, %f)", screenPosition.X, screenPosition.Y);

			Vector2<float> worldPosition = scene.ScreenSpaceToWorldSpace(screenPosition);
			ImGui::Text("Mouse World Position : (%f, %f)", worldPosition.X, worldPosition.Y);

			IGrid* sceneWithGrid = dynamic_cast<IGrid*>(&scene);
			if (sceneWithGrid)
			{
				Vector2<float> gridPosition = sceneWithGrid->ScreenSpaceToGrid(screenPosition, true);
				ImGui::Text("Mouse Grid Position  : (%f, %f)", gridPosition.X, gridPosition.Y);

			}
			ImGui::Text("Camera Zoom          : (%f)", scene.MainCamera.GetComponent<Zoom>().Value);
			ImGui::Text("Number of Entities   : (%zu)", EntityMemoryPool::Instance().GetEntityAliveCount()); // This will include entities alive in other loaded scenes.

			ImGui::End();
			settings.SetTargetFrameRate(maxFPS);
		}
		scene.Update(deltaTime);

		// End the Dear ImGui frame, no more commands to create/alter UI should come after this point.
		ImGui::Render(); // This calls ImGui::EndFrame. Doesn't actually render it, I suppose.
	}
}