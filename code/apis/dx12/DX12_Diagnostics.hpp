#pragma once

#include <Windows.h>

namespace RoseGold::DirectX12
{
	bool LogAction(HRESULT aResult, const char* aMessage);
	bool AssertAction(HRESULT aResult, const char* aMessage);
	bool AssertSuccess(HRESULT aResult);
}