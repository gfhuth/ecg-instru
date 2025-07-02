#include <WiFi.h>
#include <WebServer.h>
#include <Ticker.h>
#include <time.h>
#include <ArduinoJson.h>
#include "html.h"

const char* ssid = "h";
const char* password = "123456789";

/* ---------- Globals ---------- */
WebServer server(80);

const int ecgPin = 35;
const uint16_t sampleRateHz = 200;
const uint16_t bufferSize = 1000;

volatile uint16_t ecgBuffer[bufferSize];
volatile uint16_t bufferIndex = 0;
Ticker sampler;

/* Peak detection */
const uint16_t peakThreshold = 2200;     // ajuste conforme sinal
const uint32_t refractoryMs = 250;       // 0,25 s = máx 240 BPM
volatile uint32_t lastPeakMillis = 0;
volatile float currentBPM = 0;

/* Log */
struct BeatInfo { float bpm; float timestamp; };
const uint16_t maxBeats = 1024;
BeatInfo beatLog[maxBeats];
volatile uint16_t beatCount = 0;
volatile bool recording = false;
uint32_t startTimeMillis = 0;

/* ---------- Sampling ISR ---------- */
void IRAM_ATTR sampleISR() {
  uint16_t value = analogRead(ecgPin);
  uint32_t now = millis();

  ecgBuffer[bufferIndex] = value;
  bufferIndex = (bufferIndex + 1) % bufferSize;

  /* Peak‑detect: sobe > limiar, respeita refratário */
  static bool above = false;
  if (value > peakThreshold && !above) {
    if (now - lastPeakMillis > refractoryMs) {
      if (lastPeakMillis != 0) {
        float period = (now - lastPeakMillis) / 1000.0; // s
        currentBPM = 60.0 / period;

        if (recording && beatCount < maxBeats) {
          beatLog[beatCount].bpm = currentBPM;
          beatLog[beatCount].timestamp = time(nullptr);          beatCount++;
        }
      }
      lastPeakMillis = now;
    }
    above = true;
  } else if (value < peakThreshold) {
    above = false;
  }
}

/* ---------- HTTP handlers ---------- */
void handleRoot()        { server.send(200, "text/html", MAIN_page); }

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

void handleBPM(){ 
  StaticJsonDocument<64> doc;
  doc["bpm"] = currentBPM;
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

void handleStart() {
  String mode = server.arg("mode"); // start/stop
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
    DynamicJsonDocument doc(128);
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    if (!error && doc.containsKey("epoch")) {
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

/* ---------- Wi‑Fi ---------- */
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

/* ---------- Arduino ---------- */
void setup() {
  Serial.begin(115200);
  // Conecta Wi‑Fi uma única vez
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando à rede "); Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print('.'); }
  Serial.print("\nIP: "); Serial.println(WiFi.localIP());

  analogReadResolution(12);

  // Inicia amostragem (200 Hz)
  sampler.attach_ms(1000 / sampleRateHz, sampleISR);

  // Rotas HTTP
  server.on("/",               handleRoot);
  server.on("/ecg",            handleECG);
  server.on("/bpm",            handleBPM);
  server.on("/start",          handleStart);
  server.on("/download",       handleDownload);
  server.on("/syncNTP",        handleSyncNTP);
  server.on("/syncBrowserTime",HTTP_POST, handleSyncBrowserTime);
  server.on("/time",           handleTime);
  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() { server.handleClient(); }
