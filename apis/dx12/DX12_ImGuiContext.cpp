// Filter "ImGui"
#include "DX12_ImGuiContext.hpp"
#include "DX12_Device.hpp"
#include "DX12_Manager.hpp"

#include <Core_Diagnostics.hpp>

#if IS_IMGUI_ENABLED
#include <backends/imgui_impl_dx12.h>
#endif

namespace Atrium::DirectX12
{
	ImGuiContext::ImGuiContext(DirectX12API& aGraphicsAPI, FrameGraphicsContext& aGraphicsContext, const std::shared_ptr<Core::RenderTexture>& aRenderTarget)
		: myGraphicsContext(aGraphicsContext)
		, myRenderTarget(aRenderTarget)
	{
		[[maybe_unused]] DirectX12::DirectX12API& dxAPI = static_cast<DirectX12::DirectX12API&>(aGraphicsAPI);
		#if IS_IMGUI_ENABLED
		DirectX12::ComPtr<ID3D12Device> dxDevice = dxAPI.GetDevice().GetDevice();

		myCBV_SRVHeap.reset(
			new DirectX12::RenderPassDescriptorHeap(
				dxDevice,
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
				1
			)
		);

		// Imgui internally uses num_frames_in_flight for amount of back-buffers, which should be
		// one above the amount of actual frames in flight, so increasing the number by 1 to keep it working even with only 1 frame in flight.
		// Additionally, depending on the swapchain swap-effect, they may require a minimum of 2 or crash on init.

		ImGui_ImplDX12_Init(
			dxDevice.Get(), static_cast<int>(dxAPI.GetFramesInFlightAmount() + 1),
			DirectX12::ToDXGIFormat(aRenderTarget->GetDescriptor().ColorGraphicsFormat),
			myCBV_SRVHeap->GetHeap().Get(),
			myCBV_SRVHeap->GetHeapCPUStart(),
			myCBV_SRVHeap->GetHeapGPUStart()
		);
		#endif
	}

	ImGuiContext::~ImGuiContext()
	{
		#if IS_IMGUI_ENABLED
		ImGui_ImplDX12_Shutdown();
		#endif
	}

	void ImGuiContext::MarkFrameStart()
	{
		#if IS_IMGUI_ENABLED
		ImGui_ImplDX12_NewFrame();
		#endif
	}

	void ImGuiContext::MarkFrameEnd()
	{
		#if IS_IMGUI_ENABLED
		myGraphicsContext.SetRenderTargets({ myRenderTarget }, nullptr);

		ID3D12GraphicsCommandList* commandList = myGraphicsContext.GetCommandList();
		commandList->SetDescriptorHeaps(1, myCBV_SRVHeap->GetHeap().GetAddressOf());
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
		ImGui::RenderPlatformWindowsDefault(nullptr, (void*)commandList);
		#endif
	}
}
