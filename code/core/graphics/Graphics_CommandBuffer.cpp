#include "Graphics_CommandBuffer.hpp"

namespace RoseGold::Core::Graphics
{
	void CommandBuffer::RerecordTo(CommandBuffer& anOtherCommandBuffer) const
	{
		for (const auto& commands : myRecordedCommands)
			commands(anOtherCommandBuffer);
	}

	void CommandBuffer::Clear(Color aClearColor)
	{
		myRecordedCommands.emplace_back(
			[aClearColor](CommandBuffer& aBuffer)
			{
				aBuffer.Clear(aClearColor);
			}
		);
	}

	void CommandBuffer::Clear(float aClearDepth)
	{
		myRecordedCommands.emplace_back(
			[aClearDepth](CommandBuffer& aBuffer)
			{
				aBuffer.Clear(aClearDepth);
			}
		);
	}

	void CommandBuffer::Clear(Color aClearColor, float aClearDepth)
	{
		myRecordedCommands.emplace_back(
			[aClearColor, aClearDepth](CommandBuffer& aBuffer)
			{
				aBuffer.Clear(aClearColor, aClearDepth);
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

	void CommandBuffer::DrawMesh(std::shared_ptr<Mesh> aMesh, Math::Matrix aMatrix, std::shared_ptr<CachedPipelineState> aPipelineState, int aSubmeshIndex)
	{
		myRecordedCommands.emplace_back(
			[aMesh, aMatrix, aPipelineState, aSubmeshIndex](CommandBuffer& aBuffer)
			{
				aBuffer.DrawMesh(aMesh, aMatrix, aPipelineState, aSubmeshIndex);
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

	void CommandBuffer::SetProjectionMatrix(const Math::Matrix& aMatrix)
	{
		myRecordedCommands.emplace_back(
			[aMatrix](CommandBuffer& aBuffer)
			{
				aBuffer.SetProjectionMatrix(aMatrix);
			}
		);
	}

	void CommandBuffer::SetRenderTarget(std::shared_ptr<RenderTexture> aTexture)
	{
		myRecordedCommands.emplace_back(
			[aTexture](CommandBuffer& aBuffer)
			{
				aBuffer.SetRenderTarget(aTexture);
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
