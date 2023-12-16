#pragma once

#include <Common_Debug.hpp>

#include <d3d12.h>
#include <Windows.h>

namespace RoseGold::DirectX12
{
	bool LogAction(HRESULT aResult, const char* aMessage);
	bool LogIfError(HRESULT aResult, const char* aMessage);
	bool LogError(HRESULT aResult, ID3DBlob* anErrorBlob);
	bool AssertAction(HRESULT aResult, const char* aMessage);
	bool AssertSuccess(HRESULT aResult);
}