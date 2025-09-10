// Filter "Input/GameInput"

#include "Win32_GameInputDevice.hpp"

#include "Atrium_Diagnostics.hpp"
#include "Atrium_InputEvent.hpp"

#include <sstream>

namespace Atrium::Win32
{
	GameInputDevice::GameInputDevice(IGameInput* anInputAPI, IGameInputDevice* aDevice)
		: myGameInputAPI(anInputAPI)
		, myDevice(aDevice)
	{
		if (auto reading = GetCurrentReading())
			myLastReading = reading.value();
		else
			Debug::LogError("Failed to get initial reference reading for input device.");
	}

	bool GameInputDevice::IsConnected() const
	{
		return false;
	}

	void GameInputDevice::HandleDeviceEvent(GameInputDeviceStatus aCurrentStatus, GameInputDeviceStatus aPreviousStatus)
	{
		if ((aPreviousStatus & GameInputDeviceConnected) == 0 && (aCurrentStatus & GameInputDeviceConnected) != 0)
			myLastReading = GetCurrentReading().value_or(myLastReading);
	}

	void GameInputDevice::ReportInputEvents(Atrium::InputDeviceType someDeviceTypes)
	{
		if (auto reading = GetCurrentReading())
		{
			HandleReadingEvent(someDeviceTypes, *myLastReading.Get(), *reading.value().Get());
			myLastReading = reading.value();
		}

		/*if (!myLastReading)
		{
			Microsoft::WRL::ComPtr<IGameInputReading> reading;
			if (SUCCEEDED(myGameInputAPI->GetCurrentReading(~GameInputKindUnknown, myDevice.Get(), reading.GetAddressOf())))
				myLastReading = reading;
		}
		else
		{
			HRESULT hr = S_OK;
			while (SUCCEEDED(hr))
			{
				Microsoft::WRL::ComPtr<IGameInputReading> nextReading;
				hr = myGameInputAPI->GetNextReading(
					myLastReading.Get(),
					~GameInputKindUnknown,
					myDevice.Get(),
					nextReading.ReleaseAndGetAddressOf()
				);

				if (SUCCEEDED(hr))
				{
					HandleReadingEvent(someDeviceTypes, *myLastReading.Get(), *nextReading.Get());

					myLastReading = nextReading;
				}
			}
		}*/
	}


	void GameInputDevice::HandleReadingEvent(Atrium::InputDeviceType someDeviceTypes, IGameInputReading& aPreviousReading, IGameInputReading& aReading)
	{
		if ((someDeviceTypes & Atrium::InputDeviceType::Joystick) == Atrium::InputDeviceType::Joystick)
			HandleControllerReading(aPreviousReading, aReading);

		if ((someDeviceTypes & Atrium::InputDeviceType::Keyboard) == Atrium::InputDeviceType::Keyboard)
			HandleKeyboardReading(aPreviousReading, aReading);

		if ((someDeviceTypes & Atrium::InputDeviceType::Mouse) == Atrium::InputDeviceType::Mouse)
			HandleMouseReading(aPreviousReading, aReading);

		if ((someDeviceTypes & Atrium::InputDeviceType::Gamepad) == Atrium::InputDeviceType::Gamepad)
			HandleGamepadReading(aPreviousReading, aReading);

		if ((someDeviceTypes & Atrium::InputDeviceType::Touch) == Atrium::InputDeviceType::Touch)
			HandleTouchReading(aPreviousReading, aReading);
	}

