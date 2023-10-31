#pragma once
#include "Modifier.h"
#include "../Maths/Vector2.h"

namespace Engine
{
	class NegateModifier : public Modifier
	{
	public:
		void Process(float& value) override
		{
			value = -value;
		}

		void Process(Vector2<float>& value) override
		{
			value.X = -value.X;
			value.Y = -value.Y;
		}
	};
}