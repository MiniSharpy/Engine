#pragma once
#include "Modifier.h"

namespace Engine
{
	class DeltaTimeModifier : public Modifier
	{
	private:
		const float& DeltaTime;
	public:
		DeltaTimeModifier(const float& deltaTime) : DeltaTime{ deltaTime }
		{
			
		}

		virtual void operator()(float& value)
		{
			value *= DeltaTime;
		}
		virtual void operator()(Vector2<float>& value)
		{
			value *= DeltaTime;
		}
	};
}

