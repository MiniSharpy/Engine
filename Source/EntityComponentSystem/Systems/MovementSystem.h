#pragma once
#include "BaseSystem.h"

namespace Engine
{
	class BaseScene;

	class MovementSystem : public BaseSystem
	{
	public:
		MovementSystem(BaseScene& scene);
		void Update(const float& deltaTime) override;
	private:
		BaseScene& OwningScene;
	};
}