	void GameInputDevice::HandleControllerReading(IGameInputReading& aPreviousReading, IGameInputReading& aReading)
	{
		const std::uint32_t axisCount = aReading.GetControllerAxisCount();
		const std::uint32_t buttonCount = aReading.GetControllerButtonCount();
		const std::uint32_t switchCount = aReading.GetControllerSwitchCount();
		Debug::Assert(
			aPreviousReading.GetControllerAxisCount() == axisCount &&
			aPreviousReading.GetControllerButtonCount() == buttonCount &&
			aPreviousReading.GetControllerSwitchCount() == switchCount,
			"Controller is assumed unable to change input-control amount between readings.");

		std::unique_ptr<float> prevAxisStates(new float[axisCount]);
		std::unique_ptr<bool> prevButtonStates(new bool[buttonCount]);
		std::unique_ptr<GameInputSwitchPosition> prevSwitchPositions(new GameInputSwitchPosition[switchCount]);
		aPreviousReading.GetControllerAxisState(axisCount, prevAxisStates.get());
		aPreviousReading.GetControllerButtonState(buttonCount, prevButtonStates.get());
		aPreviousReading.GetControllerSwitchState(switchCount, prevSwitchPositions.get());

		std::unique_ptr<float> axisStates(new float[axisCount]);
		std::unique_ptr<bool> buttonStates(new bool[buttonCount]);
		std::unique_ptr<GameInputSwitchPosition> switchPositions(new GameInputSwitchPosition[switchCount]);
		aReading.GetControllerAxisState(axisCount, axisStates.get());
		aReading.GetControllerButtonState(buttonCount, buttonStates.get());
		aReading.GetControllerSwitchState(switchCount, switchPositions.get());

		auto switchToBits = [](GameInputSwitchPosition aPosition) -> std::uint8_t
			{
				switch (aPosition)
				{
					default:
					case GameInputSwitchCenter:    return 0b0000;
					case GameInputSwitchUp:        return 0b1000;
					case GameInputSwitchUpRight:   return 0b1100;
					case GameInputSwitchRight:     return 0b0100;
					case GameInputSwitchDownRight: return 0b0110;
					case GameInputSwitchDown:      return 0b0010;
					case GameInputSwitchDownLeft:  return 0b0011;
					case GameInputSwitchLeft:      return 0b0001;
					case GameInputSwitchUpLeft:    return 0b1001;
				}
			};

		for (std::uint16_t i = 0; i < axisCount; ++i)
		{
			const float currentValue = axisStates.get()[i];
			const float delta = currentValue - prevAxisStates.get()[i];
			if (delta != 0.f)
				OnInput.Invoke(Atrium::InputEvent(*this, Atrium::InputSourceId::Gamepad::Axis(i), currentValue, delta));
		}

		std::uint16_t firstButtonIndex = static_cast<std::uint16_t>(switchCount * 4);
		for (std::uint16_t i = 0; i < buttonCount; ++i)
		{
			std::optional<Atrium::InputEventType> eventType;
			if (!prevButtonStates.get()[i] && buttonStates.get()[i])
				eventType = Atrium::InputEventType::Pressed;
			else if (prevButtonStates.get()[i] && !buttonStates.get()[i])
				eventType = Atrium::InputEventType::Released;
			else
				continue;

			OnInput.Invoke(Atrium::InputEvent(*this, Atrium::InputSourceId::Gamepad::Button(firstButtonIndex + i), eventType.value()));
		}

		for (std::uint16_t i = 0; i < switchCount; ++i)
		{
			const std::uint8_t prevButtons = switchToBits(prevSwitchPositions.get()[i]);
			const std::uint8_t newButtons = switchToBits(switchPositions.get()[i]);
			HandleDigitalChange(prevButtons, newButtons, 0b1000, Atrium::InputSourceId::Gamepad::Button((i * 4) + 0));
			HandleDigitalChange(prevButtons, newButtons, 0b0100, Atrium::InputSourceId::Gamepad::Button((i * 4) + 1));
			HandleDigitalChange(prevButtons, newButtons, 0b0010, Atrium::InputSourceId::Gamepad::Button((i * 4) + 2));
			HandleDigitalChange(prevButtons, newButtons, 0b0001, Atrium::InputSourceId::Gamepad::Button((i * 4) + 3));
		}
	}

	void GameInputDevice::HandleKeyboardReading(IGameInputReading& aPreviousReading, IGameInputReading& aReading)
	{
		std::map<Atrium::InputSourceId, bool> keyState;

		std::unique_ptr<GameInputKeyState> prevKeyStates(new GameInputKeyState[aPreviousReading.GetKeyCount()]);
		aPreviousReading.GetKeyState(aPreviousReading.GetKeyCount(), prevKeyStates.get());
		for (std::uint32_t i = 0; i < aPreviousReading.GetKeyCount(); ++i)
		{
			const GameInputKeyState& state = prevKeyStates.get()[i];
			if (const std::optional<Atrium::InputSourceId> source = ToInputSource(state))
				keyState.insert({ source.value(), true });
		}

		std::unique_ptr<GameInputKeyState> keyStates(new GameInputKeyState[aReading.GetKeyCount()]);
		aReading.GetKeyState(aReading.GetKeyCount(), keyStates.get());
		for (std::uint32_t i = 0; i < aReading.GetKeyCount(); ++i)
		{
			const GameInputKeyState& state = keyStates.get()[i];
			const std::optional<Atrium::InputSourceId> source = ToInputSource(state);
			if (!source)
				continue;

			const auto previousStateIterator = keyState.find(source.value());
			const bool previousState = (previousStateIterator != keyState.end()) ? previousStateIterator->second : false;

			if (!previousState)
			{
				OnInput.Invoke(Atrium::InputEvent(*this, source.value(), Atrium::InputEventType::Pressed));
				OnTextInput.Invoke(Atrium::TextInputEvent(*this, state.codePoint));
			}

			keyState[source.value()] = false;
		}

		// Key states left on "true" haven't been touched by the second loop, meaning they were released.
		for (const auto& it : keyState)
		{
			if (it.second)
				OnInput.Invoke(Atrium::InputEvent(*this, it.first, Atrium::InputEventType::Released));
		}
	}

