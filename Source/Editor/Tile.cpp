#include "Tile.h"
#include "TileAtlas.h"
#include "EntityComponentSystem/TupleHelper.h"
#include <imgui.h>

namespace Engine
{
	Tile::Tile(TileAtlas& atlas, Vector2<int> index) :
		Index(index), Size(atlas.GetTileSize())
	{
		AddComponent<Position>();
		AddComponent<Sprite>();

		Sprite& sprite = std::get<Sprite>(Components);
		sprite.SourceTexture = atlas.GetTexture();
		sprite.SourceRectangle = { {Index.X * Size.X, Index.Y * Size.Y}, Size };
		sprite.PivotOffset = (Vector2<float>)Size / 2.f;

		MandatoryComponents = EnabledComponents;
	}

	void Tile::TileEditor()
	{
		ImGui::Begin("Tile Editor", 0, ImGuiWindowFlags_AlwaysAutoResize);

		if (ImGui::BeginTabBar("MyTabBar"))
		{
			if (ImGui::BeginTabItem("AddComponents"))
			{
				ImGui::BeginTable("ComponentTable", MaxTableColumn);
				{
					std::apply([this]<typename... T>(T&... args) { (SetComponentUI<T>(), ...); }, Components);
				}
				ImGui::EndTable();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("AlterComponents"))
			{
				std::apply([this]<typename... T>(T&... args) { ( ComponentEditor<T>(), ...); }, Components);
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		ImGui::End();
	}

	template <>
	void Tile::ComponentEditor<Position>()
	{
		if (!EnabledComponents[tuple_element_index_v<Position, ComponentSlice>]) { return; }

		int& zOrder = std::get<Position>(Components).Z;
		ImGui::InputInt("ZOrder", &zOrder);
	}


	template<>
	void Tile::ComponentEditor<Collider>()
	{
		// TODO: Draw a grid representing the snapping increments.
		// TODO: Multiple unconnected line segment, currently the assumption is they'll all want to be connected to one another.
		if (!EnabledComponents[tuple_element_index_v<Collider, ComponentSlice>]) { return; }

		Texture* atlasTexture = std::get<Sprite>(Components).SourceTexture;
		std::vector<Vector2<float>>& collisionPoints = std::get<Collider>(Components).Points;

		/* INPUT */
		// Snapping choice
		ImGui::InputInt2("Snapping", &Snapping.X); // This relies on the order of Vector2 being X, Y.
		Snapping = Vector2<int>::Clamp(Snapping, { 1, 1 }, Size / 4); // TODO: Lock values to each other with optional button to disable.

		// Collision Input.
		const Vector2<float> atlasSize = (Vector2<float>)atlasTexture->GetSize();
		const Vector2<int>& tilePosition = { Index.X * Size.X, Index.Y * Size.Y };
		const Vector2<float> uvTopLeft = { tilePosition.X / atlasSize.X, tilePosition.Y / atlasSize.Y };
		const Vector2<float> uvBottomRight = { (tilePosition.X + Size.X) / atlasSize.X, (tilePosition.Y + Size.Y) / atlasSize.Y };
		ImGui::Image(*atlasTexture, atlasSize, uvTopLeft, uvBottomRight);

		Vector2<float> imagePosition = (Vector2<float>)ImGui::GetItemRectMin();
		if (ImGui::IsItemHovered()) // https://github.com/ocornut/imgui/issues/5492
		{
			// Get the hovered pixel relative to the top left corner of the image where the top left corner is 0,0 and bottom right corner is the size of the tile.
			// This is done by normalising the mouse position to 0-1 and then multiplying by the the selected tile's size.
			const Vector2<int> hoveredPixel = (Vector2<int>)Vector2<float>
				(
					(ImGui::GetMousePos().x - imagePosition.X) / atlasSize.X * (float)Size.X,
					(ImGui::GetMousePos().y - imagePosition.Y) / atlasSize.Y * (float)Size.Y
				);

			// Snapping
			const Vector2<float> snappedPixel =
			{
				Snapping.X * round(hoveredPixel.X / (float)Snapping.X),
				Snapping.Y * round(hoveredPixel.Y / (float)Snapping.Y)
			};

			// Place.
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
			point = Vector2<float>::Clamp(point, Vector2<float>::Zero(), (Vector2<float>)Size);
			ImGui::PopID();
		}

		/* DISPLAY */
		const Vector2<float> imageScale = { atlasSize.X / Size.X, atlasSize.Y / Size.Y };

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
}
