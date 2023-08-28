#pragma once
#include "BaseSystem.h"

namespace Engine
{
	class BaseScene;

	class MovementSystem : public BaseSystem
	{
	public:
		MovementSystem(BaseScene& scene);
		virtual void Update(float deltaTime);
	private:
		BaseScene& OwningScene;
	};
}