
#include <rose-common/CommandLineParser.hpp>

#include <Bootstrapper.hpp>

#include <Graphics_Tasks.hpp>

#include <Windows.h>

#include <chrono>

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
	auto renderTarget1 = roseGold.GraphicsManager->CreateRenderTextureForWindow(*window1);
	window1->OnClosed.Connect(nullptr, [&]() {
		renderTarget1.reset();
		window1.reset();
		});

	windowParams.Title = "Test window 2";
	windowParams.Size = { 200, 200 };
	auto window2 = roseGold.WindowManager->NewWindow(windowParams);
	auto renderTarget2 = roseGold.GraphicsManager->CreateRenderTextureForWindow(*window2);
	window2->OnClosed.Connect(nullptr, [&]() {
		renderTarget2.reset();
		window2.reset();
		});

	windowParams.Title = "Test window 3";
	windowParams.Size = { 200, 200 };
	auto window3 = roseGold.WindowManager->NewWindow(windowParams);
	auto renderTarget3 = roseGold.GraphicsManager->CreateRenderTextureForWindow(*window3);
	window3->OnClosed.Connect(nullptr, [&]() {
		renderTarget3.reset();
		window3.reset();
		});

	{
		using namespace RoseGold::Core::Graphics;
		GraphicsTask clearTask;

		GraphicsTask& clearWindow1 = clearTask.CreateTask("Clear window 1");
		{
			CommandBuffer& buffer = clearWindow1.AddWork();
			buffer.SetRenderTarget(renderTarget1);
			buffer.Clear(RoseGold::Color::Predefined::Tan);
		}

		GraphicsTask& clearWindow2 = clearTask.CreateTask("Clear window 2");
		{
			CommandBuffer& buffer = clearWindow2.AddWork();
			buffer.SetRenderTarget(renderTarget2);
			buffer.Clear(RoseGold::Color::Predefined::LightGreen);
		}

		GraphicsTask& clearWindow3 = clearTask.CreateTask("Clear window 3");
		{
			CommandBuffer& buffer = clearWindow3.AddWork();
			buffer.SetRenderTarget(renderTarget3);
			buffer.Clear(RoseGold::Color::Predefined::CornflowerBlue);
		}

		roseGold.GraphicsManager->MarkFrameStart();
		roseGold.GraphicsManager->ExecuteTask(clearTask);
		roseGold.GraphicsManager->MarkFrameEnd();
	}

	while (!roseGold.WindowManager->GetWindows().empty())
	{
		std::chrono::high_resolution_clock::time_point frameStart = std::chrono::high_resolution_clock::now();

		roseGold.WindowManager->Update();
		//roseGold.GraphicsManager->MarkFrameStart();
		//roseGold.GraphicsManager->MarkFrameEnd();

		std::this_thread::sleep_until(frameStart + std::chrono::milliseconds(16));
	}

	return 0;
}
