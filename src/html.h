#include <WiFi.h>
#include <WebServer.h>
#include "SPIFFS.h"

extern const char* ssid;
extern const char* password;

extern WebServer server;
extern String html;

extern bool logSerial1;

void wifi_setup();
void spiffs_setup();
void upload_programm();
void handleRoot();
void handleToggleLogs();