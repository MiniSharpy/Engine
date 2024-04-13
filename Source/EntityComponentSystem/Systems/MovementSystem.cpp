#include "MovementSystem.h"
#include "../../SceneManagement/BaseScene.h"
#include "../../EntityComponentSystem/Entity.h"
#include "../../EntityComponentSystem/EntityManager.h"
#include "../../EntityComponentSystem/Components.h"

namespace Engine
{
	MovementSystem::MovementSystem(BaseScene& scene) : OwningScene(scene) { }

	void MovementSystem::Update(const float& deltaTime)
	{
		for (auto& entity : OwningScene.GetEntityManager().GetEntities())
		{
			if (!entity.HasComponents<Position, Velocity>()) { return; }

			Position& position = entity.GetComponent<Position>();
			Velocity& velocity = entity.GetComponent<Velocity>();
			velocity.Direction.Normalise();
			position += velocity.Direction * velocity.Speed * deltaTime;

			// Just for testing.
			if (position.LengthSquared() > 1024 * 1024)
			{
				position.X = 0;
				position.Y = 0;
			}
		}
	}
}