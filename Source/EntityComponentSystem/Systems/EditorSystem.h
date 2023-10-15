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
	// It is assumed that only one EditorSystem is in use at a time.
	class EditorSystem : public BaseSystem
	{
	public:
		EditorSystem(IsometricScene& scene);

	private:
		IsometricScene& OwningScene;

		// ImGui variables
		std::vector<std::string> AvailableTextureNames;
		std::string SelectedFileName;
		Vector2<int> SelectedMapTileSize = { 128, 128 };
		std::optional<TileAtlas> CurrentTileAtlas;

		// These are mainly stored in the class rather than as statics in the
		// functions so that they can be reset when switching between tabs.
		std::optional<Vector2<int>> SelectedTile;
		std::optional<size_t> SelectedEntityID;

		// Useful helper methods
		// TODO: Get by ZOrder, or get multiple overlapping?
		std::optional<Entity> GetCollidingEntity(Vector2<float> position, Vector2<float> bounds);

		// Core logic
		void MapSettings();
		std::optional<std::pair<ComponentReferenceSlice, std::bitset<MAX_COMPONENTS>&>> TileEditor();
		void TileActions();
		void MapDebug();
		std::optional<std::pair<ComponentReferenceSlice, std::bitset<MAX_COMPONENTS>&>> EntityOutliner();

		// Tile Actions
		UndoManager OwnedUndoManager;

	public:
		virtual void Update(float deltaTime);
	};
}