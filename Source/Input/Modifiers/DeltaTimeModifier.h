#pragma once
#include "Modifier.h"

namespace Engine
{
	class DeltaTimeModifier : public Modifier
	{
	private:
		const float& DeltaTime;
	public:
		DeltaTimeModifier(const float& deltaTime) : DeltaTime{ deltaTime } {}

		void operator()(float& value) override
		{
			value *= DeltaTime;
		}

		void operator()(Vector2<float>& value) override
		{
			value *= DeltaTime;
		}
	};
}

