/**
 * Simple example app using Chromium Embedded Framework
 * Lassi Maksimainen, 2014
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <memory>
#include "include/cef_app.h"
#include "OffscreenBrowserApp.h"
#include "OffscreenBrowserClient.h"

// Entrypoint to the whole application
int main(int argc, char** argv) {
	// Initialize application object
	CefRefPtr<OffscreenBrowserApp> app = new OffscreenBrowserApp();
	CefMainArgs main_args(argc, argv);
	int exit_code = CefExecuteProcess(main_args, app.get(), nullptr);
	if (exit_code >= 0) {
		return exit_code;
	}

	// Initialize the app without sandbox on for simplicity and cookies & page cache turned off
	CefSettings settings;
	settings.windowless_rendering_enabled = true;
	settings.single_process = true;
	settings.no_sandbox = true;
	settings.persist_session_cookies = false;
	CefString(&settings.user_agent).FromASCII("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/38.0.2125.104 Safari/537.36");
	CefString(&settings.cache_path).FromASCII("");
	CefInitialize(main_args, settings, app.get(), nullptr);

	// Create windowless browser host, disable also plugins that might be available on the system
	CefRefPtr<OffscreenBrowserClient> client = new OffscreenBrowserClient();
	CefWindowInfo info;
	info.SetAsWindowless(nullptr, false);
	CefBrowserSettings browser_settings;
	browser_settings.plugins = STATE_DISABLED;
	CefRefPtr<CefBrowser> browser = CefBrowserHost::CreateBrowserSync(info, client.get(), "http://www.google.com", browser_settings, NULL);
	while (!client->GetBrowser()) {
		usleep(10000);
	}

	std::cout << std::endl;

	// Parse command line parameters
	std::string url;
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::GetGlobalCommandLine();

	url = command_line->GetSwitchValue("url");
	if (url.empty()) {
		std::cout << "Use parameter --url to provide URL for the web page that will be fetched" << std::endl;
		return -1;
	}

	std::string dest;
	dest = command_line->GetSwitchValue("out");
	if (dest.empty()) {
		std::cout << "Use parameter --out to provide location of the file where the fetched web page's HTML code will be dumped" << std::endl;
		std::cout << "Use 'stdout' for dumping the code to terminal" << std::endl;
		return -1;
	}

	// Loading time is given in seconds
	int loadingTime = 100;
	std::string time = command_line->GetSwitchValue("time");
	if (!time.empty()) {
		loadingTime = atoi(time.c_str()) * 10;
	}

	// Start loading web page, wait for 10 seconds to ensure that javascript etc have been succesfully executed and then terminate load
	client->GetBrowser()->GetMainFrame()->LoadURL(url);
	std::cout << "Starting to load the page from URL: " << url << std::endl;
	std::cout << "Waiting for " << (loadingTime/10) << " secs to let everything load on the page" << std::endl;
	for (int i = 0; i < loadingTime; i++) {
		CefDoMessageLoopWork();
		usleep(100 * 1000);
	}
	client->GetBrowser()->StopLoad();
	CefDoMessageLoopWork();

	// Get HTML source
	std::string source = client->GetHTMLSource();

	// Dump source to terminal or to a file
	if (dest.compare("stdout") == 0) {
		std::cout << "==HTML-SOURCE-BEGIN==" << std::endl << std::flush;
		std::cout << source << std::endl << std::flush;
		std::cout << "==HTML-SOURCE-END==" << std::endl << std::flush;
	}
	else {
		std::cout << "starting to write page! " << std::endl;
		std::ofstream page;
		page.open(dest, std::ofstream::out);
		page << source;
		page.flush();
		page.close();
	}

	// Everything done, time to quit
	CefShutdown();
	return 0;
}
