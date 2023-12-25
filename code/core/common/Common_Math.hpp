#pragma once

#include <rose-common/math/Constants.hpp>
#include <rose-common/math/Common.hpp>
#include <rose-common/math/Geometry.hpp>
#include <rose-common/math/Matrix.hpp>
#include <rose-common/math/MakeMatrix.h>
#include <rose-common/math/Trigonometry.hpp>
#include <rose-common/math/Vector.hpp>

namespace RoseGold
{
	using Point = RoseCommon::Math::Vector2<int>;
	using Size = RoseCommon::Math::Vector2<int>;

	namespace Math
	{
		using Vector2 = RoseCommon::Math::Vector2<float>;
		using Vector3 = RoseCommon::Math::Vector3<float>;
		using Vector4 = RoseCommon::Math::Vector4<float>;

		template <typename T> using Vector2T = RoseCommon::Math::Vector2<T>;
		template <typename T> using Vector3T = RoseCommon::Math::Vector3<T>;
		template <typename T> using Vector4T = RoseCommon::Math::Vector4<T>;

		using Rectangle = RoseCommon::Math::Box<Vector2>;
		using Box = RoseCommon::Math::Box<Vector3>;
		using Circle = RoseCommon::Math::Sphere<Vector2>;
		using Sphere = RoseCommon::Math::Sphere<Vector3>;

		template <typename T> using RectangleT = RoseCommon::Math::Box<Vector2T<T>>;
		template <typename T> using BoxT = RoseCommon::Math::Box<Vector3T<T>>;
		template <typename T> using CircleT = RoseCommon::Math::Sphere<Vector2T<T>>;
		template <typename T> using SphereT = RoseCommon::Math::Sphere<Vector3T<T>>;

		using Matrix = RoseCommon::Math::Matrix<4, 4, float>;
		using MakeMatrix = RoseCommon::Math::MakeMatrix<float>;
	}
}