#pragma once
struct SDL_Window;
struct SDL_Surface;

namespace Engine
{
	class Window
	{
	public:
		static Window& Instance();

	private:
		Window();
		~Window();

	public:
		// https://en.cppreference.com/w/cpp/language/rule_of_three
		Window(const Window& other) = delete; // Copy Constructor
		Window& operator=(const Window& other) = delete; // Copy Assignment

		/// <summary>
		/// When there's a resize event, the Events class will automatically update its representation of the window size
		/// which should be preferred for it is a Vector2 type.
		/// </summary>
		void GetWindowSize(int& width, int& height);

		operator SDL_Window* () { return ManagedWindow; } // Returns native renderer when passed into a SDL_Window * paramater.

	private:
		SDL_Window* ManagedWindow;
		SDL_Surface* ManagedIcon;
	};
}