#include "UndoManager.h"
#include "Command.h"

namespace Engine
{
	UndoManager::~UndoManager()
	{
		ClearCommandStack(RedoHistory);
		ClearCommandStack(UndoHistory);
	}

	void UndoManager::ClearCommandStack(std::stack<Command*>& stack)
	{
		while (!stack.empty())
		{
			Command* command = stack.top();
			stack.pop();
			delete command;
		}
	}

	void UndoManager::Redo()
	{
		if (RedoHistory.empty()) { return; }

		Command* command = RedoHistory.top();
		command->Execute();
		RedoHistory.pop();
		UndoHistory.push(command);
	}

	void UndoManager::Undo()
	{
		if (UndoHistory.empty()) { return; }

		Command* command = UndoHistory.top();
		command->Undo();
		UndoHistory.pop();
		RedoHistory.push(command);
	}
}