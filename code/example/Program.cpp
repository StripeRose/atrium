
#include <rose-common/CommandLineParser.hpp>
#include <rose-common/math/Common.hpp>
#include <rose-common/math/Trigonometry.hpp>

#include <Bootstrapper.hpp>

#include <Graphics_Mesh.hpp>
#include <Graphics_Pipeline.hpp>
#include <Graphics_Tasks.hpp>

#include <Windows.h>

#include <chrono>

std::chrono::high_resolution_clock::time_point ourStartTime;
std::shared_ptr<RoseGold::Core::Graphics::RenderTexture> ourRT1, ourRT2, ourRT3;

std::shared_ptr<RoseGold::Core::Graphics::CachedPipelineState> ourMeshPipelineState;
std::shared_ptr<RoseGold::Core::Graphics::Shader> ourVertexShader, ourPixelShader;

std::shared_ptr<RoseGold::Core::Graphics::Mesh> ourMesh;

void SetupResources(RoseGold::Client::BootstrapResult& roseGold)
{
	RoseGold::Core::Platform::WindowManager::CreationParameters windowParams;

	windowParams.Title = "Window 1";
	windowParams.Position = { 200, 200 };
	windowParams.Size = { 200, 200 };
	auto window1 = roseGold.WindowManager->NewWindow(windowParams);
	ourRT1 = roseGold.GraphicsManager->CreateRenderTextureForWindow(*window1);
	window1->Closed.Connect(nullptr, [&](RoseGold::Core::Platform::Window&) {
		ourRT1.reset();
		});

	windowParams.Title = "Window 2";
	windowParams.Position = { 410, 200 };
	windowParams.Size = { 400, 200 };
	auto window2 = roseGold.WindowManager->NewWindow(windowParams);
	ourRT2 = roseGold.GraphicsManager->CreateRenderTextureForWindow(*window2);
	window2->Closed.Connect(nullptr, [&](RoseGold::Core::Platform::Window&) {
		ourRT2.reset();
		});

	windowParams.Title = "Window 3";
	windowParams.Position = { 200, 440 };
	windowParams.Size = { 200, 400 };
	auto window3 = roseGold.WindowManager->NewWindow(windowParams);
	ourRT3 = roseGold.GraphicsManager->CreateRenderTextureForWindow(*window3);
	window3->Closed.Connect(nullptr, [&](RoseGold::Core::Platform::Window&) {
		ourRT3.reset();
		});

	auto onWindowRectChange = [window2, window3](RoseGold::Core::Platform::Window& aWindow) {
		const RoseGold::Point point = aWindow.GetPosition();
		const RoseGold::Size size = aWindow.GetSize();
		const RoseGold::Point bottomRight = point + size;

		window2->SetPosition({ bottomRight.X + 10, point.Y });
		window3->SetPosition({ point.X, bottomRight.Y + 40 });
	};

	window1->Moved		.Connect(nullptr, onWindowRectChange);
	window1->SizeChanged.Connect(nullptr, onWindowRectChange);

	{
		std::vector<RoseGold::Core::Graphics::Vertex> vertices;

		auto& v1 = vertices.emplace_back();
		v1.Position = { 0.f, 0.25f, 0.f };
		v1.Normal = { 1, 0, 0 };
		v1.Color = 0xFFFF0000;

		auto& v2 = vertices.emplace_back();
		v2.Position = { 0.25f, -0.25f, 0.f };
		v2.Normal = { 0, 1, 0 };
		v2.Color = 0xFF00FF00;

		auto& v3 = vertices.emplace_back();
		v3.Position = { -0.25f, -0.25f, 0.f };
		v3.Normal = { 0, 0, 1 };
		v3.Color = 0xFF0000FF;

		ourMesh = roseGold.GraphicsManager->CreateMesh();
		ourMesh->SetFromList(vertices);
	}

	{
		using namespace RoseGold::Core::Graphics;
		ourVertexShader = roseGold.GraphicsManager->CreateShader("shaders/MeshVertex.hlsl", RoseGold::Core::Graphics::Shader::Type::Vertex);
		ourPixelShader = roseGold.GraphicsManager->CreateShader("shaders/MeshPixel.hlsl", RoseGold::Core::Graphics::Shader::Type::Pixel);

		RoseGold::Core::Graphics::PipelineState pipelineState;
		pipelineState.InputLayout = ourMesh->GetInputLayout();

		pipelineState.VertexShader = ourVertexShader;
		pipelineState.PixelShader = ourPixelShader;

		ourMeshPipelineState = roseGold.GraphicsManager->CreateOrGetPipelineState(pipelineState);
	}
}

