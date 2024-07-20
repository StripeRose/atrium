#pragma once

#include <Graphics_Enums.hpp>
#include <Graphics_Manager.hpp>
#include <Platform_WindowManagement.hpp>

#if IS_IMGUI_ENABLED
#include <DX12_DescriptorHeap.hpp>
#endif

namespace RoseGold
{
	class ImGuiHandler
	{
	public:
		ImGuiHandler(Core::GraphicsAPI& aGraphicsAPI, Core::Window& aPrimaryWindow, std::shared_ptr<Core::RenderTexture> aTarget);
		~ImGuiHandler();

		void MarkFrameStart();
		void MarkFrameEnd();
		
#if IS_IMGUI_ENABLED
	private:
		void InitForWindow(Core::Window& aWindow);
		void SetupBackend(Core::GraphicsAPI& aGraphicsAPI, Core::Window& aWindow, Core::GraphicsFormat aTargetFormat);
		void Cleanup();

		void StyleColorsNord();

		Core::GraphicsAPI& myGraphicsAPI;
		std::shared_ptr<Core::RenderTexture> myRenderTarget;
		Core::Window* myWindow;

		std::unique_ptr<DirectX12::RenderPassDescriptorHeap> myCBV_SRVHeap;
#endif
	};
}
