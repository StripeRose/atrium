// Filter "Graphics"

#pragma once

#include "Core_Diagnostics.hpp"
#include "Core_GraphicsBuffer.hpp"
#include "Core_GraphicsEnums.hpp"
#include "Core_GraphicsPipeline.hpp"
#include "Core_RenderTexture.hpp"

#include <rose-common/Color.hpp>
#include <rose-common/math/Geometry.hpp>

#include <functional>
#include <memory>
#include <source_location>
#include <vector>

namespace Atrium::Core
{
	/**
	 * @brief Handles graphics commands and GPU data submission for a specific frame.
	 */
	class FrameGraphicsContext
	{
	public:

		//--------------------------------------------------
		// * Types
		//--------------------------------------------------
		#pragma region Types

		struct ProfileContextZone;

		#pragma endregion

		//--------------------------------------------------
		// * Methods
		//--------------------------------------------------
		#pragma region Methods

		/**
		 * @brief Create a new graphics profiling zone.
		 *        Do not use directly, use "CONTEXT_ZONE" macro instead.
		 */
		virtual void BeginProfileZone(ProfileContextZone& aZoneScope
#ifdef TRACY_ENABLE
			, const tracy::SourceLocationData& aLocation
#endif
		) = 0;

		/**
		 * @brief Clear a render texture's color to a specific value.
		 *        If the chosen target has no color buffer, the call does nothing.
		 * 
		 * @param aTarget Target to clear the color buffer of.
		 * @param aClearColor Color value to clear to.
		 */
		virtual void ClearColor(const std::shared_ptr<RenderTexture>& aTarget, ColorT<float> aClearColor) = 0;

		/**
		 * @brief Clear a render texture's depth and stencil to a specified value.
		 *        If the target has no depth buffer, the call does nothing.
		 * 
		 * @param aTarget Target to clear the depth buffer of.
		 * @param aDepth Depth value to clear to.
		 * @param aStencil Stencil value to clear to.
		 */
		virtual void ClearDepth(const std::shared_ptr<RenderTexture>& aTarget, float aDepth, std::uint8_t aStencil) = 0;

		/**
		 * @brief Disable scissor test culling and utilize the full render target.
		 */
		virtual void DisableScissorRect() = 0;

		virtual void Dispatch(std::uint32_t aGroupCountX, std::uint32_t aGroupCountY, std::uint32_t aGroupCountZ) = 0;
		virtual void Dispatch1D(std::uint32_t aThreadCountX, std::uint32_t aGroupSizeX) = 0;
		virtual void Dispatch2D(std::uint32_t aThreadCountX, std::uint32_t aThreadCountY, std::uint32_t aGroupSizeX, std::uint32_t aGroupSizeY) = 0;
		virtual void Dispatch3D(std::uint32_t aThreadCountX, std::uint32_t aThreadCountY, std::uint32_t aThreadCountZ, std::uint32_t aGroupSizeX, std::uint32_t aGroupSizeY, std::uint32_t aGroupSizeZ) = 0;

		/**
		 * @brief Draw primitives.
		 * 
		 * @param aVertexCount Number of vertices in the primitive.
		 * @param aVertexStartOffset Index for the first vertex of the primitive.
		 */
		virtual void Draw(std::uint32_t aVertexCount, std::uint32_t aVertexStartOffset) = 0;

		/**
		 * @brief Draw indexed primitives.
		 * 
		 * @param anIndexCount Number of vertex indices in the primitive.
		 * @param aStartIndexLocation Location for the first verteex index of the primitive.
		 * @param aBaseVertexLocation Vertex location offset.
		 */
		virtual void DrawIndexed(std::uint32_t anIndexCount, std::uint32_t aStartIndexLocation, std::uint32_t aBaseVertexLocation) = 0;

		/**
		 * @brief Draw instanced primitives.
		 * 
		 * @param aVertexCountPerInstance Number of vertices in the primitive per instance.
		 * @param anInstanceCount Number of instances to draw.
		 * @param aStartVertexLocation Vertex index to use as vertex index 0.
		 * @param aStartInstanceLocation Instance location offset.
		 */
		virtual void DrawInstanced(std::uint32_t aVertexCountPerInstance, std::uint32_t anInstanceCount, std::uint32_t aStartVertexLocation, std::uint32_t aStartInstanceLocation) = 0;

		/**
		 * @brief Draw indexed, instanced primitives.
		 * 
		 * @param anIndexCountPerInstance Number of indices read from the index buffer for each instance.
		 * @param anInstanceCount Number of instances to draw.
		 * @param aStartIndexLocation Location for the first verteex index of the primitive.
		 * @param aBaseVertexLocation Vertex location offset.
		 * @param aStartInstanceLocation Instance location offset.
		 */
		virtual void DrawIndexedInstanced(std::uint32_t anIndexCountPerInstance, std::uint32_t anInstanceCount, std::uint32_t aStartIndexLocation, std::uint32_t aBaseVertexLocation, std::uint32_t aStartInstanceLocation) = 0;

