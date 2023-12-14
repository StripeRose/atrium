
#include <rose-common/CommandLineParser.hpp>
#include <rose-common/math/Common.hpp>
#include <rose-common/math/Trigonometry.hpp>

#include <Bootstrapper.hpp>

#include <Graphics_Tasks.hpp>

#include <Windows.h>

#include <chrono>

std::chrono::high_resolution_clock::time_point ourStartTime;
std::shared_ptr<RoseGold::Core::Graphics::RenderTexture> ourRT1, ourRT2, ourRT3;

void DrawFrame(RoseGold::Core::Graphics::Manager& aManager)
{
	const std::chrono::high_resolution_clock::duration timeSinceStart = (std::chrono::high_resolution_clock::now() - ourStartTime);
	const std::chrono::milliseconds msSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceStart);
	const float secondsSinceStart = msSinceStart.count() / 1000.f;
	const float lerp = (RoseCommon::Math::Sine<float>(secondsSinceStart) + 1.f) / 2.f;

	using namespace RoseGold::Core::Graphics;
	GraphicsTask clearTask;

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

	aManager.ExecuteTask(clearTask);
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

	RoseGold::Core::Platform::WindowManager::CreationParameters windowParams;

	windowParams.Title = "Test window";
	windowParams.Size = { 200, 200 };
	auto window1 = roseGold.WindowManager->NewWindow(windowParams);
	ourRT1 = roseGold.GraphicsManager->CreateRenderTextureForWindow(*window1);
	window1->OnClosed.Connect(nullptr, [&]() {
		ourRT1.reset();
		window1.reset();
		});

	windowParams.Title = "Test window 2";
	windowParams.Size = { 200, 200 };
	auto window2 = roseGold.WindowManager->NewWindow(windowParams);
	ourRT2 = roseGold.GraphicsManager->CreateRenderTextureForWindow(*window2);
	window2->OnClosed.Connect(nullptr, [&]() {
		ourRT2.reset();
		window2.reset();
		});

	windowParams.Title = "Test window 3";
	windowParams.Size = { 200, 200 };
	auto window3 = roseGold.WindowManager->NewWindow(windowParams);
	ourRT3 = roseGold.GraphicsManager->CreateRenderTextureForWindow(*window3);
	window3->OnClosed.Connect(nullptr, [&]() {
		ourRT3.reset();
		window3.reset();
		});

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

	return 0;
}
