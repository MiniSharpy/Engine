#pragma once
namespace Engine
{
	class BaseSystem
	{
	public:
		virtual ~BaseSystem() {}
		virtual void Update(float deltaTime) = 0;
	};
}