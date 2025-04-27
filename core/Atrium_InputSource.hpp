// Filter "Input"

#pragma once

#include "Atrium_InputDevice.hpp"

#include <optional>
#include <utility>

namespace Atrium::Core
{
	/**
	 * @brief Short-hand ID for referring to a specific input source on any device of the given type.
	 *        Not intended to be used for persistence, as IDs may change between versions.
	 * 
	 *        May contain device-specific flags for additional state information, such as modifier keys.
	 */
	class InputSourceId
	{
	public:

		//--------------------------------------------------
		// * Types
		//--------------------------------------------------
		#pragma region Types

		struct Keyboard;
		struct Mouse;
		struct Gamepad;

		#pragma endregion

		//--------------------------------------------------
		// * Construction
		//--------------------------------------------------
		#pragma region Construction

		constexpr InputSourceId() noexcept
			: Device(InputDeviceType::Unknown)
			, Index(0)
		{

		}

		#pragma endregion

		//--------------------------------------------------
		// * Properties
		//--------------------------------------------------
		#pragma region Properties

		InputDeviceType Device;

		std::uint16_t Index;

		#pragma endregion

		//--------------------------------------------------
		// * Operators
		//--------------------------------------------------
		#pragma region Operators

		constexpr operator std::uint32_t() const noexcept
		{
			static_assert(sizeof(InputDeviceType) <= sizeof(std::uint16_t), "Function implementation must be updated if device type enum size changes.");
			return (static_cast<std::uint16_t>(Device) << 16) | Index;
		}

		constexpr std::strong_ordering operator<=>(const InputSourceId& anId) const noexcept
		{
			const std::strong_ordering order = std::strong_order(Device, anId.Device);

			if (order != std::strong_ordering::equal)
				return order;

			return std::strong_order(Index, anId.Index);
		}

		/**
		 * @brief Check if the id is exactly the same as another.
		 */
		constexpr bool operator==(const InputSourceId& anId) const noexcept
		{
			return Device == anId.Device && Index == anId.Index;
		}

		/**
		 * @brief Check if the id differs at all from another.
		 */
		constexpr bool operator!=(const InputSourceId& anId) const noexcept
		{
			return Device != anId.Device || Index != anId.Index;
		}

		#pragma endregion

	protected:
		constexpr InputSourceId(InputDeviceType aDevice, std::uint16_t anIndex) noexcept
			: Device(aDevice)
			, Index(anIndex)
		{

		}
	};

	/**
	 * @brief Refers to keys on a ANSI/ISO QWERTY layout keyboard.
	 */
	struct InputSourceId::Keyboard
	{
		static constexpr InputSourceId Key(std::uint16_t anIndex)
		{
			return InputSourceId(Core::InputDeviceType::Keyboard, 0x1000 | anIndex);
		}

		//--------------------------------------------------
		// * Miscellaneous
		//--------------------------------------------------
		static constexpr InputSourceId Backspace                  { Core::InputDeviceType::Keyboard, 0x0000 };
		static constexpr InputSourceId Tab                        { Core::InputDeviceType::Keyboard, 0x0001 };
		static constexpr InputSourceId Return                     { Core::InputDeviceType::Keyboard, 0x0002 };
		static constexpr InputSourceId Pause                      { Core::InputDeviceType::Keyboard, 0x0003 };

		static constexpr InputSourceId NumLock                    { Core::InputDeviceType::Keyboard, 0x0004 };
		static constexpr InputSourceId ScrollLock                 { Core::InputDeviceType::Keyboard, 0x0005 };
		static constexpr InputSourceId CapsLock                   { Core::InputDeviceType::Keyboard, 0x0006 };

		static constexpr InputSourceId Escape                     { Core::InputDeviceType::Keyboard, 0x0007 };
		static constexpr InputSourceId Spacebar                   { Core::InputDeviceType::Keyboard, 0x0008 };
		static constexpr InputSourceId PageUp                     { Core::InputDeviceType::Keyboard, 0x0009 };
		static constexpr InputSourceId PageDown                   { Core::InputDeviceType::Keyboard, 0x000A };

		static constexpr InputSourceId Home                       { Core::InputDeviceType::Keyboard, 0x000B };
		static constexpr InputSourceId End                        { Core::InputDeviceType::Keyboard, 0x000C };
		static constexpr InputSourceId Insert                     { Core::InputDeviceType::Keyboard, 0x000D };
		static constexpr InputSourceId Delete                     { Core::InputDeviceType::Keyboard, 0x000E };

