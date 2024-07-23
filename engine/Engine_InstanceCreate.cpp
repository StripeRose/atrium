#include "Engine_Instance.hpp"

#if _WIN32
#include "DX12_Instancer.hpp"
#include "Win32_WindowManagement.hpp"
#endif

namespace Atrium
{
	std::unique_ptr<EngineInstance> EngineInstance::Create()
	{
		ZoneScoped;

		std::unique_ptr<EngineInstance> instance(new EngineInstance());

#if _WIN32
		instance->myGraphicsAPI.reset(DirectX12::CreateDX12Manager().release());
		instance->myWindowManager.reset(new Win32::WindowManager());
#else
		static_assert(false, "No engine instance available for this platform yet.");
#endif

		return instance;
	}
}