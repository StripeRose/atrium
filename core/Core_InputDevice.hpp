// Filter "Input"

#pragma once

#include <rose-common/Enum.hpp>
#include <rose-common/EventSlot.hpp>

#include <span>

namespace Atrium::Core
{
	enum class InputDeviceType : std::uint8_t
	{
		Unknown  = 0x00,
		Keyboard = 0x01,
		Mouse    = 0x02,
		Gamepad  = 0x04,
		Touch    = 0x08,
		Joystick = 0x10,
		Other    = 0x80
	};

	struct InputEvent;
	struct TextInputEvent;
	class InputDevice
	{
	public:
		virtual ~InputDevice() = default;

		virtual bool IsConnected() const = 0;

		RoseCommon::EventSlot<const Core::InputEvent&> OnInput;
		RoseCommon::EventSlot<const Core::TextInputEvent&> OnTextInput;
	};
}

ENUM_FLAGS(Atrium::Core::InputDeviceType);