		static constexpr InputSourceId Tilde                      { Core::InputDeviceType::Keyboard, 0x000F };

		//--------------------------------------------------
		// * Modifier keys
		//--------------------------------------------------
		static constexpr InputSourceId LeftShift                  { Core::InputDeviceType::Keyboard, 0x0010 };
		static constexpr InputSourceId RightShift                 { Core::InputDeviceType::Keyboard, 0x0011 };

		static constexpr InputSourceId LeftControl                { Core::InputDeviceType::Keyboard, 0x0012 };
		static constexpr InputSourceId RightControl               { Core::InputDeviceType::Keyboard, 0x0013 };

		static constexpr InputSourceId LeftAlt                    { Core::InputDeviceType::Keyboard, 0x0014 };
		static constexpr InputSourceId RightAlt                   { Core::InputDeviceType::Keyboard, 0x0015 };

		static constexpr InputSourceId LeftCommand                { Core::InputDeviceType::Keyboard, 0x0016 };
		static constexpr InputSourceId RightCommand               { Core::InputDeviceType::Keyboard, 0x0017 };

		//--------------------------------------------------
		// * Alphanumerical
		//--------------------------------------------------
		static constexpr InputSourceId A                          { Core::InputDeviceType::Keyboard, 0x0020 };
		static constexpr InputSourceId B                          { Core::InputDeviceType::Keyboard, 0x0021 };
		static constexpr InputSourceId C                          { Core::InputDeviceType::Keyboard, 0x0022 };
		static constexpr InputSourceId D                          { Core::InputDeviceType::Keyboard, 0x0023 };
		static constexpr InputSourceId E                          { Core::InputDeviceType::Keyboard, 0x0024 };
		static constexpr InputSourceId F                          { Core::InputDeviceType::Keyboard, 0x0025 };
		static constexpr InputSourceId G                          { Core::InputDeviceType::Keyboard, 0x0026 };
		static constexpr InputSourceId H                          { Core::InputDeviceType::Keyboard, 0x0027 };
		static constexpr InputSourceId I                          { Core::InputDeviceType::Keyboard, 0x0028 };
		static constexpr InputSourceId J                          { Core::InputDeviceType::Keyboard, 0x0029 };
		static constexpr InputSourceId K                          { Core::InputDeviceType::Keyboard, 0x002A };
		static constexpr InputSourceId L                          { Core::InputDeviceType::Keyboard, 0x002B };
		static constexpr InputSourceId M                          { Core::InputDeviceType::Keyboard, 0x002C };
		static constexpr InputSourceId N                          { Core::InputDeviceType::Keyboard, 0x002D };
		static constexpr InputSourceId O                          { Core::InputDeviceType::Keyboard, 0x002E };
		static constexpr InputSourceId P                          { Core::InputDeviceType::Keyboard, 0x002F };
		static constexpr InputSourceId Q                          { Core::InputDeviceType::Keyboard, 0x0030 };
		static constexpr InputSourceId R                          { Core::InputDeviceType::Keyboard, 0x0031 };
		static constexpr InputSourceId S                          { Core::InputDeviceType::Keyboard, 0x0032 };
		static constexpr InputSourceId T                          { Core::InputDeviceType::Keyboard, 0x0033 };
		static constexpr InputSourceId U                          { Core::InputDeviceType::Keyboard, 0x0034 };
		static constexpr InputSourceId V                          { Core::InputDeviceType::Keyboard, 0x0035 };
		static constexpr InputSourceId W                          { Core::InputDeviceType::Keyboard, 0x0036 };
		static constexpr InputSourceId X                          { Core::InputDeviceType::Keyboard, 0x0037 };
		static constexpr InputSourceId Y                          { Core::InputDeviceType::Keyboard, 0x0038 };
		static constexpr InputSourceId Z                          { Core::InputDeviceType::Keyboard, 0x0039 };

		static constexpr InputSourceId Alpha1                     { Core::InputDeviceType::Keyboard, 0x003A };
		static constexpr InputSourceId Alpha2                     { Core::InputDeviceType::Keyboard, 0x003B };
		static constexpr InputSourceId Alpha3                     { Core::InputDeviceType::Keyboard, 0x003C };
		static constexpr InputSourceId Alpha4                     { Core::InputDeviceType::Keyboard, 0x003D };
		static constexpr InputSourceId Alpha5                     { Core::InputDeviceType::Keyboard, 0x003E };
		static constexpr InputSourceId Alpha6                     { Core::InputDeviceType::Keyboard, 0x003F };
		static constexpr InputSourceId Alpha7                     { Core::InputDeviceType::Keyboard, 0x0040 };
		static constexpr InputSourceId Alpha8                     { Core::InputDeviceType::Keyboard, 0x0041 };
		static constexpr InputSourceId Alpha9                     { Core::InputDeviceType::Keyboard, 0x0042 };
		static constexpr InputSourceId Alpha0                     { Core::InputDeviceType::Keyboard, 0x0043 };

