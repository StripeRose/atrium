// Filter "Input/GameInput"

#pragma once

#include "Win32_GameInputDevice.hpp"

#include "Core_InputDeviceAPI.hpp"
#include "Core_InputEvent.hpp"

#include <GameInput.h>
#include <wrl.h>

#include <map>
#include <memory>

namespace Atrium::Win32
{
	class GameInputHandler
	{
	public:
		GameInputHandler(Core::InputDeviceAPI& anInputDeviceAPI);
		~GameInputHandler();

		void ReportInputEvents(Core::InputDeviceType someDeviceTypes);

	private:
		static void HandleDeviceEvent(
			GameInputCallbackToken aCallbackToken,
			void* aContext,
			IGameInputDevice* aDevice,
			uint64_t aTimestamp,
			GameInputDeviceStatus aCurrentStatus,
			GameInputDeviceStatus aPreviousStatus
		);

		GameInputDevice& CreateOrGetDevice(IGameInputDevice* aDevice);

		Microsoft::WRL::ComPtr<IGameInput> myGameInputAPI;
		GameInputCallbackToken myDeviceCallbackToken;

		std::map<Microsoft::WRL::ComPtr<IGameInputDevice>, std::unique_ptr<GameInputDevice>> myDevices;

		Core::InputDeviceAPI& myInputDeviceAPI;
	};
}