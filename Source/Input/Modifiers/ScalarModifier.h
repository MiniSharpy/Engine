#pragma once
#include "Modifier.h"
#include "../../Maths/Vector2.h"

namespace Engine
{
	class ScalarModifier : public Modifier
	{
	private:
		float Scalar;
	public:
		ScalarModifier() : Scalar(10.f) {}
		ScalarModifier(float scalar) : Scalar(scalar) {}

		void operator()(float& value) override
		{
			value *= Scalar;
		}

		void operator()(Vector2<float>& value) override
		{
			value *= Scalar;
		}
	};
}