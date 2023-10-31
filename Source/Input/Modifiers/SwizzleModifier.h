#pragma once
#include "Modifier.h"

namespace Engine
{
	class SwizzleModifier : public Modifier
	{
	public:
		void Process(float& value) override {} // Do nothing, doesn't make sense to swizzle a float.
		void Process(Vector2<float>& value) override
		{
			value = value.YX();
		}
	};
}