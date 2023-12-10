#include "DX12_Diagnostics.hpp"

#include <Common_Debug.hpp>

#include <comdef.h>

namespace RoseGold::DirectX12
{
	bool LogAction(HRESULT aResult, const char* aMessage)
	{
		_com_error error(aResult);

		if (SUCCEEDED(aResult))
		{
			Debug::Log("[DX12] %s", aMessage);
		}
		else
		{
			Debug::LogError(
				"[DX12] %s\nDirectX 12 Error: %s\n\n%s",
				aMessage,
				static_cast<const char*>(error.ErrorMessage()),
				static_cast<const char*>(error.Description())
			);
		}

		return SUCCEEDED(aResult);
	}

	bool AssertAction(HRESULT aResult, const char* aMessage)
	{
		_com_error error(aResult);

		if (SUCCEEDED(aResult))
		{
			Debug::Log("[DX12] %s", aMessage);
		}
		else
		{
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
