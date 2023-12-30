// Filter "Commands"

#include "DX12_CommandBuffer.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_Pipeline.hpp"

namespace RoseGold::DirectX12
{
	ResolvedCommandBuffer::ResolvedCommandBuffer(FrameGraphicsContext& aGraphicsContext)
		: myGraphicsContext(aGraphicsContext)
		, myLastProjectionMatrix(Math::Matrix::Identity())
		, myLastViewMatrix(Math::Matrix::Identity())
	{ }

	void ResolvedCommandBuffer::Resolve(const CommandBuffer& aCommandBuffer)
	{
		aCommandBuffer.RerecordTo(*this);
	}

	void ResolvedCommandBuffer::Clear(std::shared_ptr<Core::Graphics::RenderTexture> aTarget, Color aClearColor)
	{
		myGraphicsContext.AddBarrier(static_cast<RenderTarget&>(*aTarget).GetGPUResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		myGraphicsContext.FlushBarriers();

		myGraphicsContext.ClearColor(*aTarget, aClearColor);
	}

	void ResolvedCommandBuffer::Clear(std::shared_ptr<Core::Graphics::RenderTexture> aTarget, float aClearDepth)
	{
		myGraphicsContext.AddBarrier(static_cast<RenderTarget&>(*aTarget).GetGPUResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		myGraphicsContext.FlushBarriers();

		myGraphicsContext.ClearDepth(*aTarget, aClearDepth, 0);
	}

	void ResolvedCommandBuffer::Clear(std::shared_ptr<Core::Graphics::RenderTexture> aTarget, Color aClearColor, float aClearDepth)
	{
		myGraphicsContext.AddBarrier(static_cast<RenderTarget&>(*aTarget).GetGPUResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		myGraphicsContext.FlushBarriers();

		myGraphicsContext.ClearColor(*aTarget, aClearColor);
		myGraphicsContext.ClearDepth(*aTarget, aClearDepth, 0);
	}

	void ResolvedCommandBuffer::DisableScissorRect()
	{
		myGraphicsContext.DisableScissorRect();
	}

	void ResolvedCommandBuffer::DrawMesh(std::shared_ptr<Core::Graphics::Mesh> aMesh, Math::Matrix aMatrix, int aSubmeshIndex)
	{
		aSubmeshIndex;

		struct CameraConstants
		{
			Math::Matrix Projection;
			Math::Matrix Model;
			Math::Matrix View;
		} cameraConstants;

		cameraConstants.Model = aMatrix;
		cameraConstants.Projection = myLastProjectionMatrix;
		cameraConstants.View = myLastViewMatrix;

		myGraphicsContext.SetConstantBufferData(&cameraConstants, sizeof(CameraConstants));

		std::shared_ptr<const VertexBuffer> vertexBuffer = std::static_pointer_cast<const VertexBuffer>(aMesh->GetVertexBuffer());
		myGraphicsContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myGraphicsContext.SetVertexBuffer(0, *vertexBuffer);
		//myGraphicsContext.SetIndexBuffer(&indexBufferView);

		myGraphicsContext.Draw(vertexBuffer->GetCount(), 0);
	}

	void ResolvedCommandBuffer::SetPipelineState(std::shared_ptr<Core::Graphics::CachedPipelineState> aPipelineState)
	{
		DirectX12::CachedPipelineState& pipelineState = static_cast<DirectX12::CachedPipelineState&>(*aPipelineState);

		for (const std::shared_ptr<Core::Graphics::RenderTexture>& outputTexture : pipelineState.Outputs)
		{
			RenderTarget* renderTarget = static_cast<RenderTarget*>(outputTexture.get());
			myGraphicsContext.AddBarrier(renderTarget->GetGPUResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		}

		myGraphicsContext.FlushBarriers();

		myGraphicsContext.SetPipelineState(pipelineState);
	}

	void ResolvedCommandBuffer::SetProjectionMatrix(const Math::Matrix& aMatrix)
	{
		myLastProjectionMatrix = aMatrix;
	}

	void ResolvedCommandBuffer::SetScissorRect(const Math::RectangleT<int>& aRectangle)
	{
		myGraphicsContext.SetScissorRect(aRectangle);
	}

	void ResolvedCommandBuffer::SetViewMatrix(const Math::Matrix& aMatrix)
	{
		myLastViewMatrix = aMatrix.Invert().value_or(Math::Matrix::Identity());
	}

	void ResolvedCommandBuffer::SetViewport(const Math::Rectangle& aRectangle)
	{
		myGraphicsContext.SetViewport(aRectangle);
	}
}
