#pragma once
namespace Engine
{
	class Modifier
	{
	public:
		virtual ~Modifier() = default;
		virtual void Process(float& value) = 0;
		virtual void Process(Vector2<float>& value) = 0;
	};
}

