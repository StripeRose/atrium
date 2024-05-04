#include "stdafx.hpp"

#include "ExampleGame.hpp"

#include <Engine_Bootstrapper.hpp>

#include <Windows.h>

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
			roseGold.WindowManager->Update();
			roseGold.GraphicsAPI->MarkFrameStart();
			game.OnLoop();
			roseGold.GraphicsAPI->MarkFrameEnd();
		}

		game.OnExit();
	}

	return 0;
}
