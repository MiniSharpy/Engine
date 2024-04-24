#include "IsometricScene.h"
#include "../Core/Events.h"
#include "../Core/Renderer.h"
#include "../Maths/Vector2.h"
#include "../EntityComponentSystem/Entity.h"
#include "../EntityComponentSystem/EntityManager.h"
#include "../EntityComponentSystem/Systems/EditorSystem.h"
#include "../EntityComponentSystem/Systems/MovementSystem.h"
#include "../EntityComponentSystem/Systems/AnimationSystem.h"
#include "../EntityComponentSystem/Systems/PathfindingSystem.h"
#include "../Input/Action.h"
#include "../Input/Modifiers/NegateModifier.h"
#include "../Input/Modifiers/SwizzleModifier.h"
#include "../Input/Modifiers/DeadZoneModifier.h"
#include "../Input/Modifiers/ScalarModifier.h"
#include "../Input/Modifiers/DeltaTimeModifier.h"
#include "../Input/Conditions/PressedCondition.h"
#include "../Input/Conditions/ReleasedCondition.h"
#include "../Pathfinding/NavigationGraph.h"
#include <execution>
#include <memory>
#include <numbers>
#include <SDL.h>
#include <format>

namespace Engine
{
	IsometricScene::IsometricScene(const float& deltaTime) : BaseScene(deltaTime), ManagedNavigationGraph(NavigationGraph(*this)), Editor(std::make_unique<EditorSystem>(*this))
	{
		// Base class constructor is called implicitly.
		// Initialiser lists copy construct, and because unique pointers can't be copy constructed need to add to the vector instead.
		Systems.emplace_back(std::make_unique<MovementSystem>(*this));
		Systems.emplace_back(std::make_unique<AnimationSystem>(*this));
		Systems.emplace_back(std::make_unique<PathfindingSystem>(*this));

		// Player Character
		Entity player = GetEntityManager().AddEntity("Player");
		player.AddComponent<Position>();
		player.AddComponent<Velocity>();
		player.AddComponent<Animation>();
		player.AddComponent<Pathfinding>();
		player.AddComponent<Sprite>();

		Sprite& sprite = player.GetComponent<Sprite>();
		strcpy(sprite.TextureName, "AnimationSheet.png");
		sprite.SourceRectangle = { {}, TileSize};
		sprite.PivotOffset = { static_cast<float>(TileSize.X) / 2.f, static_cast<float>(TileSize.Y) / 1.5f };

		// Inputs
		// Input Behaviour
		std::function zoomBehaviour = [this](float value)
		{
			constexpr float zoomStep = 10.f;
			float zoom = value / zoomStep;
			MainCamera.GetComponent<Zoom>().Value = std::clamp(MainCamera.GetComponent<Zoom>().Value + zoom, 0.5f, 2.f);
		};

		std::function moveBehaviour = [this](Vector2<float> value)
		{
			// No need to constrain value as direction gets normalised anyway.
			Velocity& velocity = MainCamera.GetComponent<Velocity>();
			velocity.Direction = value;

			// Need to clamp to prevent moving faster diagonally, while still allowing for joystick sensitivity be accounted
			// for.
			velocity.Speed = std::clamp(value.Length() * 512, 0.f, 512.f); // 512 would be max speed.
		};

		std::function moveReleaseBehaviour = [this]()
		{
			MainCamera.GetComponent<Velocity>().Speed = 0;
		};

		std::function mouseBehaviour = [this](Vector2<float> value)
		{
			if (!Events::Instance().IsKeyDown(SDL_SCANCODE_LCTRL)) { return; }

			const Vector2<float> goal = ScreenSpaceToGrid(ImGui::GetMousePos());

			Pathfinding& pathfinding = GetEntityManager().GetEntitiesByTag("Player")[0].GetComponent<Pathfinding>();
			pathfinding.Current = {};
			pathfinding.Goal = goal;
		};

		std::function editorBehaviour = [this]()
		{
			Editor->IsEnabled = !Editor->IsEnabled;
		};

		std::function saveBehaviour = [this]()
		{
			GetEntityManager().Save("Test");
		};

		std::function loadBehaviour = [this]()
		{
			ManagedEntityManager = EntityManager();
			ManagedEntityManager.Load("Test");
			MainCamera = ManagedEntityManager.GetEntitiesByTag("Camera")[0];
		};

		// Input Binding
		// Camera Zoom.
		Action<float>& zoomAction = InputManager.AddAction(zoomBehaviour);
		zoomAction.BindInput("Mouse Wheel Y");
		// Order is important with dead zone, it should go first so that it acts on the unaltered value.
		// Really it should probably be altered to only react to the raw value.
		zoomAction.BindInput<DeadZoneModifier, ScalarModifier>(SDL_GameControllerGetStringForAxis(SDL_CONTROLLER_AXIS_RIGHTY));
		zoomAction.GetInput(SDL_GameControllerGetStringForAxis(SDL_CONTROLLER_AXIS_RIGHTY))
			.AddModifier<DeltaTimeModifier>(deltaTime);
		zoomAction.BindInput<ScalarModifier>(SDL_GetScancodeName(SDL_SCANCODE_UP));
		zoomAction.GetInput(SDL_GetScancodeName(SDL_SCANCODE_UP)).AddModifier<DeltaTimeModifier>(deltaTime);
		zoomAction.BindInput<NegateModifier, ScalarModifier>(
			SDL_GetScancodeName(SDL_SCANCODE_DOWN));
		zoomAction.GetInput(SDL_GetScancodeName(SDL_SCANCODE_DOWN)).AddModifier<DeltaTimeModifier>(deltaTime);

		// Camera Movement. This will get normalised so combining inputs won't make you faster.
		Action<Vector2<float>>& moveAction = InputManager.AddAction(moveBehaviour, true);
		moveAction.BindInput<SwizzleModifier, NegateModifier>(SDL_GetScancodeName(SDL_SCANCODE_W));
		moveAction.BindInput<SwizzleModifier>(SDL_GetScancodeName(SDL_SCANCODE_S));
		moveAction.BindInput(SDL_GetScancodeName(SDL_SCANCODE_D));
		moveAction.BindInput<NegateModifier>(SDL_GetScancodeName(SDL_SCANCODE_A));
		moveAction.BindInput<SwizzleModifier, DeadZoneModifier>(
			SDL_GameControllerGetStringForAxis(SDL_CONTROLLER_AXIS_LEFTY));
		moveAction.BindInput<DeadZoneModifier>(SDL_GameControllerGetStringForAxis(SDL_CONTROLLER_AXIS_LEFTX));

		Action<>& moveReleaseAction = InputManager.AddAction(moveReleaseBehaviour);
		moveReleaseAction.BindInput(SDL_GetScancodeName(SDL_SCANCODE_W));
		moveReleaseAction.BindInput(SDL_GetScancodeName(SDL_SCANCODE_S));
		moveReleaseAction.BindInput(SDL_GetScancodeName(SDL_SCANCODE_D));
		moveReleaseAction.BindInput(SDL_GetScancodeName(SDL_SCANCODE_A));
		moveReleaseAction.GetInput(SDL_GetScancodeName(SDL_SCANCODE_W)).AddCondition<ReleasedCondition>();
		moveReleaseAction.GetInput(SDL_GetScancodeName(SDL_SCANCODE_S)).AddCondition<ReleasedCondition>();
		moveReleaseAction.GetInput(SDL_GetScancodeName(SDL_SCANCODE_D)).AddCondition<ReleasedCondition>();
		moveReleaseAction.GetInput(SDL_GetScancodeName(SDL_SCANCODE_A)).AddCondition<ReleasedCondition>();

		// Character Movement
		Action<Vector2<float>>& mouseAction = InputManager.AddAction(mouseBehaviour);
		mouseAction.BindInput("Mouse Button Left");
		mouseAction.GetInput("Mouse Button Left").AddCondition<PressedCondition>();

		// Editor Toggle
		Action<>& editorAction = InputManager.AddAction(editorBehaviour);
		editorAction.BindInput(SDL_GetScancodeName(SDL_SCANCODE_GRAVE));
		editorAction.GetInput(SDL_GetScancodeName(SDL_SCANCODE_GRAVE)).AddCondition<PressedCondition>();

		// Save/Load test
		Action<>& saveAction = InputManager.AddAction(saveBehaviour);
		saveAction.BindInput(SDL_GetScancodeName(SDL_SCANCODE_0));

		Action<>& loadAction = InputManager.AddAction(loadBehaviour);
		loadAction.BindInput(SDL_GetScancodeName(SDL_SCANCODE_9));
	}

