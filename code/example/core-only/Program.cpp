#include "stdafx.hpp"

#include "ExampleGame.hpp"

#include <Bootstrapper.hpp>

#include <Common_Profiling.hpp>

#include <Windows.h>

#include <chrono>
#include <thread>

#if defined(_CONSOLE)
int main(int, const char**)
#else
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
#endif
{
	RoseGold::Client::BootstrapResult roseGold = RoseGold::Client::Bootstrap();

	{
		ExampleGame game;
		game.OnStart(roseGold);

		while (!roseGold.WindowManager->GetWindows().empty())
		{
			FrameMark;

			roseGold.WindowManager->Update();
			roseGold.GraphicsManager->MarkFrameStart();
			game.OnLoop();
			roseGold.GraphicsManager->MarkFrameEnd();
		}

		game.OnExit();
	}

	return 0;
}
