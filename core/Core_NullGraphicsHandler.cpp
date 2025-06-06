// Filter "Graphics"

#include "Core_NullGraphicsHandler.hpp"

namespace Atrium::Core
{
	class NullFrameGraphicsContext final : public FrameGraphicsContext
	{
	public:
		void BeginProfileZone(ProfileContextZone&
			#ifdef TRACY_ENABLE
			, const tracy::SourceLocationData&
			#endif
		) override {
		}

		void ClearColor(const std::shared_ptr<RenderTexture>&, ColorT<float>) override {}
		void ClearDepth(const std::shared_ptr<RenderTexture>&, float, std::uint8_t) override {}

		void DisableScissorRect() override {}

		void Dispatch(std::uint32_t, std::uint32_t, std::uint32_t) override {}
		void Dispatch1D(std::uint32_t, std::uint32_t) override {}
		void Dispatch2D(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t) override {}
		void Dispatch3D(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t) override {}

		void Draw(std::uint32_t, std::uint32_t) override {}
		void DrawIndexed(std::uint32_t, std::uint32_t, std::uint32_t) override {}
		void DrawInstanced(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t) override {}
		void DrawIndexedInstanced(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t) override {}

		void SetBlendFactor(ColorT<float>) override {}
		void SetPipelineState(const std::shared_ptr<PipelineState>&) override {}
		void SetVertexBuffer(const std::shared_ptr<const GraphicsBuffer>&, unsigned int) override {}
		void SetPipelineResource(ResourceUpdateFrequency, std::uint32_t, const std::shared_ptr<GraphicsBuffer>&) override {}
		void SetPipelineResource(ResourceUpdateFrequency, std::uint32_t, const std::shared_ptr<Texture>&) override {}
		void SetPrimitiveTopology(PrimitiveTopology) override {}
		void SetScissorRect(const RectangleT<int>&) override {}
		void SetStencilRef(std::uint32_t) override {}
		void SetRenderTargets(const std::vector<std::shared_ptr<RenderTexture>>&, const std::shared_ptr<RenderTexture>&) override {}
		void SetViewportAndScissorRect(const SizeT<int>&) override {}
		void SetViewport(const RectangleT<float>&) override {}
	};

	class NullResourceManager final : public GraphicsAPI::ResourceManager
	{
		std::shared_ptr<RenderTexture> CreateRenderTextureForWindow(Window&) { return nullptr; }
		std::shared_ptr<GraphicsBuffer> CreateGraphicsBuffer(GraphicsBuffer::Target, std::uint32_t, std::uint32_t) { return nullptr; }
		std::shared_ptr<PipelineState> CreatePipelineState(const PipelineStateDescription&) { return nullptr; }
		std::unique_ptr<RootSignatureBuilder> CreateRootSignature() { return nullptr; }
		std::shared_ptr<Shader> CreateShader(const std::filesystem::path&, Shader::Type, const char*) { return nullptr; }
		std::shared_ptr<Texture2D> CreateTexture2D(unsigned int, unsigned int, TextureFormat) { return nullptr; }
		std::shared_ptr<Texture3D> CreateTexture3D(unsigned int, unsigned int, unsigned int, TextureFormat) { return nullptr; }
		std::shared_ptr<TextureCube> CreateTextureCube(unsigned int, TextureFormat) { return nullptr; }
		std::shared_ptr<Texture> LoadTexture(const std::filesystem::path&) { return nullptr; }
	};

	NullGraphicsHandler::NullGraphicsHandler()
		: myFrameCounter(0)
	{
	}

	FrameGraphicsContext& NullGraphicsHandler::GetCurrentFrameContext()
	{
		static NullFrameGraphicsContext nullContext;
		return nullContext;
	}

	std::uint_least64_t NullGraphicsHandler::GetCurrentFrameIndex() const
	{
		return myFrameCounter;
	}

	GraphicsAPI::ResourceManager& NullGraphicsHandler::GetResourceManager()
	{
		static NullResourceManager nullResourceManager;
		return nullResourceManager;
	}

	void NullGraphicsHandler::MarkFrameStart()
	{
		myFrameCounter += 1;
	}

	void NullGraphicsHandler::MarkFrameEnd()
	{
		
	}

	bool NullGraphicsHandler::SupportsMultipleWindows() const
	{
		return false;
	}
}