		//--------------------------------------------------
		// * Number pad
		//--------------------------------------------------
		static constexpr InputSourceId Numpad1                    { Core::InputDeviceType::Keyboard, 0x0050 };
		static constexpr InputSourceId Numpad2                    { Core::InputDeviceType::Keyboard, 0x0051 };
		static constexpr InputSourceId Numpad3                    { Core::InputDeviceType::Keyboard, 0x0052 };
		static constexpr InputSourceId Numpad4                    { Core::InputDeviceType::Keyboard, 0x0053 };
		static constexpr InputSourceId Numpad5                    { Core::InputDeviceType::Keyboard, 0x0054 };
		static constexpr InputSourceId Numpad6                    { Core::InputDeviceType::Keyboard, 0x0055 };
		static constexpr InputSourceId Numpad7                    { Core::InputDeviceType::Keyboard, 0x0056 };
		static constexpr InputSourceId Numpad8                    { Core::InputDeviceType::Keyboard, 0x0057 };
		static constexpr InputSourceId Numpad9                    { Core::InputDeviceType::Keyboard, 0x0058 };
		static constexpr InputSourceId Numpad0                    { Core::InputDeviceType::Keyboard, 0x0059 };

		static constexpr InputSourceId NumpadDecimal              { Core::InputDeviceType::Keyboard, 0x005A };
		static constexpr InputSourceId NumpadAdd                  { Core::InputDeviceType::Keyboard, 0x005B };
		static constexpr InputSourceId NumpadSubtract             { Core::InputDeviceType::Keyboard, 0x005C };
		static constexpr InputSourceId NumpadMultiply             { Core::InputDeviceType::Keyboard, 0x005D };
		static constexpr InputSourceId NumpadDivide               { Core::InputDeviceType::Keyboard, 0x005E };
		static constexpr InputSourceId NumpadEnter                { Core::InputDeviceType::Keyboard, 0x005F };

		//--------------------------------------------------
		// * Function keys
		//--------------------------------------------------
		static constexpr InputSourceId F1                         { Core::InputDeviceType::Keyboard, 0x0060 };
		static constexpr InputSourceId F2                         { Core::InputDeviceType::Keyboard, 0x0061 };
		static constexpr InputSourceId F3                         { Core::InputDeviceType::Keyboard, 0x0062 };
		static constexpr InputSourceId F4                         { Core::InputDeviceType::Keyboard, 0x0063 };
		static constexpr InputSourceId F5                         { Core::InputDeviceType::Keyboard, 0x0064 };
		static constexpr InputSourceId F6                         { Core::InputDeviceType::Keyboard, 0x0065 };
		static constexpr InputSourceId F7                         { Core::InputDeviceType::Keyboard, 0x0066 };
		static constexpr InputSourceId F8                         { Core::InputDeviceType::Keyboard, 0x0067 };
		static constexpr InputSourceId F9                         { Core::InputDeviceType::Keyboard, 0x0068 };
		static constexpr InputSourceId F10                        { Core::InputDeviceType::Keyboard, 0x0069 };
		static constexpr InputSourceId F11                        { Core::InputDeviceType::Keyboard, 0x006A };
		static constexpr InputSourceId F12                        { Core::InputDeviceType::Keyboard, 0x006B };
		static constexpr InputSourceId F13                        { Core::InputDeviceType::Keyboard, 0x006C };
		static constexpr InputSourceId F14                        { Core::InputDeviceType::Keyboard, 0x006D };
		static constexpr InputSourceId F15                        { Core::InputDeviceType::Keyboard, 0x006E };
		static constexpr InputSourceId F16                        { Core::InputDeviceType::Keyboard, 0x006F };
		static constexpr InputSourceId F17                        { Core::InputDeviceType::Keyboard, 0x0070 };
		static constexpr InputSourceId F18                        { Core::InputDeviceType::Keyboard, 0x0071 };
		static constexpr InputSourceId F19                        { Core::InputDeviceType::Keyboard, 0x0072 };
		static constexpr InputSourceId F20                        { Core::InputDeviceType::Keyboard, 0x0073 };

