#pragma once
#include "Modifier.h"
namespace Engine
{
	class DeadzoneModifier : public Modifier
	{
	private:
		inline static float Deadzone = 0.20f;
	public:
		void Process(float& value) override
		{
			value = abs(value) > Deadzone ? value : 0;
		}

		void Process(Vector2<float>& value) override
		{
			value.X = abs(value.X) > Deadzone ? value.X : 0;
			value.Y = abs(value.Y) > Deadzone ? value.Y : 0;
		}
	};
}
