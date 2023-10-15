#pragma once
#include "../EntityComponentSystem/Components.h"
#include "../EntityComponentSystem/EntityMemoryPool.h"
#include <array>
#include <bitset>
#include "../Maths/Vector2.h"
#include "../Core/Texture.h"
#include <imgui.h>
namespace Engine
{
	// Not all components need to be editable from the editor.
	template <typename T>
	void ComponentEditor(ComponentReferenceSlice& components, const std::bitset<MAX_COMPONENTS>& EnabledComponents) {}

	// TODO: Separate Z order out into its own thing. It means Position and ZOrder can have their own components and the TileEditor doesn't need to touch XY.
	// It also means the Position component doesn't have confusing inheritance where the Z is left unaffected by assignments and what not.
	template <>
	inline void ComponentEditor<Position>(ComponentReferenceSlice& components, const std::bitset<MAX_COMPONENTS>& EnabledComponents)
	{
		const bool isEnabled = EnabledComponents[tuple_element_index_v<Position, ComponentSlice>];
		if (!isEnabled) { return; }

		int& zOrder = std::get<Position&>(components).Z;
		ImGui::InputInt("ZOrder", &zOrder);
	}

	// TODO: Need to change all instances of std::get<T> to be std::get<T&>.

	template<>
	inline void ComponentEditor<Collider>(ComponentReferenceSlice& components, const std::bitset<MAX_COMPONENTS>& EnabledComponents)
	{
		const bool isEnabled = EnabledComponents[tuple_element_index_v<Collider, ComponentSlice>];
		if (!isEnabled) { return; }

		const bool isMissingDependencies = !EnabledComponents[tuple_element_index_v<Sprite, ComponentSlice>];
		if (isMissingDependencies) { ImGui::Text("COLLIDER COMPONENT REQUIRES SPRITE COMPONENT!"); return; } // TODO: Report missing sprite component in whatever system deals with colliders?

		const Sprite& sprite = std::get<Sprite&>(components);
		if (!sprite.SourceTexture) { ImGui::Text("MISSING TEXTURE!"); return; }

		// TODO: Pass in an EditorEntity, which manages either a component slice or an actual corresponding entity so that IsEnabled checks can be done.
		// This may as well be set for all calls. TODO: This may run into problems with the clamping if two different editors are opened with different tile sizes.
		static Vector2<int> Snapping = { 64, 64 };

		// TODO: Draw a grid representing the snapping increments.
		// TODO: Multiple unconnected line segment, currently the assumption is they'll all want to be connected to one another.
		const Vector2<float> tileSize = (Vector2<float>)sprite.SourceRectangle.Size;

		std::vector<Vector2<float>>& collisionPoints = std::get<Collider&>(components).Points;

		/* INPUT */
		// Snapping choice
		ImGui::InputInt2("Snapping", &Snapping.X); // This relies on the order of Vector2 being X, Y.
		Snapping = Vector2<int>::Clamp(Snapping, { 1, 1 }, sprite.SourceRectangle.Size / 4); // TODO: Lock values to each other with optional button to disable.

		// Collision Input.
		const Vector2<float> atlasSize = (Vector2<float>)sprite.SourceTexture->GetSize();
		const Vector2<float> uvTopLeft = { (float)sprite.SourceRectangle.Position.X / atlasSize.X, (float)sprite.SourceRectangle.Position.Y / atlasSize.Y };
		const Vector2<float> uvBottomRight = { ((float)sprite.SourceRectangle.Position.X + tileSize.X) / atlasSize.X, ((float)sprite.SourceRectangle.Position.Y + tileSize.Y) / atlasSize.Y };
		ImGui::Image(*sprite.SourceTexture, atlasSize, uvTopLeft, uvBottomRight);

		const Vector2<float> imagePosition = (Vector2<float>)ImGui::GetItemRectMin();
		if (ImGui::IsItemHovered()) // https://github.com/ocornut/imgui/issues/5492
		{
			// Get the hovered pixel relative to the top left corner of the image, where the top left corner is 0,0 and bottom right corner is the size of the tile.
			// This is done by normalising the mouse position to 0-1 and then multiplying by the the selected tile's size.
			const Vector2<int> hoveredPixel = (Vector2<int>)Vector2<float>
				(
					(ImGui::GetMousePos().x - imagePosition.X) / atlasSize.X * tileSize.X,
					(ImGui::GetMousePos().y - imagePosition.Y) / atlasSize.Y * tileSize.Y
				);

			const Vector2<float> snappedPixel =
			{
				Snapping.X * round(hoveredPixel.X / (float)Snapping.X),
				Snapping.Y * round(hoveredPixel.Y / (float)Snapping.Y)
			};

			// Place
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				// Prevent placing points that can't form a line.
				if (collisionPoints.empty() || // If empty there's no possible wrong behaviour.
					collisionPoints.back() != snappedPixel // Prevent placing point at same position. No UB because empty checked first.
					// TODO: Prevent duplicate lines.
					)
				{
					collisionPoints.push_back(snappedPixel);
				}
			}

			// Erase.
			if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
			{
				// std::erase will reshuffle the vector to handle gaps,
				// which is more difficult to handle with an std::pair to represent edges.
				std::erase(collisionPoints, snappedPixel);
			}
		}

