// Filter "Input/GameInput"

#pragma once

#include "Win32_GameInput.h"
#include "Win32_GameInputDevice.hpp"

#include "Atrium_InputDeviceAPI.hpp"
#include "Atrium_InputEvent.hpp"

#include <wrl.h>

#include <map>
#include <memory>

namespace Atrium::Win32
{
	class GameInputHandler
	{
	public:
		GameInputHandler(Atrium::InputDeviceAPI& anInputDeviceAPI);
		~GameInputHandler();

		void ReportInputEvents(Atrium::InputDeviceType someDeviceTypes);

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

		Atrium::InputDeviceAPI& myInputDeviceAPI;
	};
}