#pragma once
namespace Engine
{
	// TODO: Delta Time Modifier
	class Modifier
	{
	public:
		virtual ~Modifier() = default;
		virtual void operator()(float& value) = 0;
		virtual void operator()(Vector2<float>& value) = 0;
	};
}

