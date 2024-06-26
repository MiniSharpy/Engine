#include "EditorSystem.h"
#include "../../Core/Renderer.h"
#include "../../EntityComponentSystem/EntityManager.h"
#include "../../SceneManagement/IsometricScene.h"
#include "../../Commands/CreateEntityCommand.h"
#include "../../Commands/DeleteEntityCommand.h"
#include "../../Commands/UndoManager.h"
#include "../../Pathfinding/NavigationGraph.h"
#include "../../Maths/Vector2.h"
#include "../../Collision/Intersections.h"
#include "../../Editor/ComponentEditor.h"
#include"../../Core/Timer.h"
#include <imgui.h>
#include <filesystem>
#include <string>
#include <optional>
#include <limits>
#include <format>
#include <numbers>
#include <cmath>

namespace Engine
{
	EditorSystem::EditorSystem(IsometricScene& scene) : OwningScene{ scene }
	{
		// TODO: Allow refreshing during runtime.
		// Get files names from directory. Filesystem is quite heavy, making it worthwhile to cache results.
		std::filesystem::path textureDirectory = std::filesystem::current_path() / "Data" / "Textures"; // TODO: Put somewhere global for access elsewhere.
		// TODO: Fetch only allowed image files.
		for (const auto& entry : std::filesystem::directory_iterator(textureDirectory)) // Throws an exception if there's no path.
		{
			AvailableTextureNames.push_back(entry.path().filename().string());
		}

		// Just in case there does end up being a file without an extension called None that's actually a texture. TODO: This won't be necessary if only valid extensions are allowed.
		if (std::find(AvailableTextureNames.begin(), AvailableTextureNames.end(), "None") == AvailableTextureNames.end())
		{
			SelectedFileName = "None";
		}
	}

	std::optional<Entity> EditorSystem::GetCollidingEntity(Vector2<float> position, Vector2<float> bounds)
	{
		for (auto& entity : OwningScene.GetEntityManager().GetEntitiesByTag("TileSet"))
		{
			// As the entities are retrieved by tags components can be assumed.
			auto collision = Collision::RectangleIntersection
			(
				{ position, bounds },
				{ entity.GetComponent<Position>(), bounds }
			);

			if (collision)
			{
				return entity;
			}
		}

		return {};
	}

	void EditorSystem::MapSettings()
	{
		ImGui::Begin("Map Settings", 0, ImGuiWindowFlags_AlwaysAutoResize);

		// Max for sliders is half of the int maximum for some reason.
		ImGui::SliderInt("TileWidth:", &SelectedMapTileSize.X, 64, std::numeric_limits<int>::max() / 2, "%d", ImGuiSliderFlags_AlwaysClamp); // Limiting this value is too much effort for me to want to bother.
		ImGui::SliderInt("TileHeight:", &SelectedMapTileSize.Y, 64, std::numeric_limits<int>::max() / 2, "%d", ImGuiSliderFlags_AlwaysClamp);

		if (!CurrentTileAtlas) { ImGui::End(); return; }

		// TODO: Fix bug where if you place a tile without a collider, change map size, and then place a tile with
		// a collider the display for each loop will incorrectly end early.
		if (ImGui::Button("Apply"))
		{
			OwningScene.SetTileSize(SelectedMapTileSize.X, SelectedMapTileSize.Y);
			CurrentTileAtlas = {};
			SelectedTile = {};
		}
		ImGui::End();
	}

	std::optional<std::pair<ComponentReferenceSlice, std::bitset<MAX_COMPONENTS>&>> EditorSystem::TileEditor()
	{
		// Stop out of index errors if there are no tile atlases.
		if (AvailableTextureNames.size() == 0) { return {}; }

		// File Selection
		if (ImGui::BeginCombo("Image", SelectedFileName.c_str()))
		{
			for (int currentEntry = 0; currentEntry < AvailableTextureNames.size(); currentEntry++)
			{
				const bool isSelected = AvailableTextureNames[currentEntry] == SelectedFileName; // Highlights selected in dropdown.
				if (ImGui::Selectable(AvailableTextureNames[currentEntry].c_str(), isSelected))
				{
					if (SelectedFileName != AvailableTextureNames[currentEntry])
					{
						SelectedFileName = AvailableTextureNames[currentEntry];
						CurrentTileAtlas = { SelectedFileName, SelectedMapTileSize };
						SelectedTile = {};
					}
				}
			}
			ImGui::EndCombo();
		}

		// Display editor and handle input into the scene.
		if (CurrentTileAtlas)
		{
			TileActions();

			std::optional<Vector2<int>> clickedTile = CurrentTileAtlas->AtlasEditor(SelectedTile);

			if (clickedTile == SelectedTile) { SelectedTile = {}; } // Toggle selection.
			else if (clickedTile) { SelectedTile = clickedTile; } // Set to selection

			if (SelectedTile)
			{
				Tile& selectedTile = (*CurrentTileAtlas)(SelectedTile->X, SelectedTile->Y);
				return { {selectedTile.GetReferenceSlice(), selectedTile.GetEnabledComponents()} };
			}
		}

		return {};
	}

