#include "Settings.h"
namespace Engine
{
	Settings& Settings::Instance()
	{
		static Settings instance;
		return instance;
	}

	Settings::Settings()
	{
		SetTargetFrameRate(200);
	}

	int Settings::GetTargetFrameRate() const
	{
		return TargetFrameRate;
	}

	float Settings::GetTargetFrameTime() const
	{
		return TargetFrameTime;
	}

	void Settings::SetTargetFrameRate(int frameRate)
	{
		TargetFrameRate = frameRate;
		TargetFrameTime = frameRate == 0 ? 0 : 1.0 / frameRate; // Let's not divide by zero.
	}
}