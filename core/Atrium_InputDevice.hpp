// Filter "Input"

#pragma once

#include <rose-common/EventSlot.hpp>

namespace Atrium
{
	struct InputEvent;
	struct TextInputEvent;

	enum class InputDeviceType : std::uint8_t
	{
		Keyboard,
		Mouse,
		Gamepad,
		Touch,
		Joystick,
		Other
	};

	/**
	 * @brief A basic device capable of listening to inputs.
	 */
	class InputDevice
	{
	public:
		virtual ~InputDevice() = default;

		//--------------------------------------------------
		// * Properties
		//--------------------------------------------------
	#pragma region Properties

	/**
	 * @brief Slot for listening to input events from the device.
	 */
		EventSlot<const InputEvent&> OnInput;

		/**
		 * @brief Slot for listening to text input events from the device.
		 */
		EventSlot<const TextInputEvent&> OnTextInput;

	#pragma endregion

		//--------------------------------------------------
		// * Methods
		//--------------------------------------------------
	#pragma region Methods

	/**
	 * @brief Check whether the device is connected and usable.
	 * @return True if the device is connected and usable.
	 */
		virtual bool IsConnected() const = 0;

	#pragma endregion
	};
}
