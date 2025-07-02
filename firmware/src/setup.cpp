#include <WiFi.h>
#include "setup.h"
#include "sampling.h"
#include "handlers.h"

const char* ssid = "h";
const char* password = "123456789";

void setup() {
  Serial.begin(115200);
  setupWiFi();
  setupSampling();
  setupHttpRoutes();
  server.begin();
  Serial.println("Servidor iniciado");
}