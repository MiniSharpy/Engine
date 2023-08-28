#pragma once
#include "BaseScene.h"
#include "IGrid.h"
#include "../Maths/Vector2.h"
#include "../EntityComponentSystem/Entity.h"
#include "../Pathfinding/NavigationGraph.h"


namespace Engine
{
	class Renderer;

	class IsometricScene : public BaseScene, public IGrid
	{
	public:
		/// <summary>
		/// Scenes should never be constructed manually, only through scene manager! 
		/// See SceneManager.h for explanation.
		/// </summary>
		IsometricScene();
		~IsometricScene() override;

		NavigationGraph ManagedNavigationGraph;


		// There are two ways to store tile size. The first is currently used. TODO: Convert to the second?
		// 1. Full tile size and division in calculation to get the base of the tile.
		// 2. Just the base of the tile size, already divided.
		Vector2<int> TileSize = { 128, 128 };

		void Update(float deltaTime) override;
		void Render(Renderer& renderer) override;
		void RenderScene(Renderer& renderer);
		void RenderGrid(Renderer& renderer);

		static void SortEntities(std::vector<Entity>& entities);

		std::vector<Entity> GetRenderableEntities();
		void SetTileSize(int width, int height);

		Vector2<float> ScreenSpaceToGrid(Vector2<float> screen, bool floor = true) const override;
		Vector2<float> GridToWorldSpace(Vector2<float> grid) const override;
	};
}