		//--------------------------------------------------
		// * Arrow keys
		//--------------------------------------------------
		static constexpr InputSourceId Left                       { Core::InputDeviceType::Keyboard, 0x0080 };
		static constexpr InputSourceId Up                         { Core::InputDeviceType::Keyboard, 0x0081 };
		static constexpr InputSourceId Right                      { Core::InputDeviceType::Keyboard, 0x0082 };
		static constexpr InputSourceId Down                       { Core::InputDeviceType::Keyboard, 0x0083 };
	};

	struct InputSourceId::Mouse
	{
		static constexpr InputSourceId X                          { Core::InputDeviceType::Mouse, 0x0010 };
		static constexpr InputSourceId Y                          { Core::InputDeviceType::Mouse, 0x0011 };
		static constexpr InputSourceId AbsoluteX                  { Core::InputDeviceType::Mouse, 0x0012 };
		static constexpr InputSourceId AbsoluteY                  { Core::InputDeviceType::Mouse, 0x0013 };

		static constexpr InputSourceId ScrollWheel                { Core::InputDeviceType::Mouse, 0x0014 };
		static constexpr InputSourceId ScrollWheelHorizontal      { Core::InputDeviceType::Mouse, 0x0015 };

		static constexpr InputSourceId LeftButton                 { Core::InputDeviceType::Mouse, 0x0000 };
		static constexpr InputSourceId RightButton                { Core::InputDeviceType::Mouse, 0x0001 };
		static constexpr InputSourceId MiddleButton               { Core::InputDeviceType::Mouse, 0x0002 };

		static constexpr InputSourceId ThumbButton1               { Core::InputDeviceType::Mouse, 0x0003 };
		static constexpr InputSourceId ThumbButton2               { Core::InputDeviceType::Mouse, 0x0004 };
	};

	struct InputSourceId::Gamepad
	{
		static constexpr InputSourceId Axis(std::uint16_t anIndex)
		{
			return InputSourceId(Core::InputDeviceType::Gamepad, 0x1000 | anIndex);
		}

		static constexpr InputSourceId Button(std::uint16_t anIndex)
		{
			return InputSourceId(Core::InputDeviceType::Gamepad, 0x2000 | anIndex);
		}

		static constexpr InputSourceId LeftX                      { Core::InputDeviceType::Gamepad, 0x0000 };
		static constexpr InputSourceId LeftY                      { Core::InputDeviceType::Gamepad, 0x0001 };
		static constexpr InputSourceId RightX                     { Core::InputDeviceType::Gamepad, 0x0002 };
		static constexpr InputSourceId RightY                     { Core::InputDeviceType::Gamepad, 0x0003 };
		static constexpr InputSourceId LeftTrigger                { Core::InputDeviceType::Gamepad, 0x0004 };
		static constexpr InputSourceId RightTrigger               { Core::InputDeviceType::Gamepad, 0x0005 };

		static constexpr InputSourceId LeftThumbstick             { Core::InputDeviceType::Gamepad, 0x0006 };
		static constexpr InputSourceId RightThumbstick            { Core::InputDeviceType::Gamepad, 0x0007 };

		static constexpr InputSourceId FaceButtonBottom           { Core::InputDeviceType::Gamepad, 0x0008 };
		static constexpr InputSourceId FaceButtonRight            { Core::InputDeviceType::Gamepad, 0x0009 };
		static constexpr InputSourceId FaceButtonLeft             { Core::InputDeviceType::Gamepad, 0x000A };
		static constexpr InputSourceId FaceButtonTop              { Core::InputDeviceType::Gamepad, 0x000B };

		static constexpr InputSourceId LeftShoulder               { Core::InputDeviceType::Gamepad, 0x000C };
		static constexpr InputSourceId RightShoulder              { Core::InputDeviceType::Gamepad, 0x000D };

		static constexpr InputSourceId LeftSpecial                { Core::InputDeviceType::Gamepad, 0x000E };
		static constexpr InputSourceId RightSpecial               { Core::InputDeviceType::Gamepad, 0x000F };

		static constexpr InputSourceId DPadDown                   { Core::InputDeviceType::Gamepad, 0x0010 };
		static constexpr InputSourceId DPadRight                  { Core::InputDeviceType::Gamepad, 0x0011 };
		static constexpr InputSourceId DPadLeft                   { Core::InputDeviceType::Gamepad, 0x0012 };
		static constexpr InputSourceId DPadUp                     { Core::InputDeviceType::Gamepad, 0x0013 };
	};
}
