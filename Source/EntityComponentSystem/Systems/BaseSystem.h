#pragma once
namespace Engine
{
	// TODO: Mayhap the OwningScene should be stored here, and a CTOR takes it in.
	class BaseSystem
	{
	public:
		virtual ~BaseSystem() = default;
		virtual void Update(const float& deltaTime) = 0;
	};
}