#pragma once
namespace Engine
{
	class Settings;
	class BaseScene;
	class Events;

	class Game
	{
	public:
		static Game& Instance();

	private:
		Game();
		~Game();

	public:
		// https://en.cppreference.com/w/cpp/language/rule_of_three
		Game(const Game& other) = delete; // Copy Constructor
		Game& operator=(const Game& other) = delete; // Copy Assignment

		void Update(const float& deltaTime, Settings& settings, BaseScene& scene, Events& events);
	};
}