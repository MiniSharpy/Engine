#include "Core/Settings.h"
#include "Core/Game.h"
#include "Core/Window.h"
#include "Core/Renderer.h"
#include "Core/Events.h"
#include "SceneManagement/SceneManager.h"
#include "Core/Timer.h"
#include "SceneManagement/IsometricScene.h"
#include <SDL.h>

using namespace Engine;

/// <summary>
/// Calculate the frame time (Time between each iteration of the game loop), and or ensure the frame time meets the target
/// frame time.
/// </summary>
/// <remarks>
/// System sleeping can only be so accurate, typically to a millisecond, so to ensure exact frame times at higher frame
/// rates the function will sleep for slightly less than the target time and then keep the CPU busy by looping until
/// the target time. This ensures the correct FPS, but does result in a heavier hit to the CPU than the equivalent frame rate 
/// with VSync enabled.
/// </remarks>  
float FrameTimeManagement(Timer& frameTimer, float targetFrameTime)
{
	float frameTime = frameTimer.Time<float>(); // In seconds. aka deltaTime.
	if (frameTime < targetFrameTime) // If current frame is too fast...
	{
		// ... sleep less than needed to allow spinning for accuracy. A higher FPS will hit the thread harder.
		int timeToWait = static_cast<int>((targetFrameTime - frameTime) * 1000.f) - 1; // In milliseconds.
		if (timeToWait > 0) // SDL_Delay requires an unsigned int, don't wont to overflow.
		{
			SDL_Delay(timeToWait); // TODO: Does VSync cause stutters with this?
		}
	}
	while ((frameTime = frameTimer.Time<float>()) < targetFrameTime); // End of frame. Spin for an accurate frameTime and retrieve its value.
	if (frameTime > 1.0f) // Assume hit breakpoint, frame lock for one second.
	{
		frameTime = targetFrameTime;
	}

	frameTimer.Reset(); // Everything done in the frame MUST be in between the reset and the start of while loop to be accurate.

	return frameTime;
}

// Main code
int main(int, char**)
{
	// Setup
	EntityMemoryPool::Instance(); // Needs to be initialised early so that later destructors don't try to index destructed IDs. 
	Settings& settings = Settings::Instance();
	Game& game = Game::Instance();
	Window& window = Window::Instance();
	Renderer& renderer = Renderer::Instance();
	Events& events = Events::Instance();
	SceneManager& sceneManager = SceneManager::Instance();

	// Main loop
	Timer frameTimer;
	float deltaTime = settings.GetTargetFrameTime(); // Assume target frame time for first frame.
	sceneManager.LoadScene<IsometricScene>(deltaTime);
	while (events.Process()) 
	{
		// Pass delta time by reference so that it can be stored as a reference ins objects and
		// not need to be passed delta time as a parameter.
		game.Update(deltaTime, settings, sceneManager.GetCurrentScene(), events);
		renderer.Render(sceneManager.GetCurrentScene());
		deltaTime = FrameTimeManagement(frameTimer, settings.GetTargetFrameTime());
	}

	return 0;
}