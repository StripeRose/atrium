#pragma once

#include "Graphics_Buffer.hpp"
#include "Graphics_FrameContext.hpp"
#include "Graphics_RenderTexture.hpp"
#include "Graphics_Pipeline.hpp"
#include "Graphics_Texture2D.hpp"
#include "Graphics_Texture3D.hpp"
#include "Graphics_TextureCube.hpp"

#include <filesystem>

namespace RoseGold::Core { class Window; }

namespace RoseGold::Core
{
	class GraphicsAPI
	{
	public:
		virtual ~GraphicsAPI() = default;
		
		virtual std::shared_ptr<RenderTexture> CreateRenderTextureForWindow(Window& aWindow) = 0;

		virtual std::shared_ptr<GraphicsBuffer> CreateGraphicsBuffer(GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride) = 0;

		virtual std::shared_ptr<PipelineState> CreateOrGetPipelineState(const PipelineStateDescription& aPipelineState) = 0;

		inline std::shared_ptr<Shader> CreateShader(const std::filesystem::path& aShaderSource, Shader::Type aShaderType) { return CreateShader(aShaderSource, aShaderType, "main"); }

		virtual std::shared_ptr<Shader> CreateShader(const std::filesystem::path& aShaderSource, Shader::Type aShaderType, const char* anEntryPoint) = 0;

		virtual std::shared_ptr<Texture2D> CreateTexture2D(unsigned int aWidth, unsigned int aHeight, TextureFormat aTextureFormat) = 0;
		virtual std::shared_ptr<Texture3D> CreateTexture3D(unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, TextureFormat aTextureFormat) = 0;
		virtual std::shared_ptr<TextureCube> CreateTextureCube(unsigned int aWidth, TextureFormat aTextureFormat) = 0;

		virtual FrameContext& GetCurrentFrameContext() = 0;

		virtual std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& aPath) = 0;

		virtual bool SupportsMultipleWindows() const = 0;

		virtual void MarkFrameStart() = 0;
		virtual void MarkFrameEnd() = 0;
	};
}