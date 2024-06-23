#pragma once

#include <vector>

#include "Graphics_Buffer.hpp"
#include "Graphics_Manager.hpp"
#include "Graphics_Pipeline.hpp"

template <typename VertexType>
class Mesh
{
public:
	Mesh()
		: myManager(nullptr)
		, myVertexCount(0)
	{ }

	Mesh(RoseGold::Core::GraphicsAPI* aManager)
		: myManager(aManager)
		, myVertexCount(0)
		, myName(L"Mesh")
	{ }

	void DrawToFrame(RoseGold::Core::FrameContext& aFrameContext)
	{
		aFrameContext.SetPrimitiveTopology(RoseGold::Core::PrimitiveTopology::TriangleList);
		aFrameContext.SetVertexBuffer(myVertexBuffer);
		aFrameContext.Draw(myVertexCount, 0);
	}

	void SetVertices(const std::vector<VertexType>& someVertices)
	{
		if (someVertices.empty())
			return;

		myVertexCount = static_cast<std::uint32_t>(someVertices.size());

		myVertexBuffer = myManager->GetResourceManager().CreateGraphicsBuffer(
			RoseGold::Core::GraphicsBuffer::Target::Vertex,
			myVertexCount,
			sizeof(VertexType)
		);
		myVertexBuffer->SetName((myName + L" vertices").c_str());
		myVertexBuffer->SetData(
			&someVertices.front(),
			static_cast<std::uint32_t>(myVertexCount * sizeof(VertexType))
		);
	}

	void SetName(const std::wstring& aName)
	{
		myName = aName;
		if (myVertexBuffer)
			myVertexBuffer->SetName((aName + L" vertices").c_str());
	}

protected:
	RoseGold::Core::GraphicsAPI* myManager;
	std::shared_ptr<RoseGold::Core::GraphicsBuffer> myVertexBuffer;
	std::uint32_t myVertexCount;
	std::wstring myName;
};

struct ColoredVertex
{
	static inline std::vector<RoseGold::Core::PipelineStateDescription::InputLayoutEntry> GetInputLayout()
	{
		std::vector<RoseGold::Core::PipelineStateDescription::InputLayoutEntry> layout;
		layout.emplace_back("POSITION", RoseGold::Core::GraphicsFormat::R32G32B32_SFloat);
		layout.emplace_back("COLOR", RoseGold::Core::GraphicsFormat::R32G32B32A32_SFloat);
		return layout;
	}

	float Position[3];
	float Color[4];
};
using ColoredMesh = Mesh<ColoredVertex>;