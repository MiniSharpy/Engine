#include "Texture.h"
#include "../Maths/Vector2.h"
#include <SDL.h>
#include <SDL_image.h>
#include <string>

namespace Engine
{
	Texture::Texture(SDL_Renderer* renderer, const std::string name)
	{
		SDL_Log("Texture Initialisation!");
		std::string path("Data/Textures/" + name);

		ManagedTexture = IMG_LoadTexture(renderer, path.c_str());
		if (!ManagedTexture)
		{
			SDL_Log("Error: %s", SDL_GetError());
			// TODO: Set to error texture
		}
		else
		{
			SDL_Log("Initialising texture %s at address %p!", name.c_str(), ManagedTexture);
		}
		SDL_QueryTexture(ManagedTexture, NULL, NULL, &Size.X, &Size.Y);
		auto IsPowerOfTwo = [](int value) -> bool { return !(value & (value - 1)) && value; };
		if (!IsPowerOfTwo(Size.X) && !IsPowerOfTwo(Size.Y))
		{
			SDL_Log("%s, texture is not a power of two! Width: %d. Height: %d", path.c_str(), Size.X, Size.Y);
		}
	}

	Texture::~Texture()
	{
		// When an SDL_Renderer is destroyed it will free all SDL_Textures that were created using it, potentially
		// resulting in ManagedTexture becoming a dangling pointer.
		if (ManagedTexture == nullptr) { return; }
		SDL_Log("Destroying texture at address %p!", ManagedTexture);
		SDL_DestroyTexture(ManagedTexture); // SDL_Error will report errors if nullptr.
	}

	Texture::Texture(Texture&& other) noexcept
	{
		SDL_DestroyTexture(ManagedTexture);
		ManagedTexture = other.ManagedTexture;
		other.ManagedTexture = nullptr;

		SDL_QueryTexture(ManagedTexture, NULL, NULL, &Size.X, &Size.Y);
	}

	Texture::Texture(SDL_Texture* other) noexcept
	{
		SDL_DestroyTexture(ManagedTexture);
		ManagedTexture = other;

		SDL_QueryTexture(ManagedTexture, NULL, NULL, &Size.X, &Size.Y);
	}

	Texture& Texture::operator=(Texture&& other) noexcept
	{
		SDL_DestroyTexture(ManagedTexture);
		ManagedTexture = other.ManagedTexture;
		other.ManagedTexture = nullptr;

		SDL_QueryTexture(ManagedTexture, NULL, NULL, &Size.X, &Size.Y);
		return *this;
	}
}
