#pragma once
#include "BaseSystem.h"
#include "../../Maths/Vector2.h"
#include "../../SceneManagement/IsometricScene.h"
#include "../../Commands/UndoManager.h"
#include "../../Editor/TileAtlas.h"
#include <optional>
#include <string>
#include <vector>

namespace Engine
{
	// TODO: Using setter for altering scene.
	// TODO: Make agnostic to isometric scenes.
	class EditorSystem : public BaseSystem
	{
	public:
		EditorSystem(IsometricScene& scene);

	private:
		IsometricScene& OwningScene;

		// ImGui variables
		std::vector<std::string> AvailableTextureNames;
		std::string SelectedFileName;
		Vector2<int> SelectedTile;
		Vector2<int> SelectedMapTileSize = { 128, 128 };
		std::optional<TileAtlas> CurrentTileAtlas;

		// Useful helper methods
		// TODO: Get by ZOrder, or get multiple overlapping?
		std::optional<Entity> GetCollidingEntity(Vector2<float> position, Vector2<float> bounds);

		// Core logic
		void MapSettings();
		void FileSelection();
		void TileActions();
		void MapDebug();

		// Tile Actions
		UndoManager OwnedUndoManager;

	public:
		virtual void Update(float deltaTime);
	};
}