	void GameInputDevice::HandleMouseReading(IGameInputReading& aPreviousReading, IGameInputReading& aReading)
	{
		GameInputMouseState prevState;
		GameInputMouseState state;
		if (aPreviousReading.GetMouseState(&prevState) && aReading.GetMouseState(&state))
		{
			HandleDigitalChange(prevState, state, &GameInputMouseState::buttons, GameInputMouseLeftButton, Atrium::InputSourceId::Mouse::LeftButton);
			HandleDigitalChange(prevState, state, &GameInputMouseState::buttons, GameInputMouseRightButton, Atrium::InputSourceId::Mouse::RightButton);
			HandleDigitalChange(prevState, state, &GameInputMouseState::buttons, GameInputMouseMiddleButton, Atrium::InputSourceId::Mouse::MiddleButton);
			HandleDigitalChange(prevState, state, &GameInputMouseState::buttons, GameInputMouseButton4, Atrium::InputSourceId::Mouse::ThumbButton1);
			HandleDigitalChange(prevState, state, &GameInputMouseState::buttons, GameInputMouseButton5, Atrium::InputSourceId::Mouse::ThumbButton2);

			HandleAnalogChange(prevState, state, &GameInputMouseState::positionX, Atrium::InputSourceId::Mouse::X);
			HandleAnalogChange(prevState, state, &GameInputMouseState::positionY, Atrium::InputSourceId::Mouse::Y);
			HandleAnalogChange(prevState, state, &GameInputMouseState::wheelX, Atrium::InputSourceId::Mouse::ScrollWheelHorizontal);
			HandleAnalogChange(prevState, state, &GameInputMouseState::wheelY, Atrium::InputSourceId::Mouse::ScrollWheel);
		}
	}

	void GameInputDevice::HandleGamepadReading(IGameInputReading& aPreviousReading, IGameInputReading& aReading)
	{
		GameInputGamepadState prevState;
		GameInputGamepadState state;
		if (aPreviousReading.GetGamepadState(&prevState) && aReading.GetGamepadState(&state))
			HandleGamepadReading(prevState, state);
	}

	void GameInputDevice::HandleTouchReading(IGameInputReading& aPreviousReading, IGameInputReading& aReading)
	{
		aPreviousReading;
		aReading;

		/*const std::uint32_t touchCount = aReading.GetTouchCount();
		Debug::Assert(
			aPreviousReading.GetTouchCount() == touchCount,
			"Controller is assumed to not change how many touch-inputs it can handle.");

		std::unique_ptr<GameInputTouchState> touchStates(new GameInputTouchState[touchCount]);
		aReading.GetTouchState(touchCount, touchStates.get());*/
	}

