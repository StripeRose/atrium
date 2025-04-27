// Filter "Input"

#pragma once

#include "Atrium_Diagnostics.hpp"
#include "Atrium_InputSource.hpp"

#include <bitset>
#include <span>
#include <string>

namespace Atrium::Core
{
	class InputDevice;

	enum class InputEventType
	{
		Pressed,
		Released,
		Analog
	};

	/**
	 * @brief Information about an input device event, such as a button press or analog input change.
	 */
	struct InputEvent
	{
		//--------------------------------------------------
		// * Construction
		//--------------------------------------------------
		#pragma region Construction

		/**
		 * @brief Construct an event representing a button press or release.
		 * @param aDevice Device the event originates from.
		 * @param aSource Input source, such as a keyboard key or controller button.
		 * @param anEventType Type of event. Either "Pressed" or "Released".
		 */
		InputEvent(InputDevice& aDevice, const InputSourceId& aSource, InputEventType anEventType) noexcept
			: Device(aDevice)
			, Source(aSource)
			, Type(anEventType)
			, Delta(0.f)
			, Value(anEventType == InputEventType::Pressed ? 1.f : 0.f)
		{
			Debug::Assert(
				anEventType == InputEventType::Pressed || anEventType == InputEventType::Released,
				"Requires event type to be either Pressed or Released."
			);
		}

		/**
		 * @brief Construct an event representing an analog input event.
		 * @param aDevice Device the event originates from.
		 * @param aSource Input source, such as a joystick or touch coordinate.
		 * @param aValue Value of the event.
		 * @param aDelta Delta-value of the event.
		 */
		InputEvent(InputDevice& aDevice, const InputSourceId& aSource, float aValue, float aDelta) noexcept
			: Device(aDevice)
			, Source(aSource)
			, Value(aValue)
			, Delta(aDelta)
			, Type(InputEventType::Analog)
		{

		}

		InputEvent(const InputEvent& anEvent)
			: Device(anEvent.Device)
			, Source(anEvent.Source)
			, Type(anEvent.Type)
			, Value(anEvent.Value)
			, Delta(anEvent.Delta)
		{

		}

		#pragma endregion

		//--------------------------------------------------
		// * Properties
		//--------------------------------------------------
		#pragma region Properties

		InputDevice& Device;

		InputSourceId Source;

		InputEventType Type;

		float Value;

		float Delta;

		#pragma endregion
	};

	/**
	 * @brief Information about a text input event, such as typing on a keyboard.
	 */
	struct TextInputEvent
	{
		//--------------------------------------------------
		// * Construction
		//--------------------------------------------------
		#pragma region Construction

		/**
		 * @brief Construct an event representing a text-input event with a Unicode codepoint.
		 * @param aDevice Device the event originated from.
		 * @param aCodepoint Unicode codepoint that was entered.
		 */
		TextInputEvent(InputDevice& aDevice, const std::uint64_t& aCodepoint) noexcept
			: Device(aDevice)
			, Codepoint(aCodepoint)
		{

		}

		#pragma endregion

		//--------------------------------------------------
		// * Properties
		//--------------------------------------------------
		#pragma region Properties

		InputDevice& Device;

		std::uint64_t Codepoint;

		#pragma endregion
	};
}
