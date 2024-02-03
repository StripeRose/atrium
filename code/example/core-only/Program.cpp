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

		static constexpr const char* ourFrameMark = "Frame";

		while (!roseGold.WindowManager->GetWindows().empty())
		{
			std::chrono::high_resolution_clock::time_point frameStart = std::chrono::high_resolution_clock::now();

			FrameMarkStart(ourFrameMark);

			roseGold.WindowManager->Update();
			roseGold.GraphicsManager->MarkFrameStart();
			game.OnLoop();
			roseGold.GraphicsManager->MarkFrameEnd();

			FrameMarkEnd(ourFrameMark);

			std::this_thread::sleep_until(frameStart + std::chrono::milliseconds(16));
		}

		game.OnExit();
	}

	return 0;
}
