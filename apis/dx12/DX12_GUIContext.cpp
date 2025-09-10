// Filter "GUI"
#include "DX12_GUIContext.hpp"
#include "DX12_Device.hpp"
#include "DX12_Manager.hpp"

#include <Atrium_Diagnostics.hpp>

#if IS_IMGUI_ENABLED
#include <backends/imgui_impl_dx12.h>
#endif

namespace Atrium::DirectX12
{
	GUIBackendContext::GUIBackendContext(DirectX12API& aGraphicsAPI, FrameGraphicsContext& aGraphicsContext, const std::shared_ptr<Atrium::RenderTexture>& aRenderTarget)
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
				GUIBackendContext* context = reinterpret_cast<GUIBackendContext*>(initInfo->UserData);

				DescriptorHeapHandle handle = context->myCBV_SRVHeap->GetHeapHandleBlock(1);
				if (Debug::Verify(outCPUHandle != nullptr, "There is a CPU handle out pointer."))
					(*outCPUHandle) = handle.GetCPUHandle();
				if (Debug::Verify(outGPUHandle != nullptr, "There is a GPU handle out pointer."))
					(*outGPUHandle) = handle.GetGPUHandle();

				context->myCBV_SRVHeapHandles.insert({ handle.GetCPUHandle().ptr, handle });
			};

		initInfo.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* initInfo, D3D12_CPU_DESCRIPTOR_HANDLE aCPUHandle, D3D12_GPU_DESCRIPTOR_HANDLE)
			{
				GUIBackendContext* context = reinterpret_cast<GUIBackendContext*>(initInfo->UserData);
				context->myCBV_SRVHeapHandles.erase(aCPUHandle.ptr);
			};

		ImGui_ImplDX12_Init(&initInfo);
		#endif
	}

	GUIBackendContext::~GUIBackendContext()
	{
		#if IS_IMGUI_ENABLED
		ImGui_ImplDX12_Shutdown();
		#endif
	}

	void GUIBackendContext::MarkFrameStart()
	{
		#if IS_IMGUI_ENABLED
		ImGui_ImplDX12_NewFrame();
		#endif
	}

	void GUIBackendContext::MarkFrameEnd()
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
