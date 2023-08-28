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
		double Time()
		{
			return double(SDL_GetPerformanceCounter() - Start) / SDL_GetPerformanceFrequency();
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