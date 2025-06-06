#pragma once

#include <rose-common/math/Constants.hpp>
#include <rose-common/math/Common.hpp>
#include <rose-common/math/Geometry.hpp>
#include <rose-common/math/Matrix3D.hpp>
#include <rose-common/math/Trigonometry.hpp>
#include <rose-common/math/Vector.hpp>

namespace Atrium
{
	using Matrix = Matrix3DT<float>;
	using Point = PointT<int>;
	using PointF = PointT<float>;
	using Rectangle = RectangleT<int>;
	using RectangleF = RectangleT<float>;
	using Size = SizeT<int>;
	using SizeF = SizeT<float>;

	using Vector2 = Vector2T<float>;
	using Vector3 = Vector3T<float>;
	using Vector4 = Vector4T<float>;
}