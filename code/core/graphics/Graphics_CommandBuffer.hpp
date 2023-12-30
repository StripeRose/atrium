#pragma once

#include "Graphics_Mesh.hpp"
#include "Graphics_Pipeline.hpp"
#include "Graphics_RenderTexture.hpp"

#include "Common_Color.hpp"

#include <functional>
#include <vector>

namespace RoseGold::Core::Graphics
{
	class CommandBuffer
	{
	public:
		void RerecordTo(CommandBuffer& anOtherCommandBuffer) const;

		// virtual void Blit(Texture aSource, RenderTarget aTarget);
		// virtual void Blit(Texture aSource, RenderTarget aTarget, Material aMaterial);
		// virtual void Blit(Texture aSource, RenderTarget aTarget, Vector2 aScale, Vector2 anOffset);
		// virtual void Blit(RenderTarget aSource, RenderTarget aTarget);
		// virtual void Blit(RenderTarget aSource, RenderTarget aTarget, Material aMaterial);
		// virtual void Blit(RenderTarget aSource, RenderTarget aTarget, Vector2 aScale, Vector2 anOffset);

		virtual void Clear(std::shared_ptr<RenderTexture> aTarget, Color aClearColor);
		virtual void Clear(std::shared_ptr<RenderTexture> aTarget, float aClearDepth);
		virtual void Clear(std::shared_ptr<RenderTexture> aTarget, Color aClearColor, float aClearDepth);

		// virtual void ConvertTexture(Texture aSource, Texture aDestination);

		// virtual void CopyBuffer(GraphicsBuffer aSource, GraphicsBuffer aDestination);

		// virtual void CopyTexture(RenderTarget aSource, RenderTarget aDestination);

		// virtual Rendering::GraphicsFence CreateAsyncGraphicsFence();
		// virtual Rendering::GraphicsFence CreateGraphicsFence();

		//virtual void CustomCommand(std::function<void(CommandBuffer&)> aCallback);

		virtual void DisableScissorRect();

		// virtual void DispatchCompute(ComputeShader aShader);

		virtual void DrawMesh(std::shared_ptr<Mesh> aMesh, Math::Matrix aMatrix, int aSubmeshIndex);
		//virtual void DrawMesh(std::shared_ptr<Mesh> aMesh, Math::Matrix aMatrix, std::shared_ptr<Material> aMaterial, int aSubmeshIndex);

		// Todo: Figure out mesh instancing.

		// virtual void SetBufferData(ComputeBuffer aBuffer, void* someData, size_t aDataSize);
		// template <typename T> virtual void SetBufferData(ComputeBuffer aBuffer, const List<T>& someData);
		// virtual void SetBufferData(GraphicsBuffer aBuffer, void* someData, size_t aDataSize);
		// template <typename T> virtual void SetBufferData(GraphicsBuffer aBuffer, const List<T>& someData);

		virtual void SetPipelineState(std::shared_ptr<CachedPipelineState> aPipelineState);

		virtual void SetProjectionMatrix(const Math::Matrix& aMatrix);

		virtual void SetScissorRect(const Math::RectangleT<int>& aRectangle);

		// Transforms world space into camera space.
		virtual void SetViewMatrix(const Math::Matrix& aMatrix);

		virtual void SetViewport(const Math::Rectangle& aRectangle);

		// virtual void WaitOnGraphicsFence(GraphicsFence aFence);

	protected:
		std::vector<std::function<void(CommandBuffer&)>> myRecordedCommands;
	};
}
