#pragma once

#include <rose-common/math/Constants.hpp>
#include <rose-common/math/Common.hpp>
#include <rose-common/math/Geometry.hpp>
#include <rose-common/math/Trigonometry.hpp>
#include <rose-common/math/Vector.hpp>

namespace RoseGold
{
	namespace Math
	{
		template <typename T>
		using Box = RoseCommon::Math::Box<RoseCommon::Math::Vector3<T>>;
		template <typename T>
		using Rectangle = RoseCommon::Math::Box<RoseCommon::Math::Vector2<T>>;

		using Vector2f = RoseCommon::Math::Vector2<float>;
		using Vector3f = RoseCommon::Math::Vector3<float>;
		using Vector4f = RoseCommon::Math::Vector4<float>;
	}
}