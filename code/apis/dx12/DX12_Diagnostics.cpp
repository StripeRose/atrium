#include "DX12_Diagnostics.hpp"

#include <comdef.h>

namespace RoseGold::DirectX12
{
	bool LogAction(HRESULT aResult, const char* aMessage)
	{
		if (SUCCEEDED(aResult))
		{
			Debug::Log("[DX12] %s", aMessage);
		}
		else
		{
			_com_error error(aResult);

			Debug::LogError(
				"[DX12] %s\nDirectX 12 Error: %s\n\n%s",
				aMessage,
				static_cast<const char*>(error.ErrorMessage()),
				static_cast<const char*>(error.Description())
			);
		}

		return SUCCEEDED(aResult);
	}

	bool LogIfError(HRESULT aResult, const char* aMessage)
	{
		if (FAILED(aResult))
		{
			_com_error error(aResult);

			Debug::LogError(
				"[DX12] %s\nDirectX 12 Error: %s\n\n%s",
				aMessage,
				static_cast<const char*>(error.ErrorMessage()),
				static_cast<const char*>(error.Description())
			);
		}

		return SUCCEEDED(aResult);
	}

	bool LogError(HRESULT aResult, ID3DBlob* anErrorBlob)
	{
		if (FAILED(aResult))
		{
			_com_error error(aResult);

			Debug::LogError(
				"[DX12] %s\nDirectX 12 Error: %s\n\n%s",
				(const char*) anErrorBlob->GetBufferPointer(),
				static_cast<const char*>(error.ErrorMessage()),
				static_cast<const char*>(error.Description())
			);
		}

		return SUCCEEDED(aResult);
	}

	bool AssertAction(HRESULT aResult, const char* aMessage)
	{
		if (SUCCEEDED(aResult))
		{
			Debug::Log("[DX12] %s", aMessage);
		}
		else
		{
			_com_error error(aResult);

			Debug::LogFatal(
				"[DX12] %s\nDirectX 12 Error: %s\n\n%s",
				aMessage,
				static_cast<const char*>(error.ErrorMessage()),
				static_cast<const char*>(error.Description())
			);
		}

		return SUCCEEDED(aResult);
	}

	bool AssertSuccess(HRESULT aResult)
	{
		_com_error error(aResult);
		Debug::Assert(SUCCEEDED(aResult),
			"DirectX 12 Error: %s\n\n%s",
			static_cast<const char*>(error.ErrorMessage()),
			static_cast<const char*>(error.Description())
		);

		return SUCCEEDED(aResult);
	}
}
