
#include <rose-common/CommandLineParser.hpp>

#include <Bootstrapper.hpp>

#include <Common_Math.hpp>

#include <Graphics_Mesh.hpp>
#include <Graphics_Pipeline.hpp>
#include <Graphics_Tasks.hpp>
#include <Graphics_Texture2D.hpp>

#include <Windows.h>

#include <chrono>
#include <thread>

std::chrono::high_resolution_clock::time_point ourStartTime;
std::shared_ptr<RoseGold::Core::Graphics::RenderTexture> ourRT1, ourRT2;

std::shared_ptr<RoseGold::Core::Graphics::PipelineState> ourRT1GenericMeshPipelineState, ourRT2GenericMeshPipelineState;
std::shared_ptr<RoseGold::Core::Graphics::PipelineState> ourRT1ColoredMeshPipelineState, ourRT2ColoredMeshPipelineState;

std::shared_ptr<RoseGold::Core::Graphics::Texture2D> ourDDSFile;
std::shared_ptr<RoseGold::Core::Graphics::Mesh> ourColoredCube, ourGenericPlane, ourGenericSphere;

void SetupResources(RoseGold::Client::BootstrapResult& roseGold)
{
	ourDDSFile = std::static_pointer_cast<RoseGold::Core::Graphics::Texture2D>(
		roseGold.GraphicsManager->LoadTexture(L"debug/images/checkered2.dds")
	);

	RoseGold::Core::Platform::WindowManager::CreationParameters windowParams;

	windowParams.Title = "Window 1";
	windowParams.Size = { 640, 480 };
	auto window1 = roseGold.WindowManager->NewWindow(windowParams);
	ourRT1 = roseGold.GraphicsManager->CreateRenderTextureForWindow(*window1);
	window1->Closed.Connect(nullptr, [&](RoseGold::Core::Platform::Window&) {
		ourRT1.reset();
		});

	windowParams.Title = "Window 2";
	windowParams.Size = { 640, 480 };
	auto window2 = roseGold.WindowManager->NewWindow(windowParams);
	ourRT2 = roseGold.GraphicsManager->CreateRenderTextureForWindow(*window2);
	window2->Closed.Connect(nullptr, [&](RoseGold::Core::Platform::Window&) {
		ourRT2.reset();
		});

	{
		ourColoredCube = std::make_shared<RoseGold::Core::Graphics::ColoredMesh>(*roseGold.GraphicsManager);
		ourColoredCube->SetFromPrimitive(RoseGold::Core::Graphics::MeshPrimitiveType::Cube);

		ourGenericPlane = std::make_shared<RoseGold::Core::Graphics::GenericMesh>(*roseGold.GraphicsManager);
		ourGenericPlane->SetFromPrimitive(RoseGold::Core::Graphics::MeshPrimitiveType::Plane);

		ourGenericSphere = std::make_shared<RoseGold::Core::Graphics::GenericMesh>(*roseGold.GraphicsManager);
		ourGenericSphere->SetFromPrimitive(RoseGold::Core::Graphics::MeshPrimitiveType::Icosphere);
	}

	{
		using namespace RoseGold::Core::Graphics;
		RoseGold::Core::Graphics::PipelineStateDescription pipelineState;
		pipelineState.InputLayout = ourColoredCube->GetInputLayout();

		pipelineState.VertexShader = roseGold.GraphicsManager->CreateShader("shaders/MeshVertex.hlsl", RoseGold::Core::Graphics::Shader::Type::Vertex, "ColoredMesh");
		pipelineState.PixelShader = roseGold.GraphicsManager->CreateShader("shaders/MeshPixel.hlsl", RoseGold::Core::Graphics::Shader::Type::Pixel);
		
		pipelineState.Outputs = { ourRT1 };
		ourRT1ColoredMeshPipelineState = roseGold.GraphicsManager->CreateOrGetPipelineState(pipelineState);

		pipelineState.Outputs = { ourRT2 };
		ourRT2ColoredMeshPipelineState = roseGold.GraphicsManager->CreateOrGetPipelineState(pipelineState);

		pipelineState.InputLayout = ourGenericPlane->GetInputLayout();
		pipelineState.VertexShader = roseGold.GraphicsManager->CreateShader("shaders/MeshVertex.hlsl", RoseGold::Core::Graphics::Shader::Type::Vertex, "GenericMesh");
		pipelineState.Outputs = { ourRT1 };
		ourRT1GenericMeshPipelineState = roseGold.GraphicsManager->CreateOrGetPipelineState(pipelineState);

		pipelineState.Outputs = { ourRT2 };
		ourRT2GenericMeshPipelineState = roseGold.GraphicsManager->CreateOrGetPipelineState(pipelineState);
	}
}

void CleanupResources()
{
	ourRT1GenericMeshPipelineState.reset();
	ourRT2GenericMeshPipelineState.reset();
	ourRT1ColoredMeshPipelineState.reset();
	ourRT2ColoredMeshPipelineState.reset();
	ourColoredCube.reset();
	ourGenericPlane.reset();
	ourGenericSphere.reset();
	ourRT1.reset();
	ourRT2.reset();
}

