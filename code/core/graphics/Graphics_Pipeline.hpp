#pragma once

#include "Graphics_Enums.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace RoseGold::Core
{
	class Shader
	{
	public:
		enum class Type
		{
			Vertex,
			Pixel
		};
	};

	enum class ResourceUpdateFrequency
	{
		PerObject,
		PerMaterial,
		PerPass,
		PerFrame,
		Constant
	};

	enum class ShaderVisibility
	{
		All,
		Vertex,
		Pixel,
	};

	class RootSignature
	{
	public:
		virtual ~RootSignature() = default;
	};

	class GraphicsAPI;

	class RootSignatureBuilder
	{
	public:
		virtual ~RootSignatureBuilder() = default;

		class DescriptorTable
		{
		public:
			virtual DescriptorTable& AddCBVRange(unsigned int aCount, unsigned int aRegister, ResourceUpdateFrequency anUpdateFrequency) = 0;
			virtual DescriptorTable& AddSRVRange(unsigned int aCount, unsigned int aRegister, ResourceUpdateFrequency anUpdateFrequency) = 0;
			virtual DescriptorTable& AddUAVRange(unsigned int aCount, unsigned int aRegister, ResourceUpdateFrequency anUpdateFrequency) = 0;
			virtual DescriptorTable& AddSamplerRange(unsigned int aCount, unsigned int aRegister, ResourceUpdateFrequency anUpdateFrequency) = 0;
		};

		class Sampler
		{
		public:
			virtual Sampler& Address(TextureWrapMode aMode) = 0;
			virtual Sampler& AddressU(TextureWrapMode aMode) = 0;
			virtual Sampler& AddressV(TextureWrapMode aMode) = 0;
			virtual Sampler& AddressW(TextureWrapMode aMode) = 0;

			virtual Sampler& Filter(FilterMode aFilter) = 0;

			virtual Sampler& LevelOfDetail(float aLevel) = 0;
			virtual Sampler& LevelOfDetail(std::pair<float, float> aLevelRange) = 0;

			virtual Sampler& MaxAnisotropy(unsigned int aMax) = 0;
		};

	public:
		virtual void AddCBV(unsigned int aRegister, ResourceUpdateFrequency anUpdateFrequency) = 0;
		virtual void AddConstant(unsigned int aRegister, ResourceUpdateFrequency anUpdateFrequency) = 0;
		virtual void AddConstants(unsigned int aCount, unsigned int aRegister, ResourceUpdateFrequency anUpdateFrequency) = 0;

		virtual DescriptorTable& AddTable() = 0;

		virtual Sampler& AddSampler(unsigned int aRegister) = 0;

		virtual void AddSRV(unsigned int aRegister, ResourceUpdateFrequency anUpdateFrequency) = 0;
		virtual void AddUAV(unsigned int aRegister, ResourceUpdateFrequency anUpdateFrequency) = 0;

		virtual std::shared_ptr<RootSignature> Finalize() const = 0;

		virtual void SetVisibility(ShaderVisibility aShaderVisibility) = 0;
	};

	class PipelineStateDescription
	{
	public:
		struct InputLayoutEntry
		{
		public:
			InputLayoutEntry(std::string aSemanticName, GraphicsFormat aFormat)
				: SemanticName(aSemanticName)
				, Format(aFormat)
			{ }

			std::string SemanticName;
			unsigned int SemanticIndex = 0;
			GraphicsFormat Format;
		};

	public:
		inline bool IsValid() const { return VertexShader && PixelShader; }

	public:
		std::shared_ptr<RootSignature> RootSignature;

		std::vector<InputLayoutEntry> InputLayout;

		// (IA) Input assembler

		// (VS) Vertex shader
		std::shared_ptr<Shader> VertexShader;

		// (HS) Hull shader

		// (TS) Tesselation

		// (DS) Domain shader

		// (GS) Geometry shader

		// (SO) Stream output

		// (RS) Rasterizer

		// (PS) Pixel shader
		std::shared_ptr<Shader> PixelShader;

		// (OM) Output merger
		std::vector<GraphicsFormat> OutputFormats;
		std::optional<GraphicsFormat> DepthTargetFormat;
	};

	class PipelineState
	{
	public:
		virtual ~PipelineState() = default;
	};
}