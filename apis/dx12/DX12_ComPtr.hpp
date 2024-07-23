#pragma once

#include <wrl.h>

namespace Atrium::DirectX12
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
}