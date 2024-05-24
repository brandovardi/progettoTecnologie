#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>

#pragma comment(lib, "user32.lib") // user32.dll windows' library

HHOOK g_hook;
HHOOK mouseHook;

// Keyboard hook
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
	{
		KBDLLHOOKSTRUCT* pKeyboardStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

		bool isShiftPressed = ((GetKeyState(VK_LSHIFT) & 0x8000) != 0) || ((GetKeyState(VK_RSHIFT) & 0x8000) != 0);
		bool isCapsLockActive = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;

		bool isCtrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
		bool isAltPressed = (GetKeyState(VK_MENU) & 0x8000) != 0;

		int vkCode = pKeyboardStruct->vkCode;

		MessageBox(NULL, (LPCWSTR)(char)vkCode, L"Titolo", 1);

		std::cout << vkCode << std::endl;

	}
	// return control to the Operating System
	return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

// main function
int main()
{
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0);

	if (g_hook == nullptr) return EXIT_FAILURE;

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(g_hook);

	return ERROR_SUCCESS;
}
