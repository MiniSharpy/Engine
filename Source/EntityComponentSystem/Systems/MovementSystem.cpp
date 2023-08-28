#include "MovementSystem.h"
#include "../../SceneManagement/BaseScene.h"
#include "../../EntityComponentSystem/Entity.h"
#include "../../EntityComponentSystem/EntityManager.h"
#include "../../EntityComponentSystem/Components.h"

namespace Engine
{
	MovementSystem::MovementSystem(BaseScene& scene) : OwningScene(scene) { }

	void MovementSystem::Update(float deltaTime)
	{
		for (auto& entity : OwningScene.GetEntityManager().GetEntities())
		{
			if (entity.HasComponents<Position, Velocity>())
			{
				Velocity& velocity = entity.GetComponent<Velocity>();
				entity.GetComponent<Position>() += velocity;
				velocity = {};
			}
		}
	}
}