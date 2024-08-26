#pragma once

#include <rose-common/math/Constants.hpp>
#include <rose-common/math/Common.hpp>
#include <rose-common/math/Geometry.hpp>
#include <rose-common/math/Matrix3D.hpp>
#include <rose-common/math/Trigonometry.hpp>
#include <rose-common/math/Vector.hpp>

namespace Atrium
{
	using Point = RoseCommon::Math::Point<int>;
	using Size = RoseCommon::Math::Size<int>;

	template <typename T>
	using PointT = RoseCommon::Math::Point<T>;
	template <typename T>
	using SizeT = RoseCommon::Math::Size<T>;

	namespace Math
	{
		using namespace RoseCommon::Math;

		using Vector2 = RoseCommon::Math::Vector2<float>;
		using Vector3 = RoseCommon::Math::Vector3<float>;
		using Vector4 = RoseCommon::Math::Vector4<float>;

		template <typename T> using Vector2T = RoseCommon::Math::Vector2<T>;
		template <typename T> using Vector3T = RoseCommon::Math::Vector3<T>;
		template <typename T> using Vector4T = RoseCommon::Math::Vector4<T>;

		using Rectangle = RoseCommon::Math::Rectangle<float>;

		template <typename T>
		using RectangleT = RoseCommon::Math::Rectangle<T>;

		using Matrix = RoseCommon::Math::Matrix3D<float>;
	}
}