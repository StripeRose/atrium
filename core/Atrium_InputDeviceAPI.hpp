// Filter "Input"

#pragma once

#include "Atrium_InputDevice.hpp"

#include <rose-common/EventSlot.hpp>

#include <span>

namespace Atrium
{
	struct InputEvent;
	struct TextInputEvent;

	class InputDevice;

	/**
	 * @brief Interface for accessing and using devices such as keyboards, gamepads, joysticks and touch input.
	 */
	class InputDeviceAPI
	{
	public:
		virtual ~InputDeviceAPI() = default;

		//--------------------------------------------------
		// * Properties
		//--------------------------------------------------
		#pragma region Properties

		/**
		 * @brief Slot for listening to input events from any device.
		 */
		EventSlot<const InputEvent&> OnInput;

		/**
		 * @brief Slot for listening to text input events from any device.
		 */
		EventSlot<const TextInputEvent&> OnTextInput;

		#pragma endregion

		//--------------------------------------------------
		// * Methods
		//--------------------------------------------------
		#pragma region Methods

		/**
		 * @brief List all devices that are or have been connected to the application.
		 * @return A span of references to the devices.
		 */
		virtual std::span<std::reference_wrapper<InputDevice>> ListDevices() const = 0;

		/**
		 * @brief Trigger events for all input-events since the last report.
		 * @param someDeviceTypes Flags of which input-device types to report.
		 */
		virtual void ReportInputEvents(InputDeviceType someDeviceTypes) = 0;

		#pragma endregion
	};
}
