// Filter "Input"

#include "Win32_InputDeviceAPI.hpp"

#include "Win32_GameInputHandler.hpp"

namespace Atrium::Win32
{
	InputDeviceAPI::InputDeviceAPI()
	{
		myGameInputHandler.reset(new GameInputHandler(*this));
	}

	std::span<std::reference_wrapper<Atrium::Core::InputDevice>> InputDeviceAPI::ListDevices() const
	{
		return std::span<std::reference_wrapper<Atrium::Core::InputDevice>>();
	}

	void InputDeviceAPI::ReportInputEvents(Atrium::Core::InputDeviceType someDeviceTypes)
	{
		myGameInputHandler->ReportInputEvents(someDeviceTypes);
	}
}