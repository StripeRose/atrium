// Filter "Commands"

#include "DX12_CommandBuffer.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_Pipeline.hpp"

namespace RoseGold::DirectX12
{
	ResolvedCommandBuffer::ResolvedCommandBuffer(Device& aDevice, ID3D12CommandAllocator* aCommandAllocator)
		: myLastColorHandle({ NULL })
		, myLastDepthHandle({ NULL })
		, myDevice(&aDevice)
		, myLastProjectionMatrix(Math::Matrix::Identity())
		, myLastViewMatrix(Math::Matrix::Identity())
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

	void ResolvedCommandBuffer::DisableScissorRect()
	{
		myCommandList->RSSetScissorRects(0, nullptr);
	}

	void ResolvedCommandBuffer::DrawMesh(std::shared_ptr<Core::Graphics::Mesh> aMesh, Math::Matrix aMatrix, std::shared_ptr<Core::Graphics::CachedPipelineState> aPipelineState, int aSubmeshIndex)
	{
		aSubmeshIndex;

		myCommandList->SetGraphicsRootSignature(myDevice->GetPipeline().GetRootSignature());

		{
			struct CameraConstants
			{
				Math::Matrix Projection;
				Math::Matrix Model;
				Math::Matrix View;
			} cameraConstants;

			cameraConstants.Model = aMatrix;
			cameraConstants.Projection = myLastProjectionMatrix;
			cameraConstants.View = myLastViewMatrix;

			std::shared_ptr<ConstantBuffer> cameraConstantBuffer = std::static_pointer_cast<ConstantBuffer>(myDevice->GetPipeline().CreateFrameConstantBuffer(sizeof(CameraConstants)));
			cameraConstantBuffer->SetData(&cameraConstants, sizeof(CameraConstants));

			RenderPassDescriptorHeap& renderPassHeap = myDevice->GetDescriptorHeapManager().GetFrameHeap();
			std::shared_ptr<DescriptorHeapHandle> mvpBufferHandle = renderPassHeap.GetHeapHandleBlock(1);

			myDevice->GetDevice()->CopyDescriptorsSimple(
				1,
				mvpBufferHandle->GetCPUHandle(),
				cameraConstantBuffer->GetViewHandle().GetCPUHandle(),
				renderPassHeap.GetHeapType()
			);

			ID3D12DescriptorHeap* descriptorHeaps[] = { renderPassHeap.GetHeap().Get() };
			myCommandList->SetDescriptorHeaps(1, descriptorHeaps);
			myCommandList->SetGraphicsRootDescriptorTable(0, mvpBufferHandle->GetGPUHandle());
		}

		CachedPipelineState* pso = static_cast<CachedPipelineState*>(aPipelineState.get());
		myCommandList->SetPipelineState(pso->PipelineState.Get());

		std::shared_ptr<const VertexBuffer> vertexBuffer = std::static_pointer_cast<const VertexBuffer>(aMesh->GetVertexBuffer());

		myCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myCommandList->IASetVertexBuffers(0, 1, &vertexBuffer->GetBufferView());

		//myCommandList->IASetIndexBuffer(&indexBufferView);
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

	void ResolvedCommandBuffer::SetProjectionMatrix(const Math::Matrix& aMatrix)
	{
		myLastProjectionMatrix = aMatrix;
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
			D3D12_RESOURCE_BARRIER barrier = target->GetGPUResource().UpdateUsageState(D3D12_RESOURCE_STATE_RENDER_TARGET);
			myCommandList->ResourceBarrier(1, &barrier);

			D3D12_CPU_DESCRIPTOR_HANDLE colorHandle = target->GetColorView()->GetCPUHandle();
			D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle = target->GetDepthStencilView()->GetCPUHandle();
			myCommandList->OMSetRenderTargets(1, &colorHandle, false, &depthStencilHandle);
		}
	}

	void ResolvedCommandBuffer::SetViewMatrix(const Math::Matrix& aMatrix)
	{
		myLastViewMatrix = aMatrix.Invert().value_or(Math::Matrix::Identity());
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
			barriers.push_back(
				renderTarget->GetGPUResource().UpdateUsageState(
					renderTarget->IsSwapChain() ? D3D12_RESOURCE_STATE_PRESENT : D3D12_RESOURCE_STATE_COMMON
				));
		}

		myCommandList->ResourceBarrier(static_cast<UINT>(barriers.size()), &barriers.front());
	}
}
