#ifndef _INPUT_H
#define _INPUT_H
#define WIN32_LEAN_AND_MEAN

class Input;

#include <windows.h>
#include <WindowsX.h>
#include <string>
#include <XInput.h>
#include "constants.h"
#include "gameError.h"


#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT) 0x02)
#endif


namespace inputNS
{
	const int KEYS_ARRAY_LEN = 256;
	const UCHAR KEYS_DOWN = 1;
	const UCHAR KEYS_PRESSED = 2;
	const UCHAR MOUSE = 4;
	const UCHAR TEXT_IN = 8;
	const UCHAR KEYS_MOUSE_TEXT = KEYS_DOWN + KEYS_PRESSED + MOUSE + TEXT_IN;
}

const DWORD MAX_CONTROLLERS = 4;
const DWORD GAMEPAD_THUMBSTICK_DEADZONE = 0.20f * float(0X7FFF);
const DWORD GAMEPAD_TRIGGER_DEADZONE = 30;

struct ControllerState
{
	XINPUT_STATE state;
	XINPUT_VIBRATION vibration;
	float vibrateTimeLeft;
	float vibrateTimeRight;
	bool connected;
};

const DWORD GAMEPAD_DPAD_UP = 0x0001;
const DWORD GAMEPAD_DPAD_DOWN = 0x0002;
const DWORD GAMEPAD_DPAD_LEFT = 0x0004;
const DWORD GAMEPAD_DPAD_RIGHT = 0x0008;
const DWORD GAMEPAD_START_BUTTON = 0x0010;
const DWORD GAMEPAD_BACK_BUTTON = 0x0020;
const DWORD GAMEPAD_LEFT_THUMB = 0x0040;
const DWORD GAMEPAD_RIGHT_THUMB = 0x0080;
const DWORD GAMEPAD_LEFT_SHOULDER = 0x0100;
const DWORD GAMEPAD_RIGHT_SHOULDER = 0x0200;
const DWORD GAMEPAD_A = 0x1000;
const DWORD GAMEPAD_B = 0x2000;
const DWORD GAMEPAD_X = 0x4000;
const DWORD GAMEPAD_Y = 0x8000;

class Input {

private:
	bool keysDown[inputNS::KEYS_ARRAY_LEN];
	bool keysPressed[inputNS::KEYS_ARRAY_LEN];

	std::string textIn;
	char charIn;
	bool newLine;
	int mouseX, mouseY;
	int mouseRawX, mouseRawY;
	RAWINPUTDEVICE Rid[1];
	bool mouseCaptured;
	bool mouseLButton;
	bool mouseRButton;
	bool mouseMButton;
	bool mouseX1Button;
	bool mouseX2Button;
	ControllerState controllers[MAX_CONTROLLERS];

public:
	Input();
	~Input();
	void initialize(HWND hwnd, bool capture);
	void keyDown(WPARAM wParam);
	void keyUp(WPARAM wParam);
	void keyIn(WPARAM wParam);
	bool isKeyDown(UCHAR vkey) const;
	bool wasKeyPressed(UCHAR vkey) const;
	bool anyKeyPressed() const;
	std::string getTextIn() { return textIn; }
	char getCharIn() { return charIn; }
	void clearKeyPress(UCHAR vkey);
	void clearTextIn() { textIn.clear(); }
	void clearAll() { clear(inputNS::KEYS_MOUSE_TEXT); }
	void clear(UCHAR what);
	void mouseIn(LPARAM lParam);
	void mouseRawIn(LPARAM lParam);
	void setMouseLButton(bool b) { mouseLButton = b; }
	void setMouseRButton(bool b) { mouseRButton = b; }
	void setMouseMButton(bool b) { mouseMButton = b; }

	void setMouseXButton(WPARAM wParam)
	{
		mouseX1Button = (wParam & MK_XBUTTON1) ? true : false;
		mouseX2Button = (wParam & MK_XBUTTON2) ? true : false;
	}