	IsometricScene::~IsometricScene()
	{
		SDL_Log("Destroying scene!");
	}

	void IsometricScene::Update(const float& deltaTime)
	{
		BaseScene::Update(deltaTime);
		Editor->Update(deltaTime);
	}

	void IsometricScene::Render(Renderer& renderer)
	{
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
			if (sprite.TextureName == "\0")
			{
				// This shouldn't happen unless the sprite component is incorrectly initialised/altered.
				// It needs the texture to be a pointer so that there's a default value for when the components 
				// are re-initialised.
				SDL_Log("Entity %lld has no texture in its sprite component!", entity.GetID()); // TODO: This is tremendously annoying, use an error texture.
				continue;
			}

			Texture& texture = Renderer::Instance().GetTexture(sprite.TextureName);
			Vector2<float> renderPosition = WorldSpaceToRenderSpace(position - sprite.PivotOffset);
			Vector2<float> renderSize = (Vector2<float>)sprite.SourceRectangle.Size * zoom;

			Rectangle<float> renderRectangle = { {renderPosition.X, renderPosition.Y}, {renderSize.X, renderSize.Y} };

			renderer.RenderSprite(texture, sprite.SourceRectangle, renderRectangle);
		}
	}

	void IsometricScene::RenderGrid(Renderer& renderer)
	{
		if (!Editor->IsEnabled) { return; }

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
		std::sort(std::execution::par_unseq, entities.begin(), entities.end(),
			[](Entity a, Entity b) { return a.GetComponent<Position>().Z < b.GetComponent<Position>().Z; });

		// Sort each previous section as the transitions between Z layers are discovered.
		int lastZOrderChange = 0;
		for (size_t i = 1; i < entities.size(); ++i)
		{
			// This won't trigger for the final section.
			if (entities[i-1].GetComponent<Position>().Z != entities[i].GetComponent<Position>().Z)
			{
				std::sort(std::execution::par_unseq, entities.begin() + lastZOrderChange, entities.begin() + i,
					[](Entity a, Entity b) { return a.GetComponent<Position>().Y < b.GetComponent<Position>().Y; });
				lastZOrderChange = i;
			}
		}

		// Sort the final section.
		std::sort(std::execution::par_unseq, entities.begin() + lastZOrderChange, entities.end(),
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
		return WorldSpaceToGrid(world, floor);
	}

	Vector2<float> IsometricScene::WorldSpaceToGrid(Vector2<float> world, bool floor) const
	{
		float tileHeight = (TileSize.Y / 2.f);
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