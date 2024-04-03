#pragma once
#pragma once
#include "BaseSystem.h"

namespace Engine
{
	class BaseScene;

	class AnimationSystem : public BaseSystem 
	{
	public:
		AnimationSystem(BaseScene& scene);

		void Update(const float& deltaTime) override;
	private:
		BaseScene& OwningScene;

		// These could go on the component themselves, depends on how much variety the designer wants.
		/// <summary>
		/// Time in milliseconds that it takes to do a full loop of the animation sprite sheet.
		///	FPS = 24
		/// </summary>
		int TimeToLoop = static_cast<int>(24.f / 60.f * 1000);
		int NumberOfDirections = 8;
		int NumberOfFrames = 13;
	};

}
