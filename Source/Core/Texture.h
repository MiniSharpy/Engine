#pragma once
struct SDL_Texture;
struct SDL_Renderer;
typedef void* ImTextureID;
#include "../Maths/Vector2.h"
#include <string>

namespace Engine
{
	class Texture
	{
	public:
		Texture() = default;
		Texture(SDL_Renderer* renderer, std::string fileName);
		~Texture();

		// https://en.cppreference.com/w/cpp/language/rule_of_three
		Texture(const Texture& other) = delete; // Copy Constructor
		Texture(Texture&& other) noexcept; // Move Constructor
		Texture(SDL_Texture* other) noexcept; // Move Constructor
		Texture& operator=(const Texture& other) = delete; // Copy Assignment
		Texture& operator=(Texture&& other) noexcept; // Move Assignment
		Vector2<int> GetSize() const { return Size; }

		operator SDL_Texture* () { return ManagedTexture; } // Returns managed texture when passed into a SDL_Texture * paramater.
		operator SDL_Texture* () const { return ManagedTexture; } // Returns managed texture when passed into a SDL_Texture * paramater.
		operator ImTextureID () { return (void*)ManagedTexture; } // Returns managed texture when passed into a ImTextureID paramater.

	private:
		SDL_Texture* ManagedTexture = nullptr; // This is freed when a renderer is destroyed, potentially leaving a dangling pointer.
		Vector2<int> Size;
	};
}