	void GameInputDevice::HandleGamepadReading(GameInputGamepadState& aPreviousReading, GameInputGamepadState& aReading)
	{
		HandleDigitalChange(aPreviousReading, aReading, &GameInputGamepadState::buttons, GameInputGamepadView, Atrium::InputSourceId::Gamepad::LeftSpecial);
		HandleDigitalChange(aPreviousReading, aReading, &GameInputGamepadState::buttons, GameInputGamepadMenu, Atrium::InputSourceId::Gamepad::RightSpecial);

		HandleDigitalChange(aPreviousReading, aReading, &GameInputGamepadState::buttons, GameInputGamepadA, Atrium::InputSourceId::Gamepad::FaceButtonBottom);
		HandleDigitalChange(aPreviousReading, aReading, &GameInputGamepadState::buttons, GameInputGamepadB, Atrium::InputSourceId::Gamepad::FaceButtonRight);
		HandleDigitalChange(aPreviousReading, aReading, &GameInputGamepadState::buttons, GameInputGamepadX, Atrium::InputSourceId::Gamepad::FaceButtonLeft);
		HandleDigitalChange(aPreviousReading, aReading, &GameInputGamepadState::buttons, GameInputGamepadY, Atrium::InputSourceId::Gamepad::FaceButtonTop);

		HandleDigitalChange(aPreviousReading, aReading, &GameInputGamepadState::buttons, GameInputGamepadDPadUp, Atrium::InputSourceId::Gamepad::DPadUp);
		HandleDigitalChange(aPreviousReading, aReading, &GameInputGamepadState::buttons, GameInputGamepadDPadDown, Atrium::InputSourceId::Gamepad::DPadDown);
		HandleDigitalChange(aPreviousReading, aReading, &GameInputGamepadState::buttons, GameInputGamepadDPadLeft, Atrium::InputSourceId::Gamepad::DPadLeft);
		HandleDigitalChange(aPreviousReading, aReading, &GameInputGamepadState::buttons, GameInputGamepadDPadRight, Atrium::InputSourceId::Gamepad::DPadRight);

		HandleDigitalChange(aPreviousReading, aReading, &GameInputGamepadState::buttons, GameInputGamepadLeftShoulder, Atrium::InputSourceId::Gamepad::LeftShoulder);
		HandleDigitalChange(aPreviousReading, aReading, &GameInputGamepadState::buttons, GameInputGamepadRightShoulder, Atrium::InputSourceId::Gamepad::RightShoulder);
		HandleDigitalChange(aPreviousReading, aReading, &GameInputGamepadState::buttons, GameInputGamepadLeftThumbstick, Atrium::InputSourceId::Gamepad::LeftThumbstick);
		HandleDigitalChange(aPreviousReading, aReading, &GameInputGamepadState::buttons, GameInputGamepadRightThumbstick, Atrium::InputSourceId::Gamepad::RightThumbstick);

		HandleAnalogChange(aPreviousReading, aReading, &GameInputGamepadState::leftThumbstickX, Atrium::InputSourceId::Gamepad::LeftX);
		HandleAnalogChange(aPreviousReading, aReading, &GameInputGamepadState::leftThumbstickY, Atrium::InputSourceId::Gamepad::LeftY);
		HandleAnalogChange(aPreviousReading, aReading, &GameInputGamepadState::leftTrigger, Atrium::InputSourceId::Gamepad::LeftTrigger);
		HandleAnalogChange(aPreviousReading, aReading, &GameInputGamepadState::rightThumbstickX, Atrium::InputSourceId::Gamepad::RightX);
		HandleAnalogChange(aPreviousReading, aReading, &GameInputGamepadState::rightThumbstickY, Atrium::InputSourceId::Gamepad::RightY);
		HandleAnalogChange(aPreviousReading, aReading, &GameInputGamepadState::rightTrigger, Atrium::InputSourceId::Gamepad::RightTrigger);
	}

	std::optional<Microsoft::WRL::ComPtr<IGameInputReading>> GameInputDevice::GetCurrentReading()
	{
		Microsoft::WRL::ComPtr<IGameInputReading> reading;
		if (SUCCEEDED(myGameInputAPI->GetCurrentReading(~GameInputKindUnknown, myDevice.Get(), reading.GetAddressOf())))
			return reading;
		else
			return {};
	}

	std::optional<Atrium::InputSourceId> GameInputDevice::ToInputSource(const GameInputKeyState& aKeyState)
	{
		const UINT returnScancode = ::MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);

