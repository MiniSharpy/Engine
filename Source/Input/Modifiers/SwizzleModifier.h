#pragma once
#include "Modifier.h"

namespace Engine
{
	class SwizzleModifier : public Modifier
	{
	public:
		void operator()(float& value) override {} // Do nothing, doesn't make sense to swizzle a float.
		void operator()(Vector2<float>& value) override
		{
			value = value.YX();
		}
	};
}