void CleanupResources()
{
	ourMeshPipelineState.reset();
	ourVertexShader.reset();
	ourPixelShader.reset();
	ourMesh.reset();
	ourRT1.reset();
	ourRT2.reset();
	ourRT3.reset();
}

void DrawFrame(RoseGold::Core::Graphics::Manager& aManager)
{
	const std::chrono::high_resolution_clock::duration timeSinceStart = (std::chrono::high_resolution_clock::now() - ourStartTime);
	const std::chrono::milliseconds msSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceStart);
	const float secondsSinceStart = msSinceStart.count() / 1000.f;
	const float lerp = (RoseCommon::Math::Sine<float>(secondsSinceStart) + 1.f) / 2.f;

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
				buffer.SetViewport(RoseGold::Math::Rectangle::FromExtents({ 0, 0 }, { static_cast<float>(ourRT1->GetWidth()), static_cast<float>(ourRT1->GetHeight()) }));
				buffer.SetScissorRect(RoseGold::Math::RectangleT<int>::FromExtents({ 0, 0 }, { static_cast<int>(ourRT1->GetWidth()), static_cast<int>(ourRT1->GetHeight()) }));

				buffer.SetRenderTarget(ourRT1);
				buffer.DrawMesh(ourMesh, RoseGold::Math::MakeMatrix::RotationY(secondsSinceStart), ourMeshPipelineState, 0);
			}

			if (ourRT2)
			{
				buffer.SetViewport(RoseGold::Math::Rectangle::FromExtents({ 0, 0 }, { static_cast<float>(ourRT2->GetWidth() / 2), static_cast<float>(ourRT2->GetHeight() / 2) }));
				buffer.SetScissorRect(RoseGold::Math::RectangleT<int>::FromExtents({ 0, 0 }, { static_cast<int>(ourRT2->GetWidth() / 2), static_cast<int>(ourRT2->GetHeight() / 2) }));

				buffer.SetRenderTarget(ourRT2);
				buffer.DrawMesh(ourMesh, RoseGold::Math::MakeMatrix::RotationY(secondsSinceStart), ourMeshPipelineState, 0);
			}
		}
	}

	{
		GraphicsTask& clearWindow1 = clearTask.CreateTask("Clear window 1");
		{
			CommandBuffer& buffer = clearWindow1.AddWork();
			buffer.SetRenderTarget(ourRT1);
			buffer.Clear(RoseCommon::Math::Lerp<RoseGold::Color>(
				RoseGold::Color::Predefined::Aqua,
				RoseGold::Color::Predefined::Tan,
				lerp
			));
		}

		GraphicsTask& clearWindow2 = clearTask.CreateTask("Clear window 2");
		{
			CommandBuffer& buffer = clearWindow2.AddWork();
			buffer.SetRenderTarget(ourRT2);
			buffer.Clear(RoseCommon::Math::Lerp<RoseGold::Color>(
				RoseGold::Color::Predefined::Tomato,
				RoseGold::Color::Predefined::LightGreen,
				lerp
			));
		}

		GraphicsTask& clearWindow3 = clearTask.CreateTask("Clear window 3");
		{
			CommandBuffer& buffer = clearWindow3.AddWork();
			buffer.SetRenderTarget(ourRT3);
			buffer.Clear(RoseCommon::Math::Lerp<RoseGold::Color>(
				RoseGold::Color::Predefined::LightYellow,
				RoseGold::Color::Predefined::CornflowerBlue,
				lerp
			));
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

		std::this_thread::sleep_until(frameStart + std::chrono::milliseconds(16));
	}

	CleanupResources();

	return 0;
}