void DrawFrame(RoseGold::Core::Graphics::Manager& aManager)
{
	const std::chrono::high_resolution_clock::duration timeSinceStart = (std::chrono::high_resolution_clock::now() - ourStartTime);
	const std::chrono::milliseconds msSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceStart);
	const float secondsSinceStart = msSinceStart.count() / 1000.f;

	using namespace RoseGold::Core::Graphics;
	GraphicsTask drawFrame;

	GraphicsTask& clearTask = drawFrame.CreateTask("Clear frame");

	{
		GraphicsTask& drawMesh = drawFrame.CreateTask("Draw meshes");
		drawMesh.AddDependency(clearTask);
		{
			CommandBuffer& buffer = drawMesh.AddWork();

			if (ourRT1)
			{
				const float aspectRatio = static_cast<float>(ourRT1->GetWidth()) / static_cast<float>(ourRT1->GetHeight());
				buffer.SetProjectionMatrix(RoseGold::Math::MakeMatrix::PerspectiveFieldOfView(RoseGold::Math::ToRadians<float>(60.f), aspectRatio, 0.01f, 50.f));
				buffer.SetViewMatrix(RoseGold::Math::MakeMatrix::LookAt({ 2, 2, -3 }, { 0, 0, 0 }, RoseGold::Math::Vector3::Up()));

				buffer.SetViewport(RoseGold::Math::Rectangle::FromExtents({ 0, 0 }, { static_cast<float>(ourRT1->GetWidth()), static_cast<float>(ourRT1->GetHeight()) }));
				buffer.SetScissorRect(RoseGold::Math::RectangleT<int>::FromExtents({ 0, 0 }, { static_cast<int>(ourRT1->GetWidth()), static_cast<int>(ourRT1->GetHeight()) }));

				buffer.SetPipelineState(ourRT1GenericMeshPipelineState);
				buffer.DrawMesh(ourGenericPlane, RoseGold::Math::MakeMatrix::Translation(0, -1.5f, 0), 0);
				buffer.DrawMesh(ourGenericSphere, RoseGold::Math::MakeMatrix::Scale(0.5f) * RoseGold::Math::MakeMatrix::Translation(0, 1, 0), 0);
				buffer.SetPipelineState(ourRT1ColoredMeshPipelineState);
				buffer.DrawMesh(ourColoredCube, RoseGold::Math::MakeMatrix::RotationY(secondsSinceStart), 0);
			}

			if (ourRT2)
			{
				const float aspectRatio = static_cast<float>(ourRT2->GetWidth()) / static_cast<float>(ourRT2->GetHeight());
				buffer.SetProjectionMatrix(RoseGold::Math::MakeMatrix::Orthographic(aspectRatio * 4, 4, 0, 50.f));
				buffer.SetViewMatrix(RoseGold::Math::MakeMatrix::LookAt({ -5, 3, -5 }, { 0, 0, 0 }, RoseGold::Math::Vector3::Up()));
				
				buffer.SetViewport(RoseGold::Math::Rectangle::FromExtents({ 0, 0 }, { static_cast<float>(ourRT2->GetWidth()), static_cast<float>(ourRT2->GetHeight()) }));
				buffer.SetScissorRect(RoseGold::Math::RectangleT<int>::FromExtents({ 0, 0 }, { static_cast<int>(ourRT2->GetWidth()), static_cast<int>(ourRT2->GetHeight()) }));

				buffer.SetPipelineState(ourRT2ColoredMeshPipelineState);
				buffer.DrawMesh(ourColoredCube, RoseGold::Math::MakeMatrix::RotationX(secondsSinceStart), 0);
				buffer.SetPipelineState(ourRT2GenericMeshPipelineState);
				buffer.DrawMesh(ourGenericPlane, RoseGold::Math::MakeMatrix::Translation(0, 1.5f, 0), 0);
				buffer.DrawMesh(ourGenericSphere, RoseGold::Math::MakeMatrix::Translation(2.f, 0, 0), 0);
			}
		}
	}

	{
		GraphicsTask& clearWindow1 = clearTask.CreateTask("Clear window 1");
		{
			CommandBuffer& buffer = clearWindow1.AddWork();
			buffer.Clear(ourRT1, RoseGold::Color::Predefined::CornflowerBlue, 1.f);
		}

		GraphicsTask& clearWindow2 = clearTask.CreateTask("Clear window 2");
		{
			CommandBuffer& buffer = clearWindow2.AddWork();
			buffer.Clear(ourRT2, RoseGold::Color::Predefined::Black, 1.f);
		}
	}

	aManager.ExecuteTask(drawFrame);
}

#if defined(_CONSOLE)
int main(int anArgumentCount, const char** someCommandArguments)
#else
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR someCommandArguments, _In_ int)
#endif
{
	RoseCommon::CommandLineParser<> commandLine(someCommandArguments
#if defined(_CONSOLE)
		, anArgumentCount
#endif
	);

	RoseGold::Client::BootstrapResult roseGold = RoseGold::Client::Bootstrap();

	SetupResources(roseGold);

	ourStartTime = std::chrono::high_resolution_clock::now();

	while (!roseGold.WindowManager->GetWindows().empty())
	{
		std::chrono::high_resolution_clock::time_point frameStart = std::chrono::high_resolution_clock::now();

		roseGold.WindowManager->Update();
		roseGold.GraphicsManager->MarkFrameStart();
		DrawFrame(*roseGold.GraphicsManager);
		roseGold.GraphicsManager->MarkFrameEnd();

		std::this_thread::sleep_until(frameStart + std::chrono::milliseconds(8));
	}

	CleanupResources();

	return 0;
}
