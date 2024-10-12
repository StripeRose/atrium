// Filter "Input"

#pragma once

#include "Core_InputSource.hpp"

#include <bitset>
#include <span>
#include <string>

namespace Atrium::Core
{
	enum class InputEventType
	{
		Any,
		Pressed,
		Released,
		Analog
	};

	class InputDevice;
	struct InputEvent
	{
		InputEvent(InputDevice& aDevice, const InputSourceId& aSource, InputEventType anEventType) noexcept
			: Device(aDevice)
			, Source(aSource)
			, Type(anEventType)
			, Delta(0.f)
			, Value(anEventType == InputEventType::Pressed ? 1.f : 0.f)
		{

		}

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

		InputDevice& Device;
		InputSourceId Source;
		InputEventType Type;
		float Value;
		float Delta;
	};

	struct TextInputEvent
	{
		TextInputEvent(InputDevice& aDevice, const std::uint64_t& aCodepoint) noexcept
			: Device(aDevice)
			, Codepoint(aCodepoint)
		{

		}

		InputDevice& Device;
		std::uint64_t Codepoint;
	};
}
