#pragma once
#include <SDL.h>

namespace Engine
{
	class TapCondition : public Condition
	{
	private:
		ProcessState PreviousProcessState = Stop;
		Uint32 TimeOfLastPress = 0;
		int PressCount = 0;
	public:
		/// <summary>
		/// In milliseconds.
		/// </summary>
		int MaxTimeBetweenPresses = 500;
		int RequiredPresses = 2;

		bool operator()(const Input& input) override
		{
			bool isHeld = input.CurrentProcessState == PreviousProcessState;
			bool isPressed = !isHeld && input.CurrentProcessState != Release;
			PreviousProcessState = input.CurrentProcessState;

			if (!isPressed) { return false; }

			Uint32 currentTime = SDL_GetTicks(); // Time since SDL initialisation in milliseconds.
			if (SDL_TICKS_PASSED(currentTime, TimeOfLastPress + MaxTimeBetweenPresses))
			{
				PressCount = 0;
			}
			TimeOfLastPress = currentTime;
			PressCount++;

			return PressCount >= RequiredPresses;
		}
	};
}
