#pragma once

#include "Common_Color.hpp"
#include "Common_Profiling.hpp"
#include "Common_Math.hpp"

#include "Graphics_Buffer.hpp"
#include "Graphics_RenderTexture.hpp"
#include "Graphics_Pipeline.hpp"

#include <functional>
#include <memory>
#include <source_location>
#include <vector>

namespace RoseGold::Core::Graphics
{
	enum class ResourceUpdateFrequency
	{
		PerObject,
		PerMaterial,
		PerPass,
		PerFrame,
		Constant
	};

	enum class PrimitiveTopology
	{
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip
	};

	class FrameContext
	{
	public:
		struct ContextZone
		{
			~ContextZone() { Destructor(*this); }

			std::uint_least8_t Data[32];
			std::function<void(ContextZone&)> Destructor;
		};

	public:
		virtual void BeginZone(ContextZone& aZoneScope
#ifdef TRACY_ENABLE
			, const tracy::SourceLocationData& aLocation
#endif
		) = 0;

		virtual void ClearColor(const std::shared_ptr<Core::Graphics::RenderTexture>& aTarget, Color aClearColor) = 0;
		virtual void ClearDepth(const std::shared_ptr<Core::Graphics::RenderTexture>& aTarget, float aDepth, std::uint8_t aStencil) = 0;

		virtual void DisableScissorRect() = 0;

		virtual void Dispatch(std::uint32_t aGroupCountX, std::uint32_t aGroupCountY, std::uint32_t aGroupCountZ) = 0;
		virtual void Dispatch1D(std::uint32_t aThreadCountX, std::uint32_t aGroupSizeX) = 0;
		virtual void Dispatch2D(std::uint32_t aThreadCountX, std::uint32_t aThreadCountY, std::uint32_t aGroupSizeX, std::uint32_t aGroupSizeY) = 0;
		virtual void Dispatch3D(std::uint32_t aThreadCountX, std::uint32_t aThreadCountY, std::uint32_t aThreadCountZ, std::uint32_t aGroupSizeX, std::uint32_t aGroupSizeY, std::uint32_t aGroupSizeZ) = 0;

		virtual void Draw(std::uint32_t aVertexCount, std::uint32_t aVertexStartOffset) = 0;
		virtual void DrawIndexed(std::uint32_t anIndexCount, std::uint32_t aStartIndexLocation, std::uint32_t aBaseVertexLocation) = 0;
		virtual void DrawInstanced(std::uint32_t aVertexCountPerInstance, std::uint32_t anInstanceCount, std::uint32_t aStartVertexLocation, std::uint32_t aStartInstanceLocation) = 0;
		virtual void DrawIndexedInstanced(std::uint32_t anIndexCountPerInstance, std::uint32_t anInstanceCount, std::uint32_t aStartIndexLocation, std::uint32_t aBaseVertexLocation, std::uint32_t aStartInstanceLocation) = 0;

		virtual void SetBlendFactor(Color aBlendFactor) = 0;
		virtual void SetPipelineState(const std::shared_ptr<Core::Graphics::PipelineState>& aPipelineState) = 0;
		virtual void SetVertexBuffer(const std::shared_ptr<const Core::Graphics::GraphicsBuffer>& aVertexBuffer) = 0;
		virtual void SetPipelineResource(ResourceUpdateFrequency anUpdateFrequency, std::uint32_t aRegisterIndex, const std::shared_ptr<Core::Graphics::GraphicsBuffer>& aBuffer) = 0;
		virtual void SetPipelineResource(ResourceUpdateFrequency anUpdateFrequency, std::uint32_t aRegisterIndex, const std::shared_ptr<Core::Graphics::Texture>& aTexture) = 0;
		virtual void SetPrimitiveTopology(PrimitiveTopology aTopology) = 0;
		virtual void SetScissorRect(const Math::RectangleT<int>& aRectangle) = 0;
		virtual void SetStencilRef(std::uint32_t aStencilRef) = 0;
		virtual void SetRenderTargets(const std::vector<std::shared_ptr<Core::Graphics::RenderTexture>>& someTargets, const std::shared_ptr<Core::Graphics::RenderTexture>& aDepthTarget) = 0;
		virtual void SetViewportAndScissorRect(const Size& aScreenSize) = 0;
		virtual void SetViewport(const Math::Rectangle& aRectangle) = 0;
	};
}

#ifdef TRACY_ENABLE
#define CONTEXT_ZONE(aContext, aName) \
	static constexpr tracy::SourceLocationData TracyConcat(__tracy_source_location, TracyLine) { aName, TracyFunction, TracyFile, (uint32_t)TracyLine, 0 };  \
	RoseGold::Core::Graphics::FrameContext::ContextZone TracyConcat(contextZoneScope, __LINE__); \
	aContext.BeginZone(TracyConcat(contextZoneScope, __LINE__), TracyConcat(__tracy_source_location, TracyLine));
#else
#define CONTEXT_ZONE(aContext, aName)
#endif