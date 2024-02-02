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

namespace RoseGold::DirectX12
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
			Core::Graphics::ResourceUpdateFrequency myUpdateFrequency;
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
				Core::Graphics::ResourceUpdateFrequency anUpdateFrequency,
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
		};

	public:
		void AddMapping(
			Core::Graphics::ResourceUpdateFrequency anUpdateFrequency,
			RegisterType aRegisterType,
			unsigned int aRegisterIndex
		);

		Table& AddTable();
		
		std::optional<ParameterInfo> GetParameterInfo(
			Core::Graphics::ResourceUpdateFrequency anUpdateFrequency,
			RegisterType aRegisterType,
			unsigned int aRegisterIndex
		) const;

	private:
		unsigned int GetNextParameterIndex() const;

		std::vector<std::pair<Parameter, unsigned int>> mySingleParameters;
		std::vector<Table> myTableParameters;
	};

	class RootSignature
	{
		friend class RootSignatureCreator;

	public:
		const ComPtr<ID3D12RootSignature>& GetRootSignatureObject() const { return myRootSignature; }

		std::optional<RootParameterMapping::ParameterInfo> GetParameterInfo(
			Core::Graphics::ResourceUpdateFrequency anUpdateFrequency,
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

	class RootSignatureCreator
	{
	private:
		class Parameter
		{
			friend RootSignatureCreator;
		protected:
			enum class Type { Table, Constant, CBV, SRV, UAV, Sampler } myType = Type::Constant;

			unsigned int myShaderRegister = 0;
			Core::Graphics::ResourceUpdateFrequency myUpdateFrequency = Core::Graphics::ResourceUpdateFrequency::PerObject;
			unsigned int myCount = 0;
			D3D12_SHADER_VISIBILITY myVisibility = D3D12_SHADER_VISIBILITY_ALL;
		};

	public:
		class DescriptorTable : protected Parameter
		{
			friend RootSignatureCreator;
		public:
			DescriptorTable& AddCBVRange(unsigned int aCount, unsigned int aRegister, Core::Graphics::ResourceUpdateFrequency anUpdateFrequency) { return AddRange(Parameter::Type::CBV, aCount, aRegister, anUpdateFrequency); }
			DescriptorTable& AddSRVRange(unsigned int aCount, unsigned int aRegister, Core::Graphics::ResourceUpdateFrequency anUpdateFrequency) { return AddRange(Parameter::Type::SRV, aCount, aRegister, anUpdateFrequency); }
			DescriptorTable& AddUAVRange(unsigned int aCount, unsigned int aRegister, Core::Graphics::ResourceUpdateFrequency anUpdateFrequency) { return AddRange(Parameter::Type::UAV, aCount, aRegister, anUpdateFrequency); }
			DescriptorTable& AddSamplerRange(unsigned int aCount, unsigned int aRegister, Core::Graphics::ResourceUpdateFrequency anUpdateFrequency) { return AddRange(Parameter::Type::Sampler, aCount, aRegister, anUpdateFrequency); }

		private:
			DescriptorTable() { myType = Type::Table; }

			DescriptorTable& AddRange(Parameter::Type aType, unsigned int aCount, unsigned int aRegister, Core::Graphics::ResourceUpdateFrequency anUpdateFrequency)
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

		class Sampler : public D3D12_STATIC_SAMPLER_DESC
		{
			friend RootSignatureCreator;
		public:
			Sampler()
			{
				ZeroMemory(this, sizeof(Sampler));
				D3D12_STATIC_SAMPLER_DESC::MaxLOD = D3D12_FLOAT32_MAX;
				D3D12_STATIC_SAMPLER_DESC::MaxAnisotropy = 16;
			}

			Sampler& Address(D3D12_TEXTURE_ADDRESS_MODE aMode) { return AddressU(aMode).AddressV(aMode).AddressW(aMode); }
			Sampler& AddressU(D3D12_TEXTURE_ADDRESS_MODE aMode) { D3D12_STATIC_SAMPLER_DESC::AddressU = aMode; return *this; }
			Sampler& AddressV(D3D12_TEXTURE_ADDRESS_MODE aMode) { D3D12_STATIC_SAMPLER_DESC::AddressV = aMode; return *this; }
			Sampler& AddressW(D3D12_TEXTURE_ADDRESS_MODE aMode) { D3D12_STATIC_SAMPLER_DESC::AddressW = aMode; return *this; }

			Sampler& Filter(D3D12_FILTER aFilter) { D3D12_STATIC_SAMPLER_DESC::Filter = aFilter; return *this; }

			Sampler& Lod(FLOAT aLodLevel) { return Lod({ aLodLevel, aLodLevel }); }
			Sampler& Lod(std::pair<FLOAT, FLOAT> aLodRange)
			{
				D3D12_STATIC_SAMPLER_DESC::MinLOD = aLodRange.first;
				D3D12_STATIC_SAMPLER_DESC::MaxLOD = aLodRange.second;
				return *this;
			}

			Sampler& MaxAnisotropy(UINT aMax) { D3D12_STATIC_SAMPLER_DESC::MaxAnisotropy = aMax; return *this; }
		};

	public:
		void AddCBV(unsigned int aRegister, Core::Graphics::ResourceUpdateFrequency anUpdateFrequency) { AddParameter(Parameter::Type::CBV, aRegister, anUpdateFrequency); }
		void AddConstant(unsigned int aRegister, Core::Graphics::ResourceUpdateFrequency anUpdateFrequency) { AddParameter(Parameter::Type::Constant, aRegister, anUpdateFrequency); }
		void AddConstants(unsigned int aCount, unsigned int aRegister, Core::Graphics::ResourceUpdateFrequency anUpdateFrequency) { AddParameter(Parameter::Type::Constant, aRegister, anUpdateFrequency).myCount = aCount; }

		DescriptorTable& AddDescriptorTable() { return AddParameter<DescriptorTable>(); }

		Sampler& AddSampler(unsigned int aRegister)
		{
			Sampler& sampler = myStaticSamplers.emplace_back();
			sampler.ShaderVisibility = myCurrentVisibility;
			sampler.ShaderRegister = aRegister;
			sampler.RegisterSpace = static_cast<unsigned int>(Core::Graphics::ResourceUpdateFrequency::Constant);
			return sampler;
		}

		void AddSRV(unsigned int aRegister, Core::Graphics::ResourceUpdateFrequency anUpdateFrequency) { AddParameter(Parameter::Type::SRV, aRegister, anUpdateFrequency); }
		void AddUAV(unsigned int aRegister, Core::Graphics::ResourceUpdateFrequency anUpdateFrequency) { AddParameter(Parameter::Type::UAV, aRegister, anUpdateFrequency); }

		std::shared_ptr<RootSignature> Finalize(ID3D12Device* aDevice) const;

		void SetVisibility(D3D12_SHADER_VISIBILITY aVisibility) { myCurrentVisibility = aVisibility; }

	private:
		static bool PopulateTable(RootParameterMapping& aParameterMapping, std::vector<D3D12_DESCRIPTOR_RANGE1>& someRanges, D3D12_ROOT_PARAMETER1& aResult, const DescriptorTable& aTable);

		Parameter& AddParameter(Parameter::Type aType, unsigned int aRegister, Core::Graphics::ResourceUpdateFrequency anUpdateFrequency)
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

		std::vector<std::unique_ptr<Parameter>> myParameters;
		std::vector<Sampler> myStaticSamplers;

		D3D12_SHADER_VISIBILITY myCurrentVisibility = D3D12_SHADER_VISIBILITY_ALL;
	};

	class PipelineState : public Core::Graphics::PipelineState
	{
	public:
		static std::shared_ptr<PipelineState> CreateFrom(ID3D12Device& aDevice, const std::shared_ptr<RootSignature>& aRootSignature, const Core::Graphics::PipelineStateDescription& aPipelineStateDescription);

		const ComPtr<ID3D12PipelineState>& GetPipelineStateObject() const { return myPipelineState; }
		const std::shared_ptr<RootSignature>& GetRootSignature() const { return myRootSignature; }

		const std::shared_ptr<DirectX12::Shader>& GetVertexShader() const { return myVertexShader; }
		const std::shared_ptr<DirectX12::Shader>& GetPixelShader() const { return myPixelShader; }

	private:
		PipelineState() = default;

		ComPtr<ID3D12PipelineState> myPipelineState;

		std::shared_ptr<RootSignature> myRootSignature;
		std::shared_ptr<DirectX12::Shader> myVertexShader, myPixelShader;
	};
}