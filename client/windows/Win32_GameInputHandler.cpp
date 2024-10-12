// Filter "Input/GameInput"

#include "Win32_GameInputHandler.hpp"

#include "Core_Diagnostics.hpp"

#include <sstream>

namespace Atrium::Win32
{
	GameInputHandler::GameInputHandler(Core::InputDeviceAPI& anInputDeviceAPI)
		: myInputDeviceAPI(anInputDeviceAPI)
		, myDeviceCallbackToken(0)
	{
		if (FAILED(GameInputCreate(myGameInputAPI.ReleaseAndGetAddressOf())))
		{
			myGameInputAPI.Reset();
			return;
		}

		myGameInputAPI->SetFocusPolicy(GameInputFocusPolicy::GameInputExclusiveForegroundInput);

		static constexpr IGameInputDevice* anyDevice = nullptr;
		static constexpr GameInputKind anyKind = ~GameInputKindUnknown;

		myGameInputAPI->RegisterDeviceCallback(
			anyDevice,
			anyKind,
			GameInputDeviceAnyStatus,
			GameInputBlockingEnumeration,
			this,
			&HandleDeviceEvent,
			&myDeviceCallbackToken
		);
	}

	GameInputHandler::~GameInputHandler()
	{
		
	}

	void GameInputHandler::ReportInputEvents(Core::InputDeviceType someDeviceTypes)
	{
		for (const auto& it : myDevices)
			it.second->ReportInputEvents(someDeviceTypes);
	}

	void GameInputHandler::HandleDeviceEvent(GameInputCallbackToken /*aCallbackToken*/, void* aContext, IGameInputDevice* aDevice, uint64_t /*aTimestamp*/, GameInputDeviceStatus aCurrentStatus, GameInputDeviceStatus aPreviousStatus)
	{
		GameInputHandler& self = *reinterpret_cast<GameInputHandler*>(aContext);

		GameInputDevice& device = self.CreateOrGetDevice(aDevice);
		device.HandleDeviceEvent(aCurrentStatus, aPreviousStatus);
	}

	GameInputDevice& GameInputHandler::CreateOrGetDevice(IGameInputDevice* aDevice)
	{
		auto device = myDevices.find(aDevice);
		if (device != myDevices.end())
			return *device->second.get();

		myDevices.insert({
			aDevice,
			std::make_unique<GameInputDevice>(myGameInputAPI.Get(), aDevice)
			});

		GameInputDevice* newDevice = myDevices.at(aDevice).get();
		newDevice->OnInput.Connect(this, [&](const Core::InputEvent& anEvent) { myInputDeviceAPI.OnInput.Invoke(anEvent); });
		newDevice->OnTextInput.Connect(this, [&](const Core::TextInputEvent& anEvent) { myInputDeviceAPI.OnTextInput.Invoke(anEvent); });

		return *newDevice;
	}
}