		// Input for fine tuning, and exact coordinates.
		for (size_t i = 0; i < collisionPoints.size(); i++)
		{
			Vector2<float>& point = collisionPoints[i];
			ImGui::PushID(i);
			ImGui::InputFloat2("", &point.X);
			point = Vector2<float>::Clamp(point, Vector2<float>::Zero(), tileSize);
			ImGui::PopID();
		}

		/* DISPLAY */
		const Vector2<float> imageScale = { atlasSize.X / tileSize.X, atlasSize.Y / tileSize.Y };

		// Display selected points.
		for (const auto& point : collisionPoints)
		{
			ImGui::GetWindowDrawList()->AddCircleFilled(Vector2{ point.X * imageScale.X, point.Y * imageScale.Y } + imagePosition, 4, IM_COL32(255, 255, 255, 255));
		}

		// Display lines between selected points.
		if (!collisionPoints.empty())
		{
			for (auto it = collisionPoints.cbegin(); it != collisionPoints.cend() - 1; ++it)
			{
				Vector2<float> point0 = Vector2<float>{ it->X * imageScale.X, it->Y * imageScale.Y } + imagePosition;
				Vector2<float> point1 = Vector2<float>{ (it + 1)->X * imageScale.X, (it + 1)->Y * imageScale.Y } + imagePosition;
				ImGui::GetWindowDrawList()->AddLine(point0, point1, IM_COL32(255, 255, 255, 255), 2);
			}
		}
	}

	template <typename T>
	void SetComponentUI(T& component, std::bitset<MAX_COMPONENTS>::reference isEnabled)
	{
		const std::string label = typeid(component).name(); // Result is implementation dependent, MSVC is clear and understandable.
		bool checked = isEnabled;

		ImGui::TableNextColumn();
		if (ImGui::Checkbox(("##" + label).c_str(), &checked))
		{
			isEnabled = checked; // Doesn't need to be within the if statement but it makes debugging easier.
		}

		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		{
			ImGui::SetTooltip("%s", label.c_str());
		}
	}

	// TODO: Pass in an object that encapsulates components and enabledComponents so that getters can be used to make handling undo/redo easier. Though that would mean relying on ImGui altering values via reference won't quite work...
	inline void ComponentsEditor(ComponentReferenceSlice& components, std::bitset<MAX_COMPONENTS>& enabledComponents)
	{
		ImGui::Begin("Components Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

		// TODO: Improve formatting, particularly with many components.
		// Could perhaps have a dropdown at the bottom for adding components, and collapsible for each component. Similar to Unity.

		static constexpr int maxTableColumn = 5;
		ImGui::BeginTable("ComponentTable", maxTableColumn);
		{
			std::apply([&enabledComponents]<typename... T>(T&... component) { (SetComponentUI<T>(component, enabledComponents[tuple_element_index_v<T, ComponentSlice>]), ...); }, components);
		}
		ImGui::EndTable();
		ImGui::Separator();

		std::apply([&components, &enabledComponents]<typename... T>(T&... component) { (ComponentEditor<T>(components, enabledComponents), ...); }, components);
		ImGui::End();
	}
}

