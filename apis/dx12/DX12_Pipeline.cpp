// Filter "Resources"

#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_Enums.hpp"
#include "DX12_Pipeline.hpp"

namespace Atrium::DirectX12
{
	void RootParameterMapping::Table::AddMapping(Core::ResourceUpdateFrequency anUpdateFrequency, RegisterType aRegisterType, unsigned int aRegisterIndex, unsigned int aCount)
	{
		Parameter& p = myRanges.emplace_back();
		p.myUpdateFrequency = anUpdateFrequency;
		p.myRegisterType = aRegisterType;
		p.myRegisterIndex = aRegisterIndex;
		p.myCount = aCount;
	}

	void RootParameterMapping::AddMapping(Core::ResourceUpdateFrequency anUpdateFrequency, RegisterType aRegisterType, unsigned int aRegisterIndex)
	{
		Parameter& p = mySingleParameters.emplace_back(Parameter(), GetNextParameterIndex()).first;
		p.myUpdateFrequency = anUpdateFrequency;
		p.myRegisterType = aRegisterType;
		p.myRegisterIndex = aRegisterIndex;
		p.myCount = 1;
	}

	RootParameterMapping::Table& RootParameterMapping::AddTable()
	{
		const unsigned int nextIndex = GetNextParameterIndex();
		Table& table = myTableParameters.emplace_back();
		table.myRootParameterIndex = nextIndex;
		return table;
	}

	std::optional<RootParameterMapping::ParameterInfo> RootParameterMapping::GetParameterInfo(Core::ResourceUpdateFrequency anUpdateFrequency, RegisterType aRegisterType, unsigned int aRegisterIndex) const
	{
		for (const auto& param : mySingleParameters)
		{
			const Parameter& p = param.first;
			if (p.myUpdateFrequency == anUpdateFrequency &&
				p.myRegisterType == aRegisterType &&
				p.myRegisterIndex == aRegisterIndex)
			{
				ParameterInfo info;
				info.RootParameterIndex = param.second;
				info.Count = 1;
				info.RegisterOffset = 0;
				return info;
			}
		}

		for (const auto& table : myTableParameters)
		{
			for (const auto& range : table.myRanges)
			{
				if (range.myUpdateFrequency != anUpdateFrequency ||
					range.myRegisterType != aRegisterType)
					continue;

				if (aRegisterIndex >= range.myRegisterIndex &&
					aRegisterIndex < (range.myRegisterIndex + range.myCount))
				{
					ParameterInfo info;
					info.RootParameterIndex = table.myRootParameterIndex;
					info.Count = range.myCount;
					info.RegisterOffset = (aRegisterIndex - range.myRegisterIndex);
					return info;
				}
			}
		}

		return { };
	}

	unsigned int RootParameterMapping::GetNextParameterIndex() const
	{
		return static_cast<unsigned int>(mySingleParameters.size() + myTableParameters.size());
	}

	RootSignatureCreator::Sampler::Sampler(D3D12_STATIC_SAMPLER_DESC& aDescriptor)
		: myDescriptor(aDescriptor)
	{
		ZeroMemory(&aDescriptor, sizeof(Sampler));
		myDescriptor.MaxLOD = D3D12_FLOAT32_MAX;
		myDescriptor.MaxAnisotropy = 16;
	}

	Core::RootSignatureBuilder::Sampler& RootSignatureCreator::Sampler::Address(TextureWrapMode aMode)
	{
		return AddressU(aMode).AddressV(aMode).AddressW(aMode);
	}

	Core::RootSignatureBuilder::Sampler& RootSignatureCreator::Sampler::AddressU(TextureWrapMode aMode)
	{
		myDescriptor.AddressU = WrapMode(aMode);
		return *this;
	}

	Core::RootSignatureBuilder::Sampler& RootSignatureCreator::Sampler::AddressV(TextureWrapMode aMode)
	{
		myDescriptor.AddressV = WrapMode(aMode);
		return *this;
	}

	Core::RootSignatureBuilder::Sampler& RootSignatureCreator::Sampler::AddressW(TextureWrapMode aMode)
	{
		myDescriptor.AddressW = WrapMode(aMode);
		return *this;
	}

