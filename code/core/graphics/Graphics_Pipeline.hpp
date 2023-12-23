#pragma once

#include <memory>

namespace RoseGold::Core::Graphics
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

	class PipelineState
	{
	public:
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
	};

	class CachedPipelineState
	{

	};
}