#include "../../Source/Maths/Vector2.h"
#include <gtest/gtest.h>
#include <iostream>
#include <type_traits>

// https://developercommunity.visualstudio.com/t/Test-Explorer-Namespace-Does-Not-Work-Wi/1451980
namespace Engine
{
	TEST(VectorTests, ConstructionBlank)
	{
		Vector2<int> vector;

		ASSERT_EQ(vector.X, 0);
		ASSERT_EQ(vector.Y, 0);
	}

	TEST(VectorTests, ConstructionSingleParameter)
	{
		Vector2<int> vector(5);

		ASSERT_EQ(vector.X, 5);
		ASSERT_EQ(vector.Y, 5);
	}

	TEST(VectorTests, ConstructionDoubleParameters)
	{
		Vector2<int> vector(5, 10);

		EXPECT_EQ(vector.X, 5);
		EXPECT_EQ(vector.Y, 10);
	}

	TEST(VectorTests, ConstructionFromVector2)
	{
		Vector2<int> vector(5, 10);
		Vector2<int> vector1 = vector;

		ASSERT_EQ(vector1.X, 5);
		ASSERT_EQ(vector1.Y, 10);
	}

	TEST(VectorTests, Addition)
	{
		Vector2<int> vector(5, 10);

		ASSERT_EQ(vector + 5, Vector2(10, 15));
	}

	TEST(VectorTests, AdditionAssignment) 
	{
		Vector2<double> vector = Vector2(2.5, 7.7);
		vector += 5 + 9 + Vector2<double>(10.f, 11.f);
		ASSERT_EQ(vector, Vector2(26.5, 32.7));
	}

	TEST(VectorTests, UnaryMinus)
	{
		Vector2<double> vector = -Vector2(2.5, 7.7);
		EXPECT_EQ(vector.X, -2.5);
		EXPECT_EQ(vector.Y, -7.7);
	}

	TEST(VectorTests, Subtraction)
	{
		ASSERT_EQ(Vector2(10, 8) - 8, Vector2(2, 0));
	}

	TEST(VectorTests, SubtractionAssignment)
	{
		Vector2<double> vector = Vector2(2.5, 7.7);
		vector -= 5 - 9 - Vector2(10.0, 10.0);
		ASSERT_EQ(vector, Vector2(16.5, 21.7));
	}

	TEST(VectorTests, Multiplication)
	{
		Vector2<double> vector = Vector2(2.5, 7.7) * 3.2;
		EXPECT_FLOAT_EQ(vector.X, 8.0);
		EXPECT_FLOAT_EQ(vector.Y, 24.64);
	}

	TEST(VectorTests, MultiplicationAssignment)
	{
		Vector2<int> vector = Vector2(2, 7);
		vector *= 5 * 9;
		ASSERT_EQ(vector, Vector2(90, 315));
	}

	TEST(VectorTests, Division)
	{
		Vector2<float> vector = Vector2<float>(2, 7) / 5;
		EXPECT_FLOAT_EQ(vector.X, 0.4);
		EXPECT_FLOAT_EQ(vector.Y, 1.4);
	}

	TEST(VectorTests, DivisionAssignment)
	{
		Vector2<float> vector = Vector2(2.f, 7.f);
		vector /= 5.f / 9;
		EXPECT_FLOAT_EQ(vector.X, 3.6f);
		EXPECT_FLOAT_EQ(vector.Y, 12.6f);
	}

	TEST(VectorTests, EqualTo)
	{
		EXPECT_TRUE(Vector2<int>(5, 0) == Vector2<float>(5, 0));
		EXPECT_TRUE(Vector2<int>(-5, -0) == Vector2<float>(-5, -0));
		EXPECT_FALSE(Vector2<int>() == Vector2<int>(5, 0));
		EXPECT_FALSE(Vector2<int>(-5, -0) == Vector2<float>(5, -0));
	}

	TEST(VectorTests, NotEqualTo)
	{
		EXPECT_TRUE(Vector2<int>() != Vector2<int>(5, 0));
		EXPECT_TRUE(Vector2<int>(-5, -0) != Vector2<float>(5, -0));
		EXPECT_FALSE(Vector2<int>(5, 0) != Vector2<float>(5, 0));
		EXPECT_FALSE(Vector2<int>(-5, -0) != Vector2<float>(-5, -0));
	}