	int getMouseX() const { return mouseX; }
	int getMouseY() const { return mouseY; }
	int getRawMouseX() const { return mouseRawX; }
	int getRawMouseY() const { return mouseRawY; }
	bool getMouseLButton() const { return mouseLButton; }
	bool getMouseRButton() const { return mouseRButton; }
	bool getMouseMButton() const { return mouseMButton; }
	bool getMouseX1Button() const { return mouseX1Button; }
	bool getMouseX2Button() const { return mouseX2Button; }
	void readControllers();
	void checkControllers();
	void vibrateControllers(float frameTime);

	const ControllerState* getControllerState(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return &controllers[n];
	}

	const WORD getGamePadButtons(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return controllers[n].state.Gamepad.wButtons;
	}

	//Return state of controller n D-pad Up.
	bool getGamepadDPadUp(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons&GAMEPAD_DPAD_UP) != 0);
	}
	//Return state of controller n D-pad Down.
	bool getGamepadDPadDown(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons&GAMEPAD_DPAD_DOWN) != 0);
	}

	// Return state of controller n D-pad Left.
	bool getGamepadDPadLeft(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons&GAMEPAD_DPAD_LEFT) != 0);
	}

	// Return state of controller n D-pad Right.
	bool getGamepadDPadRight(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_DPAD_RIGHT) != 0);
	}

	// Return state of controller n Start button.
	bool getGamepadStart(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_START_BUTTON) != 0);
	}

	// Return state of controller n Back button.
	bool getGamepadBack(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_BACK_BUTTON) != 0);
	}

	// Return state of controller n Left Thumb button.
	bool getGamepadLeftThumb(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_LEFT_THUMB) != 0);
	}

	// Return state of controller n Right Thumb button.
	bool getGamepadRightThumb(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_RIGHT_THUMB) != 0);
	}

	// Return state of controller n Left Shoulder button.
	bool getGamepadLeftShoulder(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_LEFT_SHOULDER) != 0);
	}

	// Return state of controller n Right Shoulder button.
	bool getGamepadRightShoulder(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_RIGHT_SHOULDER) != 0);
	}

	// Return state of controller n A button.
	bool getGamepadA(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_A) != 0);
	}

	// Return state of controller n B button.
	bool getGamepadB(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_B) != 0);
	}

	// Return state of controller n X button.
	bool getGamepadX(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_X) != 0);
	}

	// Return state of controller n Y button.
	bool getGamepadY(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_Y) != 0);
	}

	// Return value of controller n Left Trigger.
	BYTE getGamepadLeftTrigger(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return (controllers[n].state.Gamepad.bLeftTrigger);
	}

	// Return value of controller n Right Trigger.
	BYTE getGamepadRightTrigger(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return (controllers[n].state.Gamepad.bRightTrigger);
	}

	// Return value of controller n Left Thumbstick X.
	SHORT getGamepadThumbLX(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return (controllers[n].state.Gamepad.sThumbLX);
	}

	// Return value of controller n Left Thumbstick Y.
	SHORT getGamepadThumbLY(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return (controllers[n].state.Gamepad.sThumbLY);
	}

	// Return value of controller n Right Thumbstick X.
	SHORT getGamepadThumbRX(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return (controllers[n].state.Gamepad.sThumbRX);
	}

	// Return value of controller n Right Thumbstick Y.
	SHORT getGamepadThumbRY(UINT n)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		return (controllers[n].state.Gamepad.sThumbRY);
	}

	void gamePadVibrateLeft(UINT n, WORD speed, float sec)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		controllers[n].vibration.wLeftMotorSpeed = speed;
		controllers[n].vibrateTimeLeft = sec;
	}

	void gamePadVibrateRight(UINT n, WORD speed, float sec)
	{
		if (n > MAX_CONTROLLERS - 1)
			n = MAX_CONTROLLERS - 1;
		controllers[n].vibration.wRightMotorSpeed = speed;
		controllers[n].vibrateTimeRight = sec;
	}

};
#endif