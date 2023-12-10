#pragma once

#include <wrl.h>

namespace RoseGold::DirectX12
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
}