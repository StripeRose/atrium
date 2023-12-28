// Filter "Resources"

#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_Shader.hpp"

#include <Graphics_Buffer.hpp>
#include <Graphics_Pipeline.hpp>

#include <d3d12.h>

#include <map>
#include <memory>
#include <vector>

namespace RoseGold::DirectX12
{
	class CachedPipelineState : public Core::Graphics::CachedPipelineState
	{
	public:
		ComPtr<ID3D12PipelineState> PipelineState;
		std::shared_ptr<DirectX12::Shader> VertexShader, PixelShader;
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
			DescriptorTable() { myType = Type::Table; }

			void AddCBVRange(unsigned int aCount, unsigned int aRegister) { AddCBVRange(aCount, aRegister, 0); }
			void AddCBVRange(unsigned int aCount, unsigned int aRegister, unsigned int aSpace) { AddRange(Parameter::Type::CBV, aCount, aRegister, aSpace); }

			void AddSRVRange(unsigned int aCount, unsigned int aRegister) { AddSRVRange(aCount, aRegister, 0); }
			void AddSRVRange(unsigned int aCount, unsigned int aRegister, unsigned int aSpace) { AddRange(Parameter::Type::SRV, aCount, aRegister, aSpace); }

			void AddUAVRange(unsigned int aCount, unsigned int aRegister) { AddUAVRange(aCount, aRegister, 0); }
			void AddUAVRange(unsigned int aCount, unsigned int aRegister, unsigned int aSpace) { AddRange(Parameter::Type::UAV, aCount, aRegister, aSpace); }

			void AddSamplerRange(unsigned int aCount, unsigned int aRegister) { AddSamplerRange(aCount, aRegister, 0); }
			void AddSamplerRange(unsigned int aCount, unsigned int aRegister, unsigned int aSpace) { AddRange(Parameter::Type::Sampler, aCount, aRegister, aSpace); }

		private:
			Parameter& AddRange(Parameter::Type aType, unsigned int aCount, unsigned int aRegister, unsigned int aSpace)
			{
				Parameter& param = myRanges.emplace_back();
				param.myType = aType;
				param.myShaderRegister = aRegister;
				param.myCount = aCount;
				param.myRegisterSpace = aSpace;
				return param;
			}

			std::vector<Parameter> myRanges;
		};

	public:
		void AddCBV(unsigned int aRegister) { AddCBV(aRegister, 0); }
		void AddCBV(unsigned int aRegister, unsigned int aSpace) { AddParameter(Parameter::Type::CBV, aRegister, aSpace); }

		void AddConstant(unsigned int aRegister) { AddConstant(aRegister, 0); }
		void AddConstant(unsigned int aRegister, unsigned int aSpace) { AddParameter(Parameter::Type::Constant, aRegister, aSpace); }

		void AddConstants(unsigned int aCount, unsigned int aRegister) { AddConstants(aCount, aRegister, 0); }
		void AddConstants(unsigned int aCount, unsigned int aRegister, unsigned int aSpace) { AddParameter(Parameter::Type::Constant, aRegister, aSpace).myCount = aCount; }

		DescriptorTable& AddDescriptorTable() { return AddParameter<DescriptorTable>(); }

		D3D12_STATIC_SAMPLER_DESC& AddSampler() { return myStaticSamplers.emplace_back(); }

		void AddSRV(unsigned int aRegister) { AddSRV(aRegister, 0); }
		void AddSRV(unsigned int aRegister, unsigned int aSpace) { AddParameter(Parameter::Type::SRV, aRegister, aSpace); }

		void AddUAV(unsigned int aRegister) { AddUAV(aRegister, 0); }
		void AddUAV(unsigned int aRegister, unsigned int aSpace) { AddParameter(Parameter::Type::UAV, aRegister, aSpace); }

		ComPtr<ID3D12RootSignature> Finalize(ID3D12Device* aDevice) const;

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
		std::vector<D3D12_STATIC_SAMPLER_DESC> myStaticSamplers;

		D3D12_SHADER_VISIBILITY myCurrentVisibility;
	};
}