		switch (aKeyState.virtualKey)
		{
			//--------------------------------------------------------------------------------------------------------------------------------
			// * Miscellaneous
			//--------------------------------------------------------------------------------------------------------------------------------
			case VK_BACK:          return Atrium::InputSourceId::Keyboard::Backspace;
			case VK_TAB:           return Atrium::InputSourceId::Keyboard::Tab;
			case VK_RETURN:
				if (aKeyState.scanCode == returnScancode)
					return Atrium::InputSourceId::Keyboard::Return;
				else
					// Microsoft why do you do this.
					// Can't directly check if it was NumpadEnter that was pressed, so hope that if it was
					// VK_RETURN and the scancode wasn't that of the return key, that it's NumpadEnter.
					return Atrium::InputSourceId::Keyboard::NumpadEnter;

			case VK_PAUSE:         return Atrium::InputSourceId::Keyboard::Pause;

			case VK_NUMLOCK:       return Atrium::InputSourceId::Keyboard::NumLock;
			case VK_CAPITAL:       return Atrium::InputSourceId::Keyboard::CapsLock;
			case VK_SCROLL:        return Atrium::InputSourceId::Keyboard::ScrollLock;

			case VK_ESCAPE:        return Atrium::InputSourceId::Keyboard::Escape;
			case VK_SPACE:         return Atrium::InputSourceId::Keyboard::Spacebar;
			case VK_PRIOR:         return Atrium::InputSourceId::Keyboard::PageUp;
			case VK_NEXT:          return Atrium::InputSourceId::Keyboard::PageDown;

			case VK_END:           return Atrium::InputSourceId::Keyboard::End;
			case VK_HOME:          return Atrium::InputSourceId::Keyboard::Home;
			case VK_INSERT:        return Atrium::InputSourceId::Keyboard::Insert;
			case VK_DELETE:        return Atrium::InputSourceId::Keyboard::Delete;

			case VK_OEM_5:         return Atrium::InputSourceId::Keyboard::Tilde;

				//--------------------------------------------------------------------------------------------------------------------------------
				// * Modifier keys
				//--------------------------------------------------------------------------------------------------------------------------------
			case VK_LSHIFT:        return Atrium::InputSourceId::Keyboard::LeftShift;
			case VK_RSHIFT:        return Atrium::InputSourceId::Keyboard::RightShift;

			case VK_LCONTROL:      return Atrium::InputSourceId::Keyboard::LeftControl;
			case VK_RCONTROL:      return Atrium::InputSourceId::Keyboard::RightControl;

			case VK_LMENU:         return Atrium::InputSourceId::Keyboard::LeftAlt;
			case VK_RMENU:         return Atrium::InputSourceId::Keyboard::RightAlt;

			case VK_LWIN:          return Atrium::InputSourceId::Keyboard::LeftCommand;
			case VK_RWIN:          return Atrium::InputSourceId::Keyboard::RightCommand;

				//--------------------------------------------------------------------------------------------------------------------------------
				// * Alphanumerical
				//--------------------------------------------------------------------------------------------------------------------------------
			case 'A':              return Atrium::InputSourceId::Keyboard::A;
			case 'B':              return Atrium::InputSourceId::Keyboard::B;
			case 'C':              return Atrium::InputSourceId::Keyboard::C;
			case 'D':              return Atrium::InputSourceId::Keyboard::D;
			case 'E':              return Atrium::InputSourceId::Keyboard::E;
			case 'F':              return Atrium::InputSourceId::Keyboard::F;
			case 'G':              return Atrium::InputSourceId::Keyboard::G;
			case 'H':              return Atrium::InputSourceId::Keyboard::H;
			case 'I':              return Atrium::InputSourceId::Keyboard::I;
			case 'J':              return Atrium::InputSourceId::Keyboard::J;
			case 'K':              return Atrium::InputSourceId::Keyboard::K;
			case 'L':              return Atrium::InputSourceId::Keyboard::L;
			case 'M':              return Atrium::InputSourceId::Keyboard::M;
			case 'N':              return Atrium::InputSourceId::Keyboard::N;
			case 'O':              return Atrium::InputSourceId::Keyboard::O;
			case 'P':              return Atrium::InputSourceId::Keyboard::P;
			case 'Q':              return Atrium::InputSourceId::Keyboard::Q;
			case 'R':              return Atrium::InputSourceId::Keyboard::R;
			case 'S':              return Atrium::InputSourceId::Keyboard::S;
			case 'T':              return Atrium::InputSourceId::Keyboard::T;
			case 'U':              return Atrium::InputSourceId::Keyboard::U;
			case 'V':              return Atrium::InputSourceId::Keyboard::V;
			case 'W':              return Atrium::InputSourceId::Keyboard::W;
			case 'X':              return Atrium::InputSourceId::Keyboard::X;
			case 'Y':              return Atrium::InputSourceId::Keyboard::Y;
			case 'Z':              return Atrium::InputSourceId::Keyboard::Z;

			case '0':              return Atrium::InputSourceId::Keyboard::Alpha0;
			case '1':              return Atrium::InputSourceId::Keyboard::Alpha1;
			case '2':              return Atrium::InputSourceId::Keyboard::Alpha2;
			case '3':              return Atrium::InputSourceId::Keyboard::Alpha3;
			case '4':              return Atrium::InputSourceId::Keyboard::Alpha4;
			case '5':              return Atrium::InputSourceId::Keyboard::Alpha5;
			case '6':              return Atrium::InputSourceId::Keyboard::Alpha6;
			case '7':              return Atrium::InputSourceId::Keyboard::Alpha7;
			case '8':              return Atrium::InputSourceId::Keyboard::Alpha8;
			case '9':              return Atrium::InputSourceId::Keyboard::Alpha9;

				//--------------------------------------------------------------------------------------------------------------------------------
				// * Number pad
				//--------------------------------------------------------------------------------------------------------------------------------
			case VK_NUMPAD0:       return Atrium::InputSourceId::Keyboard::Numpad0;
			case VK_NUMPAD1:       return Atrium::InputSourceId::Keyboard::Numpad1;
			case VK_NUMPAD2:       return Atrium::InputSourceId::Keyboard::Numpad2;
			case VK_NUMPAD3:       return Atrium::InputSourceId::Keyboard::Numpad3;
			case VK_NUMPAD4:       return Atrium::InputSourceId::Keyboard::Numpad4;
			case VK_NUMPAD5:       return Atrium::InputSourceId::Keyboard::Numpad5;
			case VK_NUMPAD6:       return Atrium::InputSourceId::Keyboard::Numpad6;
			case VK_NUMPAD7:       return Atrium::InputSourceId::Keyboard::Numpad7;
			case VK_NUMPAD8:       return Atrium::InputSourceId::Keyboard::Numpad8;
			case VK_NUMPAD9:       return Atrium::InputSourceId::Keyboard::Numpad9;

			case VK_DECIMAL:       return Atrium::InputSourceId::Keyboard::NumpadDecimal;
			case VK_ADD:           return Atrium::InputSourceId::Keyboard::NumpadAdd;
			case VK_SUBTRACT:      return Atrium::InputSourceId::Keyboard::NumpadSubtract;
			case VK_MULTIPLY:      return Atrium::InputSourceId::Keyboard::NumpadMultiply;
			case VK_DIVIDE:        return Atrium::InputSourceId::Keyboard::NumpadDivide;
				// Numpad Enter is handled by VK_RETURN.

				//--------------------------------------------------------------------------------------------------------------------------------
				// * Function keys
				//--------------------------------------------------------------------------------------------------------------------------------
			case VK_F1:            return Atrium::InputSourceId::Keyboard::F1;
			case VK_F2:            return Atrium::InputSourceId::Keyboard::F2;
			case VK_F3:            return Atrium::InputSourceId::Keyboard::F3;
			case VK_F4:            return Atrium::InputSourceId::Keyboard::F4;
			case VK_F5:            return Atrium::InputSourceId::Keyboard::F5;
			case VK_F6:            return Atrium::InputSourceId::Keyboard::F6;
			case VK_F7:            return Atrium::InputSourceId::Keyboard::F7;
			case VK_F8:            return Atrium::InputSourceId::Keyboard::F8;
			case VK_F9:            return Atrium::InputSourceId::Keyboard::F9;
			case VK_F10:           return Atrium::InputSourceId::Keyboard::F10;
			case VK_F11:           return Atrium::InputSourceId::Keyboard::F11;
			case VK_F12:           return Atrium::InputSourceId::Keyboard::F12;
			case VK_F13:           return Atrium::InputSourceId::Keyboard::F13;
			case VK_F14:           return Atrium::InputSourceId::Keyboard::F14;
			case VK_F15:           return Atrium::InputSourceId::Keyboard::F15;
			case VK_F16:           return Atrium::InputSourceId::Keyboard::F16;
			case VK_F17:           return Atrium::InputSourceId::Keyboard::F17;
			case VK_F18:           return Atrium::InputSourceId::Keyboard::F18;
			case VK_F19:           return Atrium::InputSourceId::Keyboard::F19;
			case VK_F20:           return Atrium::InputSourceId::Keyboard::F20;

				//--------------------------------------------------------------------------------------------------------------------------------
				// * Arrow keys
				//--------------------------------------------------------------------------------------------------------------------------------
			case VK_LEFT:          return Atrium::InputSourceId::Keyboard::Left;
			case VK_UP:            return Atrium::InputSourceId::Keyboard::Up;
			case VK_RIGHT:         return Atrium::InputSourceId::Keyboard::Right;
			case VK_DOWN:          return Atrium::InputSourceId::Keyboard::Down;
		}

		return Atrium::InputSourceId::Keyboard::Key(aKeyState.virtualKey);
	}
}
