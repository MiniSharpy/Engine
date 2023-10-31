#pragma once
namespace Engine
{
	class BaseSystem
	{
	public:
		virtual ~BaseSystem() = default;
		virtual void Update(float deltaTime) = 0;
	};
}