#include "Graphics_CommandBuffer.hpp"

namespace RoseGold::Core::Graphics
{
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

	void CommandBuffer::Reset()
	{
		myRecordedCommands.clear();
	}
}
