#pragma once

#include "Common_Color.hpp"

#include <functional>
#include <vector>

namespace RoseGold::Core::Graphics
{
	class CommandBuffer
	{
	public:
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

		// virtual void DrawMesh(Mesh aMesh, Matrix aMatrix, Material aMaterial, int aSubmeshIndex, MaterialPropertyBlock someProperties);

		// Todo: Figure out mesh instancing.

		//virtual void SetScissorRect(const System::Math::Rectangle<float>& aRectangle);

		// virtual void SetBufferData(ComputeBuffer aBuffer, void* someData, size_t aDataSize);
		// template <typename T> virtual void SetBufferData(ComputeBuffer aBuffer, const List<T>& someData);
		// virtual void SetBufferData(GraphicsBuffer aBuffer, void* someData, size_t aDataSize);
		// template <typename T> virtual void SetBufferData(GraphicsBuffer aBuffer, const List<T>& someData);

		//virtual void SetProjectionMatrix(const System::Math::Matrix& aMatrix);

		//virtual void SetRenderTarget(Resource<RenderTexture> aTexture);
		// virtual void SetRenderTarget(Resource<RenderTexture> aTexture, unsigned int aMipLevel);

		// Transforms world space into camera space.
		//virtual void SetViewMatrix(const System::Math::Matrix& aMatrix);

		//virtual void SetViewport(const System::Math::Rectangle<float>& aRectangle);

		// virtual void WaitOnGraphicsFence(GraphicsFence aFence);

		virtual void Reset();

	protected:
		std::vector<std::function<void(CommandBuffer&)>> myRecordedCommands;
	};
}
