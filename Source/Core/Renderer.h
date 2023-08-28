#pragma once
struct SDL_Window;
struct SDL_Renderer;
#include "Texture.h"
#include "../Maths/Vector2.h"
#include "../Maths/Rectangle.h"
#include <string>
#include <array>
#include <unordered_map>
#include<SDL.h>

namespace Engine
{
	class BaseScene;
	class Events;

	class Renderer
	{
	public:
		static Renderer& Instance();

	private:
		Renderer(SDL_Window* window);
		~Renderer();
		std::unordered_map<std::string, Texture> Textures; // std::unordered_map has faster look ups than std::map and order doesn't matter.
		std::array<SDL_Rect, 128> Glyphs; // ASCII.

		/// <summary>
		/// Construct a font object,
		/// </summary>
		void InitialiseFont(std::string fileName);

	public:
		// https://en.cppreference.com/w/cpp/language/rule_of_three
		Renderer(const Renderer&) = delete; // Copy Constructor
		Renderer& operator=(const Renderer&) = delete; // Copy Assignment

		void SetVSync(bool value);
		void Render(BaseScene& scene);
		void RenderSprite(Texture& texture, Rectangle<int> sourceRectangle, Rectangle<float> renderRectangle);
		void RenderLine(Vector2<float> position1, Vector2<float> position2);
		void RenderText(std::string font, std::string string, Vector2<float> renderPosition, float scale); // TODO: Make font an enum to prevent invalid enums.
		void SetRenderColour(int red, int green, int blue, int alpha);

		Texture& GetTexture(std::string fileName);

		operator SDL_Renderer* () { return ManagedRenderer; } // Returns native renderer when passed into a SDL_Renderer * paramater.

	private:
		SDL_Renderer* ManagedRenderer;
	};
}