// Filter "Resources"

#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_Shader.hpp"

#include <Graphics_Pipeline.hpp>
#include <Graphics_RenderTexture.hpp>

#include <d3d12.h>

#include <map>
#include <memory>
#include <vector>

namespace RoseGold::DirectX12
{
	class RootSignature
	{
		friend class RootSignatureCreator;

	public:
		const ComPtr<ID3D12RootSignature>& GetRootSignatureObject() const { return myRootSignature; }

	private:
		ComPtr<ID3D12RootSignature> myRootSignature;
	};

	class RootSignatureCreator
	{
	private:
		class Parameter
		{
			friend RootSignatureCreator;
		protected:
			enum class Type { Table, Constant, CBV, SRV, UAV, Sampler } myType;

			unsigned int myShaderRegister = 0;
			unsigned int myRegisterSpace = 0;
			unsigned int myCount = 0;
			D3D12_SHADER_VISIBILITY myVisibility = D3D12_SHADER_VISIBILITY_ALL;
		};

	public:
		class DescriptorTable : protected Parameter
		{
			friend RootSignatureCreator;
		public:
			DescriptorTable& AddCBVRange(unsigned int aCount, unsigned int aRegister) { return AddCBVRange(aCount, aRegister, 0); }
			DescriptorTable& AddCBVRange(unsigned int aCount, unsigned int aRegister, unsigned int aSpace) { return AddRange(Parameter::Type::CBV, aCount, aRegister, aSpace); }

			DescriptorTable& AddSRVRange(unsigned int aCount, unsigned int aRegister) { return AddSRVRange(aCount, aRegister, 0); }
			DescriptorTable& AddSRVRange(unsigned int aCount, unsigned int aRegister, unsigned int aSpace) { return AddRange(Parameter::Type::SRV, aCount, aRegister, aSpace); }

			DescriptorTable& AddUAVRange(unsigned int aCount, unsigned int aRegister) { return AddUAVRange(aCount, aRegister, 0); }
			DescriptorTable& AddUAVRange(unsigned int aCount, unsigned int aRegister, unsigned int aSpace) { return AddRange(Parameter::Type::UAV, aCount, aRegister, aSpace); }

			DescriptorTable& AddSamplerRange(unsigned int aCount, unsigned int aRegister) { return AddSamplerRange(aCount, aRegister, 0); }
			DescriptorTable& AddSamplerRange(unsigned int aCount, unsigned int aRegister, unsigned int aSpace) { return AddRange(Parameter::Type::Sampler, aCount, aRegister, aSpace); }

		private:
			DescriptorTable() { myType = Type::Table; }

			DescriptorTable& AddRange(Parameter::Type aType, unsigned int aCount, unsigned int aRegister, unsigned int aSpace)
			{
				Parameter& param = myRanges.emplace_back();
				param.myType = aType;
				param.myShaderRegister = aRegister;
				param.myCount = aCount;
				param.myRegisterSpace = aSpace;

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
		void AddCBV(unsigned int aRegister) { AddCBV(aRegister, 0); }
		void AddCBV(unsigned int aRegister, unsigned int aSpace) { AddParameter(Parameter::Type::CBV, aRegister, aSpace); }

		void AddConstant(unsigned int aRegister) { AddConstant(aRegister, 0); }
		void AddConstant(unsigned int aRegister, unsigned int aSpace) { AddParameter(Parameter::Type::Constant, aRegister, aSpace); }

		void AddConstants(unsigned int aCount, unsigned int aRegister) { AddConstants(aCount, aRegister, 0); }
		void AddConstants(unsigned int aCount, unsigned int aRegister, unsigned int aSpace) { AddParameter(Parameter::Type::Constant, aRegister, aSpace).myCount = aCount; }

		DescriptorTable& AddDescriptorTable() { return AddParameter<DescriptorTable>(); }

		Sampler& AddSampler(unsigned int aRegister) { return AddSampler(aRegister, 0); }
		Sampler& AddSampler(unsigned int aRegister, unsigned int aSpace)
		{
			Sampler& sampler = myStaticSamplers.emplace_back();
			sampler.ShaderVisibility = myCurrentVisibility;
			sampler.ShaderRegister = aRegister;
			sampler.RegisterSpace = aSpace;
			return sampler;
		}

		void AddSRV(unsigned int aRegister) { AddSRV(aRegister, 0); }
		void AddSRV(unsigned int aRegister, unsigned int aSpace) { AddParameter(Parameter::Type::SRV, aRegister, aSpace); }

		void AddUAV(unsigned int aRegister) { AddUAV(aRegister, 0); }
		void AddUAV(unsigned int aRegister, unsigned int aSpace) { AddParameter(Parameter::Type::UAV, aRegister, aSpace); }

		std::shared_ptr<RootSignature> Finalize(ID3D12Device* aDevice) const;

		void SetVisibility(D3D12_SHADER_VISIBILITY aVisibility) { myCurrentVisibility = aVisibility; }

	private:
		static bool PopulateTable(std::vector<D3D12_DESCRIPTOR_RANGE1>& someRanges, D3D12_ROOT_PARAMETER1& aResult, const DescriptorTable& aTable);

		Parameter& AddParameter(Parameter::Type aType, unsigned int aRegister, unsigned int aSpace)
		{
			Parameter& param = *myParameters.emplace_back(new Parameter());
			param.myType = aType;
			param.myShaderRegister = aRegister;
			param.myCount = 1;
			param.myRegisterSpace = aSpace;
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

		D3D12_SHADER_VISIBILITY myCurrentVisibility;
	};

	class PipelineState : public Core::Graphics::PipelineState
	{
	public:
		static std::shared_ptr<PipelineState> CreateFrom(ID3D12Device& aDevice, const std::shared_ptr<RootSignature>& aRootSignature, const Core::Graphics::PipelineStateDescription& aPipelineStateDescription);

		const ComPtr<ID3D12PipelineState>& GetPipelineStateObject() const { return myPipelineState; }
		const std::shared_ptr<RootSignature>& GetRootSignature() const { return myRootSignature; }

		const std::shared_ptr<DirectX12::Shader>& GetVertexShader() const { return myVertexShader; }
		const std::shared_ptr<DirectX12::Shader>& GetPixelShader() const { return myPixelShader; }

		const std::vector<std::shared_ptr<Core::Graphics::RenderTexture>>& GetOutputs() const { return myOutputs; }
		const std::shared_ptr<Core::Graphics::RenderTexture>& GetDepthTarget() const { return myDepthTarget; }

	private:
		PipelineState() = default;

		ComPtr<ID3D12PipelineState> myPipelineState;

		std::shared_ptr<RootSignature> myRootSignature;
		std::shared_ptr<DirectX12::Shader> myVertexShader, myPixelShader;
		std::vector<std::shared_ptr<Core::Graphics::RenderTexture>> myOutputs;
		std::shared_ptr<Core::Graphics::RenderTexture> myDepthTarget;
	};
}