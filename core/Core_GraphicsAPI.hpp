#pragma once

#include "Core_FrameContext.hpp"
#include "Core_GraphicsBuffer.hpp"
#include "Core_GraphicsPipeline.hpp"
#include "Core_RenderTexture.hpp"

#include <filesystem>

namespace Atrium::Core { class Window; }

namespace Atrium::Core
{
	class GraphicsAPI
	{
	public:
		class ResourceManager
		{
		public:
			virtual std::shared_ptr<RenderTexture> CreateRenderTextureForWindow(Window& aWindow) = 0;

			virtual std::shared_ptr<GraphicsBuffer> CreateGraphicsBuffer(GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride) = 0;

			virtual std::shared_ptr<PipelineState> CreatePipelineState(const PipelineStateDescription& aPipelineState) = 0;

			virtual std::unique_ptr<RootSignatureBuilder> CreateRootSignature() = 0;

			virtual std::shared_ptr<Shader> CreateShader(const std::filesystem::path& aShaderSource, Shader::Type aShaderType, const char* anEntryPoint) = 0;

			virtual std::shared_ptr<EditableTexture> CreateTexture2D(unsigned int aWidth, unsigned int aHeight, TextureFormat aTextureFormat) = 0;

			virtual std::shared_ptr<EditableTexture> CreateTexture3D(unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, TextureFormat aTextureFormat) = 0;

			virtual std::shared_ptr<EditableTexture> CreateTextureCube(unsigned int aWidth, TextureFormat aTextureFormat) = 0;

			virtual std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& aPath) = 0;
		};

	public:
		virtual ~GraphicsAPI() = default;

		virtual FrameContext& GetCurrentFrameContext() = 0;

		virtual ResourceManager& GetResourceManager() = 0;

		virtual bool SupportsMultipleWindows() const = 0;

		virtual void MarkFrameStart() = 0;
		virtual void MarkFrameEnd() = 0;
	};
}