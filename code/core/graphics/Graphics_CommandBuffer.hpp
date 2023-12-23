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

		virtual void Clear(Color aClearColor);
		virtual void Clear(float aClearDepth);
		virtual void Clear(Color aClearColor, float aClearDepth);

		// virtual void ConvertTexture(Texture aSource, Texture aDestination);

		// virtual void CopyBuffer(GraphicsBuffer aSource, GraphicsBuffer aDestination);

		// virtual void CopyTexture(RenderTarget aSource, RenderTarget aDestination);

		// virtual Rendering::GraphicsFence CreateAsyncGraphicsFence();
		// virtual Rendering::GraphicsFence CreateGraphicsFence();

		//virtual void CustomCommand(std::function<void(CommandBuffer&)> aCallback);

		//virtual void DisableScissorRect();

		// virtual void DispatchCompute(ComputeShader aShader);

		virtual void DrawMesh(std::shared_ptr<Mesh> aMesh, Math::Matrix aMatrix, std::shared_ptr<CachedPipelineState> aPipelineState, int aSubmeshIndex);
		//virtual void DrawMesh(std::shared_ptr<Mesh> aMesh, Math::Matrix aMatrix, std::shared_ptr<Material> aMaterial, int aSubmeshIndex);

		// Todo: Figure out mesh instancing.

		//virtual void SetScissorRect(const System::Math::Rectangle<float>& aRectangle);

		// virtual void SetBufferData(ComputeBuffer aBuffer, void* someData, size_t aDataSize);
		// template <typename T> virtual void SetBufferData(ComputeBuffer aBuffer, const List<T>& someData);
		// virtual void SetBufferData(GraphicsBuffer aBuffer, void* someData, size_t aDataSize);
		// template <typename T> virtual void SetBufferData(GraphicsBuffer aBuffer, const List<T>& someData);

		//virtual void SetProjectionMatrix(const System::Math::Matrix& aMatrix);

		virtual void SetRenderTarget(std::shared_ptr<RenderTexture> aTexture);
		//virtual void SetRenderTarget(RenderTexture& aTexture, unsigned int aMipLevel);

		// Transforms world space into camera space.
		//virtual void SetViewMatrix(const System::Math::Matrix& aMatrix);

		//virtual void SetViewport(const System::Math::Rectangle<float>& aRectangle);

		// virtual void WaitOnGraphicsFence(GraphicsFence aFence);

	protected:
		std::vector<std::function<void(CommandBuffer&)>> myRecordedCommands;
	};
}
