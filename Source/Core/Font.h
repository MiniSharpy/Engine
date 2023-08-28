#pragma once
#include<string>
//struct TTF_Font;
#include <SDL_ttf.h>

namespace Engine
{
	class Font
	{
	public:
		Font(std::string name, int pointSize);
		~Font();

		Font(const Font& other) = delete;
		Font(Font&& other) noexcept;
		Font& operator=(const Font& other) = delete;
		Font& operator=(Font&& other) noexcept;

		operator TTF_Font* () { return ManagedFont; }
		operator TTF_Font* () const { return ManagedFont; }

	private:
		TTF_Font* ManagedFont = nullptr;
	};
}