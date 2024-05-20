#pragma once

#include <imgui.h>

#include <Graphics_Enums.hpp>
#include <Graphics_Manager.hpp>
#include <Platform_WindowManagement.hpp>

#include <DX12_DescriptorHeap.hpp>

namespace RoseGold
{
	class ImGuiHandler
	{
	public:
		ImGuiHandler(Core::GraphicsAPI& aGraphicsAPI, Core::Window& aPrimaryWindow, std::shared_ptr<Core::RenderTexture> aTarget);
		~ImGuiHandler();

		void MarkFrameStart();
		void MarkFrameEnd();
		
	private:
		void InitForWindow(Core::Window& aWindow);
		void SetupBackend(Core::GraphicsAPI& aGraphicsAPI, Core::Window& aWindow, Core::GraphicsFormat aTargetFormat);

		void StyleColorsNord();

		Core::GraphicsAPI& myGraphicsAPI;
		std::shared_ptr<Core::RenderTexture> myRenderTarget;

		std::unique_ptr<DirectX12::RenderPassDescriptorHeap> myCBV_SRVHeap;
	};
}
