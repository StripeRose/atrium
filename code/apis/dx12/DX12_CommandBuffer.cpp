// Filter "Commands"

#include "DX12_CommandBuffer.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_Pipeline.hpp"
#include "DX12_RenderTexture.hpp"

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

	void ResolvedCommandBuffer::Clear(const std::shared_ptr<Core::Graphics::RenderTexture>& aTarget, Color aClearColor)
	{
		Debug::Assert(!!aTarget, "Clear(aRenderTarget, aClearColor) requires target to be non-null.");

		myGraphicsContext.AddBarrier(static_cast<RenderTarget&>(*aTarget).GetGPUResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		myGraphicsContext.FlushBarriers();

		myGraphicsContext.ClearColor(*aTarget, aClearColor);
	}

	void ResolvedCommandBuffer::Clear(const std::shared_ptr<Core::Graphics::RenderTexture>& aTarget, float aClearDepth)
	{
		Debug::Assert(!!aTarget, "Clear(aRenderTarget, aClearDepth) requires target to be non-null.");

		myGraphicsContext.AddBarrier(static_cast<RenderTarget&>(*aTarget).GetGPUResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		myGraphicsContext.FlushBarriers();

		myGraphicsContext.ClearDepth(*aTarget, aClearDepth, 0);
	}

	void ResolvedCommandBuffer::Clear(const std::shared_ptr<Core::Graphics::RenderTexture>& aTarget, Color aClearColor, float aClearDepth)
	{
		Debug::Assert(!!aTarget, "Clear(aRenderTarget, aClearColor, aClearDepth) requires target to be non-null.");

		myGraphicsContext.AddBarrier(static_cast<RenderTarget&>(*aTarget).GetGPUResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		myGraphicsContext.FlushBarriers();

		myGraphicsContext.ClearColor(*aTarget, aClearColor);
		myGraphicsContext.ClearDepth(*aTarget, aClearDepth, 0);
	}

	void ResolvedCommandBuffer::DisableScissorRect()
	{
		myGraphicsContext.DisableScissorRect();
	}

	void ResolvedCommandBuffer::DrawMesh(const std::shared_ptr<Core::Graphics::Mesh>& aMesh, Math::Matrix aMatrix, const std::shared_ptr<Core::Graphics::Texture>& aTexture, int aSubmeshIndex)
	{
		Debug::Assert(!!aMesh, "DrawMesh(aMesh, aMatrix, aTexture, aSubmeshIndex) requires mesh to be non-null.");

		std::shared_ptr<const VertexBuffer> vertexBuffer = std::static_pointer_cast<const VertexBuffer>(aMesh->GetVertexBuffer());
		if (!vertexBuffer->IsReady())
			return;

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

		myGraphicsContext.SetPipelineResource(
			RootParameterUpdateFrequency::PerObject,
			0,
			&cameraConstants,
			sizeof(CameraConstants));

		if (aTexture)
		{
			myGraphicsContext.SetPipelineResource(
				RootParameterUpdateFrequency::PerMaterial,
				0,
				aTexture
			);
		}

		myGraphicsContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myGraphicsContext.SetPipelineResource(*vertexBuffer);
		//myGraphicsContext.SetIndexBuffer(&indexBufferView);

		myGraphicsContext.Draw(vertexBuffer->GetCount(), 0);
	}

	void ResolvedCommandBuffer::SetPipelineState(const std::shared_ptr<Core::Graphics::PipelineState>& aPipelineState)
	{
		Debug::Assert(!!aPipelineState, "SetPipelineState(aPipelineState) requires pipeline state to be non-null.");
		myGraphicsContext.SetPipelineState(static_cast<DirectX12::PipelineState&>(*aPipelineState));
	}

	void ResolvedCommandBuffer::SetProjectionMatrix(const Math::Matrix& aMatrix)
	{
		myLastProjectionMatrix = aMatrix;
	}

	void ResolvedCommandBuffer::SetRenderTarget(const std::shared_ptr<Core::Graphics::RenderTexture>& aRenderTarget)
	{
		Debug::Assert(!!aRenderTarget, "SetRenderTarget(aRenderTarget) requires target to be non-null.");

		RenderTarget* dxRenderTarget = static_cast<RenderTarget*>(aRenderTarget.get());
		myGraphicsContext.AddBarrier(dxRenderTarget->GetGPUResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);

		RenderTarget* depthTarget = dxRenderTarget->GetDepthResource() ? dxRenderTarget : nullptr;
		if (depthTarget)
			myGraphicsContext.AddBarrier(dxRenderTarget->GetDepthGPUResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE);
		
		myGraphicsContext.FlushBarriers();

		myGraphicsContext.SetRenderTargets({ dxRenderTarget }, depthTarget);
	}

	void ResolvedCommandBuffer::SetRenderTarget(const std::shared_ptr<Core::Graphics::RenderTexture>& aRenderTarget, const std::shared_ptr<Core::Graphics::RenderTexture>& aDepthTarget)
	{
		Debug::Assert(aRenderTarget && aDepthTarget, "SetRenderTarget(aRenderTarget, aDepthTarget) requires targets to be non-null.");

		RenderTarget* dxRenderTarget = static_cast<RenderTarget*>(aRenderTarget.get());
		myGraphicsContext.AddBarrier(dxRenderTarget->GetGPUResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);

		RenderTarget* dxDepthTarget = static_cast<RenderTarget*>(aDepthTarget.get());
		myGraphicsContext.AddBarrier(dxDepthTarget->GetDepthGPUResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE);

		myGraphicsContext.FlushBarriers();

		myGraphicsContext.SetRenderTargets({ dxRenderTarget }, dxDepthTarget);
	}

	void ResolvedCommandBuffer::SetRenderTarget(const std::initializer_list<const std::shared_ptr<Core::Graphics::RenderTexture>>& someRenderTargets)
	{
		std::vector<RenderTarget*> dxRenderTargets;
		RenderTarget* depthTarget = nullptr;
		for (const auto& target : someRenderTargets)
		{
			Debug::Assert(!!target, "SetRenderTarget(someRenderTargets) requires targets to be non-null.");

			RenderTarget* dxRenderTarget = static_cast<RenderTarget*>(target.get());
			myGraphicsContext.AddBarrier(dxRenderTarget->GetGPUResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
			dxRenderTargets.push_back(dxRenderTarget);

			if (!depthTarget)
			{
				depthTarget = dxRenderTarget->GetDepthResource() ? dxRenderTarget : nullptr;
				if (depthTarget)
					myGraphicsContext.AddBarrier(dxRenderTarget->GetDepthGPUResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE);
			}
		}

		myGraphicsContext.FlushBarriers();

		myGraphicsContext.SetRenderTargets(dxRenderTargets, depthTarget);
	}

	void ResolvedCommandBuffer::SetRenderTarget(const std::initializer_list<const std::shared_ptr<Core::Graphics::RenderTexture>>& someRenderTargets, const std::shared_ptr<Core::Graphics::RenderTexture>& aDepthTarget)
	{
		std::vector<RenderTarget*> dxRenderTargets;
		for (const auto& target : someRenderTargets)
		{
			Debug::Assert(!!target, "SetRenderTarget(someRenderTargets, aDepthTarget) requires targets to be non-null.");

			RenderTarget* dxRenderTarget = static_cast<RenderTarget*>(target.get());
			myGraphicsContext.AddBarrier(dxRenderTarget->GetGPUResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
			dxRenderTargets.push_back(dxRenderTarget);
		}

		Debug::Assert(!!aDepthTarget, "SetRenderTarget(someRenderTargets, aDepthTarget) requires targets to be non-null.");
		RenderTarget* dxDepthTarget = static_cast<RenderTarget*>(aDepthTarget.get());
		myGraphicsContext.AddBarrier(dxDepthTarget->GetDepthGPUResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE);

		myGraphicsContext.FlushBarriers();

		myGraphicsContext.SetRenderTargets(dxRenderTargets, dxDepthTarget);
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
