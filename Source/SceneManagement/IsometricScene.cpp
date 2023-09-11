#include "IsometricScene.h"
#include "../Core/Events.h"
#include "../Core/Renderer.h"
#include "../Maths/Vector2.h"
#include "../EntityComponentSystem/Entity.h"
#include "../EntityComponentSystem/EntityManager.h"
#include "../EntityComponentSystem/Systems/EditorSystem.h"
#include "../EntityComponentSystem/Systems/MovementSystem.h"
#include "Pathfinding/NavigationGraph.h"
#include <memory>
#include <SDL.h>
#include <format>

namespace Engine
{
	IsometricScene::IsometricScene() : ManagedNavigationGraph(NavigationGraph(*this))
	{
		// Base class constructor is called implicitly.
		// Initialiser lists copy construct, and because unique pointers can't be copy constructed need to add to the vector instead.
		Systems.emplace_back(std::make_unique<EditorSystem>(*this)); // TODO: Construct and destruct when editor is enabled.
		Systems.emplace_back(std::make_unique<MovementSystem>(*this));
	}

	IsometricScene::~IsometricScene()
	{
		SDL_Log("Destroying scene!");
	}

	void IsometricScene::Update(float deltaTime)
	{
		BaseScene::Update(deltaTime);

		{ // TODO: Remove. Temp camera movement for testing this should be in a system.
			float speed = 512.f;
			Velocity velocity;
			Events& events = Events::Instance();
			if (events.IsKeyDown(SDL_SCANCODE_D)) // Move Right
			{
				velocity += Vector2<float>::Right();
			}
			if (events.IsKeyDown(SDL_SCANCODE_A)) // Move Left
			{
				velocity -= Vector2<float>::Right();
			}
			if (events.IsKeyDown(SDL_SCANCODE_S)) // Move Down
			{
				velocity -= Vector2<float>::Up();
			}
			if (events.IsKeyDown(SDL_SCANCODE_W)) // Move Up
			{
				velocity += Vector2<float>::Up();
			}
			velocity.Normalise(); // Want to normalise so that diagonal movement isn't faster that a single direction.
			velocity *= speed * deltaTime;

			float zoomStep = 10.f;
			float zoom = events.GetMouseWheelY() / zoomStep;

			MainCamera.GetComponent<Velocity>() += velocity;
			MainCamera.GetComponent<Zoom>().Value = std::clamp(MainCamera.GetComponent<Zoom>().Value + zoom, 0.5f, 2.f);
		}
	}

	void IsometricScene::Render(Renderer& renderer)
	{
		const float zoom = MainCamera.GetComponent<Zoom>().Value;

		Texture& texture = renderer.GetTexture("DebugMap.jpg");
		Vector2<float> renderPosition = WorldSpaceToRenderSpace({ 0 - (512 / 2), 0 });
		Vector2<float> renderSize = { 512 * zoom, 256 * zoom };
		Vector2<int> sourcePosition = { 0,0 };
		Vector2<int> sourceSize = texture.GetSize();

		Rectangle<float> renderRectangle = { {renderPosition.X, renderPosition.Y}, {renderSize.X, renderSize.Y} };
		Rectangle<int> sourceRectangle = { {sourcePosition.X, sourcePosition.Y}, {sourceSize.X, sourceSize.Y} };

		renderer.RenderSprite(texture, sourceRectangle, renderRectangle);
		
		RenderGrid(renderer);
		RenderScene(renderer);
	}

	void IsometricScene::RenderScene(Renderer& renderer)
	{
		float zoom = MainCamera.GetComponent<Zoom>().Value;
		for (auto& entity : GetRenderableEntities()) // By handling sprite entities on the scene any special sorting logic can be handled.
		{
			Position& position = entity.GetComponent<Position>();
			Sprite& sprite = entity.GetComponent<Sprite>();
			if (sprite.SourceTexture == nullptr)
			{
				// This shouldn't happen unless the sprite component is incorrectly initialised/altered.
				// It needs the texture to be a pointer so that there's a default value for when the components 
				// are re-initialised.
				SDL_Log("Entity %lld has no texture in its sprite component!", entity.GetID());
				continue;
			}

			Texture& texture = *sprite.SourceTexture; // If an object is incorrectly initialised, this could be a nullptr.
			Vector2<float> renderPosition = WorldSpaceToRenderSpace(position - sprite.PivotOffset);
			Vector2<float> renderSize = (Vector2<float>)sprite.SourceRectangle.Size * zoom;

			Rectangle<float> renderRectangle = { {renderPosition.X, renderPosition.Y}, {renderSize.X, renderSize.Y} };

			renderer.RenderSprite(texture, sprite.SourceRectangle, renderRectangle);
		}
	}

