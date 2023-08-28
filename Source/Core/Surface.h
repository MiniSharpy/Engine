#pragma once
struct SDL_Surface;
struct SDL_PixelFormat;

namespace Engine
{
	class Surface
	{
	public:
		Surface() = default;
		~Surface();

		const SDL_PixelFormat* GetPixelFormat() const;

		Surface(const Surface& other) = delete;
		Surface(Surface&& other) noexcept;
		Surface(SDL_Surface* other) noexcept;
		Surface& operator=(const Surface& other) = delete;
		Surface& operator=(Surface&& other) noexcept;

		operator SDL_Surface* () { return ManagedSurface; }
		operator SDL_Surface* () const { return ManagedSurface; }
	private:
		SDL_Surface* ManagedSurface = nullptr;
	};
}