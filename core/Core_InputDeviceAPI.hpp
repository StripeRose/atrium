// Filter "Input"

#pragma once

#include "Core_InputDevice.hpp"

#include <rose-common/EventSlot.hpp>

#include <span>

namespace Atrium::Core
{
	struct InputEvent;
	struct TextInputEvent;

	class InputDevice;

	class InputDeviceAPI
	{
	public:
		virtual ~InputDeviceAPI() = default;

		virtual std::span<std::reference_wrapper<InputDevice>> ListDevices() const = 0;

		virtual void ReportInputEvents(InputDeviceType someDeviceTypes) = 0;

		RoseCommon::EventSlot<const Core::InputEvent&> OnInput;
		RoseCommon::EventSlot<const Core::TextInputEvent&> OnTextInput;
	};
}
