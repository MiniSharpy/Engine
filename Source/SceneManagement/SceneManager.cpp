#include "SceneManager.h"
#include "BaseScene.h"
#include "IsometricScene.h"
#include<unordered_map>
#include<string>

namespace Engine
{
	SceneManager& SceneManager::Instance()
	{
		static SceneManager instance;
		return instance;
	}

	SceneManager::SceneManager() :
		CurrentScene{ *ScenesByName.emplace("default", std::make_unique<IsometricScene>()).first->second }
	{
	}
}