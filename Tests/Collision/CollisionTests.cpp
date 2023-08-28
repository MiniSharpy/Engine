#include "../../Source/Collision/Intersections.h"
#include "../../Source/Maths/Vector2.h"
#include <gtest/gtest.h>

namespace Engine
{
	TEST(CollisionTests, Collision)
	{
		Edge<float> a = { Vector2{ 2.f, 3.f }, Vector2{ -2.f, -2.f } };
		Edge<float> b = { Vector2{ -5.f, 5.f }, Vector2{ 5.f, -5.f } };
		auto collision = Collision::LineSegmentIntersection(a, b);
		ASSERT_TRUE(collision);
		ASSERT_FLOAT_EQ(collision.value().X, -0.222222328f);
		ASSERT_FLOAT_EQ(collision.value().Y, 0.222222090f);
	}

	TEST(CollisionTests, RightAngleCollision)
	{
		Edge<float> a = { Vector2{ 0.f, 0.f }, Vector2{ 4.f, 0.f } };
		Edge<float> b = { Vector2{ 2.f, 2.f }, Vector2{ 2.f, -2.f } };
		auto collision = Collision::LineSegmentIntersection(a, b);
		ASSERT_TRUE(collision);
		ASSERT_FLOAT_EQ(collision.value().X, 2.f);
		ASSERT_FLOAT_EQ(collision.value().Y, 0.f);
	}

	TEST(CollisionTests, Collinearity)
	{
		Edge<float> a = { Vector2{ 0.f, 0.f }, Vector2{ 4.f, 0.f } };
		Edge<float> b = a;
		auto collision = Collision::LineSegmentIntersection(a, b);
		ASSERT_FALSE(collision);
	}

	TEST(CollisionTests, Parallel)
	{
		Edge<float> a = { Vector2{ 0.f, 0.f }, Vector2{ 4.f, 0.f } };
		Edge<float> b = { Vector2{ 0.f, 2.f }, Vector2{ 4.f, 2.f } };
		auto collision = Collision::LineSegmentIntersection(a, b);
		ASSERT_FALSE(collision);
	}
}