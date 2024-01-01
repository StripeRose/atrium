// Filter "Commands"

#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_Device.hpp"
#include "DX12_FrameContext.hpp"

#include "Graphics_CommandBuffer.hpp"

#include <d3d12.h>
#include <vector>

namespace RoseGold::DirectX12
{
	class Device;
	class SwapChain;
	class ResolvedCommandBuffer : private Core::Graphics::CommandBuffer
	{
	public:
		ResolvedCommandBuffer(FrameGraphicsContext& aGraphicsContext);
		void Resolve(const CommandBuffer& aCommandBuffer);

		// Implements CommandBuffer
	private:
		void Clear(std::shared_ptr<Core::Graphics::RenderTexture> aTarget, Color aClearColor) override;
		void Clear(std::shared_ptr<Core::Graphics::RenderTexture> aTarget, float aClearDepth) override;
		void Clear(std::shared_ptr<Core::Graphics::RenderTexture> aTarget, Color aClearColor, float aClearDepth) override;
		void DisableScissorRect() override;
		void DrawMesh(std::shared_ptr<Core::Graphics::Mesh> aMesh, Math::Matrix aMatrix, int aSubmeshIndex) override;
		void SetPipelineState(std::shared_ptr<Core::Graphics::PipelineState> aPipelineState) override;
		void SetProjectionMatrix(const Math::Matrix& aMatrix) override;
		void SetScissorRect(const Math::RectangleT<int>& aRectangle) override;
		void SetViewMatrix(const Math::Matrix& aMatrix) override;
		void SetViewport(const Math::Rectangle& aRectangle) override;

	private:
		FrameGraphicsContext& myGraphicsContext;

		Math::Matrix myLastProjectionMatrix;
		Math::Matrix myLastViewMatrix;
	};
}