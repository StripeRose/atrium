
#include <rose-common/CommandLineParser.hpp>

#include <Bootstrapper.hpp>

#include <Windows.h>

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
	roseGold.WindowManager->NewWindow(windowParams);

	windowParams.Title = "Test window 2";
	windowParams.Size = { 800, 600 };
	roseGold.WindowManager->NewWindow(windowParams);

	windowParams.Title = "Test window 3";
	windowParams.Size = { 200, 200 };
	roseGold.WindowManager->NewWindow(windowParams);

	while (!roseGold.WindowManager->GetWindows().empty())
	{
		roseGold.WindowManager->Update();
	}

	return 0;
}