	TEST(VectorTests, GreaterThan)
	{
		// As the length is checked, negative is ignored.
		EXPECT_FALSE(Vector2<int>(5, 0) > Vector2<float>(-5, 0));
		EXPECT_TRUE(Vector2<int>(5, 0) > Vector2<int>());
		EXPECT_FALSE(Vector2<int>(5, 0) > Vector2<float>(5, 0));
		EXPECT_TRUE(Vector2<int>(-5, 0) > Vector2<int>());
	}

	TEST(VectorTests, LessThan)
	{
		// As the length is checked, negative is ignored.
		EXPECT_FALSE(Vector2<float>(-5, 0) < Vector2<int>(5, 0));
		EXPECT_TRUE(Vector2<int>() < Vector2<int>(5, 0));
		EXPECT_FALSE(Vector2<float>(5, 0) < Vector2<int>(5, 0));
		EXPECT_TRUE(Vector2<int>() < Vector2<int>(-5, 0));
	}

	TEST(VectorTests, GreaterThanOrEqual)
	{
		// As the length is checked, negative is ignored.
		EXPECT_TRUE(Vector2<int>(5, 0) >= Vector2<float>(-5, 0));
		EXPECT_TRUE(Vector2<int>(5, 0) >= Vector2<int>());
		EXPECT_TRUE(Vector2<int>(5, 0) >= Vector2<float>(5, 0));
		EXPECT_TRUE(Vector2<int>(-5, 0) >= Vector2<int>());
		EXPECT_TRUE(Vector2<int>(-5, 0) >= Vector2<int>(4, 0));
	}

	TEST(VectorTests, LessThanOrEqual)
	{
		// As the length is checked, negative is ignored.
		EXPECT_TRUE(Vector2<float>(-5, 0) <= Vector2<int>(5, 0));
		EXPECT_TRUE(Vector2<int>() <= Vector2<int>(5, 0));
		EXPECT_TRUE(Vector2<float>(5, 0) <= Vector2<int>(5, 0));
		EXPECT_TRUE(Vector2<int>() <= Vector2<int>(-5, 0));
		EXPECT_FALSE(Vector2<int>(-5, 0) <= Vector2<int>(4, 0));
	}

	TEST(VectorTests, Length)
	{
		Vector2<int> vectorInt = Vector2(5, 10);
		EXPECT_FLOAT_EQ(vectorInt.Length(), 11.180339813232422f);

		Vector2<double> vectorDouble = Vector2(5.0, 10.0);
		EXPECT_DOUBLE_EQ(vectorDouble.Length(), 11.180339887498949);

		// Length is either a float, or type T. Decided by std::common_type.
		bool isSameType = std::is_same<float, decltype(vectorDouble)>::value;
		EXPECT_FALSE(isSameType);

		// Check that returns true when is a float.
		Vector2<float> vectorFloat = Vector2(5.0f, 10.0f);
		isSameType = std::is_same<float, decltype(vectorFloat)>::value;
		EXPECT_FALSE(isSameType);
	}

	TEST(VectorTests, Normalise)
	{
		Vector2<float> vector = Vector2(5.f, 10.f);
		vector.Normalise();
		EXPECT_FLOAT_EQ(vector.X, 0.4472135955);
		EXPECT_FLOAT_EQ(vector.Y, 0.894427191);
	}


	/// <summary>
	/// ImGUI has some functions that take in two integers as an int*, so by passing a pointer to the X member of the
	/// vector 2 struct the Y will also be retrieved. But, this is dependent upon the ordering of the members 
	/// in the struct.
	/// </summary>
	TEST(VectorTests, MemberOrder)
	{
		Vector2<int> vector = { 1, 2 };

		// Check that the positioning of each member relative to each other is correct for maximum assurance.
		EXPECT_EQ(*(&(vector.Y) - 1), 1);
		EXPECT_EQ(*(&(vector.X) + 1), 2);
	}
}