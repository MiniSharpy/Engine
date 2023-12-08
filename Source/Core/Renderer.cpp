#include "Renderer.h"
#include "Window.h"
#include "Surface.h"
#include "Texture.h"
#include "Font.h"
#include "../SceneManagement/BaseScene.h"
#include "../Maths/Vector2.h"
#include "../Maths/Rectangle.h"
#include <string>
#include <map>
#include <format>
#include <bit>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace Engine
{
	Renderer& Renderer::Instance()
	{
		static Renderer instance(Window::Instance());
		return instance;
	}

	Renderer::Renderer(SDL_Window* window) : Glyphs{}
	{
		SDL_Log("Renderer Initialisation!");

		// SDL_RENDERER_ACCELERATED flag will enforce hardware acceleration, failing if unavailable. By default the SDL renderer will try to use hardware, but fall back to software if unavailable.
		ManagedRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
		if (!ManagedRenderer)
		{
			SDL_Log("Error: %s\n", SDL_GetError());
			exit(-1);
		}

		// Setup Platform/Renderer backends.
		ImGui_ImplSDL2_InitForSDLRenderer(window, ManagedRenderer);
		ImGui_ImplSDLRenderer2_Init(ManagedRenderer);

		SDL_RendererInfo info;
		SDL_GetRendererInfo(ManagedRenderer, &info);
		SDL_Log("Current SDL_Renderer: %s", info.name);

		// Set up text rendering.
		InitialiseFont("OpenSans.ttf");
	}

	Renderer::~Renderer()
	{
		Textures.clear(); // Destroy textures before renderer to prevent dangling pointers in Texture instances.
		SDL_Log("Destroying renderer and Dear ImGui links!");
		SDL_DestroyRenderer(ManagedRenderer); // Destroying the renderer will also destroy associated textures.
		ImGui_ImplSDLRenderer2_Shutdown();
		ImGui_ImplSDL2_Shutdown();
	}

	// https://www.parallelrealities.co.uk/tutorials/ttf/ttf2.php
	void Renderer::InitialiseFont(std::string fileName)
	{
		int pointSize = 64;
		Font font = Font(fileName, pointSize);
		Vector2<int> atlasDimensions;
		Surface text;

		// Calculate the maximum pixel count to fill a tile atlas, and set the dimensions accordingly.
		int pixelCount = 0;
		for (char character = ' '; character <= '~'; character++) // Every printable ASCII character.
		{
			Vector2<int> characterSize;
			std::string characterWithNullTerminator = { character }; // TTF functions need the null terminator to know when to end.
			text = TTF_RenderUTF8_Blended(font, characterWithNullTerminator.c_str(), { 255, 255, 255, 255 });
			TTF_SizeUTF8(font, characterWithNullTerminator.c_str(), &characterSize.X, &characterSize.Y);
			pixelCount += characterSize.X * characterSize.Y;
		}
		unsigned minimumSquaredDimensions = sqrtf(pixelCount); // This is not neccesarily a power of two. TODO: Does it even need to be?
		atlasDimensions = { (int)std::bit_floor(minimumSquaredDimensions), (int)std::bit_ceil(minimumSquaredDimensions) };
		if (atlasDimensions.X * atlasDimensions.Y < pixelCount) // If the smallest power of two option doesn't work, go with the larger size.
		{
			atlasDimensions = { (int)std::bit_ceil(minimumSquaredDimensions), (int)std::bit_ceil(minimumSquaredDimensions) };
		}

		// Render each ASCII character to a surface.
		Surface atlas = SDL_CreateRGBSurface(0, atlasDimensions.X, atlasDimensions.Y, 32, 0, 0, 0, 0xff);
		SDL_SetColorKey(atlas, SDL_TRUE, SDL_MapRGBA(atlas.GetPixelFormat(), 0, 0, 0, 0)); // Blit with transparency.
		SDL_Rect destination = { 0, 0, 0, 0 };
		for (char character = ' '; character <= '~'; character++)  // Every printable ASCII character.
		{
			std::string characterWithNullTerminator = { character }; // TTF functions need the null terminator to know when to end.
			text = TTF_RenderUTF8_Blended(font, characterWithNullTerminator.c_str(), { 255, 255, 255, 255 });

			// Fit text onto atlas
			TTF_SizeUTF8(font, characterWithNullTerminator.c_str(), &destination.w, &destination.h); // Height seems to be the same, but width isn't consistent.
			if (destination.x + destination.w >= atlasDimensions.X)
			{
				destination.x = 0;
				destination.y += destination.h; // Will this intersect if height are different? There seems to be some leeway any way.
				if (destination.y + destination.h >= atlasDimensions.Y)
				{
					// Normally want to avoid exceptions, but this shouldn't happen and is done on startup so it's probably fine. At least for now, maybe in the future this won't be done on startup. TODO: Non-startup error handling.
					throw std::length_error(std::format("Character {} does not fit in font atlas of size {}, {}", character, atlasDimensions.X, atlasDimensions.Y));
				}
			}

			SDL_BlitSurface(text, NULL, atlas, &destination);

			Glyphs[character] = destination;
			destination.x += destination.w;
		}

		Textures[fileName] = SDL_CreateTextureFromSurface(ManagedRenderer, atlas);
	}

	void Renderer::SetVSync(bool value)
	{
		SDL_RenderSetVSync(ManagedRenderer, value);
	}

	void Renderer::Render(BaseScene& scene)
	{
		// Prepare new frame.
		SDL_SetRenderDrawColor(ManagedRenderer, 0, 0, 0, 255);
		SDL_RenderClear(ManagedRenderer);

		// Render world.
		scene.Render(*this);
		// RenderSprite(GetTexture("OpenSans.ttf"), { 0,0 }, (Vector2<float>)GetTexture("OpenSans.ttf").GetSize(), { 0, 0 }, GetTexture("OpenSans.ttf").GetSize());
		// RenderText("OpenSans.ttf", "TEST!", { 0, 0 }, 1);

		// Render GUI.
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

		// Update screen with new frame.
		SDL_RenderPresent(ManagedRenderer);
	}

	void Renderer::RenderSprite(Texture& texture, Rectangle<int> sourceRectangle, Rectangle<float> renderRectangle)
	{
		// I feel clever for realising I can do this, but feel like I'm inviting disaster.
		SDL_RenderCopyF(ManagedRenderer, texture, reinterpret_cast<SDL_Rect*>(&sourceRectangle), reinterpret_cast<SDL_FRect*>(&renderRectangle));
	}

	void Renderer::RenderLine(Vector2<float> position1, Vector2<float> position2)
	{
		SDL_RenderDrawLineF(ManagedRenderer,
			position1.X, position1.Y,
			position2.X, position2.Y);
	}

	void Renderer::RenderText(std::string font, std::string string, Vector2<float> renderPosition, float scale)
	{
		Vector2<float> positionOffset;
		for (auto& character : string)
		{
			SDL_Rect source = Glyphs[character];
			Vector2<float> scaledSize = { source.w * scale, source.h * scale };

			SDL_FRect destination = { renderPosition.X + positionOffset.X, renderPosition.Y + positionOffset.Y, scaledSize.X, scaledSize.Y };
			SDL_RenderCopyF(ManagedRenderer, Textures[font], &source, &destination);
			positionOffset.X += scaledSize.X;
		}
	}

	void Renderer::SetRenderColour(int red, int green, int blue, int alpha)
	{
		SDL_SetRenderDrawColor(ManagedRenderer, red, green, blue, alpha);
	}

	Texture& Renderer::GetTexture(std::string fileName)
	{
		if (Textures.count(fileName) == 0) // Prevents unnecessary construction of existing texture.
		{
			Textures.try_emplace(fileName, Texture(ManagedRenderer, fileName));
		}

		return Textures[fileName];
	}
}