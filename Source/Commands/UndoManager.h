#pragma once
#include<stack>
#include "Command.h"

namespace Engine
{
	class UndoManager
	{
	public:
		~UndoManager();
	private:
		std::stack<Command*> UndoHistory;
		std::stack<Command*> RedoHistory;
		void ClearCommandStack(std::stack<Command*>& stack);

	public:
		template<typename T, typename... Args>
		void AddCommandAndExecute(Args&&... args)
		{
			Command* command = new T(std::forward<Args>(args)...);
			command->Execute();
			UndoHistory.push(command);
			// Need to clear redo after execution of new command so that history doesn't get muddled.
			ClearCommandStack(RedoHistory);
		}

		void Redo();
		void Undo();
	};
}