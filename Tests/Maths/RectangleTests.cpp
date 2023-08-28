#include "../../Source/Maths/Rectangle.h"
#include "../../Source/Maths/Vector2.h"
#include <gtest/gtest.h>
#include <SDL.h>

namespace Engine
{
	TEST(RectangleTests, ReinterpretCastInt)
	{
		Rectangle<int> myRectangle = { 10, 5, 10, 5 };
		SDL_Rect* sdlRectangle = reinterpret_cast<SDL_Rect*>(&myRectangle);

		ASSERT_EQ(myRectangle.Position.X, sdlRectangle->x);
		ASSERT_EQ(myRectangle.Position.Y, sdlRectangle->y);
		ASSERT_EQ(myRectangle.Size.X, sdlRectangle->w);
		ASSERT_EQ(myRectangle.Size.Y, sdlRectangle->h);
	}

	TEST(RectangleTests, ReinterpretCastFloat)
	{
		Rectangle<float> myRectangle = { 10.f, 5.f, 10.f, 5.f };
		SDL_FRect* sdlRectangle = reinterpret_cast<SDL_FRect*>(&myRectangle);

		ASSERT_EQ(myRectangle.Position.X, sdlRectangle->x);
		ASSERT_EQ(myRectangle.Position.Y, sdlRectangle->y);
		ASSERT_EQ(myRectangle.Size.X, sdlRectangle->w);
		ASSERT_EQ(myRectangle.Size.Y, sdlRectangle->h);
	}
}