	Core::RootSignatureBuilder::Sampler& RootSignatureCreator::Sampler::Filter(FilterMode aFilter)
	{
		switch (aFilter)
		{
		case FilterMode::Point:
			myDescriptor.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
			break;
		case FilterMode::Bilinear:
			myDescriptor.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			break;
		case FilterMode::Trilinear:
			myDescriptor.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			break;
		}
		
		return *this;
	}

	Core::RootSignatureBuilder::Sampler& RootSignatureCreator::Sampler::LevelOfDetail(std::pair<float, float> aLodRange)
	{
		myDescriptor.MinLOD = aLodRange.first;
		myDescriptor.MaxLOD = aLodRange.second;
		return *this;
	}

	Core::RootSignatureBuilder::Sampler& RootSignatureCreator::Sampler::MaxAnisotropy(unsigned int aMax)
	{
		myDescriptor.MaxAnisotropy = aMax;
		return *this;
	}

	D3D12_TEXTURE_ADDRESS_MODE RootSignatureCreator::Sampler::WrapMode(TextureWrapMode aMode) const
	{
		switch (aMode)
		{
		case TextureWrapMode::Clamp:
			return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case TextureWrapMode::Mirror:
			return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		case TextureWrapMode::MirrorOnce:
			return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
		case TextureWrapMode::Repeat:
		default:
			return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		}
	}

	std::shared_ptr<Core::RootSignature> RootSignatureCreator::Finalize() const
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = { };
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		if (FAILED(myDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		std::vector<D3D12_DESCRIPTOR_RANGE1> descriptorRanges;
		{
			// Count the total amount of descriptor ranges.
			unsigned int descriptorRangeCount = 0;
			for (const std::unique_ptr<Parameter>& param : myParameters)
			{
				if (param->myType != Parameter::Type::Table)
					continue;

				DescriptorTable* table = static_cast<DescriptorTable*>(param.get());
				descriptorRangeCount += static_cast<UINT>(table->myRanges.size());
			}

			descriptorRanges.reserve(descriptorRangeCount);
		}

		RootParameterMapping parameterMapping;

		std::vector<D3D12_ROOT_PARAMETER1> parameters;
		{
			parameters.reserve(myParameters.size());

			for (const std::unique_ptr<Parameter>& param : myParameters)
			{
				D3D12_ROOT_PARAMETER1& rootParameter = parameters.emplace_back();
				rootParameter.ShaderVisibility = param->myVisibility;

				switch (param->myType)
				{
				case Parameter::Type::Constant:
				{
					rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
					rootParameter.Constants.ShaderRegister = param->myShaderRegister;
					rootParameter.Constants.RegisterSpace = static_cast<UINT>(param->myUpdateFrequency);
					rootParameter.Constants.Num32BitValues = param->myCount;

					parameterMapping.AddMapping(
						param->myUpdateFrequency,
						RootParameterMapping::RegisterType::ConstantBuffer,
						param->myShaderRegister
					);

					break;
				}
				case Parameter::Type::Table:
				{
					rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
					if (!PopulateTable(parameterMapping, descriptorRanges, rootParameter, *static_cast<DescriptorTable*>(param.get())))
						return nullptr; // Table details were wrong, abort.
					break;
				}
				case Parameter::Type::CBV:
				case Parameter::Type::SRV:
				case Parameter::Type::UAV:
				{
					Debug::Assert(param->myCount == 1, "Only 1 descriptor per root parameter.");

					switch (param->myType)
					{
					case Parameter::Type::CBV:
						rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;

						parameterMapping.AddMapping(
							param->myUpdateFrequency,
							RootParameterMapping::RegisterType::ConstantBuffer,
							param->myShaderRegister
						);

						break;
					case Parameter::Type::SRV:
						rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;

						parameterMapping.AddMapping(
							param->myUpdateFrequency,
							RootParameterMapping::RegisterType::Texture,
							param->myShaderRegister
						);

						break;
					case Parameter::Type::UAV:
						rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;

						parameterMapping.AddMapping(
							param->myUpdateFrequency,
							RootParameterMapping::RegisterType::Unordered,
							param->myShaderRegister
						);

						break;
					}

					rootParameter.Descriptor.ShaderRegister = param->myShaderRegister;
					rootParameter.Descriptor.RegisterSpace = static_cast<unsigned int>(param->myUpdateFrequency);
					rootParameter.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;

					break;
				}
				case Parameter::Type::Sampler:
					Debug::LogError("Cannot have sampler root parameter. Use a descriptor table pointing to a range of samplers.");
					return nullptr;
				}
			}
		}

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		rootSignatureDesc.Desc_1_1.NumParameters = static_cast<UINT>(parameters.size());
		rootSignatureDesc.Desc_1_1.pParameters = parameters.empty() ? nullptr : &parameters.front();

		rootSignatureDesc.Desc_1_1.NumStaticSamplers = static_cast<UINT>(myStaticSamplers.size());
		rootSignatureDesc.Desc_1_1.pStaticSamplers = myStaticSamplers.empty() ? nullptr : &myStaticSamplers.front();

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		ComPtr<ID3D12RootSignature> dxRootSignature;

		HRESULT result = S_OK;
		result = D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error);
		if (SUCCEEDED(result))
			result = myDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(dxRootSignature.ReleaseAndGetAddressOf()));
		
