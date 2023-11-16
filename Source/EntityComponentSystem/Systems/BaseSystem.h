#pragma once
namespace Engine
{
	class BaseSystem
	{
	public:
		virtual ~BaseSystem() = default;
		virtual void Update(const float& deltaTime) = 0;
	};
}