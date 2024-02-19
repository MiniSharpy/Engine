#pragma once
#include "RawInput.h"
#include "Action.h"
#include <memory>

namespace Engine
{
	template<typename ...T>
	constexpr bool IsValidType() { return (std::same_as<T..., float> || std::same_as<T..., Vector2<float>>); }

	class Input
	{
	private:
		std::vector<std::unique_ptr<IAction>> Actions;

	public:

		template<typename ...T>
			requires (sizeof...(T) == 0 || 
		(sizeof...(T) == 1 && IsValidType<T...>()))
		Action<T...>& AddAction(std::function<void(T...)> function, bool cumulateInputs = false)
		{
			IAction* action = Actions.emplace_back(new Action(std::move(function), cumulateInputs)).get();
			return *dynamic_cast<Action<T...>*>(action);
		}

		void Process() const
		{
			for (auto& action : Actions)
			{
				action->Process();
			}
		}

		void UpdateMappedInputs(const std::string& name, const ProcessState processState) const
		{
			for (auto& action : Actions)
			{
				if (!action->HasInput(name)) { continue; }
				action->Update(name, processState);
			}
		}

		void UpdateMappedInputs(const std::string& name, const ProcessState processState, float value) const
		{
			for (auto& action : Actions)
			{
				if (!action->HasInput(name)) { continue; }
				action->Update(name, processState, value);
			}
		}

		void UpdateMappedInputs(const std::string& name, const ProcessState processState, Vector2<float> value) const
		{
			for (auto& action : Actions)
			{
				if (!action->HasInput(name)) { continue; }
				action->Update(name, processState, value);
			}
		}
	};
}