		if (!Debug::Verify(result, "Create root signature", error.Get()))
			return nullptr;

		return std::shared_ptr<RootSignature>(new RootSignature(dxRootSignature, parameterMapping));
	}

	void RootSignatureCreator::SetVisibility(Shader::Type aShaderVisibility)
	{
		switch (aShaderVisibility)
		{
		default:
		case Shader::Type::All:
			myCurrentVisibility = D3D12_SHADER_VISIBILITY_ALL;
			break;
		case Shader::Type::Vertex:
			myCurrentVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			break;
		case Shader::Type::Pixel:
			myCurrentVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			break;
		}
	}

	RootSignatureCreator::RootSignatureCreator(ID3D12Device* aDevice)
		: myDevice(aDevice)
	{ }

	void RootSignatureCreator::AddCBV(unsigned int aRegister, Core::ResourceUpdateFrequency anUpdateFrequency)
	{
		AddParameter(Parameter::Type::CBV, aRegister, anUpdateFrequency);
	}

	void RootSignatureCreator::AddConstant(unsigned int aRegister, Core::ResourceUpdateFrequency anUpdateFrequency)
	{
		AddParameter(Parameter::Type::Constant, aRegister, anUpdateFrequency);
	}

	void RootSignatureCreator::AddConstants(unsigned int aCount, unsigned int aRegister, Core::ResourceUpdateFrequency anUpdateFrequency)
	{
		AddParameter(Parameter::Type::Constant, aRegister, anUpdateFrequency).myCount = aCount;
	}

	Core::RootSignatureBuilder::DescriptorTable& RootSignatureCreator::AddTable()
	{
		return AddParameter<DescriptorTable>();
	}

	Core::RootSignatureBuilder::Sampler& RootSignatureCreator::AddSampler(unsigned int aRegister)
	{
		D3D12_STATIC_SAMPLER_DESC& descriptor = myStaticSamplers.emplace_back();
		descriptor.ShaderVisibility = myCurrentVisibility;
		descriptor.ShaderRegister = aRegister;
		descriptor.RegisterSpace = static_cast<unsigned int>(Core::ResourceUpdateFrequency::Constant);

		return mySamplerCreators.emplace_back(descriptor);
	}

	void RootSignatureCreator::AddSRV(unsigned int aRegister, Core::ResourceUpdateFrequency anUpdateFrequency)
	{
		AddParameter(Parameter::Type::SRV, aRegister, anUpdateFrequency);
	}

	void RootSignatureCreator::AddUAV(unsigned int aRegister, Core::ResourceUpdateFrequency anUpdateFrequency)
	{
		AddParameter(Parameter::Type::UAV, aRegister, anUpdateFrequency);
	}

	bool RootSignatureCreator::PopulateTable(RootParameterMapping& aParameterMapping, std::vector<D3D12_DESCRIPTOR_RANGE1>& someRanges, D3D12_ROOT_PARAMETER1& aResult, const DescriptorTable& aTable)
	{
		if (aTable.myRanges.empty())
			return true;

		RootParameterMapping::Table& mappingTable = aParameterMapping.AddTable();

		for (const Parameter& range : aTable.myRanges)
		{
			D3D12_DESCRIPTOR_RANGE1& descriptorRange = someRanges.emplace_back();
			descriptorRange.BaseShaderRegister = range.myShaderRegister;
			descriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
			descriptorRange.NumDescriptors = static_cast<UINT>(range.myCount);
			descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			switch (range.myType)
			{
			case Parameter::Type::CBV:
				descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

				mappingTable.AddMapping(
					range.myUpdateFrequency,
					RootParameterMapping::RegisterType::ConstantBuffer,
					range.myShaderRegister,
					range.myCount
				);

				break;
			case Parameter::Type::SRV:
				descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

				mappingTable.AddMapping(
					range.myUpdateFrequency,
					RootParameterMapping::RegisterType::Texture,
					range.myShaderRegister,
					range.myCount
				);

				break;
			case Parameter::Type::UAV:
				descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

				mappingTable.AddMapping(
					range.myUpdateFrequency,
					RootParameterMapping::RegisterType::Unordered,
					range.myShaderRegister,
					range.myCount
				);

				break;
			case Parameter::Type::Sampler:
				descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;

				mappingTable.AddMapping(
					range.myUpdateFrequency,
					RootParameterMapping::RegisterType::Sampler,
					range.myShaderRegister,
					range.myCount
				);

				break;
			default:
				Debug::LogError("Incorrect descriptor range type.");
				return false;
			}

			descriptorRange.RegisterSpace = static_cast<unsigned int>(range.myUpdateFrequency);
		}

		aResult.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(aTable.myRanges.size());
		aResult.DescriptorTable.pDescriptorRanges = &someRanges.at(someRanges.size() - aTable.myRanges.size());

		return true;
	}

	std::shared_ptr<PipelineState> PipelineState::CreateFrom(ID3D12Device& aDevice, const Core::PipelineStateDescription& aPipelineStateDescription)
	{
		if (!aPipelineStateDescription.IsValid())
		{
			Debug::LogError("Pipeline state description requires a root signature, vertex shader and pixel shader to be valid.");
			return nullptr;
		}

		// Define the Graphics Pipeline
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

		// Input Assembly
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;
		for (const auto& inputLayoutEntry : aPipelineStateDescription.InputLayout)
		{
			inputElementDescs.emplace_back(D3D12_INPUT_ELEMENT_DESC{
				inputLayoutEntry.SemanticName.c_str(),
				inputLayoutEntry.SemanticIndex,
				ToDXGIFormat(inputLayoutEntry.Format),
				inputLayoutEntry.InputSlot,
				D3D12_APPEND_ALIGNED_ELEMENT,
				inputLayoutEntry.InstancePerStep == 0 ? D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA : D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
				inputLayoutEntry.InstancePerStep
				});
		}

		psoDesc.InputLayout.pInputElementDescs = &inputElementDescs.front();
		psoDesc.InputLayout.NumElements = static_cast<UINT>(inputElementDescs.size());

		// Resources
		RootSignature* rootSignature = static_cast<RootSignature*>(aPipelineStateDescription.RootSignature.get());
		psoDesc.pRootSignature = rootSignature->GetRootSignatureObject().Get();

		std::shared_ptr<DirectX12::Shader> vertexShader = std::static_pointer_cast<DirectX12::Shader>(aPipelineStateDescription.VertexShader);
		std::shared_ptr<DirectX12::Shader> pixelShader = std::static_pointer_cast<DirectX12::Shader>(aPipelineStateDescription.PixelShader);
		psoDesc.VS = vertexShader->GetByteCode();
		psoDesc.PS = pixelShader->GetByteCode();

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
		blendDesc.AlphaToCoverageEnable = aPipelineStateDescription.BlendMode.AlphaToMask ? TRUE : FALSE;
		blendDesc.IndependentBlendEnable = aPipelineStateDescription.BlendMode.IndividualBlending ? TRUE : FALSE;
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			const Core::PipelineStateDescription::Blend& blend = aPipelineStateDescription.BlendMode.BlendFactors[i];

			blendDesc.RenderTarget[i] = D3D12_RENDER_TARGET_BLEND_DESC {
				blend.Enabled ? TRUE : FALSE,
				FALSE,
				ToDXBlend(blend.SourceFactor),
				ToDXBlend(blend.DestinationFactor),
				ToDXBlend(blend.Operation),
				ToDXBlend(blend.SourceAlphaFactor),
				ToDXBlend(blend.DestinationAlphaFactor),
				ToDXBlend(blend.Operation),
				D3D12_LOGIC_OP_NOOP,
				D3D12_COLOR_WRITE_ENABLE_ALL,
			};
		}
		psoDesc.BlendState = blendDesc;

		// Depth/Stencil State
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

		psoDesc.DepthStencilState.StencilEnable = FALSE;

		psoDesc.DepthStencilState.BackFace = psoDesc.DepthStencilState.FrontFace = {
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_KEEP,
			D3D12_COMPARISON_FUNC_ALWAYS
		};

		psoDesc.SampleMask = UINT_MAX;

		std::shared_ptr<PipelineState> createdPipelineState(new PipelineState());
		createdPipelineState->myRootSignature = std::static_pointer_cast<RootSignature>(aPipelineStateDescription.RootSignature);
		createdPipelineState->myVertexShader = vertexShader;
		createdPipelineState->myPixelShader = pixelShader;

		// Output
		{
			psoDesc.NumRenderTargets = static_cast<UINT>(aPipelineStateDescription.OutputFormats.size());
			psoDesc.SampleDesc.Count = 1;

			psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
			if (aPipelineStateDescription.DepthTargetFormat.has_value())
				psoDesc.DSVFormat = ToDXGIFormat(aPipelineStateDescription.DepthTargetFormat.value());

			for (std::size_t i = 0; i < aPipelineStateDescription.OutputFormats.size(); ++i)
				psoDesc.RTVFormats[i] = ToDXGIFormat(aPipelineStateDescription.OutputFormats[i]);
		}

		// Create the raster pipeline state
		if (
			Debug::Verify(
				aDevice.CreateGraphicsPipelineState(
					&psoDesc,
					IID_PPV_ARGS(
						createdPipelineState->myPipelineState.ReleaseAndGetAddressOf()
					)
				), "Create pipeline state")
			)
		{
			return createdPipelineState;
		}
		else
		{
			return nullptr;
		}
	}

	D3D12_BLEND PipelineState::ToDXBlend(Core::PipelineStateDescription::BlendFactor aFactor)
	{
		switch (aFactor)
		{
			default:
			case Core::PipelineStateDescription::BlendFactor::Zero:
				return D3D12_BLEND_ZERO;
			case Core::PipelineStateDescription::BlendFactor::One:
				return D3D12_BLEND_ONE;
			case Core::PipelineStateDescription::BlendFactor::SourceColor:
				return D3D12_BLEND_SRC_COLOR;
			case Core::PipelineStateDescription::BlendFactor::SourceAlpha:
				return D3D12_BLEND_SRC_ALPHA;
			case Core::PipelineStateDescription::BlendFactor::DestinationColor:
				return D3D12_BLEND_DEST_COLOR;
			case Core::PipelineStateDescription::BlendFactor::DestinationAlpha:
				return D3D12_BLEND_DEST_ALPHA;
			case Core::PipelineStateDescription::BlendFactor::OneMinusSourceColor:
				return D3D12_BLEND_INV_SRC_COLOR;
			case Core::PipelineStateDescription::BlendFactor::OneMinusSourceAlpha:
				return D3D12_BLEND_INV_SRC_ALPHA;
			case Core::PipelineStateDescription::BlendFactor::OneMinusDestinationColor:
				return D3D12_BLEND_INV_DEST_COLOR;
			case Core::PipelineStateDescription::BlendFactor::OneMinusDestinationAlpha:
				return D3D12_BLEND_INV_DEST_ALPHA;
		}
	}

	D3D12_BLEND_OP PipelineState::ToDXBlend(Core::PipelineStateDescription::BlendOperation anOperation)
	{
		switch (anOperation)
		{
			default:
			case Core::PipelineStateDescription::BlendOperation::Add:
				return D3D12_BLEND_OP_ADD;
			case Core::PipelineStateDescription::BlendOperation::Subtract:
				return D3D12_BLEND_OP_SUBTRACT;
			case Core::PipelineStateDescription::BlendOperation::ReverseSubtract:
				return D3D12_BLEND_OP_REV_SUBTRACT;
			case Core::PipelineStateDescription::BlendOperation::Max:
				return D3D12_BLEND_OP_MAX;
			case Core::PipelineStateDescription::BlendOperation::Min:
			return D3D12_BLEND_OP_MIN;
		}
	}
}
