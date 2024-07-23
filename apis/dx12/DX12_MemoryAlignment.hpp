#pragma once

#include <cstdint>
#include <d3d12.h>

namespace Atrium::DirectX12
{
	template <typename T>
	inline T Align(T aLocation, T anAlignment)
	{
		return (aLocation + (anAlignment - 1)) & ~(anAlignment - 1);
	}
}
