// Filter "Input"

#pragma once

#include "Atrium_InputDeviceAPI.hpp"

#include <memory>

namespace Atrium::Win32
{
	class GameInputHandler;
	class InputDeviceAPI : public Core::InputDeviceAPI
	{
	public:
		InputDeviceAPI();
		~InputDeviceAPI() = default;

		std::span<std::reference_wrapper<Core::InputDevice>> ListDevices() const override;

		void ReportInputEvents(Core::InputDeviceType someDeviceTypes) override;

	private:
		std::unique_ptr<GameInputHandler> myGameInputHandler;
	};
}