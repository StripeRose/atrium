#pragma once

#include "Core_GraphicsEnums.hpp"

#include <rose-common/Enum.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Atrium::Core
{
	class Shader
	{
	public:
		enum class Type
		{
			None = 0,
			All = ~0,

			Vertex = 1 << 0,
			Pixel = 1 << 1
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

		virtual void SetVisibility(Shader::Type aShaderVisibility) = 0;
	};

	class PipelineStateDescription
	{
	public:
		struct InputLayoutEntry
		{
		public:
			InputLayoutEntry(std::string aSemanticName, GraphicsFormat aFormat, unsigned int anInputSlot = 0, unsigned int anInstancePerStep = 0)
				: SemanticName(aSemanticName)
				, SemanticIndex(0)
				, Format(aFormat)
				, InputSlot(anInputSlot)
				, InstancePerStep(anInstancePerStep)
			{ }

			InputLayoutEntry(std::string aSemanticName, unsigned int aSemanticIndex, GraphicsFormat aFormat, unsigned int anInputSlot = 0, unsigned int anInstancePerStep = 0)
				: SemanticName(aSemanticName)
				, SemanticIndex(aSemanticIndex)
				, Format(aFormat)
				, InputSlot(anInputSlot)
				, InstancePerStep(anInstancePerStep)
			{ }

			// The HLSL semantic associated with this element.
			// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics
			std::string SemanticName;

			// The HLSL semantic index, if multiple.
			unsigned int SemanticIndex;

			GraphicsFormat Format;

			// Which vertex buffer slot to read data from.
			unsigned int InputSlot;

			// The number of instances to draw using the same per-instance data before advancing in the buffer by one element.
			// A step of 0 means it's per-vertex data.
			unsigned int InstancePerStep;
		};

		enum class BlendFactor
		{
			Zero,
			One,
			SourceColor,
			SourceAlpha,
			DestinationColor,
			DestinationAlpha,
			OneMinusSourceColor,
			OneMinusSourceAlpha,
			OneMinusDestinationColor,
			OneMinusDestinationAlpha
		};

		enum class BlendOperation
		{
			Add,
			Subtract,
			ReverseSubtract,
			Min,
			Max
		};

		struct Blend
		{
			bool Enabled = false;
			BlendFactor SourceFactor = BlendFactor::SourceAlpha;
			BlendFactor SourceAlphaFactor = BlendFactor::SourceAlpha;
			BlendFactor DestinationFactor = BlendFactor::OneMinusSourceAlpha;
			BlendFactor DestinationAlphaFactor = BlendFactor::OneMinusSourceAlpha;
			BlendOperation Operation = BlendOperation::Add;
		};

		struct BlendMode
		{
			bool AlphaToMask = false;
			bool IndividualBlending = false;
			Blend BlendFactors[8];
		};

	public:
		inline bool IsValid() const { return RootSignature && VertexShader && PixelShader; }

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
		BlendMode BlendMode;
	};

	class PipelineState
	{
	public:
		virtual ~PipelineState() = default;
	};
}

ENUM_FLAGS(Atrium::Core::Shader::Type);