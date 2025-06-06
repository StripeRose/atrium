#pragma once

#include <Core_GraphicsAPI.hpp>
#include <Core_GraphicsEnums.hpp>
#include <Core_InputDevice.hpp>
#include <Core_WindowManagement.hpp>

#if IS_IMGUI_ENABLED
#include <DX12_DescriptorHeap.hpp>
#endif

namespace Atrium
{
	class ImGuiHandler
	{
	public:
		ImGuiHandler(Core::GraphicsAPI& aGraphicsAPI, Core::Window& aPrimaryWindow, std::shared_ptr<Core::RenderTexture> aTarget);
		~ImGuiHandler();

		Core::InputDeviceType GetAllowedInputs() const;

		void MarkFrameStart();
		void MarkFrameEnd();
		
#if IS_IMGUI_ENABLED
	private:
		void InitForWindow(Core::Window& aWindow);
		void SetupBackend(Core::GraphicsAPI& aGraphicsAPI, Core::Window& aWindow, GraphicsFormat aTargetFormat);
		void Cleanup();

		void StyleColorsNord();

		Core::GraphicsAPI& myGraphicsAPI;
		std::shared_ptr<Core::RenderTexture> myRenderTarget;
		Core::Window* myWindow;

		std::unique_ptr<DirectX12::RenderPassDescriptorHeap> myCBV_SRVHeap;
#endif
	};
}
