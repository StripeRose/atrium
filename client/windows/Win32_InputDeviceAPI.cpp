// Filter "Input"

#include "Win32_InputDeviceAPI.hpp"

#include "Win32_GameInputHandler.hpp"

namespace Atrium::Win32
{
	InputDeviceAPI::InputDeviceAPI()
	{
		myGameInputHandler.reset(new GameInputHandler(*this));
	}

	std::span<std::reference_wrapper<Atrium::InputDevice>> InputDeviceAPI::ListDevices() const
	{
		return std::span<std::reference_wrapper<Atrium::InputDevice>>();
	}

	void InputDeviceAPI::ReportInputEvents(Atrium::InputDeviceType someDeviceTypes)
	{
		myGameInputHandler->ReportInputEvents(someDeviceTypes);
	}
}