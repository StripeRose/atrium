#include "Graphics_CommandBuffer.hpp"

#include "Common_Debug.hpp"

namespace RoseGold::Core::Graphics
{
	void CommandBuffer::RerecordTo(CommandBuffer& anOtherCommandBuffer) const
	{
		for (const auto& commands : myRecordedCommands)
			commands(anOtherCommandBuffer);
	}

	void CommandBuffer::Clear(const std::shared_ptr<RenderTexture>& aTarget, Color aClearColor)
	{
		Debug::Assert(!!aTarget, "Clear(aRenderTarget, aClearColor) requires target to be non-null.");

		myRecordedCommands.emplace_back(
			[aTarget, aClearColor](CommandBuffer& aBuffer)
			{
				aBuffer.Clear(aTarget, aClearColor);
			}
		);
	}

	void CommandBuffer::Clear(const std::shared_ptr<RenderTexture>& aTarget, float aClearDepth)
	{
		Debug::Assert(!!aTarget, "Clear(aRenderTarget, aClearDepth) requires target to be non-null.");

		myRecordedCommands.emplace_back(
			[aTarget, aClearDepth](CommandBuffer& aBuffer)
			{
				aBuffer.Clear(aTarget, aClearDepth);
			}
		);
	}

	void CommandBuffer::Clear(const std::shared_ptr<RenderTexture>& aTarget, Color aClearColor, float aClearDepth)
	{
		Debug::Assert(!!aTarget, "Clear(aRenderTarget, aClearColor, aClearDepth) requires target to be non-null.");

		myRecordedCommands.emplace_back(
			[aTarget, aClearColor, aClearDepth](CommandBuffer& aBuffer)
			{
				aBuffer.Clear(aTarget, aClearColor, aClearDepth);
			}
		);
	}

	void CommandBuffer::DisableScissorRect()
	{
		myRecordedCommands.emplace_back(
			[](CommandBuffer& aBuffer)
			{
				aBuffer.DisableScissorRect();
			}
		);
	}

	void CommandBuffer::DrawMesh(const std::shared_ptr<Mesh>& aMesh, Math::Matrix aMatrix, const std::shared_ptr<Texture>& aTexture, int aSubmeshIndex)
	{
		Debug::Assert(!!aMesh, "DrawMesh(aMesh, aMatrix, aSubmeshIndex) requires mesh to be non-null.");

		myRecordedCommands.emplace_back(
			[aMesh, aMatrix, aTexture, aSubmeshIndex](CommandBuffer& aBuffer)
			{
				aBuffer.DrawMesh(aMesh, aMatrix, aTexture, aSubmeshIndex);
			}
		);
	}

	void CommandBuffer::SetScissorRect(const Math::RectangleT<int>& aRectangle)
	{
		myRecordedCommands.emplace_back(
			[aRectangle](CommandBuffer& aBuffer)
			{
				aBuffer.SetScissorRect(aRectangle);
			}
		);
	}

	void CommandBuffer::SetPipelineState(const std::shared_ptr<PipelineState>& aPipelineState)
	{
		Debug::Assert(!!aPipelineState, "SetPipelineState(aPipelineState) requires pipeline state to be non-null.");

		myRecordedCommands.emplace_back(
			[aPipelineState](CommandBuffer& aBuffer)
			{
				aBuffer.SetPipelineState(aPipelineState);
			}
		);
	}

	void CommandBuffer::SetProjectionMatrix(const Math::Matrix& aMatrix)
	{
		myRecordedCommands.emplace_back(
			[aMatrix](CommandBuffer& aBuffer)
			{
				aBuffer.SetProjectionMatrix(aMatrix);
			}
		);
	}

	void CommandBuffer::SetRenderTarget(const std::shared_ptr<RenderTexture>& aRenderTarget)
	{
		Debug::Assert(!!aRenderTarget, "SetRenderTarget(aRenderTarget) requires target to be non-null.");

		myRecordedCommands.emplace_back(
			[aRenderTarget](CommandBuffer& aBuffer)
			{
				aBuffer.SetRenderTarget(aRenderTarget);
			}
		);
	}

	void CommandBuffer::SetRenderTarget(const std::shared_ptr<RenderTexture>& aRenderTarget, const std::shared_ptr<RenderTexture>& aDepthTarget)
	{
		Debug::Assert(aRenderTarget && aDepthTarget, "SetRenderTarget(aRenderTarget, aDepthTarget) requires targets to be non-null.");

		myRecordedCommands.emplace_back(
			[aRenderTarget, aDepthTarget](CommandBuffer& aBuffer)
			{
				aBuffer.SetRenderTarget(aRenderTarget, aDepthTarget);
			}
		);
	}

	void CommandBuffer::SetRenderTarget(const std::initializer_list<const std::shared_ptr<RenderTexture>>& someRenderTargets)
	{
		for (const auto& target : someRenderTargets)
			Debug::Assert(!!target, "SetRenderTarget(someRenderTargets) requires targets to be non-null.");

		myRecordedCommands.emplace_back(
			[someRenderTargets](CommandBuffer& aBuffer)
			{
				aBuffer.SetRenderTarget(someRenderTargets);
			}
		);
	}

	void CommandBuffer::SetRenderTarget(const std::initializer_list<const std::shared_ptr<RenderTexture>>& someRenderTargets, const std::shared_ptr<RenderTexture>& aDepthTarget)
	{
		for (const auto& target : someRenderTargets)
			Debug::Assert(!!target, "SetRenderTarget(someRenderTargets, aDepthTarget) requires targets to be non-null.");
		Debug::Assert(!!aDepthTarget, "SetRenderTarget(someRenderTargets, aDepthTarget) requires targets to be non-null.");

		myRecordedCommands.emplace_back(
			[someRenderTargets, aDepthTarget](CommandBuffer& aBuffer)
			{
				aBuffer.SetRenderTarget(someRenderTargets, aDepthTarget);
			}
		);
	}

	void CommandBuffer::SetViewMatrix(const Math::Matrix& aMatrix)
	{
		myRecordedCommands.emplace_back(
			[aMatrix](CommandBuffer& aBuffer)
			{
				aBuffer.SetViewMatrix(aMatrix);
			}
		);
	}

	void CommandBuffer::SetViewport(const Math::Rectangle& aRectangle)
	{
		myRecordedCommands.emplace_back(
			[aRectangle](CommandBuffer& aBuffer)
			{
				aBuffer.SetViewport(aRectangle);
			}
		);
	}
}