	void EditorSystem::TileActions()
	{
		// If mouse over Dear ImGUI UI return early to prevent clicking "through" UI.
		ImGuiIO& io = ImGui::GetIO();
		if (io.WantCaptureMouse) { return; }

		// Slightly inefficient as it only needs to be done on mouse click.
		Vector2<float> mouseGridPosition = OwningScene.ScreenSpaceToGrid(io.MousePos);
		mouseGridPosition = OwningScene.GridToWorldSpace(mouseGridPosition);
		Vector2<int> tileSize = CurrentTileAtlas->GetTileSize();

		// Place Tile
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) // TODO: Switch to is mouse down once there's a more clever overlap prevention.
		{
			if (!SelectedTile) { return; }

			// If there's no entity found at the position then create one.
			// TODO: Divide by 4 as iso tiles assume overlap. It may not always be by 4, convert to variable.
			std::optional<Entity> collision = GetCollidingEntity(mouseGridPosition, { (float)tileSize.X, tileSize.Y / 4.0f });

			Tile& tile = (*CurrentTileAtlas)(SelectedTile->X, SelectedTile->Y);

			if (!tile.HasComponent<Position>()) { return; } // Can't exactly place something in a space if doesn't have a position.


			if (!collision || tile.GetComponent<Position>().Z > 0)
			{
				// Intentional copies for adding to command.
				// TODO: Wrap ComponentSlice and bitset in an object to make getters and setters nicer.
				ComponentSlice components = tile.GetComponents();
				std::bitset<64> enabledComponents = tile.GetEnabledComponents();

				Position& position = std::get<Position>(components);
				position.X = mouseGridPosition.X; position.Y = mouseGridPosition.Y;

				OwnedUndoManager.AddCommandAndExecute<CreateEntityCommand>(components, enabledComponents, OwningScene.GetEntityManager()); // TODO: Use std::move instead of relying on compiler?
			}
		}

