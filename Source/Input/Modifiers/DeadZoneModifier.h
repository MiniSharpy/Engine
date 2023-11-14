#pragma once
#include "Modifier.h"
namespace Engine
{
	// TODO: Set PrcoessState to stop if within dead zone? Otherwise joysticks will be processed every frame
	// once they occur, even when their value is ~0.
	class DeadZoneModifier : public Modifier
	{
	private:
		float DeadZone;
	public:
		DeadZoneModifier() : DeadZone(0.2f) {}
		DeadZoneModifier(float deadZone) : DeadZone(deadZone) {}

		void operator()(float& value) override
		{
			// Axial, effectively.
			value = abs(value) > DeadZone ? value : 0;
		}

		void operator()(Vector2<float>& value) override
		{
			// https://web.archive.org/web/20190129113357/http://www.third-helix.com/2013/04/12/doing-thumbstick-dead-zones-right.html
			value.X = abs(value.X) > DeadZone ? value.X : 0;
			value.Y = abs(value.Y) > DeadZone ? value.Y : 0;
		}
	};
}
