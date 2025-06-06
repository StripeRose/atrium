// Filter "Input"

#pragma once

#include "Core_InputDeviceAPI.hpp"

namespace Atrium::Core
{
	/**
	 * @brief Empty input handler implementation which acts like functional API but does nothing.
	 */
	class NullInputHandler final : public InputDeviceAPI
	{
	public:
		std::span<std::reference_wrapper<InputDevice>> ListDevices() const override { return {}; }
		void ReportInputEvents(InputDeviceType) override { }
	};
}