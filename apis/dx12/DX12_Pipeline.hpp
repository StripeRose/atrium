// Filter "Resources"

#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_Shader.hpp"

#include "Graphics_FrameContext.hpp"
#include "Graphics_Pipeline.hpp"
#include "Graphics_RenderTexture.hpp"

#include <d3d12.h>

#include <map>
#include <memory>
#include <optional>
#include <vector>

namespace Atrium::DirectX12
{
	class RootParameterMapping
	{
	public:
		enum class RegisterType
		{
			ConstantBuffer, // HLSL register type B
			Sampler, // HLSL register type S
			Texture, // HLSL register type T
			Unordered // HLSL register type U
		};

	private:
		struct Parameter
		{
			Core::ResourceUpdateFrequency myUpdateFrequency;
			RegisterType myRegisterType;
			unsigned int myRegisterIndex;
			unsigned int myCount;
		};

	public:
		struct Table
		{
			friend RootParameterMapping;
		public:
			void AddMapping(
				Core::ResourceUpdateFrequency anUpdateFrequency,
				RegisterType aRegisterType,
				unsigned int aRegisterIndex,
				unsigned int aCount
			);

		private:
			std::vector<Parameter> myRanges;
			unsigned int myRootParameterIndex;
		};

		struct ParameterInfo
		{
			unsigned int RootParameterIndex;
			unsigned int Count;
			unsigned int RegisterOffset;
		};

	public:
		void AddMapping(
			Core::ResourceUpdateFrequency anUpdateFrequency,
			RegisterType aRegisterType,
			unsigned int aRegisterIndex
		);

		Table& AddTable();
		
		std::optional<ParameterInfo> GetParameterInfo(
			Core::ResourceUpdateFrequency anUpdateFrequency,
			RegisterType aRegisterType,
			unsigned int aRegisterIndex
		) const;

	private:
		unsigned int GetNextParameterIndex() const;

		std::vector<std::pair<Parameter, unsigned int>> mySingleParameters;
		std::vector<Table> myTableParameters;
	};

	class RootSignature : public Core::RootSignature
	{
		friend class RootSignatureCreator;

	public:
		const ComPtr<ID3D12RootSignature>& GetRootSignatureObject() const { return myRootSignature; }

		std::optional<RootParameterMapping::ParameterInfo> GetParameterInfo(
			Core::ResourceUpdateFrequency anUpdateFrequency,
			RootParameterMapping::RegisterType aRegisterType,
			unsigned int aRegisterIndex
		) const
		{
			return myParameterMapping.GetParameterInfo(anUpdateFrequency, aRegisterType, aRegisterIndex);
		}

	private:
		RootSignature(ComPtr<ID3D12RootSignature> aRootSignature, const RootParameterMapping& aParameterMapping)
			: myRootSignature(aRootSignature)
			, myParameterMapping(aParameterMapping)
		{ }

		const ComPtr<ID3D12RootSignature> myRootSignature;
		const RootParameterMapping myParameterMapping;
	};

	class RootSignatureCreator : public Core::RootSignatureBuilder
	{
	private:
		class Parameter
		{
			friend RootSignatureCreator;
		public:
			virtual ~Parameter() = default;

		protected:
			enum class Type { Table, Constant, CBV, SRV, UAV, Sampler } myType = Type::Constant;

			unsigned int myShaderRegister = 0;
			Core::ResourceUpdateFrequency myUpdateFrequency = Core::ResourceUpdateFrequency::PerObject;
			unsigned int myCount = 0;
			D3D12_SHADER_VISIBILITY myVisibility = D3D12_SHADER_VISIBILITY_ALL;
		};

	public:
		class DescriptorTable : public Core::RootSignatureBuilder::DescriptorTable, protected Parameter
		{
			friend RootSignatureCreator;
		public:
			Core::RootSignatureBuilder::DescriptorTable& AddCBVRange(unsigned int aCount, unsigned int aRegister, Core::ResourceUpdateFrequency anUpdateFrequency) override { return AddRange(Parameter::Type::CBV, aCount, aRegister, anUpdateFrequency); }
			Core::RootSignatureBuilder::DescriptorTable& AddSRVRange(unsigned int aCount, unsigned int aRegister, Core::ResourceUpdateFrequency anUpdateFrequency) override { return AddRange(Parameter::Type::SRV, aCount, aRegister, anUpdateFrequency); }
			Core::RootSignatureBuilder::DescriptorTable& AddUAVRange(unsigned int aCount, unsigned int aRegister, Core::ResourceUpdateFrequency anUpdateFrequency) override { return AddRange(Parameter::Type::UAV, aCount, aRegister, anUpdateFrequency); }
			Core::RootSignatureBuilder::DescriptorTable& AddSamplerRange(unsigned int aCount, unsigned int aRegister, Core::ResourceUpdateFrequency anUpdateFrequency) override { return AddRange(Parameter::Type::Sampler, aCount, aRegister, anUpdateFrequency); }

		private:
			DescriptorTable() { myType = Type::Table; }

