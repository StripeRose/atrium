// Filter "Commands"

#include "DX12_CommandBuffer.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_Mesh.hpp"
#include "DX12_Pipeline.hpp"

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

	void ResolvedCommandBuffer::DrawMesh(std::shared_ptr<Core::Graphics::Mesh> aMesh, Math::Matrix aMatrix, std::shared_ptr<Core::Graphics::CachedPipelineState> aPipelineState, int aSubmeshIndex)
	{
		aMatrix;
		aSubmeshIndex;

		myCommandList->SetGraphicsRootSignature(myDevice->GetPipeline().GetRootSignature());

		CachedPipelineState* pso = static_cast<CachedPipelineState*>(aPipelineState.get());
		myCommandList->SetPipelineState(pso->myPipelineState.Get());

		Mesh* dxMesh = static_cast<Mesh*>(aMesh.get());
		std::shared_ptr<VertexBuffer> vertexBuffer = dxMesh->GetVertexBuffer();

		myCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myCommandList->IASetVertexBuffers(0, 1, &vertexBuffer->GetBufferView());

		//myCommandList->IASetIndexBuffer(&indexBufferView);
		//myCommandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
		myCommandList->DrawInstanced(vertexBuffer->GetCount(), 1, 0, 0);
	}

	void ResolvedCommandBuffer::SetScissorRect(const Math::RectangleT<int>& aRectangle)
	{
		const Math::Vector2T<int> halfSize = (aRectangle.Size / 2);

		D3D12_RECT rect;
		rect.left   = aRectangle.Center.X - halfSize.X;
		rect.right  = aRectangle.Center.X + halfSize.X;
		rect.top    = aRectangle.Center.Y - halfSize.Y;
		rect.bottom = aRectangle.Center.Y + halfSize.Y;

		myCommandList->RSSetScissorRects(1, &rect);
	}

	void ResolvedCommandBuffer::SetRenderTarget(std::shared_ptr<Core::Graphics::RenderTexture> aTexture)
	{
		if (!aTexture)
			return;

		RenderTarget* target = static_cast<RenderTarget*>(aTexture.get());

		if (target->GetColorView())
			myLastColorHandle = target->GetColorView()->GetCPUHandle();
		if (target->GetDepthStencilView())
			myLastDepthHandle = target->GetDepthStencilView()->GetCPUHandle();
		if (target->GetColorView() && target->GetDepthStencilView())
			myTargetTextures.push_back(aTexture);

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
	}

	void ResolvedCommandBuffer::SetViewport(const Math::Rectangle& aRectangle)
	{
		const Math::Vector2 topLeft = aRectangle.Center - (aRectangle.Size * 0.5f);

		D3D12_VIEWPORT viewport;
		viewport.TopLeftX = topLeft.X;
		viewport.TopLeftY = topLeft.Y;
		viewport.Width = aRectangle.Size.X;
		viewport.Height = aRectangle.Size.Y;
		viewport.MinDepth = 0.1f;
		viewport.MaxDepth = 1.0f;

		myCommandList->RSSetViewports(1, &viewport);
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
		if (myTargetTextures.empty())
			return;

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
