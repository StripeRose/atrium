// Filter "Commands"

#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_Device.hpp"

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
		ResolvedCommandBuffer(Device& aDevice, ID3D12CommandAllocator* aCommandAllocator);

		void Resolve(const CommandBuffer& aCommandBuffer);

		ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return myCommandList; }

		const std::vector<std::shared_ptr<Core::Graphics::RenderTexture>>& GetUsedTargets() const { return myTargetTextures; }

		// Implements CommandBuffer
	private:
		void Clear(Color aClearColor) override;
		void Clear(float aClearDepth) override;
		void Clear(Color aClearColor, float aClearDepth) override;
		void SetRenderTarget(std::shared_ptr<Core::Graphics::RenderTexture> aTexture) override;

		/*void Clear(System::Color aClearColor) override;
		void Clear(float aClearDepth) override;
		void Clear(System::Color aClearColor, float aClearDepth) override;
		void CustomCommand(std::function<void(CommandBuffer&)> aCallback) override;
		void DisableScissorRect() override;
		void SetScissorRect(const System::Math::Rectangle<float>& aRectangle) override;
		void SetProjectionMatrix(const System::Math::Matrix& aMatrix) override;
		void SetRenderTarget(Resource<RenderTexture> aTexture) override;
		void SetViewMatrix(const System::Math::Matrix& aMatrix) override;
		void SetViewport(const System::Math::Rectangle<float>& aRectangle) override;
		void Reset() override;*/

	private:
		void Clear_Internal(Color aClearColor);
		void Clear_Internal(float aClearDepth);

		void CloseTargets();

	private:
		Device* myDevice;
		ComPtr<ID3D12GraphicsCommandList> myCommandList;

		D3D12_CPU_DESCRIPTOR_HANDLE myLastColorHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE myLastDepthHandle;

		std::vector<std::shared_ptr<Core::Graphics::RenderTexture>> myTargetTextures;
	};
}