			DescriptorTable& AddRange(Parameter::Type aType, unsigned int aCount, unsigned int aRegister, Core::ResourceUpdateFrequency anUpdateFrequency)
			{
				Parameter& param = myRanges.emplace_back();
				param.myType = aType;
				param.myShaderRegister = aRegister;
				param.myCount = aCount;
				param.myUpdateFrequency = anUpdateFrequency;

				return *this;
			}

			std::vector<Parameter> myRanges;
		};

		class Sampler : public Core::RootSignatureBuilder::Sampler
		{
			friend RootSignatureCreator;
		public:
			Sampler(D3D12_STATIC_SAMPLER_DESC& aDescriptor);

			Core::RootSignatureBuilder::Sampler& Address(Core::TextureWrapMode aMode) override;
			Core::RootSignatureBuilder::Sampler& AddressU(Core::TextureWrapMode aMode) override;
			Core::RootSignatureBuilder::Sampler& AddressV(Core::TextureWrapMode aMode) override;
			Core::RootSignatureBuilder::Sampler& AddressW(Core::TextureWrapMode aMode) override;

			Core::RootSignatureBuilder::Sampler& Filter(Core::FilterMode aFilter) override;

			Core::RootSignatureBuilder::Sampler& LevelOfDetail(float aLodLevel) override { return LevelOfDetail({ aLodLevel, aLodLevel }); }
			Core::RootSignatureBuilder::Sampler& LevelOfDetail(std::pair<float, float> aLodRange) override;

			Core::RootSignatureBuilder::Sampler& MaxAnisotropy(unsigned int aMax) override;

		private:
			D3D12_TEXTURE_ADDRESS_MODE WrapMode(Core::TextureWrapMode aMode) const;

			D3D12_STATIC_SAMPLER_DESC& myDescriptor;
		};

	public:
		RootSignatureCreator(ID3D12Device* aDevice);

		void AddCBV(unsigned int aRegister, Core::ResourceUpdateFrequency anUpdateFrequency) override;
		void AddConstant(unsigned int aRegister, Core::ResourceUpdateFrequency anUpdateFrequency) override;
		void AddConstants(unsigned int aCount, unsigned int aRegister, Core::ResourceUpdateFrequency anUpdateFrequency) override;

		Core::RootSignatureBuilder::DescriptorTable& AddTable() override;

		Core::RootSignatureBuilder::Sampler& AddSampler(unsigned int aRegister) override;

		void AddSRV(unsigned int aRegister, Core::ResourceUpdateFrequency anUpdateFrequency) override;
		void AddUAV(unsigned int aRegister, Core::ResourceUpdateFrequency anUpdateFrequency) override;

		std::shared_ptr<Core::RootSignature> Finalize() const override;

		void SetVisibility(Core::ShaderVisibility aShaderVisibility) override;

	private:
		static bool PopulateTable(RootParameterMapping& aParameterMapping, std::vector<D3D12_DESCRIPTOR_RANGE1>& someRanges, D3D12_ROOT_PARAMETER1& aResult, const DescriptorTable& aTable);

		Parameter& AddParameter(Parameter::Type aType, unsigned int aRegister, Core::ResourceUpdateFrequency anUpdateFrequency)
		{
			Parameter& param = *myParameters.emplace_back(new Parameter());
			param.myType = aType;
			param.myShaderRegister = aRegister;
			param.myCount = 1;
			param.myUpdateFrequency = anUpdateFrequency;
			param.myVisibility = myCurrentVisibility;
			return param;
		}

		template <typename T>
		T& AddParameter()
		{
			Parameter& param = *myParameters.emplace_back(static_cast<Parameter*>(new T()));
			param.myVisibility = myCurrentVisibility;
			return static_cast<T&>(param);
		}

		ID3D12Device* myDevice;
		std::vector<std::unique_ptr<Parameter>> myParameters;
		std::vector<Sampler> mySamplerCreators;
		std::vector<D3D12_STATIC_SAMPLER_DESC> myStaticSamplers;

		D3D12_SHADER_VISIBILITY myCurrentVisibility = D3D12_SHADER_VISIBILITY_ALL;
	};

	class PipelineState : public Core::PipelineState
	{
	public:
		static std::shared_ptr<PipelineState> CreateFrom(ID3D12Device& aDevice, const Core::PipelineStateDescription& aPipelineStateDescription);

		const ComPtr<ID3D12PipelineState>& GetPipelineStateObject() const { return myPipelineState; }
		const std::shared_ptr<RootSignature>& GetRootSignature() const { return myRootSignature; }

		const std::shared_ptr<DirectX12::Shader>& GetVertexShader() const { return myVertexShader; }
		const std::shared_ptr<DirectX12::Shader>& GetPixelShader() const { return myPixelShader; }

	private:
		PipelineState() = default;

		static D3D12_BLEND ToDXBlend(Core::PipelineStateDescription::BlendFactor aFactor);
		static D3D12_BLEND_OP ToDXBlend(Core::PipelineStateDescription::BlendOperation anOperation);

		ComPtr<ID3D12PipelineState> myPipelineState;

		std::shared_ptr<RootSignature> myRootSignature;
		std::shared_ptr<DirectX12::Shader> myVertexShader, myPixelShader;
	};
}