	void IsometricScene::RenderGrid(Renderer& renderer)
	{
		// TODO: Optimise. What might be better is to render once to a surface that's retained between loops and is moved according to the camera.
		renderer.SetRenderColour(255, 0, 0, 255);
		float zoom = MainCamera.GetComponent<Zoom>().Value;
		const Vector2<float> screenSize = (Vector2<float>)Events::Instance().GetWindowSize();
		const Vector2<float> gridSpacing = (Vector2<float>)TileSize / 2; // Don't need to bother with zoom here, as it's handled in world space based on screen size.
		const Vector2<float> topLeftCorner = ScreenSpaceToGrid({ 0,0 }) - Vector2<float>(1, 0); // Go left across the grid a small value to hide snapping.

		// Calculate the needed number of cells to completely fill the diagonal.
		const Vector2<float> visibleWorld = screenSize / zoom; // TODO: Error handle zoom being <=0.
		const float gridCount = ceil(Vector2<float>(visibleWorld.X / gridSpacing.X, visibleWorld.Y / gridSpacing.Y).Length());

		// In isometric there's a pattern to how grid positions change as you go across, or down 
		// which can be used to get all the grid position visible on screen. 
		// This depends on the form of the isometric grid (Diamond vs Rectangle).
		// For diamond:
		// Across: topLeftCorner.X + i, topLeftCorner.Y - i
		// Down  : topLeftCorner.X + i, topLeftCorner.Y + i
		for (size_t i = 0; i < gridCount; i++)
		{
			// Across
			Vector2<float> world = GridToWorldSpace({ topLeftCorner.X + i, topLeftCorner.Y - i });
			Vector2<float> topEdge = WorldSpaceToRenderSpace(world);

			// Down
			world = GridToWorldSpace({ topLeftCorner.X + i, topLeftCorner.Y + i });
			Vector2<float> leftEdge = WorldSpaceToRenderSpace(world);

			// Get points along opposite edge and in reverse order.
			size_t j = (gridCount - 1 - i);
			// Across
			world = GridToWorldSpace({ topLeftCorner.X + j + gridCount - 1, topLeftCorner.Y - j + gridCount - 1 });
			Vector2<float> bottomEdge = WorldSpaceToRenderSpace(world);

			// Down
			world = GridToWorldSpace({ topLeftCorner.X + j + gridCount + 1, topLeftCorner.Y + j - gridCount + 1 });
			Vector2<float> rightEdge = WorldSpaceToRenderSpace(world);

			// SDL may end up using a considerable amount of memory and cause major performance hits depending on the size of the grid.
			renderer.RenderLine(topEdge, leftEdge);
			renderer.RenderLine(topEdge, rightEdge);
			if (i > 0) // Stop overlap in middle. Might not matter much if same colour, but just in case there's minor floating point errors.
			{
				renderer.RenderLine(bottomEdge, leftEdge);
			}
		}
	}

	void IsometricScene::SortEntities(std::vector<Entity>& entities)
	{
		// Basic isometric depth buffer that relies on all sprites being only a tile in size.
		// Because there's overlap in isometric scenes, one way to determine render order is by sorting by Y 
		// position so that entities closer to the top of the screen are rendered first. But because some
		// entities need to overlap they have a Z order requiring the entities to be sorted by Y position conditionally.

		// Sort by Z to get the vector into sections so that they correctly overlap one another.
		std::sort(entities.begin(), entities.end(),
			[](Entity a, Entity b) { return a.GetComponent<Position>().Z < b.GetComponent<Position>().Z; });

		// Sort each previous section as the transitions between Z layers are discovered.
		int lastZOrderChange = 0;
		for (size_t i = 1; i < entities.size(); ++i)
		{
			// This won't trigger for the final section.
			if (entities[i-1].GetComponent<Position>().Z != entities[i].GetComponent<Position>().Z)
			{
				std::sort(entities.begin() + lastZOrderChange, entities.begin() + i,
					[](Entity a, Entity b) { return a.GetComponent<Position>().Y < b.GetComponent<Position>().Y; });
				lastZOrderChange = i;
			}
		}

		// Sort the final section.
		std::sort(entities.begin() + lastZOrderChange, entities.end(),
			[](Entity a, Entity b) { return a.GetComponent<Position>().Y < b.GetComponent<Position>().Y; });
	}

	std::vector<Entity> IsometricScene::GetRenderableEntities()
	{
		// Convert screen dimensions to visible world dimensions for entity culling.
		// Doing culling in world space means only one calculation need to be done each frame to determine visible entities, 
		// with the render position for each entity being calculated in the renderer when its needed.
		// The advantage of doing this is that only the world position needs to be stored on entities.
		const Vector2<float> lowerBound = ScreenSpaceToWorldSpace({0.f, 0.f});
		const Vector2<float> upperBound = ScreenSpaceToWorldSpace((Vector2<float>)Events::Instance().GetWindowSize());

		EntityManager& entityManager = GetEntityManager();
		std::vector<Entity> renderableEntities;
		for (auto& entity : entityManager.GetEntities())
		{
			if (!entity.HasComponents<Position, Sprite>()) { continue; }

			Position& position = entity.GetComponent<Position>();
			if (position.X > lowerBound.X && position.X < upperBound.X
				&& position.Y > lowerBound.Y && position.Y < upperBound.Y)
			{
				renderableEntities.emplace_back(entity);
			}
		}

		SortEntities(renderableEntities);

		return renderableEntities;
	}

	void IsometricScene::SetTileSize(int width, int height)
	{
		TileSize = { width, height };
	}

	Vector2<float> IsometricScene::ScreenSpaceToGrid(Vector2<float> screen, bool floor) const
	{
		Vector2<float> world = ScreenSpaceToWorldSpace(screen); // So camera offset and zoom is taken into account.

		float tileHeight = (TileSize.Y / 2);
		float tileWidth = TileSize.X;

		Vector2<float> grid = Vector2<float>(
			world.X / tileWidth + world.Y / tileHeight,
			world.Y / tileHeight - world.X / tileWidth
		);

		if (floor)
		{
			grid.X = floorf(grid.X);
			grid.Y = floorf(grid.Y);
		}

		return grid;
	}

	Vector2<float> IsometricScene::GridToWorldSpace(Vector2<float> grid) const
	{
		return
		{
			(grid.X - grid.Y) * (TileSize.X / 2), 
			(grid.X + grid.Y) * (TileSize.Y / 4)
		};
	}
}