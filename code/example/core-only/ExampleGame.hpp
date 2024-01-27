#pragma once

#include <Bootstrapper.hpp>

#include <Graphics_Buffer.hpp>
#include <Graphics_Manager.hpp>
#include <Graphics_Texture.hpp>

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

	Mesh(RoseGold::Core::Graphics::Manager* aManager)
		: myManager(aManager)
		, myVertexCount(0)
	{ }

	void DrawToFrame(RoseGold::Core::Graphics::FrameContext& aFrameContext)
	{
		aFrameContext.SetPrimitiveTopology(RoseGold::Core::Graphics::PrimitiveTopology::TriangleList);
		aFrameContext.SetVertexBuffer(myVertexBuffer);
		aFrameContext.Draw(myVertexCount, 0);
	}

	void SetVertices(const std::vector<VertexType>& someVertices)
	{
		if (someVertices.empty())
			return;

		myVertexCount = static_cast<std::uint32_t>(someVertices.size());

		myVertexBuffer = myManager->CreateGraphicsBuffer(
			RoseGold::Core::Graphics::GraphicsBuffer::Target::Vertex,
			myVertexCount,
			sizeof(VertexType)
		);
		myVertexBuffer->SetData(
			&someVertices.front(),
			static_cast<std::uint32_t>(myVertexCount * sizeof(VertexType))
		);
	}

protected:
	RoseGold::Core::Graphics::Manager* myManager;
	std::shared_ptr<RoseGold::Core::Graphics::GraphicsBuffer> myVertexBuffer;
	std::uint32_t myVertexCount;
};

struct TexturedVertex
{
	static std::vector<RoseGold::Core::Graphics::PipelineStateDescription::InputLayoutEntry> GetInputLayout();

	float Position[3];
	float UV[2];
};

using TexturedMesh = Mesh<TexturedVertex>;

struct ColoredVertex
{
	static std::vector<RoseGold::Core::Graphics::PipelineStateDescription::InputLayoutEntry> GetInputLayout();

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

class ExampleGame
{
public:
	void OnStart(RoseGold::Client::BootstrapResult& aCoreSetup);
	void OnLoop();
	void OnExit();

private:
	void OnStart_SetupWindows();
	void OnStart_DefineMeshes();
	void OnStart_SetupPipelineStates();
	void OnStart_CreateMVPBuffers();

	RoseGold::Client::BootstrapResult* myCoreSetup;

	std::shared_ptr<RoseGold::Core::Graphics::RenderTexture> myWindow1;
	std::shared_ptr<RoseGold::Core::Graphics::RenderTexture> myWindow2;

	std::shared_ptr<RoseGold::Core::Graphics::Texture> myWhite;
	std::shared_ptr<RoseGold::Core::Graphics::Texture> myCheckerboard;

	std::shared_ptr<RoseGold::Core::Graphics::PipelineState> myColoredMeshPipelineState;
	std::shared_ptr<RoseGold::Core::Graphics::PipelineState> myTexturedMeshPipelineState;

	ColoredMesh myColoredTriangle;
	ColoredMesh myColoredPlane;
	TexturedMesh myTexturedPyramid;

	std::shared_ptr<RoseGold::Core::Graphics::GraphicsBuffer> myRT1PyramidConstants, myRT1TriangleConstants, myRT1PlaneConstants;
	std::shared_ptr<RoseGold::Core::Graphics::GraphicsBuffer> myRT2PyramidConstants, myRT2TriangleConstants, myRT2PlaneConstants;

	std::chrono::high_resolution_clock::time_point myStartTime;
};
