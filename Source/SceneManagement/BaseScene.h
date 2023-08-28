#pragma once
#include "../Core/Events.h"
#include"../Maths/Vector2.h"
#include "../EntityComponentSystem/Entity.h"
#include "../EntityComponentSystem/EntityManager.h"
#include "../EntityComponentSystem/Systems/BaseSystem.h"
#include <memory>


namespace Engine
{
	class Renderer;

	class BaseScene
	{
	protected:
		EntityManager ManagedEntityManager;
		std::vector<std::unique_ptr<BaseSystem>> Systems;

	public:
		Entity MainCamera;

		BaseScene() : MainCamera{ ManagedEntityManager.AddEntity("Camera") }
		{
			MainCamera.AddComponent<Position>();
			MainCamera.AddComponent<Velocity>();
			MainCamera.AddComponent<Zoom>();
			MainCamera.GetComponent<Zoom>().Value = 1;
		}
		virtual ~BaseScene() = default;

		virtual void Update(float deltaTime) 
		{
			ManagedEntityManager.Update();

			for (const auto& system : Systems)
			{
				system->Update(deltaTime);
			}
		}
		virtual void Render(Renderer& renderer) = 0;
		EntityManager& GetEntityManager() { return ManagedEntityManager; }

		/// <summary>
		/// Converts screen space coordinates (pixels) to world space coordinates (pixels), taking into account camera offset and zoom.
		/// </summary>
		/// <remarks>
		/// SDL has a function called SDL_RenderSetScale which would ordinarily negate accounting for zoom, however it #
		/// seems to have an unintended effect on Dear ImGui in that the input doesn't account for mouse scaling.
		/// Handling scaling on the engine end does mean there's more control however.
		/// </remarks>
		Vector2<float> ScreenSpaceToWorldSpace(Vector2<float> screen) const
		{
			// Do the opposite of WorldSpaceToRenderSpace!
			Vector2<float> world = ((screen - (Vector2<float>)Events::Instance().GetWindowSize() / 2) // Account for centred screen...
				/ MainCamera.GetComponent<Zoom>().Value) // ... zoom...
				+ MainCamera.GetComponent<Position>(); // and camera position. 
			return world;
		}

		/// <summary>
		/// Converts world space coordinates (pixels) to render space coordinates (pixels), taking into account camera offset and zoom.
		/// </summary>
		/// <remarks>
		/// SDL has a function called SDL_RenderSetScale which would ordinarily negate accounting for zoom, however it #
		/// seems to have an unintended effect on Dear ImGui in that the input doesn't account for mouse scaling.
		/// Handling scaling on the engine end does mean there's more control however.
		/// </remarks>
		virtual Vector2<float> WorldSpaceToRenderSpace(Vector2<float> world) const
		{
			// Do the opposite of ScreenSpaceToWorldSpace!
			Vector2<float> render = (world - MainCamera.GetComponent<Position>()) // Account for camera position...
				* MainCamera.GetComponent<Zoom>().Value  // ... zoom...
				+ (Vector2<float>)Events::Instance().GetWindowSize() / 2; // ... and centred screen. 
			return render;
		}
	};
}
