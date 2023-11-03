#pragma once
#include "Modifier.h"
namespace Engine
{
	class DeadZoneModifier : public Modifier
	{
	private:
		inline static float DeadZone = 0.20f;
	public:
		void Process(float& value) override
		{
			// Axial, effectively.
			value = abs(value) > DeadZone ? value : 0;
		}

		void Process(Vector2<float>& value) override
		{
			// https://web.archive.org/web/20190129113357/http://www.third-helix.com/2013/04/12/doing-thumbstick-dead-zones-right.html
			// Axial
			value.X = abs(value.X) > DeadZone ? value.X : 0;
			value.Y = abs(value.Y) > DeadZone ? value.Y : 0;
		}
	};
}
