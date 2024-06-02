#pragma once

#include <Engine_Game.hpp>

#include <Graphics_Buffer.hpp>
#include <Graphics_Manager.hpp>
#include <Graphics_Texture.hpp>

#include "ChartDebugger.hpp"

#include <memory>
#include <vector>

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

struct TexturedVertex
{
	static std::vector<RoseGold::Core::PipelineStateDescription::InputLayoutEntry> GetInputLayout();

	float Position[3];
	float UV[2];
};

using TexturedMesh = Mesh<TexturedVertex>;

struct ColoredVertex
{
	static std::vector<RoseGold::Core::PipelineStateDescription::InputLayoutEntry> GetInputLayout();

	float Position[3];
	float Color[4];
};
using ColoredMesh = Mesh<ColoredVertex>;

struct CameraConstants
{
	RoseGold::Math::Matrix Projection;
	RoseGold::Math::Matrix Model;
	RoseGold::Math::Matrix View;
};

class ExampleGame : public RoseGold::Game
{
public:
	void OnStart() override;
	void OnLoop() override;
	void OnImGui() override;
	void OnExit() override;

private:
	void OnStart_SetupWindows();
	void OnStart_DefineMeshes();
	void OnStart_SetupPipelineStates();
	void OnStart_CreateMVPBuffers();

	std::shared_ptr<RoseGold::Core::RenderTexture> myWindow1;
	std::shared_ptr<RoseGold::Core::RenderTexture> myWindow2;

	std::shared_ptr<RoseGold::Core::Texture> myWhite;
	std::shared_ptr<RoseGold::Core::Texture> myCheckerboard;

	std::shared_ptr<RoseGold::Core::PipelineState> myColoredMeshPipelineState;
	std::shared_ptr<RoseGold::Core::PipelineState> myTexturedMeshPipelineState;

	ColoredMesh myColoredTriangle;
	ColoredMesh myColoredPlane;
	TexturedMesh myTexturedPyramid;

	std::shared_ptr<RoseGold::Core::GraphicsBuffer> myRT1PyramidConstants, myRT1TriangleConstants, myRT1PlaneConstants;
	std::shared_ptr<RoseGold::Core::GraphicsBuffer> myRT2PyramidConstants, myRT2TriangleConstants, myRT2PlaneConstants;

	std::chrono::high_resolution_clock::time_point myStartTime;

	ChartInspector myChartDebugger;
};
