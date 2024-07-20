#include "stdafx.hpp"

#include "DX12_Diagnostics.hpp"

#include <comdef.h>

namespace RoseGold::DirectX12
{
	void AssertAction(HRESULT aResult, const char* anAction)
	{
		if (SUCCEEDED(aResult))
		{
			Debug::Log("[DX12] %s", anAction);
		}
		else
		{
			_com_error error(aResult);

			Debug::LogFatal(
				"[DX12] Action failed:\n%s\nError: %s",
				anAction,
				static_cast<const char*>(error.ErrorMessage())
			);
		}
	}

	void AssertActionWithLog(HRESULT aResult, const char* anAction)
	{
		if (FAILED(aResult))
		{
			_com_error error(aResult);

			Debug::LogFatal(
				"[DX12] Action failed:\n%s\nError: %s",
				anAction,
				static_cast<const char*>(error.ErrorMessage())
			);
		}
	}

	bool VerifyAction(HRESULT aResult, const char* aMessage)
	{
		if (FAILED(aResult))
		{
			_com_error error(aResult);

			Debug::LogError(
				"[DX12] Action failed:\n%s\nError: %s",
				aMessage,
				static_cast<const char*>(error.ErrorMessage())
			);
		}

		return SUCCEEDED(aResult);
	}

	bool VerifyAction(HRESULT aResult, const char* aMessage, ID3DBlob* anErrorBlob)
	{
		if (FAILED(aResult))
		{
			_com_error error(aResult);

			Debug::LogError(
				"[DX12] Action failed: %s\nError: %s\n%s",
				aMessage,
				static_cast<const char*>(error.ErrorMessage()),
				(const char*)anErrorBlob->GetBufferPointer()
			);
		}

		return SUCCEEDED(aResult);
	}

	bool VerifyActionWithLog(HRESULT aResult, const char* aMessage)
	{
		if (SUCCEEDED(aResult))
		{
			Debug::Log("[DX12] %s", aMessage);
		}
		else
		{
			_com_error error(aResult);

			Debug::LogError(
				"[DX12] %s\nDirectX 12 Error: %s",
				aMessage,
				static_cast<const char*>(error.ErrorMessage())
			);
		}

		return SUCCEEDED(aResult);
	}
}
