#pragma once
/// <summary>
/// Add option for SDL_HINT_RENDER_SCALE_QUALITY to select between nearest pixel and linear filtering.
/// </summary>
namespace Engine
{
	class Settings // Seperate from engine as this will probably get quite large, and need to be passed to the window and renderer.
	{
	public:
		static Settings& Instance();

	private:
		Settings();

	public:
		/// <summary>
		/// Get the target FPS.
		/// </summary>
		/// <returns>Frames per second</returns>
		int GetTargetFrameRate() const;
		/// <summary>
		/// Get the target frame time.
		/// </summary>
		/// <returns>Milliseconds between frames.</returns>
		float GetTargetFrameTime() const;
		/// <summary>
		/// Sets FPS and updates _TargetFrameTime to match. 
		/// If paramater provided is 0, frame rate is uncapped.
		/// </summary>
		/// <param name="frameRate">Frames per seconds</param>
		void SetTargetFrameRate(int frameRate);
	private:
		/// <summary>
		/// Target frames per second.
		/// </summary>
		int TargetFrameRate;
		/// <summary>
		/// Target seconds between frames.
		/// </summary>
		float TargetFrameTime;
	};
}