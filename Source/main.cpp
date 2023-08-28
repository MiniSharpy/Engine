#include "Core/Settings.h"
#include "Core/Game.h"
#include "Core/Window.h"
#include "Core/Renderer.h"
#include "Core/Events.h"
#include "SceneManagement/SceneManager.h"
#include "Core/Timer.h"
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
double FrameTimeManagement(Timer& frameTimer, double targetFrameTime)
{
	double frameTime = frameTimer.Time(); // In seconds. Aka deltaTime.
	if (frameTime < targetFrameTime) // If current frame is too fast...
	{
		// ... sleep less than needed to allow spinning for accuracy. A higher FPS will hit the thread harder.
		int timeToWait = int((targetFrameTime - frameTime) * 1000) - 1; // In milliseconds.
		if (timeToWait > 0) // SDL_Delay requires an unsigned int, don't wont to overflow.
		{
			SDL_Delay(timeToWait); // TODO: Does VSync cause stutters with this?
		}
	}
	while ((frameTime = frameTimer.Time()) < targetFrameTime); // End of frame. Spin for an accurate frameTime and retrieve its value.
	if (frameTime > 1.0) // Assume hit breakpoint, frame lock for one second.
	{
		frameTime = targetFrameTime;
	}

	return frameTime;
}

// Main code
int main(int, char**)
{
	// Setup
	Settings& settings = Settings::Instance();
	Game& game = Game::Instance();
	Window& window = Window::Instance();
	Renderer& renderer = Renderer::Instance();
	Events& events = Events::Instance();
	SceneManager& sceneManager = SceneManager::Instance();

	// Main loop
	Timer frameTimer;
	double frameTime = settings.GetTargetFrameTime(); // Assume target frame time for first frame.
	while (events.Process()) 
	{
		game.Update((float)frameTime, settings, sceneManager.GetCurrentScene(), events);
		renderer.Render(sceneManager.GetCurrentScene());
		frameTime = FrameTimeManagement(frameTimer, settings.GetTargetFrameTime());
		frameTimer.Reset(); // Everything done in the frame MUST be in between the reset and the while loop to be accurate.
	}

	return 0;
}