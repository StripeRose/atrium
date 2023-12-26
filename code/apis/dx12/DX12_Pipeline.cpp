// Filter "Resource"

#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_Pipeline.hpp"
#include "DX12_Shader.hpp"

namespace RoseGold::DirectX12
{
	Pipeline::Pipeline(Device& aDevice)
		: myDevice(aDevice)
	{
		SetupRootSignature();
	}

	void Pipeline::MarkFrameEnd()
	{
		myFrameConstantBuffers.clear();
	}

	std::shared_ptr<CachedPipelineState> Pipeline::CreateOrGetState(const Core::Graphics::PipelineState& aPipelineState)
	{
		//  Define the Graphics Pipeline
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

		// Input Assembly
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;
		for (const auto& inputLayoutEntry : aPipelineState.InputLayout)
		{
			inputElementDescs.emplace_back(D3D12_INPUT_ELEMENT_DESC {
				inputLayoutEntry.SemanticName.c_str(),
				inputLayoutEntry.SemanticIndex,
				ToDXGIFormat(inputLayoutEntry.Format),
				0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				0
			});
		}

		psoDesc.InputLayout.pInputElementDescs = &inputElementDescs.front();
		psoDesc.InputLayout.NumElements = static_cast<UINT>(inputElementDescs.size());

		// Resources
		psoDesc.pRootSignature = myRootSignature.Get();
		psoDesc.VS = std::static_pointer_cast<DirectX12::Shader>(aPipelineState.VertexShader)->GetByteCode();
		psoDesc.PS = std::static_pointer_cast<DirectX12::Shader>(aPipelineState.PixelShader)->GetByteCode();

		// Rasterization
		D3D12_RASTERIZER_DESC rasterDesc;
		rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterDesc.CullMode = D3D12_CULL_MODE_BACK;
		rasterDesc.FrontCounterClockwise = FALSE;
		rasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		rasterDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterDesc.DepthClipEnable = TRUE;
		rasterDesc.MultisampleEnable = FALSE;
		rasterDesc.AntialiasedLineEnable = FALSE;
		rasterDesc.ForcedSampleCount = 0;
		rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		psoDesc.RasterizerState = rasterDesc;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// Color/Blend
		D3D12_BLEND_DESC blendDesc;
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = {
			FALSE,
			FALSE,
			D3D12_BLEND_ONE,
			D3D12_BLEND_ZERO,
			D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE,
			D3D12_BLEND_ZERO,
			D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL,
		};
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			blendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;
		psoDesc.BlendState = blendDesc;

		// Depth/Stencil State
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;

		// Output
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;

		std::shared_ptr<CachedPipelineState> cachedState = std::make_shared<CachedPipelineState>();

		// Create the raster pipeline state
		if (
			LogIfError(
				myDevice.GetDevice()->CreateGraphicsPipelineState(
					&psoDesc,
					IID_PPV_ARGS(
						cachedState->myPipelineState.ReleaseAndGetAddressOf()
					)
				), "Create pipeline state")
			)
		{
			return cachedState;
		}
		else
		{
			return nullptr;
		}
	}

	std::shared_ptr<Core::Graphics::GraphicsBuffer> Pipeline::CreateFrameConstantBuffer(std::uint32_t aBufferSize)
	{
		return myFrameConstantBuffers.emplace_back(
			std::make_shared<ConstantBuffer>(myDevice, aBufferSize)
		);
	}

	void Pipeline::SetupRootSignature()
	{
		ID3D12Device* dxDevice = myDevice.GetDevice().Get();

		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = { };
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		
		if (FAILED(dxDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		D3D12_DESCRIPTOR_RANGE1 ranges[1];
		ranges[0].BaseShaderRegister = 0;
		ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		ranges[0].NumDescriptors = 1;
		ranges[0].RegisterSpace = 0;
		ranges[0].OffsetInDescriptorsFromTableStart = 0;
		ranges[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

		D3D12_ROOT_PARAMETER1 rootParameters[1];
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

		rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
		rootParameters[0].DescriptorTable.pDescriptorRanges = ranges;

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		rootSignatureDesc.Desc_1_1.Flags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		rootSignatureDesc.Desc_1_1.NumParameters = 1;
		rootSignatureDesc.Desc_1_1.pParameters = rootParameters;
		rootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
		rootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr;

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;

		HRESULT result = S_OK;
		result = D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error);
		if (SUCCEEDED(result))
			result = dxDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(myRootSignature.ReleaseAndGetAddressOf()));

		if (FAILED(result))
		{
			LogError(result, error.Get());
			return;
		}
	}
}
