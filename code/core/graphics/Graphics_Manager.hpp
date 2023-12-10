#pragma once

namespace RoseGold::Core::Platform { class Window; }

namespace RoseGold::Core::Graphics
{
	class Manager
	{
	public:
		virtual ~Manager() = default;
		
		virtual void CreateSwapChain(Platform::Window& aWindow) = 0;

		virtual bool SupportsMultipleWindows() const = 0;
	};
}