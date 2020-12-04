#include <Windows.h>
#include <stdio.h>

BOOL flag[3] = { 0 }, keyPressed = FALSE;

VOID RotateScreen();
LRESULT CALLBACK CheckkeySequence(int code, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HMODULE current, HMODULE previous, LPSTR cmd, INT show)
{
	HHOOK KeyboardHook;
	BOOL res;
	MSG msg;

	KeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, CheckkeySequence, GetModuleHandle(NULL), 0);
	while ((res = GetMessage(&msg, NULL, 0, 0))) {
		if (res != -1) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}

LRESULT CALLBACK CheckkeySequence(int code, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT *hookStruct = (KBDLLHOOKSTRUCT *)lParam;

	if (wParam == WM_KEYDOWN) {
		if (hookStruct->vkCode == VK_CONTROL || hookStruct->vkCode == VK_LCONTROL || hookStruct->vkCode == VK_RCONTROL) {
			flag[0] = 1;
			keyPressed = TRUE;
		}
		if (hookStruct->vkCode == VK_SHIFT || hookStruct->vkCode == VK_LSHIFT || hookStruct->vkCode == VK_RSHIFT) {
			if (keyPressed)
				flag[1] = 1;
			else {
				keyPressed = FALSE;
				return CallNextHookEx(NULL, code, wParam, lParam);
			}
		}
		if (hookStruct->vkCode == 0x39) {
			flag[2] = 1;
			if (keyPressed) {
				for (int i = 0; i < 3; i++) {
					if(flag[i] != 1)
						return CallNextHookEx(NULL, code, wParam, lParam);
					flag[i] = 0;
				}
				RotateScreen();
			}
		}
	}
	if (wParam == WM_KEYUP) {
		if (hookStruct->vkCode == VK_CONTROL || hookStruct->vkCode == VK_LCONTROL || hookStruct->vkCode == VK_RCONTROL) {
			flag[0] = 0;
			keyPressed = FALSE;
		}
		else if(hookStruct->vkCode == VK_SHIFT || hookStruct->vkCode == VK_LSHIFT || hookStruct->vkCode == VK_RSHIFT) {
			flag[1] = 0;
			keyPressed = FALSE;
		}
	}

	return CallNextHookEx(NULL, code, wParam, lParam);
}

VOID RotateScreen() 
{
	DEVMODE devMode;

	memset(&devMode, 0, sizeof(DEVMODE));
	devMode.dmSize = sizeof(DEVMODE);

	if (EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &devMode) != 0) 
	{
		DWORD temp = devMode.dmPelsHeight;
		devMode.dmPelsHeight = devMode.dmPelsWidth;
		devMode.dmPelsWidth = temp;

		switch (devMode.dmDisplayOrientation)
		{
		case DMDO_DEFAULT:
			devMode.dmDisplayOrientation = DMDO_270;
			break;
		case DMDO_270:
			devMode.dmDisplayOrientation = DMDO_180;
			break;
		case DMDO_180:
			devMode.dmDisplayOrientation = DMDO_90;
			break;
		case DMDO_90:
			devMode.dmDisplayOrientation = DMDO_DEFAULT;
			break;
		default:
			break;
		}

		ChangeDisplaySettingsA(&devMode, 0);
	}
}