// Filter "Backends"

#if IS_IMGUI_BACKEND_DIRECTX12
#include "DearImGui_DirectX12.hpp"

#include "DX12_Device.hpp"

#include <Atrium_AtriumApplication.hpp>
#include <Atrium_Diagnostics.hpp>

#include <backends/imgui_impl_dx12.h>

namespace Atrium::Extension
{
	DearImGuiBackendContext_DirectX12::DearImGuiBackendContext_DirectX12(const std::shared_ptr<Atrium::RenderTexture>& aRenderTarget)
		: myRenderTarget(aRenderTarget)
	{
		DirectX12::DirectX12API& dxAPI = static_cast<DirectX12::DirectX12API&>(AtriumApplication::GetRunningInstance()->GetGraphicsHandler());
		DirectX12::ComPtr<ID3D12Device> dxDevice = dxAPI.GetDevice().GetDevice();

		myCBV_SRVHeap.reset(
			new DirectX12::RenderPassDescriptorHeap(
				dxDevice,
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
				8
			)
		);

		// Imgui internally uses num_frames_in_flight for amount of back-buffers, which should be
		// one above the amount of actual frames in flight, so increasing the number by 1 to keep it working even with only 1 frame in flight.
		// Additionally, depending on the swapchain swap-effect, they may require a minimum of 2 or crash on init.
		//
		// Todo: Find a proper reference for this.

		ImGui_ImplDX12_InitInfo initInfo = {};
		initInfo.Device = dxDevice.Get();
		initInfo.CommandQueue = dxAPI.GetCommandQueueManager().GetGraphicsQueue().GetCommandQueue().Get();
		initInfo.NumFramesInFlight = static_cast<int>(dxAPI.GetFramesInFlightAmount() + 1);
		initInfo.RTVFormat = DirectX12::ToDXGIFormat(aRenderTarget->GetDescriptor().ColorGraphicsFormat);

		initInfo.UserData = this;
		initInfo.SrvDescriptorHeap = myCBV_SRVHeap->GetHeap().Get();
		initInfo.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* initInfo, D3D12_CPU_DESCRIPTOR_HANDLE* outCPUHandle, D3D12_GPU_DESCRIPTOR_HANDLE* outGPUHandle)
			{
				DearImGuiBackendContext_DirectX12* context = reinterpret_cast<DearImGuiBackendContext_DirectX12*>(initInfo->UserData);

				DirectX12::DescriptorHeapHandle handle = context->myCBV_SRVHeap->GetHeapHandleBlock(1);
				if (Debug::Verify(outCPUHandle != nullptr, "There is a CPU handle out pointer."))
					(*outCPUHandle) = handle.GetCPUHandle();
				if (Debug::Verify(outGPUHandle != nullptr, "There is a GPU handle out pointer."))
					(*outGPUHandle) = handle.GetGPUHandle();

				context->myCBV_SRVHeapHandles.insert({ handle.GetCPUHandle().ptr, handle });
			};

		initInfo.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* initInfo, D3D12_CPU_DESCRIPTOR_HANDLE aCPUHandle, D3D12_GPU_DESCRIPTOR_HANDLE)
			{
				DearImGuiBackendContext_DirectX12* context = reinterpret_cast<DearImGuiBackendContext_DirectX12*>(initInfo->UserData);
				context->myCBV_SRVHeapHandles.erase(aCPUHandle.ptr);
			};

		ImGui_ImplDX12_Init(&initInfo);
	}

	DearImGuiBackendContext_DirectX12::~DearImGuiBackendContext_DirectX12()
	{
		ImGui_ImplDX12_Shutdown();
	}

	void DearImGuiBackendContext_DirectX12::MarkFrameStart()
	{
		ImGui_ImplDX12_NewFrame();
	}

	void DearImGuiBackendContext_DirectX12::Render(Atrium::FrameGraphicsContext& aFrameContext)
	{
		aFrameContext.SetRenderTargets({ myRenderTarget }, nullptr);

		ID3D12GraphicsCommandList* commandList = static_cast<DirectX12::FrameGraphicsContext&>(aFrameContext).GetCommandList();
		commandList->SetDescriptorHeaps(1, myCBV_SRVHeap->GetHeap().GetAddressOf());
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
		ImGui::RenderPlatformWindowsDefault(nullptr, (void*)commandList);
	}
}

#endif