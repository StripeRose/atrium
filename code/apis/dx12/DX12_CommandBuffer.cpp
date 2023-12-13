// Filter "Commands"

#include "DX12_CommandBuffer.hpp"
#include "DX12_Diagnostics.hpp"

namespace RoseGold::DirectX12
{
	ResolvedCommandBuffer::ResolvedCommandBuffer(Device& aDevice, ID3D12CommandAllocator* aCommandAllocator)
		: myLastColorHandle({ NULL })
		, myLastDepthHandle({ NULL })
		, myDevice(&aDevice)
	{
		AssertSuccess(
			aDevice.GetDevice()->CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				aCommandAllocator,
				nullptr,
				IID_PPV_ARGS(myCommandList.ReleaseAndGetAddressOf())
			)
		);
	}

	void ResolvedCommandBuffer::Resolve(const CommandBuffer& aCommandBuffer)
	{
		aCommandBuffer.RerecordTo(*this);
		CloseTargets();
	}

	void ResolvedCommandBuffer::Clear(Color aClearColor)
	{
		Clear_Internal(aClearColor);
	}

	void ResolvedCommandBuffer::Clear(float aClearDepth)
	{
		Clear_Internal(aClearDepth);
	}

	void ResolvedCommandBuffer::Clear(Color aClearColor, float aClearDepth)
	{
		Clear_Internal(aClearColor);
		Clear_Internal(aClearDepth);
	}

	void ResolvedCommandBuffer::SetRenderTarget(std::shared_ptr<Core::Graphics::RenderTexture> aTexture)
	{
		if (!aTexture)
			return;

		myTargetTextures.push_back(aTexture);

		RenderTarget* target = static_cast<RenderTarget*>(aTexture.get());

		if (target->GetColorResource())
		{
			D3D12_RESOURCE_BARRIER targetBarrier = { };
			targetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			targetBarrier.Transition.pResource = target->GetColorResource();
			targetBarrier.Transition.StateBefore = target->IsSwapChain() ? D3D12_RESOURCE_STATE_PRESENT : D3D12_RESOURCE_STATE_COMMON;
			targetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			myCommandList->ResourceBarrier(1, &targetBarrier);

			D3D12_CPU_DESCRIPTOR_HANDLE colorHandle = target->GetColorView()->GetCPUHandle();
			D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle = target->GetDepthStencilView()->GetCPUHandle();
			myCommandList->OMSetRenderTargets(1, &colorHandle, false, &depthStencilHandle);
		}

		if (target->GetColorView())
			myLastColorHandle = target->GetColorView()->GetCPUHandle();
		if (target->GetDepthStencilView())
			myLastDepthHandle = target->GetDepthStencilView()->GetCPUHandle();
	}

	void ResolvedCommandBuffer::Clear_Internal(Color aClearColor)
	{
		if (myLastColorHandle.ptr != NULL)
		{
			float color[4] = {
				aClearColor.R,
				aClearColor.G,
				aClearColor.B,
				aClearColor.A
			};
			myCommandList->ClearRenderTargetView(
				myLastColorHandle,
				color,
				0,
				nullptr
			);
		}
	}

	void ResolvedCommandBuffer::Clear_Internal(float aClearDepth)
	{
		if (myLastDepthHandle.ptr != NULL)
		{
			myCommandList->ClearDepthStencilView(
				myLastDepthHandle,
				D3D12_CLEAR_FLAG_DEPTH,
				aClearDepth,
				0, 0, nullptr
			);
		}
	}

	void ResolvedCommandBuffer::CloseTargets()
	{
		std::vector<D3D12_RESOURCE_BARRIER> barriers;
		for (const auto& target : myTargetTextures)
		{
			RenderTarget* renderTarget = static_cast<RenderTarget*>(target.get());

			D3D12_RESOURCE_BARRIER& barrier = barriers.emplace_back();
			barrier.Transition.pResource = renderTarget->GetColorResource();
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = renderTarget->IsSwapChain() ? D3D12_RESOURCE_STATE_PRESENT : D3D12_RESOURCE_STATE_COMMON;
		}

		myCommandList->ResourceBarrier(static_cast<UINT>(barriers.size()), &barriers.front());
	}
}
