#include "Tile.h"
#include "TileAtlas.h"
#include "EntityComponentSystem/TupleHelper.h"
#include "ComponentEditor.h"
#include <imgui.h>

namespace Engine
{
	Tile::Tile(TileAtlas& atlas, Vector2<int> index)
	{
		AddComponent<Position>();
		AddComponent<Sprite>();

		Vector2<int> size = atlas.GetTileSize();

		Sprite& sprite = std::get<Sprite>(ComponentSliceData);
		strcpy(sprite.TextureName, atlas.GetName().c_str());
		sprite.SourceRectangle = { {index.X * size.X, index.Y * size.Y}, size };
		sprite.PivotOffset = (Vector2<float>)size / 2.f;
	}
}