		/**
		 * @brief Set the blend factor that modulate values for a pixel-shader, render-target, or both.
		 * 
		 * @param aBlendFactor Color containing the blend-factors for each color component.
		 * 
		 * @refactor Should this be a Color, or some other structure?
		 */
		virtual void SetBlendFactor(ColorT<float> aBlendFactor) = 0;

		/**
		 * @brief Set the active Pipeline State Object, including its root signature.
		 * 
		 * @param aPipelineState Pipeline State Object to set.
		 */
		virtual void SetPipelineState(const std::shared_ptr<PipelineState>& aPipelineState) = 0;

		/**
		 * @brief Set a graphics buffer containing vertices as a current vertex buffer.
		 * 
		 * @param aVertexBuffer Graphics buffer to use.
		 * @param aSlot Index into the device's zero-based array of vertex buffer slots to set the graphics buffer.
		 */
		virtual void SetVertexBuffer(const std::shared_ptr<const GraphicsBuffer>& aVertexBuffer, unsigned int aSlot = 0) = 0;

		/**
		 * @brief Bind a graphics buffer into a specific slot of the root signature, for use in shaders.
		 * 
		 * @param anUpdateFrequency Update frequency of the chosen resource, to inform which register to use.
		 * @param aRegisterIndex Index of the register to use.
		 * @param aBuffer Graphics buffer to bind.
		 */
		virtual void SetPipelineResource(ResourceUpdateFrequency anUpdateFrequency, std::uint32_t aRegisterIndex, const std::shared_ptr<GraphicsBuffer>& aBuffer) = 0;

		/**
		 * @brief Bind a texture buffer into a specific slot of the root signature, for use in shaders.
		 * 
		 * @param anUpdateFrequency Update frequency of the chosen resource, to inform which register to use.
		 * @param aRegisterIndex Index of the register to use.
		 * @param aTexture Texture buffer to bind.
		 */
		virtual void SetPipelineResource(ResourceUpdateFrequency anUpdateFrequency, std::uint32_t aRegisterIndex, const std::shared_ptr<Texture>& aTexture) = 0;

		/**
		 * @brief Select the type of primitive topology that describes the input data for the Input Assembler stage.
		 * 
		 * @param aTopology Type of primitive.
		 */
		virtual void SetPrimitiveTopology(PrimitiveTopology aTopology) = 0;

		/**
		 * @brief Specify a sub-section of the render target limit drawing within.
		 * 
		 * @param aRectangle Sub-section rectangle in pixels.
		 */
		virtual void SetScissorRect(const RectangleT<int>& aRectangle) = 0;

		/**
		 * @brief Set the reference value for depth stencil tests.
		 * 
		 * @param aStencilRef Reference value to perform against when doing depth-stencil tests.
		 */
		virtual void SetStencilRef(std::uint32_t aStencilRef) = 0;

		/**
		 * @brief Set a list of render targets to use for the graphics pipeline.
		 * 
		 * @param someTargets List of render-targets for shader output. Can be empty if none should be bound.
		 * @param aDepthTarget Depth stencil target to use. Can be null if none should be bound.
		 */
		virtual void SetRenderTargets(const std::vector<std::shared_ptr<RenderTexture>>& someTargets, const std::shared_ptr<RenderTexture>& aDepthTarget) = 0;

		/**
		 * @brief Specify both viewport and scissor rect to cover the screen with the specified screen size.
		 * 
		 * @refactor Doesn't do much different than calling SetScissorRect() and SetViewport().
		 *           Could be removed to simplify.
		 */
		virtual void SetViewportAndScissorRect(const SizeT<int>& aScreenSize) = 0;

		/**
		 * @brief Specify a sub-section of the render target that should be used for drawing.
		 * 
		 * @param aRectangle Sub-section of the render-target, where (0,0) is the center.
		 */
		virtual void SetViewport(const RectangleT<float>& aRectangle) = 0;

		#pragma endregion
	};

	/**
	 * @brief Container for profiling scope data specific to each API.
	 *        This way we prevent a ton of tiny heap allocations, but can still keep the object in memory
	 *        and destroy it when leaving the scope.
	 *        Should not be used directly, but via the "CONTEXT_SCOPE" macro.
	 */
	struct FrameGraphicsContext::ProfileContextZone
	{
		~ProfileContextZone() { Destructor(*this); }

		std::uint_least8_t Data[32];
		std::function<void(ProfileContextZone&)> Destructor;
	};
}

#ifdef TRACY_ENABLE
#define CONTEXT_ZONE(aContext, aName) \
	static constexpr tracy::SourceLocationData TracyConcat(__tracy_source_location, TracyLine) { aName, TracyFunction, TracyFile, (uint32_t)TracyLine, 0 };  \
	Atrium::FrameGraphicsContext::ProfileContextZone TracyConcat(contextZoneScope, __LINE__); \
	(aContext).BeginProfileZone(TracyConcat(contextZoneScope, __LINE__), TracyConcat(__tracy_source_location, TracyLine));
#else
#define CONTEXT_ZONE(aContext, aName)
#endif