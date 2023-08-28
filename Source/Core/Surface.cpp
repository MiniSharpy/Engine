#include "Surface.h"
#include <SDL.h>

namespace Engine
{
	Engine::Surface::~Surface()
	{
		SDL_FreeSurface(ManagedSurface);
	}

	const SDL_PixelFormat* Surface::GetPixelFormat() const
	{
		return ManagedSurface->format;
	}

	Engine::Surface::Surface(Surface&& other) noexcept
	{
		SDL_FreeSurface(ManagedSurface);
		ManagedSurface = other.ManagedSurface;
		other.ManagedSurface = nullptr;
	}

	Engine::Surface::Surface(SDL_Surface* other) noexcept
	{
		SDL_FreeSurface(ManagedSurface);
		ManagedSurface = other;
	}

	Surface& Engine::Surface::operator=(Surface&& other) noexcept
	{
		SDL_FreeSurface(ManagedSurface);
		ManagedSurface = other.ManagedSurface;
		other.ManagedSurface = nullptr;

		return *this;
	}
}
