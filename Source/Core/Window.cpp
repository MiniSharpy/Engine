#include "Window.h"
#include <SDL.h>
#include <SDL_image.h>

namespace Engine
{
	Window& Window::Instance()
	{
		static Window instance;
		return instance;
	}

	Window::Window()
	{
		SDL_Log("Window Initialisation!");
		ManagedWindow = SDL_CreateWindow("Engine²", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI));
		if (!ManagedWindow)
		{
			SDL_Log("Error: %s\n", SDL_GetError());
			exit(-1);
		}

		ManagedIcon = IMG_Load("Data/icon.png"); // TODO: Fetch from better location, probably manage its memory there too.
		if (!ManagedIcon)
		{
			SDL_Log("Error: %s\n", SDL_GetError());
			return;
		}
		SDL_SetWindowIcon(ManagedWindow, ManagedIcon);
	}

	Window::~Window()
	{
		SDL_Log("Destroying window!");
		SDL_FreeSurface(ManagedIcon);
		SDL_DestroyWindow(ManagedWindow);
	}
	void Window::GetWindowSize(int& width, int& height)
	{
		SDL_GetWindowSize(ManagedWindow, &width, &height);
	}
}