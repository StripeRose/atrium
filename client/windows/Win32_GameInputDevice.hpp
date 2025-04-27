// Filter "Input/GameInput"

#pragma once

#include "Win32_GameInput.h"

#include "Atrium_InputDevice.hpp"
#include "Atrium_InputEvent.hpp"
#include "Atrium_InputSource.hpp"

#include <wrl.h>

#include <optional>
#include <span>

namespace Atrium::Win32
{
	class GameInputDevice final : public Core::InputDevice
	{
	public:
		GameInputDevice(IGameInput* anInputAPI, IGameInputDevice* aDevice);

		bool IsConnected() const override;

		void HandleDeviceEvent(GameInputDeviceStatus aCurrentStatus, GameInputDeviceStatus aPreviousStatus);

		void ReportInputEvents(Core::InputDeviceType someDeviceTypes);

	private:
		struct ControllerReadingReference
		{
			std::span<float> Axes;
			std::span<bool> Buttons;
			std::span<GameInputSwitchPosition> Switches;
		};

		void HandleReadingEvent(Core::InputDeviceType someDeviceTypes, IGameInputReading& aPreviousReading, IGameInputReading& aReading);

		void HandleControllerReading(IGameInputReading& aPreviousReading, IGameInputReading& aReading);
		void HandleKeyboardReading(IGameInputReading& aPreviousReading, IGameInputReading& aReading);
		void HandleMouseReading(IGameInputReading& aPreviousReading, IGameInputReading& aReading);
		void HandleGamepadReading(IGameInputReading& aPreviousReading, IGameInputReading& aReading);
		void HandleTouchReading(IGameInputReading& aPreviousReading, IGameInputReading& aReading);

		void HandleGamepadReading(GameInputGamepadState& aPreviousReading, GameInputGamepadState& aReading);

		void HandleDigitalChange(const auto& aPreviousState, const auto& aState, auto aMask, Core::InputSourceId aSource)
		{
			const bool prevState = (aPreviousState & aMask) == aMask;
			const bool state = (aState & aMask) == aMask;

			if (!prevState && state)
				OnInput.Invoke(Core::InputEvent(*this, aSource, Core::InputEventType::Pressed));
			else if (prevState && !state)
				OnInput.Invoke(Core::InputEvent(*this, aSource, Core::InputEventType::Released));
		}

		void HandleDigitalChange(const auto& aPreviousState, const auto& aState, const auto& aMember, auto aMask, Core::InputSourceId aSource)
		{
			HandleDigitalChange(aPreviousState.*aMember, aState.*aMember, aMask, aSource);
		}

		void HandleAnalogChange(const auto& aPreviousState, const auto& aState, Core::InputSourceId aSource)
		{
			const auto currentValue = aState;
			const auto delta = (currentValue - aPreviousState);
			if (delta != 0)
				OnInput.Invoke(Core::InputEvent(*this, aSource, static_cast<float>(currentValue), static_cast<float>(delta)));
		}

		void HandleAnalogChange(const auto& aPreviousState, const auto& aState, const auto& aMember, Core::InputSourceId aSource)
		{
			HandleAnalogChange(aPreviousState.*aMember, aState.*aMember, aSource);
		}

		std::optional<Microsoft::WRL::ComPtr<IGameInputReading>> GetCurrentReading();

		std::optional<Core::InputSourceId> ToInputSource(const GameInputKeyState& aKeyState);

		Microsoft::WRL::ComPtr<IGameInputDevice> myDevice;
		Microsoft::WRL::ComPtr<IGameInput> myGameInputAPI;

		Microsoft::WRL::ComPtr<IGameInputReading> myLastReading;
	};
}
