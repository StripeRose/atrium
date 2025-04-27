#pragma once

#include <rose-common/math/Constants.hpp>
#include <rose-common/math/Common.hpp>
#include <rose-common/math/Geometry.hpp>
#include <rose-common/math/Matrix3D.hpp>
#include <rose-common/math/Trigonometry.hpp>
#include <rose-common/math/Vector.hpp>

namespace Atrium
{
	using Matrix = RoseCommon::Math::Matrix3D<float>;
	using Point = RoseCommon::Math::Point<int>;
	using PointF = RoseCommon::Math::Point<float>;
	using Rectangle = RoseCommon::Math::Rectangle<int>;
	using RectangleF = RoseCommon::Math::Rectangle<float>;
	using Size = RoseCommon::Math::Size<int>;
	using SizeF = RoseCommon::Math::Size<float>;

	using Vector2 = RoseCommon::Math::Vector2<float>;
	using Vector3 = RoseCommon::Math::Vector3<float>;
	using Vector4 = RoseCommon::Math::Vector4<float>;

	namespace Math
	{
		using namespace RoseCommon::Math;
	}
}