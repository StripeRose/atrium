// Filter "Input"

#pragma once

#include "Atrium_InputDevice.hpp"

#include <optional>
#include <utility>

namespace Atrium
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
			return InputSourceId(InputDeviceType::Keyboard, 0x1000 | anIndex);
		}

		//--------------------------------------------------
		// * Miscellaneous
		//--------------------------------------------------
		static constexpr InputSourceId Backspace                  { InputDeviceType::Keyboard, 0x0000 };
		static constexpr InputSourceId Tab                        { InputDeviceType::Keyboard, 0x0001 };
		static constexpr InputSourceId Return                     { InputDeviceType::Keyboard, 0x0002 };
		static constexpr InputSourceId Pause                      { InputDeviceType::Keyboard, 0x0003 };

		static constexpr InputSourceId NumLock                    { InputDeviceType::Keyboard, 0x0004 };
		static constexpr InputSourceId ScrollLock                 { InputDeviceType::Keyboard, 0x0005 };
		static constexpr InputSourceId CapsLock                   { InputDeviceType::Keyboard, 0x0006 };

		static constexpr InputSourceId Escape                     { InputDeviceType::Keyboard, 0x0007 };
		static constexpr InputSourceId Spacebar                   { InputDeviceType::Keyboard, 0x0008 };
		static constexpr InputSourceId PageUp                     { InputDeviceType::Keyboard, 0x0009 };
		static constexpr InputSourceId PageDown                   { InputDeviceType::Keyboard, 0x000A };

		static constexpr InputSourceId Home                       { InputDeviceType::Keyboard, 0x000B };
		static constexpr InputSourceId End                        { InputDeviceType::Keyboard, 0x000C };
		static constexpr InputSourceId Insert                     { InputDeviceType::Keyboard, 0x000D };
		static constexpr InputSourceId Delete                     { InputDeviceType::Keyboard, 0x000E };

		static constexpr InputSourceId Tilde                      { InputDeviceType::Keyboard, 0x000F };

		//--------------------------------------------------
		// * Modifier keys
		//--------------------------------------------------
		static constexpr InputSourceId LeftShift                  { InputDeviceType::Keyboard, 0x0010 };
		static constexpr InputSourceId RightShift                 { InputDeviceType::Keyboard, 0x0011 };

		static constexpr InputSourceId LeftControl                { InputDeviceType::Keyboard, 0x0012 };
		static constexpr InputSourceId RightControl               { InputDeviceType::Keyboard, 0x0013 };

		static constexpr InputSourceId LeftAlt                    { InputDeviceType::Keyboard, 0x0014 };
		static constexpr InputSourceId RightAlt                   { InputDeviceType::Keyboard, 0x0015 };

		static constexpr InputSourceId LeftCommand                { InputDeviceType::Keyboard, 0x0016 };
		static constexpr InputSourceId RightCommand               { InputDeviceType::Keyboard, 0x0017 };

		//--------------------------------------------------
		// * Alphanumerical
		//--------------------------------------------------
		static constexpr InputSourceId A                          { InputDeviceType::Keyboard, 0x0020 };
		static constexpr InputSourceId B                          { InputDeviceType::Keyboard, 0x0021 };
		static constexpr InputSourceId C                          { InputDeviceType::Keyboard, 0x0022 };
		static constexpr InputSourceId D                          { InputDeviceType::Keyboard, 0x0023 };
		static constexpr InputSourceId E                          { InputDeviceType::Keyboard, 0x0024 };
		static constexpr InputSourceId F                          { InputDeviceType::Keyboard, 0x0025 };
		static constexpr InputSourceId G                          { InputDeviceType::Keyboard, 0x0026 };
		static constexpr InputSourceId H                          { InputDeviceType::Keyboard, 0x0027 };
		static constexpr InputSourceId I                          { InputDeviceType::Keyboard, 0x0028 };
		static constexpr InputSourceId J                          { InputDeviceType::Keyboard, 0x0029 };
		static constexpr InputSourceId K                          { InputDeviceType::Keyboard, 0x002A };
		static constexpr InputSourceId L                          { InputDeviceType::Keyboard, 0x002B };
		static constexpr InputSourceId M                          { InputDeviceType::Keyboard, 0x002C };
		static constexpr InputSourceId N                          { InputDeviceType::Keyboard, 0x002D };
		static constexpr InputSourceId O                          { InputDeviceType::Keyboard, 0x002E };
		static constexpr InputSourceId P                          { InputDeviceType::Keyboard, 0x002F };
		static constexpr InputSourceId Q                          { InputDeviceType::Keyboard, 0x0030 };
		static constexpr InputSourceId R                          { InputDeviceType::Keyboard, 0x0031 };
		static constexpr InputSourceId S                          { InputDeviceType::Keyboard, 0x0032 };
		static constexpr InputSourceId T                          { InputDeviceType::Keyboard, 0x0033 };
		static constexpr InputSourceId U                          { InputDeviceType::Keyboard, 0x0034 };
		static constexpr InputSourceId V                          { InputDeviceType::Keyboard, 0x0035 };
		static constexpr InputSourceId W                          { InputDeviceType::Keyboard, 0x0036 };
		static constexpr InputSourceId X                          { InputDeviceType::Keyboard, 0x0037 };
		static constexpr InputSourceId Y                          { InputDeviceType::Keyboard, 0x0038 };
		static constexpr InputSourceId Z                          { InputDeviceType::Keyboard, 0x0039 };

		static constexpr InputSourceId Alpha1                     { InputDeviceType::Keyboard, 0x003A };
		static constexpr InputSourceId Alpha2                     { InputDeviceType::Keyboard, 0x003B };
		static constexpr InputSourceId Alpha3                     { InputDeviceType::Keyboard, 0x003C };
		static constexpr InputSourceId Alpha4                     { InputDeviceType::Keyboard, 0x003D };
		static constexpr InputSourceId Alpha5                     { InputDeviceType::Keyboard, 0x003E };
		static constexpr InputSourceId Alpha6                     { InputDeviceType::Keyboard, 0x003F };
		static constexpr InputSourceId Alpha7                     { InputDeviceType::Keyboard, 0x0040 };
		static constexpr InputSourceId Alpha8                     { InputDeviceType::Keyboard, 0x0041 };
		static constexpr InputSourceId Alpha9                     { InputDeviceType::Keyboard, 0x0042 };
		static constexpr InputSourceId Alpha0                     { InputDeviceType::Keyboard, 0x0043 };

		//--------------------------------------------------
		// * Number pad
		//--------------------------------------------------
		static constexpr InputSourceId Numpad1                    { InputDeviceType::Keyboard, 0x0050 };
		static constexpr InputSourceId Numpad2                    { InputDeviceType::Keyboard, 0x0051 };
		static constexpr InputSourceId Numpad3                    { InputDeviceType::Keyboard, 0x0052 };
		static constexpr InputSourceId Numpad4                    { InputDeviceType::Keyboard, 0x0053 };
		static constexpr InputSourceId Numpad5                    { InputDeviceType::Keyboard, 0x0054 };
		static constexpr InputSourceId Numpad6                    { InputDeviceType::Keyboard, 0x0055 };
		static constexpr InputSourceId Numpad7                    { InputDeviceType::Keyboard, 0x0056 };
		static constexpr InputSourceId Numpad8                    { InputDeviceType::Keyboard, 0x0057 };
		static constexpr InputSourceId Numpad9                    { InputDeviceType::Keyboard, 0x0058 };
		static constexpr InputSourceId Numpad0                    { InputDeviceType::Keyboard, 0x0059 };

		static constexpr InputSourceId NumpadDecimal              { InputDeviceType::Keyboard, 0x005A };
		static constexpr InputSourceId NumpadAdd                  { InputDeviceType::Keyboard, 0x005B };
		static constexpr InputSourceId NumpadSubtract             { InputDeviceType::Keyboard, 0x005C };
		static constexpr InputSourceId NumpadMultiply             { InputDeviceType::Keyboard, 0x005D };
		static constexpr InputSourceId NumpadDivide               { InputDeviceType::Keyboard, 0x005E };
		static constexpr InputSourceId NumpadEnter                { InputDeviceType::Keyboard, 0x005F };

		//--------------------------------------------------
		// * Function keys
		//--------------------------------------------------
		static constexpr InputSourceId F1                         { InputDeviceType::Keyboard, 0x0060 };
		static constexpr InputSourceId F2                         { InputDeviceType::Keyboard, 0x0061 };
		static constexpr InputSourceId F3                         { InputDeviceType::Keyboard, 0x0062 };
		static constexpr InputSourceId F4                         { InputDeviceType::Keyboard, 0x0063 };
		static constexpr InputSourceId F5                         { InputDeviceType::Keyboard, 0x0064 };
		static constexpr InputSourceId F6                         { InputDeviceType::Keyboard, 0x0065 };
		static constexpr InputSourceId F7                         { InputDeviceType::Keyboard, 0x0066 };
		static constexpr InputSourceId F8                         { InputDeviceType::Keyboard, 0x0067 };
		static constexpr InputSourceId F9                         { InputDeviceType::Keyboard, 0x0068 };
		static constexpr InputSourceId F10                        { InputDeviceType::Keyboard, 0x0069 };
		static constexpr InputSourceId F11                        { InputDeviceType::Keyboard, 0x006A };
		static constexpr InputSourceId F12                        { InputDeviceType::Keyboard, 0x006B };
		static constexpr InputSourceId F13                        { InputDeviceType::Keyboard, 0x006C };
		static constexpr InputSourceId F14                        { InputDeviceType::Keyboard, 0x006D };
		static constexpr InputSourceId F15                        { InputDeviceType::Keyboard, 0x006E };
		static constexpr InputSourceId F16                        { InputDeviceType::Keyboard, 0x006F };
		static constexpr InputSourceId F17                        { InputDeviceType::Keyboard, 0x0070 };
		static constexpr InputSourceId F18                        { InputDeviceType::Keyboard, 0x0071 };
		static constexpr InputSourceId F19                        { InputDeviceType::Keyboard, 0x0072 };
		static constexpr InputSourceId F20                        { InputDeviceType::Keyboard, 0x0073 };

		//--------------------------------------------------
		// * Arrow keys
		//--------------------------------------------------
		static constexpr InputSourceId Left                       { InputDeviceType::Keyboard, 0x0080 };
		static constexpr InputSourceId Up                         { InputDeviceType::Keyboard, 0x0081 };
		static constexpr InputSourceId Right                      { InputDeviceType::Keyboard, 0x0082 };
		static constexpr InputSourceId Down                       { InputDeviceType::Keyboard, 0x0083 };
	};

	struct InputSourceId::Mouse
	{
		static constexpr InputSourceId X                          { InputDeviceType::Mouse, 0x0010 };
		static constexpr InputSourceId Y                          { InputDeviceType::Mouse, 0x0011 };
		static constexpr InputSourceId AbsoluteX                  { InputDeviceType::Mouse, 0x0012 };
		static constexpr InputSourceId AbsoluteY                  { InputDeviceType::Mouse, 0x0013 };

		static constexpr InputSourceId ScrollWheel                { InputDeviceType::Mouse, 0x0014 };
		static constexpr InputSourceId ScrollWheelHorizontal      { InputDeviceType::Mouse, 0x0015 };

		static constexpr InputSourceId LeftButton                 { InputDeviceType::Mouse, 0x0000 };
		static constexpr InputSourceId RightButton                { InputDeviceType::Mouse, 0x0001 };
		static constexpr InputSourceId MiddleButton               { InputDeviceType::Mouse, 0x0002 };

		static constexpr InputSourceId ThumbButton1               { InputDeviceType::Mouse, 0x0003 };
		static constexpr InputSourceId ThumbButton2               { InputDeviceType::Mouse, 0x0004 };
	};

	struct InputSourceId::Gamepad
	{
		static constexpr InputSourceId Axis(std::uint16_t anIndex)
		{
			return InputSourceId(InputDeviceType::Gamepad, 0x1000 | anIndex);
		}

		static constexpr InputSourceId Button(std::uint16_t anIndex)
		{
			return InputSourceId(InputDeviceType::Gamepad, 0x2000 | anIndex);
		}

		static constexpr InputSourceId LeftX                      { InputDeviceType::Gamepad, 0x0000 };
		static constexpr InputSourceId LeftY                      { InputDeviceType::Gamepad, 0x0001 };
		static constexpr InputSourceId RightX                     { InputDeviceType::Gamepad, 0x0002 };
		static constexpr InputSourceId RightY                     { InputDeviceType::Gamepad, 0x0003 };
		static constexpr InputSourceId LeftTrigger                { InputDeviceType::Gamepad, 0x0004 };
		static constexpr InputSourceId RightTrigger               { InputDeviceType::Gamepad, 0x0005 };

		static constexpr InputSourceId LeftThumbstick             { InputDeviceType::Gamepad, 0x0006 };
		static constexpr InputSourceId RightThumbstick            { InputDeviceType::Gamepad, 0x0007 };

		static constexpr InputSourceId FaceButtonBottom           { InputDeviceType::Gamepad, 0x0008 };
		static constexpr InputSourceId FaceButtonRight            { InputDeviceType::Gamepad, 0x0009 };
		static constexpr InputSourceId FaceButtonLeft             { InputDeviceType::Gamepad, 0x000A };
		static constexpr InputSourceId FaceButtonTop              { InputDeviceType::Gamepad, 0x000B };

		static constexpr InputSourceId LeftShoulder               { InputDeviceType::Gamepad, 0x000C };
		static constexpr InputSourceId RightShoulder              { InputDeviceType::Gamepad, 0x000D };

		static constexpr InputSourceId LeftSpecial                { InputDeviceType::Gamepad, 0x000E };
		static constexpr InputSourceId RightSpecial               { InputDeviceType::Gamepad, 0x000F };

		static constexpr InputSourceId DPadDown                   { InputDeviceType::Gamepad, 0x0010 };
		static constexpr InputSourceId DPadRight                  { InputDeviceType::Gamepad, 0x0011 };
		static constexpr InputSourceId DPadLeft                   { InputDeviceType::Gamepad, 0x0012 };
		static constexpr InputSourceId DPadUp                     { InputDeviceType::Gamepad, 0x0013 };
	};
}
