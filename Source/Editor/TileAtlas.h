#pragma once
#include <optional>

#include "../Maths/Vector2.h"
#include "Tile.h"
#include <vector>
#include <string>

namespace Engine
{
	class Texture;

	class TileAtlas
	{
	public:
		TileAtlas(std::string name, Vector2<int> tileSize);

		Texture* GetTexture();
		Vector2<int> GetSize();
		Vector2<int> GetTileSize();
		int GetTileCountX();
		int GetTileCountY();
		int GetTileCount();

		Tile& operator() (size_t x, size_t y);
		Tile operator() (size_t x, size_t y) const;

		std::optional<Vector2<int>> AtlasEditor(const std::optional<Vector2<int>>& previouslySelectedTile);

	private:
		std::string Name;
		Texture* CorrespondingTexture = nullptr; // Memory is managed elsewhere.
		Vector2<int> Size;
		Vector2<int> TileSize;
		int TileCountX;
		int TileCountY;	
		int TileCount;

		std::vector<Tile> Tiles; // 1D vector benefits from elements being contiguous in memory, unlike with a 2D vector.
	};
}