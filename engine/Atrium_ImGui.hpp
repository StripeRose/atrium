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
		ImGuiHandler(GraphicsAPI& aGraphicsAPI, Window& aPrimaryWindow, std::shared_ptr<RenderTexture> aTarget);
		~ImGuiHandler();

		InputDeviceType GetAllowedInputs() const;

		void MarkFrameStart();
		void MarkFrameEnd();
		
#if IS_IMGUI_ENABLED
	private:
		void InitForWindow(Window& aWindow);
		void SetupBackend(GraphicsAPI& aGraphicsAPI, Window& aWindow, GraphicsFormat aTargetFormat);
		void Cleanup();

		void StyleColorsNord();

		GraphicsAPI& myGraphicsAPI;
		std::shared_ptr<RenderTexture> myRenderTarget;
		Window* myWindow;

		std::unique_ptr<DirectX12::RenderPassDescriptorHeap> myCBV_SRVHeap;
#endif
	};
}
