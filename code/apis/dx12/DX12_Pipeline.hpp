// Filter "Resources"

#pragma once

#include "DX12_ComPtr.hpp"

#include <Graphics_Buffer.hpp>
#include <Graphics_Pipeline.hpp>

#include <map>
#include <memory>
#include <vector>

namespace RoseGold::DirectX12
{
	class CachedPipelineState : public Core::Graphics::CachedPipelineState
	{
	public:
		ComPtr<ID3D12PipelineState> myPipelineState;
	};

	class Device;
	class Pipeline
	{
	public:
		Pipeline(Device& aDevice);

		void MarkFrameEnd();

		std::shared_ptr<CachedPipelineState> CreateOrGetState(const Core::Graphics::PipelineState& aPipelineState);

		ID3D12RootSignature* GetRootSignature() { return myRootSignature.Get(); }

		std::shared_ptr<Core::Graphics::GraphicsBuffer> CreateFrameConstantBuffer(std::uint32_t aBufferSize);
		
	private:
		void SetupRootSignature();

		Device& myDevice;

		ComPtr<ID3D12RootSignature> myRootSignature;
		std::vector<std::shared_ptr<Core::Graphics::GraphicsBuffer>> myFrameConstantBuffers;
	};
}