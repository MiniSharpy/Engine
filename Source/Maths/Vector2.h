#pragma once
#include <concepts>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <functional>

// TODO: Dot product, different orders, const corrected if Vector2 is const. What about constexpr?
// TODO: Comparisons can cause undesired results. For example std::map uses the less than operator for contains(),
// meaning negative vectors will be considered the same as positive vectors.
// TODO: Hadamard product
// TODO: Just make a matrix class and have there be a type alias?
namespace Engine
{
	/// <summary>
	/// Mathematical 2-dimensional vector.
	/// </summary>
	/// <typeparam name="T">An integral or floating point type.</typeparam>
	template <typename T>
		requires std::integral<T> || std::floating_point<T>
	struct Vector2
	{
	public:
		/* STATIC METHODS */
		static constexpr Vector2 Zero() { return { 0, 0 }; }
		static constexpr Vector2 One() { return {1, 1}; }
		static constexpr Vector2 Up() { return { 0, -1 }; }
		static constexpr Vector2 Down() { return { 0, 1 }; }
		static constexpr Vector2 Right() { return { 1, 0 }; }
		static constexpr Vector2 Left() { return { -1, 0 }; }
		static Vector2 Clamp(Vector2 value, Vector2 minimum, Vector2 maximum) { return { std::clamp(value.X, minimum.X, maximum.X), std::clamp(value.Y, minimum.Y, maximum.Y) }; }
		static T CrossProduct(Vector2 a, Vector2 b) { return a.X * b.Y - a.Y * b.X; }

		/* CONSTRUCTORS */
		T X, Y;

		Vector2() : X(0), Y(0) {}
		Vector2(T value) : X(value), Y(value) {}
		Vector2(T x, T y) : X(x), Y(y) {}

		template <typename U> // No need to specify requires again, can't construct a Vector2 of the disallowed types.
		explicit Vector2(const Vector2<U>& right) : X{ (T)right.X }, Y{ (T)right.Y } {}

		/* OPERATORS */ // TODO: Add, Subtract, Multiply Vector2 by any other Vector 2.
		constexpr Vector2 operator+ (const T right) const { return { X + right, Y + right }; };
		constexpr Vector2 operator+ (const Vector2& right) const { return { X + right.X, Y + right.Y }; };
		constexpr Vector2& operator+= (const T right) { X += right; Y += right; return *this; };
		constexpr Vector2& operator+= (const Vector2& right) { X += right.X; Y += right.Y; return *this; };

		constexpr Vector2 operator- () const { return { -X, -Y }; }; // Unary minus.
		constexpr Vector2 operator- (const T right) const { return { X - right, Y - right }; };
		constexpr Vector2 operator- (const Vector2& right) const { return { X - right.X, Y - right.Y }; };
		constexpr Vector2& operator-= (const T right) { X -= right; Y -= right; return *this; };
		constexpr Vector2& operator-= (const Vector2& right) { X -= right.X; Y -= right.Y; return *this; };

		constexpr Vector2 operator* (const T right) const { return { X * right, Y * right }; };
		constexpr Vector2& operator*= (const T right) { X *= right; Y *= right; return *this; };
		constexpr Vector2& operator*= (const Vector2& right) { X *= right.X; Y *= right.Y; return *this; };
		constexpr Vector2 operator/ (const T right) const { return { X / right, Y / right }; };
		constexpr Vector2& operator/= (const  Vector2& right) { X /= right.X; Y /= right.Y; return *this; };
		constexpr Vector2& operator/= (const T right) { X /= right; Y /= right; return *this; };

		// Want to be able to compare to Vector2s of differing types.
		template <typename U>
		bool operator== (const Vector2<U>& right) const { return X == right.X && Y == right.Y; };

		template <typename U>
		bool operator!= (const Vector2<U>& right) const { return X != right.X || Y != right.Y; };

		template <typename U>
		bool operator> (const Vector2<U>& right) const { return LengthSquared() > right.LengthSquared(); };

		template <typename U>
		bool operator< (const Vector2<U>& right) const { return LengthSquared() < right.LengthSquared(); };

		template <typename U>
		bool operator>= (const Vector2<U>& right) const { return LengthSquared() >= right.LengthSquared(); };

		template <typename U>
		bool operator<= (const Vector2<U>& right) const { return LengthSquared() <= right.LengthSquared(); };

		// TODO: non-int operators. Make sure to specify requires.
		friend Vector2 operator+ (int left, const Vector2& right) { return { left + right.X, left + right.Y }; };
		friend Vector2 operator- (int left, const Vector2& right) { return { left - right.X, left - right.Y }; };
		friend Vector2 operator* (int left, const Vector2& right) { return { left * right.X, left * right.Y }; };
		friend Vector2 operator/ (int left, const Vector2& right) { return { left / right.X, left / right.Y }; };

		friend std::ostream& operator<<(std::ostream& os, const Vector2<T>& vector2)
		{
			return os << "X:" << vector2.X << " " << "Y:" << vector2.Y;
		}

		/* METHODS */
		/// <summary>
		/// Calculate length using Pythagoras's Theorem. 
		/// For length comparisons prefer LengthSquared() to avoid unnecessary square root.
		/// Uses double square root to avoid loss of precision.
		/// </summary>
		/// <returns> A float, or T. Whichever has higher precision. </returns>
		using FloatOrT = std::common_type<float, T>::type;
		FloatOrT Length() const { return (FloatOrT)sqrt(LengthSquared()); }

		/// <summary>
		/// Calculate the sum of the areas of two squares.
		/// Useful for length comparisons where actual length doesn't matter.
		/// </summary>
		T LengthSquared() const { return X * X + Y * Y; };

		/// <summary>
		/// Normalise member variables to range 0-1.
		/// </summary>
		void Normalise() requires std::floating_point<T>
		{
			if (LengthSquared() == 0) { return; }

			float scale = 1 / Length();
			X *= scale;
			Y *= scale;
		}

		/// <summary>
		/// Swizzle the X and Y components.
		/// </summary>
		/// <returns>The swapped X and Y.</returns>
		Vector2 YX() const // TODO: Unit Test
		{
			return { Y, X };
		}

		Vector2 Absolute() const
		{
			return { std::abs(X), std::abs(Y) };
		}
	};

	template<class T>
	using Edge = std::pair<Vector2<T>, Vector2<T>>;
}

namespace std
{
	template <class T>
	struct hash<Engine::Vector2<T>>
	{
		std::size_t operator()(const Engine::Vector2<T>& vector) const noexcept
		{
			std::size_t h1 = std::hash<T>{}(vector.X);
			std::size_t h2 = std::hash<T>{}(vector.Y);

			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2)); // From boost.
		}
	};
}