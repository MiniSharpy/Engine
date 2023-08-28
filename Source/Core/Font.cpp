#include "Font.h"
#include <SDL_ttf.h>
#include <string>
namespace Engine
{
	Font::Font(std::string name, int pointSize)
	{
		std::string path("Data/" + name);
		ManagedFont = TTF_OpenFont(path.c_str(), pointSize);
		if (!ManagedFont)
		{
			SDL_Log("Error: %s", SDL_GetError());
		}
		else
		{
			SDL_Log("Initialising font %s at address %p!", name.c_str(), ManagedFont);
		}
	}

	Font::~Font()
	{
		TTF_CloseFont(ManagedFont);
	}

	Font::Font(Font&& other) noexcept
	{
		TTF_CloseFont(ManagedFont);
		ManagedFont = other.ManagedFont;
		other.ManagedFont = nullptr;
	}

	Font& Font::operator=(Font&& other) noexcept
	{
		TTF_CloseFont(ManagedFont);
		ManagedFont = other.ManagedFont;
		other.ManagedFont = nullptr;
		return *this;
	}
}