		// Erase Tile
		if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			// If entity exists remove it.
			std::optional<Entity> entity = GetCollidingEntity(mouseGridPosition, { (float)tileSize.X, tileSize.Y / 4.0f });
			if (entity)
			{
				OwnedUndoManager.AddCommandAndExecute<DeleteEntityCommand>(entity.value(), OwningScene.GetEntityManager());
			}
		}

		// Order matters because they share two similar keypresses. 
		// Also ImGui::IsKeyPress creates a satisfying slow build up, and does not immediately delete everything.
		// Redo
		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsKeyPressed(ImGuiKey_Z))
		{
			SDL_Log("Attempting redo!");
			OwnedUndoManager.Redo();
		}
		// Undo
		else if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Z))
		{
			SDL_Log("Attempting undo!");
			OwnedUndoManager.Undo();
		}

	}

	void EditorSystem::MapDebug()
	{
		ImGui::SetNextWindowPos({ 0.f,0.f });
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		ImGui::Begin("GridDrawing", 0,
			ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		// Draw colliders
		for (auto& entity : OwningScene.GetEntityManager().GetEntities())
		{
			if (!entity.HasComponents<Collider, Position, Sprite>()) { continue; }

			Collider& collider = entity.GetComponent<Collider>();
			std::vector<Vector2<float>> collisionPoints(collider.Points.begin(), collider.Points.begin() + collider.NumberOfPoints); // Sequence of nodes to form edge of collider.

			if (collisionPoints.empty()) { continue; }

			Position& position = entity.GetComponent<Position>();
			Sprite& sprite = entity.GetComponent<Sprite>();
			for (auto it = collisionPoints.cbegin(); it != collisionPoints.cend() - 1; ++it)
			{
				Vector2<float> point0 = OwningScene.WorldSpaceToRenderSpace(*it + position - sprite.PivotOffset);
				Vector2<float> point1 = OwningScene.WorldSpaceToRenderSpace(*(it + 1) + position - sprite.PivotOffset);
				ImGui::GetWindowDrawList()->AddLine(point0, point1, IM_COL32(0, 255, 0, 255), 2);
			}
		}

		// Draw path to mouse.
		//if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			// TODO: This is quite awkward because the start and goal need to be a particular distance apart for the neighbour checking.
			// It might make more sense to use the grid position, but that'll need conversion inside the function to handle collisions.
			Position& playerPosition = OwningScene.GetEntityManager().GetEntitiesByTag("Player")[0].GetComponent<Position>();
			const Vector2<int> start = static_cast<Vector2<int>>(OwningScene.WorldSpaceToGrid({playerPosition.X, playerPosition.Y}));
			const Vector2<int> goal = static_cast<Vector2<int>>(OwningScene.ScreenSpaceToGrid(ImGui::GetMousePos()));

			auto cameFrom = OwningScene.ManagedNavigationGraph.AStar(start, goal);
			auto path = OwningScene.ManagedNavigationGraph.ConstructPath(cameFrom, start, goal);

			for (int i = 1; i < path.size(); ++i)
			{
				Vector2<float> point0 = OwningScene.WorldSpaceToRenderSpace(static_cast<Vector2<float>>(path[i - 1]));
				Vector2<float> point1 = OwningScene.WorldSpaceToRenderSpace(static_cast<Vector2<float>>(path[i]));
				ImGui::GetWindowDrawList()->AddLine(point0, point1, IM_COL32(0, 0, 255, 255), 2);
			}
		}

		ImGui::End();
	}

	std::optional<std::pair<ComponentReferenceSlice, std::bitset<MAX_COMPONENTS>&>> EditorSystem::EntityOutliner()
	{
		// TODO: Take to entity on double click, some sort of highlight?
		EntityManager& entityManager = OwningScene.GetEntityManager();
		if (ImGui::BeginTable("EntityTable", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
		{
			for (Entity entity : entityManager.GetEntities())
			{
				const bool isSelected = (SelectedEntityID == entity.GetID());
				std::string label = std::format("Entity {}", entity.GetID());
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				if (ImGui::Selectable(label.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns))
				{
					if (SelectedEntityID != entity.GetID())
					{
						SelectedEntityID = entity.GetID();
					}
					else
					{
						SelectedEntityID = {};
					}
				}
				ImGui::TableNextColumn();
				ImGui::Text("%s", entity.GetTag().c_str());
			}

			ImGui::EndTable();
		}

		if (SelectedEntityID)
		{
			return { { entityManager.GetReferenceSlice(*SelectedEntityID), entityManager.GetEnabledComponents(*SelectedEntityID) } };
		}

		return {};
	}

	void EditorSystem::SpawnEntities()
	{
		ImGui::InputInt("Entity Count:", &numberOfEntities);

		EntityManager& entityManager = OwningScene.GetEntityManager();
		std::vector<Entity>& testNPCs = entityManager.GetEntitiesByTag("TestNPC");

		for (int i = testNPCs.size(); i < numberOfEntities; ++i) // Spawn new entities.
		{
			Entity entity = entityManager.AddEntity("TestNPC");
			entity.AddComponent<Position>();
			entity.AddComponent<Velocity>();
			entity.AddComponent<Sprite>();
			entity.AddComponent<Animation>();

			Velocity& velocity = entity.GetComponent<Velocity>();

			constexpr int segments = 128;
			constexpr float angleIncrements = (2 * std::numbers::pi_v<float>) / segments;
			velocity.Speed = 64 * ((i / segments) + 1);
			velocity.Direction.X = std::cos(angleIncrements * (i % segments));
			velocity.Direction.Y = std::sin(angleIncrements * (i % segments));

			Sprite& sprite = entity.GetComponent<Sprite>();
			strcpy(sprite.TextureName, "AnimationSheet.png");
			sprite.SourceRectangle = { {}, OwningScene.TileSize };
			sprite.PivotOffset = { static_cast<float>(OwningScene.TileSize.X) / 2.f, static_cast<float>(OwningScene.TileSize.Y) / 1.5f };
		}

		for (int i = testNPCs.size(); i > numberOfEntities; --i) // Delete exceeded entities.
		{
			entityManager.Destroy(testNPCs[i - 1]);
		}
	}

	void EditorSystem::Update(const float& deltaTime)
	{
		if (!IsEnabled) { return; }

		// TODO: Hotkey to enable debug. Perhaps tilde?
		MapSettings();
		MapDebug();

		std::optional<std::pair <ComponentReferenceSlice, std::bitset<MAX_COMPONENTS>&>> selectedEntity;

		ImGui::Begin("MainEditor", nullptr, ImGuiWindowFlags_None);
		if (ImGui::BeginTabBar("EntityEditing", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Entity Outliner"))
			{
				SpawnEntities();
				selectedEntity = EntityOutliner();
				SelectedTile = {};
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Tile Atlas Editor"))
			{
				selectedEntity = TileEditor();
				SelectedEntityID = {};
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();

		if (selectedEntity)
		{
			ComponentsEditor(selectedEntity->first, selectedEntity->second);
		}

	}
}
