#pragma once
#include "BaseScene.h"
#include<unordered_map>
#include<string>

namespace Engine
{
	/// <summary>
	/// Due to "friend" shenanigans it means that the manager can't use std data structures
	/// to construct scene objects without allowing unwanted access elsewhere, so
	/// may as well just have the Scene constructors public for now.
	/// Perhaps if pointers were stored then that would allow for construction, 
	/// but does mean some manual memory clean up.
	/// </summary>
	class SceneManager
	{
	public:
		static SceneManager& Instance();

	private:
		SceneManager();
		/// <summary>
		/// A map of all loaded scenes in memory.
		/// </summary>
		std::unordered_map<std::string, std::unique_ptr<BaseScene>> ScenesByName;
		BaseScene* CurrentScene;

	public:
		BaseScene& GetCurrentScene() const { return *CurrentScene; } // TODO: Handle undefined behaviour properly.
		template<typename T>
			requires(std::is_base_of<BaseScene, T>::value) // Is this really neccesary if the emplaced type is a BaseScene?
		void LoadScene(const float& deltaTime)
		{
			ScenesByName.emplace("default", std::make_unique<T>(deltaTime));
			CurrentScene = ScenesByName["default"].get();
		}
		// TODO: LoadScene<DerivedScene>(params); // Construct scene into ScenesByName, forwarding the necessary parameters.
	};
}