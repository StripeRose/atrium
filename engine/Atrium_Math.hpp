#pragma once

#include <rose-common/math/Constants.hpp>
#include <rose-common/math/Common.hpp>
#include <rose-common/math/Geometry.hpp>
#include <rose-common/math/Matrix44.hpp>
#include <rose-common/math/Trigonometry.hpp>
#include <rose-common/math/Vector.hpp>

namespace Atrium
{
	using Matrix3D = Matrix44<float>;

	using RectangleI = Rectangle<int>;
	using RectangleF = Rectangle<float>;

	using Vector2F = Vector2<float>;
	using Vector3F = Vector3<float>;
	using Vector4F = Vector4<float>;
}