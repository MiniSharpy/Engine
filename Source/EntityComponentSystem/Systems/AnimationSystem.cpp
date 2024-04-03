


#include "AnimationSystem.h"
#include "../../SceneManagement/BaseScene.h"
#include <numbers>

namespace Engine
{
	AnimationSystem::AnimationSystem(BaseScene& scene) : OwningScene(scene)
	{
		
	}

	void AnimationSystem::Update(const float& deltaTime)
	{
		for (Entity entity : OwningScene.GetEntityManager().GetEntities())
		{
			if (!entity.HasComponents<Velocity, Sprite, Animation>()) { return; }
			Velocity& velocity = entity.GetComponent<Velocity>();
			Sprite& sprite = entity.GetComponent<Sprite>();
			Animation& animation = entity.GetComponent<Animation>();

			// CALCULATE Y COORDINATE
			// Direction determines which column on the sprite map to render.
			const float angleRadians = std::atan2(velocity.Direction.X, velocity.Direction.Y);
			const float angleDegrees = angleRadians * (180.f / std::numbers::pi_v<float>);

			// Want to get in the range 0-360, where Vector2::Up is 0. Unaltered it is 180 degrees.
			const float angle = std::abs(angleDegrees - 180);

			const int column = static_cast<int>(angle / (360.f / NumberOfDirections));

			// CALCULATE X COORDINATE
			// Speed determines whether X = 0.
			const int row = velocity.Speed > 0 ? 
				std::clamp(animation.Time / (TimeToLoop / NumberOfFrames) + 1, 1, NumberOfFrames)
			: 0;

			sprite.SourceRectangle.Position = 
			{
				row * sprite.SourceRectangle.Size.X,
				column * sprite.SourceRectangle.Size.Y
			};

			animation.Time += static_cast<int>(deltaTime * 1000);
			animation.Time %= TimeToLoop;

			SDL_Log("row: %d, column: %d", row, column);
		}
	}
}


