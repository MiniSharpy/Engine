#include "TileAtlas.h"
#include "../Core/Renderer.h"
#include "../Core/Texture.h"
#include <imgui.h>
#include <optional>

namespace Engine
{
	TileAtlas::TileAtlas(std::string name, Vector2<int> tileSize) :
		Name(name),
		CorrespondingTexture(&Renderer::Instance().GetTexture(Name)),
		Size(CorrespondingTexture->GetSize()),
		TileSize(tileSize),
		TileCountX(Size.X / TileSize.X),
		TileCountY(Size.Y / TileSize.Y),
		TileCount(TileCountX * TileCountY)
	{
		for (int y = 0; y < TileCountY; y++)
		{
			for (int x = 0; x < TileCountX; x++)
			{

				Tiles.push_back(Tile(*this, { x, y }));
			}
		}
	}

	Texture* TileAtlas::GetTexture()
	{
		return CorrespondingTexture;
	}

	Vector2<int> TileAtlas::GetSize()
	{
		return Size;
	}

	Vector2<int> TileAtlas::GetTileSize()
	{
		return TileSize;
	}

	int TileAtlas::GetTileCountX()
	{
		return TileCountX;
	}

	int TileAtlas::GetTileCountY()
	{
		return TileCountY;
	}

	int TileAtlas::GetTileCount()
	{
		return TileCount;
	}

	Tile& TileAtlas::operator()(size_t x, size_t y)
	{
		return Tiles[TileCountX * y + x];
	}

	Tile TileAtlas::operator()(size_t x, size_t y) const
	{
		return Tiles[TileCountX * y + x];
	}

	std::optional<Vector2<int>> TileAtlas::AtlasEditor(const std::optional<Vector2<int>>& previouslySelectedTile)
	{
		std::optional<Vector2<int>> clickedTile = {};

		ImGui::Text("Pointer = %p", (void*)CorrespondingTexture);
		ImGui::Text("Texture Size = %d x %d", Size.X, Size.Y);

		for (int i = 0; i < TileCount; i++)
		{
			const Vector2<int> currentTile = 
				{ i % TileCountX,
				i / TileCountX }; // Will truncate to the correct index. E.G in a 4 column tileset: tile 3 is 0.75 which becomes 0, tile 4 is 1, tile 5 is 1.25 which becomes 1.
				

			ImGui::PushID(i); // As long all the pushed ID are popped, the id can be reused in a different scope. Though, conflicts could arise if called in the middle of another loop that's setting ID.
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			const Vector2<float> uvTopLeft(currentTile.X / (float)TileCountX, currentTile.Y / (float)TileCountY);
			const Vector2<float> uvBottomRight((currentTile.X + 1) / (float)TileCountX, (currentTile.Y + 1) / (float)TileCountY);


			const ImVec4 tintColour = (previouslySelectedTile == currentTile) ? ImVec4{1, 1, 1, 1} : ImVec4{ 1, 1, 1, 0.25 };

			// If the tile atlas is split into too small chunks this can cause some serious slow downs.
			if (ImGui::ImageButton(*CorrespondingTexture, (Vector2<float>)TileSize, uvTopLeft, uvBottomRight, 0, {}, tintColour)) // Might be more efficient to have one image with overlapping invisible buttons? Only want a miniscule amount of padding so we know what tile's which.
			{
				clickedTile = currentTile;
			}
			if (currentTile.X != TileCountX - 1) // If not new row.
			{
				ImGui::SameLine();
			}
			ImGui::PopStyleVar();
			ImGui::PopID();
		}

		return clickedTile;
	}

}

