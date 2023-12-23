#pragma once

#include "Graphics_Buffer.hpp"
#include "Graphics_CommandBuffer.hpp"
#include "Graphics_Mesh.hpp"
#include "Graphics_RenderTexture.hpp"
#include "Graphics_Pipeline.hpp"
#include "Graphics_Tasks.hpp"

#include <filesystem>

namespace RoseGold::Core::Platform { class Window; }

namespace RoseGold::Core::Graphics
{
	class Manager
	{
	public:
		virtual ~Manager() = default;
		
		virtual std::shared_ptr<RenderTexture> CreateRenderTextureForWindow(Platform::Window& aWindow) = 0;

		virtual std::shared_ptr<GraphicsBuffer> CreateGraphicsBuffer(GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride) = 0;

		virtual std::shared_ptr<Mesh> CreateMesh() = 0;

		virtual std::shared_ptr<CachedPipelineState> CreateOrGetPipelineState(const PipelineState& aPipelineState) = 0;

		virtual std::shared_ptr<Shader> CreateShader(const std::filesystem::path& aShaderSource, Shader::Type aShaderType) = 0;
		

		virtual void ExecuteCommandBuffer(const CommandBuffer& aCommandBuffer) = 0;
		virtual void ExecuteTask(const GraphicsTask& aGraphicsTask) = 0;

		virtual bool SupportsMultipleWindows() const = 0;

		virtual void MarkFrameStart() = 0;
		virtual void MarkFrameEnd() = 0;
	};
}