// Filter "Graphics"

#pragma once

#include "Core_FrameContext.hpp"
#include "Core_GraphicsBuffer.hpp"
#include "Core_GraphicsPipeline.hpp"
#include "Core_RenderTexture.hpp"

#include <filesystem>

namespace Atrium::Core
{
	class Window;

	/**
	 * @brief Interface for accessing and using graphics devices.
	 */
	class GraphicsAPI
	{
	public:
		virtual ~GraphicsAPI() = default;

		//--------------------------------------------------
		// * Types
		//--------------------------------------------------
		#pragma region Types

		class ResourceManager;

		#pragma endregion

		//--------------------------------------------------
		// * Methods
		//--------------------------------------------------
		#pragma region Methods

		/**
		 * @brief Create a new frame graphics-context to record graphics-commands to.
		 *        Any commands added will be automatically submitted when the frame ends.
		 * @return A pointer to the created context.
		 */
		virtual std::shared_ptr<FrameGraphicsContext> CreateFrameGraphicsContext() = 0;

		/**
		 * @brief Get the current graphics-frame index.
		 * @return The index as an unsigned integer at least 64 bit long.
		 */
		virtual std::uint_least64_t GetCurrentFrameIndex() const = 0;

		/**
		 * @brief Get the graphics resource manager.
		 */
		virtual ResourceManager& GetResourceManager() = 0;

		/**
		 * @brief Mark the start of a graphics frame.
		 */
		virtual void MarkFrameStart() = 0;

		/**
		 * @brief Mark the end of a graphics frame.
		 */
		virtual void MarkFrameEnd() = 0;

		/**
		 * @brief Check whether the API supports rendering to multiple windows.
		 * 
		 * @return True if there is support for rendering to multiple windows simultaneously.
		 */
		virtual bool SupportsMultipleWindows() const = 0;

		#pragma endregion
	};

	class GraphicsAPI::ResourceManager
	{
	public:

		/**
		 * @brief Create a render texture for a particular window.
		 * 
		 * @param aWindow Window to create the texture for.
		 * @return The created render texture.
		 */
		virtual std::shared_ptr<RenderTexture> CreateRenderTextureForWindow(Window& aWindow) = 0;

		/**
		 * @brief Create a new graphics buffer.
		 * 
		 * @param someTargets Flags specifying which targets the buffer should be set up to support.
		 * @param aCount Amount of elements the buffer should contain.
		 * @param aStride Size of an individual element.
		 * @return The created graphics buffer with a size equal to or greater than (aCount * aStride).
		 */
		virtual std::shared_ptr<GraphicsBuffer> CreateGraphicsBuffer(GraphicsBuffer::Target someTargets, std::uint32_t aCount, std::uint32_t aStride) = 0;
		
		/**
		 * @brief Create a new Pipeline State Object according to the provided Pipeline State Description.
		 * 
		 * @param aPipelineState Description for the pipeline state requested.
		 * @return The created Pipeline State Object.
		 */
		virtual std::shared_ptr<PipelineState> CreatePipelineState(const PipelineStateDescription& aPipelineState) = 0;

		/**
		 * @brief Create a new Root Signature Builder which allows you to create a new Root Signature object.
		 * 
		 * @return The created RootSignatureBuilder.
		 */
		virtual std::unique_ptr<RootSignatureBuilder> CreateRootSignature() = 0;

		/**
		 * @brief Compile a shader from a file-system source file.
		 * 
		 * @param aShaderSource Path to the shader source code file.
		 * @param aShaderType Type of shader to compile.
		 * @param anEntryPoint Name of the shader entry-point function to compile.
		 * @return The created shader.
		 */
		virtual std::shared_ptr<Shader> CreateShader(const std::filesystem::path& aShaderSource, Shader::Type aShaderType, const char* anEntryPoint) = 0;

		/**
		 * @brief Create a CPU-editable texture.
		 * 
		 * @param aWidth Width of the texture.
		 * @param aHeight Height of the texture.
		 * @param aDepth Depth of the texture.
		 * @param aTextureFormat Texture format of the texture.
		 * @param aDimension The texture dimension. Deduced from size dimensions if omitted.
		 * @param anArrayCount An amount of textures in the array. Defaults to 1.
		 * @return The created texture.
		 */
		std::shared_ptr<Core::Texture> CreateTexture(unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, TextureFormat aTextureFormat, std::optional<TextureDimension> aDimension)
		{
			return CreateTexture(aWidth, aHeight, aDepth, 1, aTextureFormat, aDimension);
		}

		virtual std::shared_ptr<Core::Texture> CreateTexture(unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, unsigned int anArrayCount, TextureFormat aTextureFormat, std::optional<TextureDimension> aDimension) = 0;

		/**
		 * @brief Load a texture from a file-system path.
		 * 
		 * @param aPath Path to the texture-file.
		 * @return The loaded texture.
		 */
		virtual std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& aPath) = 0;
	};
}