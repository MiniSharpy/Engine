#pragma once
#pragma once
#include "BaseSystem.h"

namespace Engine
{
	class BaseScene;

	class PathfindingSystem : public BaseSystem
	{
	public:
		PathfindingSystem(BaseScene& scene);
		void Update(const float& deltaTime) override;
	private:
		BaseScene& OwningScene;
	};

}
