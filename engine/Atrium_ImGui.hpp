#pragma once

#include <Atrium_GraphicsAPI.hpp>
#include <Atrium_GraphicsEnums.hpp>
#include <Atrium_InputDevice.hpp>
#include <Atrium_WindowManagement.hpp>

#if IS_IMGUI_ENABLED
#include <DX12_DescriptorHeap.hpp>
#endif

namespace Atrium
{
	class ImGuiHandler
	{
	public:
		ImGuiHandler(Core::GraphicsAPI& aGraphicsAPI, Window& aPrimaryWindow, std::shared_ptr<Core::RenderTexture> aTarget);
		~ImGuiHandler();

		Core::InputDeviceType GetAllowedInputs() const;

		void MarkFrameStart();
		void MarkFrameEnd();
		
#if IS_IMGUI_ENABLED
	private:
		void InitForWindow(Window& aWindow);
		void SetupBackend(Core::GraphicsAPI& aGraphicsAPI, Window& aWindow, Core::GraphicsFormat aTargetFormat);
		void Cleanup();

		void StyleColorsNord();

		Core::GraphicsAPI& myGraphicsAPI;
		std::shared_ptr<Core::RenderTexture> myRenderTarget;
		Window* myWindow;

		std::unique_ptr<DirectX12::RenderPassDescriptorHeap> myCBV_SRVHeap;
#endif
	};
}
