#include "Graphics_CommandBuffer.hpp"

namespace RoseGold::Core::Graphics
{
	void CommandBuffer::RerecordTo(CommandBuffer& anOtherCommandBuffer) const
	{
		for (const auto& commands : myRecordedCommands)
			commands(anOtherCommandBuffer);
	}

	void CommandBuffer::Clear(std::shared_ptr<RenderTexture> aTarget, Color aClearColor)
	{
		if (!aTarget)
			return;

		myRecordedCommands.emplace_back(
			[aTarget, aClearColor](CommandBuffer& aBuffer)
			{
				aBuffer.Clear(aTarget, aClearColor);
			}
		);
	}

	void CommandBuffer::Clear(std::shared_ptr<RenderTexture> aTarget, float aClearDepth)
	{
		if (!aTarget)
			return;

		myRecordedCommands.emplace_back(
			[aTarget, aClearDepth](CommandBuffer& aBuffer)
			{
				aBuffer.Clear(aTarget, aClearDepth);
			}
		);
	}

	void CommandBuffer::Clear(std::shared_ptr<RenderTexture> aTarget, Color aClearColor, float aClearDepth)
	{
		if (!aTarget)
			return;

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

	void CommandBuffer::DrawMesh(std::shared_ptr<Mesh> aMesh, Math::Matrix aMatrix, int aSubmeshIndex)
	{
		myRecordedCommands.emplace_back(
			[aMesh, aMatrix, aSubmeshIndex](CommandBuffer& aBuffer)
			{
				aBuffer.DrawMesh(aMesh, aMatrix, aSubmeshIndex);
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

	void CommandBuffer::SetPipelineState(std::shared_ptr<PipelineState> aPipelineState)
	{
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
