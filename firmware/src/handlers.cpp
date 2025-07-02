#include <ArduinoJson.h>
#include <time.h>
#include "handlers.h"
#include "setup.h"
#include "sampling.h"
#include "index_html.h"

WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

void handleECG() {
  const uint16_t chunk = 40;
  String json = "[";
  noInterrupts();
  int16_t i = (bufferIndex - chunk + bufferSize) % bufferSize;
  for (uint16_t c = 0; c < chunk; c++) {
    json += String(ecgBuffer[i]);
    if (c < chunk - 1) json += ",";
    i = (i + 1) % bufferSize;
  }
  interrupts();
  json += "]";
  server.send(200, "application/json", json);
}

void handleBPM() {
  JsonDocument doc;
  doc["bpm"] = currentBPM;
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

void handleStart() {
  String mode = server.arg("mode");
  if (mode == "start") {
    recording = true;
    beatCount = 0;
    startTimeMillis = millis();
  } else {
    recording = false;
  }
  server.send(200, "text/plain", "OK");
}

void handleDownload() {
  String csv = "bpm,timestamp_epoch_s\n";
  for (uint16_t i = 0; i < beatCount; i++) {
    time_t absolute = (startTimeMillis / 1000) + beatLog[i].timestamp;
    uint32_t millisFrac = (uint32_t)((beatLog[i].timestamp - (uint32_t)beatLog[i].timestamp) * 1000);
    csv += String(beatLog[i].bpm, 2) + "," + String(absolute) + "." + String(millisFrac) + "\n";
  }
  server.sendHeader("Content-Disposition", "attachment; filename=\"ecg_bpm.csv\"");
  server.send(200, "text/csv", csv);
}

void syncTimeWithNTP() {
  configTime(0, 0, "pool.ntp.org");
  Serial.println("NTP sync requested.");
}

void handleSyncNTP() {
  syncTimeWithNTP();
  server.send(200, "text/plain", "NTP sync started");
}

void handleSyncBrowserTime() {
  if (server.hasArg("plain")) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    if (!error && doc["epoch"].is<time_t>()) {
      time_t t = doc["epoch"].as<time_t>();
      struct timeval tv = { .tv_sec = t, .tv_usec = 0 };
      settimeofday(&tv, nullptr);
      server.send(200, "text/plain", "Hora atualizada com navegador");
      return;
    }
  }
  server.send(400, "text/plain", "Erro ao processar hora");
}

void handleTime() {
  time_t now = time(nullptr);
  struct tm *t = localtime(&now);
  char buf[16];
  sprintf(buf, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
  server.send(200, "text/plain", buf);
}

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando à rede ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.print("\nIP: "); Serial.println(WiFi.localIP());
}

void setupHttpRoutes() {
  server.on("/", handleRoot);
  server.on("/ecg", handleECG);
  server.on("/bpm", handleBPM);
  server.on("/start", handleStart);
  server.on("/download", handleDownload);
  server.on("/syncNTP", handleSyncNTP);
  server.on("/syncBrowserTime", HTTP_POST, handleSyncBrowserTime);
  server.on("/time", handleTime);
}