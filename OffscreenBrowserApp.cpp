/**
 * Offscreen browser app class for handling the browser process
 * Simple example app using Chromium Embedded Framework
 * Lassi Maksimainen, 2014
 */

#include <iostream>
#include <string>
#include "OffscreenBrowserApp.h"

// Constructor
OffscreenBrowserApp::OffscreenBrowserApp() : CefApp() {
}

// Destructor
OffscreenBrowserApp::~OffscreenBrowserApp() {
}

// For passing messages to UI thread
bool OffscreenBrowserApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
	return true;
}

// Called after the browser process thread
void OffscreenBrowserApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
}
