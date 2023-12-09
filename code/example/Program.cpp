
#include <rose-common/CommandLineParser.hpp>

#include <Win32_WindowManagement.hpp>

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
	
	RoseGold::Platform::Win32::WindowManager windowManager;

	RoseGold::Core::Platform::WindowManager::CreationParameters windowParams;

	windowParams.Title = "Test window";
	windowManager.NewWindow(windowParams);

	windowParams.Title = "Test window 2";
	windowParams.Size = { 800, 600 };
	windowManager.NewWindow(windowParams);

	windowParams.Title = "Test window 3";
	windowParams.Size = { 200, 200 };
	windowManager.NewWindow(windowParams);

	while (!windowManager.GetWindows().empty())
	{
		windowManager.Update();
	}

	return 0;
}