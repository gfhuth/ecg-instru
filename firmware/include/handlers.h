#ifndef HANDLERS_H
#define HANDLERS_H
#include <WebServer.h>

extern WebServer server;

void setupWiFi();
void setupHttpRoutes();
void handleRoot();
void handleECG();
void handleBPM();
void handleStart();
void handleDownload();
void handleSyncNTP();
void handleSyncBrowserTime();
void handleTime();

#endif