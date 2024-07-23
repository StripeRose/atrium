#pragma once

#include "Common_Diagnostics.hpp"

#include <d3d12.h>
#include <Windows.h>

namespace Atrium::DirectX12
{
	// Asserts that the result was successful, with no message otherwise.
	void AssertAction(HRESULT aResult, const char* anAction);

	// Asserts that the result was successful and logs that it happened.
	void AssertActionWithLog(HRESULT aResult, const char* anAction);

	// Logs message if a result failed and returns whether the action was successful.
	bool VerifyAction(HRESULT aResult, const char* aMessage);
	bool VerifyAction(HRESULT aResult, const char* aMessage, ID3DBlob* anErrorBlob);

	// Logs an action with an error if applicable, and returns whether the action was successful.
	bool VerifyActionWithLog(HRESULT aResult, const char* aMessage);
}

#ifdef TRACY_ENABLE
#pragma warning(push)
#pragma warning(disable:26495 4100 6201)
#include <tracy/TracyD3D12.hpp>
#pragma warning(pop)

#else

#define TracyD3D12Zone(...)
#define TracyD3D12NewFrame(...)
#define TracyD3D12Collect(...)

#endif