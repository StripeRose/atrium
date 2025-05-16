
#include <Atrium_Instance.hpp>

#include <Windows.h>

std::unique_ptr<Atrium::EngineInstance> ourEngineInstance;

std::shared_ptr<Atrium::RenderTexture> ourWindowTarget;

void HandleStart()
{
	Atrium::WindowManager::CreationParameters windowParameters;
	windowParameters.Title = "Test window";
	windowParameters.Size = { 640, 480 };

	std::shared_ptr<Atrium::Window> window = ourEngineInstance->GetWindowManager().NewWindow(windowParameters);
	ourWindowTarget = ourEngineInstance->GetGraphicsAPI().GetResourceManager().CreateRenderTextureForWindow(*window);
	window->Closed.Connect(nullptr, [](Atrium::Window&) { ourWindowTarget.reset(); });
}

void HandleLoop()
{
	if (ourEngineInstance->GetWindowManager().GetWindows().empty())
	{
		ourEngineInstance->Stop();
		return;
	}

	Atrium::FrameGraphicsContext& frameContext = ourEngineInstance->GetGraphicsAPI().GetCurrentFrameContext();
	if (ourWindowTarget)
	{
		frameContext.ClearColor(ourWindowTarget, Atrium::Color::Predefined::CornflowerBlue);
	}
}

void HandleExit()
{
	ourWindowTarget.reset();
}

#if defined(_CONSOLE)
int main(int, const char**)
#else
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
#endif
{
	if (ourEngineInstance = Atrium::EngineInstance::Create())
	{
		ourEngineInstance->OnStart.Connect(nullptr, HandleStart);
		ourEngineInstance->OnLoop.Connect(nullptr, HandleLoop);
		ourEngineInstance->OnExit.Connect(nullptr, HandleExit);

		ourEngineInstance->Run();
	}

	ourEngineInstance.reset();

	return 0;
}