/**
 * Offscreen browser client class for handling the requests from app side
 * Simple example app using Chromium Embedded Framework
 * Lassi Maksimainen, 2014
 */

#include <iostream>
#include <unistd.h>
#include "include/cef_app.h"
#include "OffscreenBrowserClient.h"

// Constructor
OffscreenBrowserClient::OffscreenBrowserClient() {
	visitActive = false;
	pageHtml = "";
}

// Destructor
OffscreenBrowserClient::~OffscreenBrowserClient() {
}

// Fetches HTML source synchronically
std::string OffscreenBrowserClient::GetHTMLSource() {
	visitActive = false;
	pageHtml = "";
	mBrowser->GetMainFrame()->GetSource(this);

	while (!visitActive) {
		usleep(10000);
		CefDoMessageLoopWork();
	}
	std::cout << "visit done" << std::endl;

	return pageHtml;
}

// Called after the browser client is succesfully created
void OffscreenBrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	mBrowser = browser;
}

// Called before the browser is shutdown
void OffscreenBrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	mBrowser = nullptr;
}

// Called everytime there is something to paint to a "page". This is ignored, for drawing to e.g. offscreen bitmap, implement the logic here
void OffscreenBrowserClient::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height) {
}

// Called when access to HTML source is given on the UI thread side
void OffscreenBrowserClient::Visit(const CefString& html) {
	pageHtml = html.ToString();
	visitActive = true;
}
