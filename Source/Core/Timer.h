#pragma once
#include <SDL.h>

namespace Engine
{
	/// <summary>
	/// Time starts ticking from construction.
	/// </summary>
	class Timer
	{
		Uint64 Start;

	public:

		Timer() : Start{ SDL_GetPerformanceCounter() } {}

		/// <returns>Time in seconds.</returns>
		template <typename T>
			requires std::floating_point<T>
		T Time()
		{
			return static_cast<T>(SDL_GetPerformanceCounter() - Start) / static_cast<T>(SDL_GetPerformanceFrequency());
		}

		/// <summary>
		/// Reset start time to current now.
		/// </summary>
		void Reset()
		{
			Start = SDL_GetPerformanceCounter();
		}
	};
}