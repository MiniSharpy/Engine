#pragma once

namespace Engine
{
	class Input;

	/// <summary>
	/// Polymorphic functor that that tests an input for specific conditions.
	///	Strictly speaking testing for Stop does not need to be done as that is filtered away first.
	/// </summary>
	class Condition
	{
	public:
		virtual ~Condition() = default;
		virtual bool operator()(const Input& input) = 0;
	};


}