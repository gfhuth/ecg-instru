#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "setup.h"
#include "wifi_config.h"
#include "web_page.h"
#include "sampling.h"

WebServer server(80);

// Buffer para armazenar amostras para envio via web
struct WebSample {
    uint16_t adcValue;
    uint32_t bufferIndex;
    uint32_t timestamp;
};

#define WEB_BUFFER_SIZE 50
WebSample webBuffer[WEB_BUFFER_SIZE];
volatile uint32_t webBufferIndex = 0;
volatile uint32_t webBufferCount = 0;

/**
 * Configura a conexão WiFi
 */
void setupWiFi() {
    Serial.println("Conectando ao WiFi...");
    Serial.printf("SSID: %s\n", ssid);
    
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println();
    Serial.println("WiFi conectado!");
    Serial.printf("Endereço IP: %s\n", WiFi.localIP().toString().c_str());
}

/**
 * Handler para a página principal
 */
void handleRoot() {
    server.send(200, "text/html", WEB_PAGE);
}

/**
 * Handler para enviar dados via JSON
 */
void handleData() {
    JsonDocument doc;
    JsonArray samplesArray = doc.createNestedArray("samples");
    
    // Copia dados do buffer web para o JSON
    noInterrupts();
    uint32_t count = webBufferCount;
    uint32_t startIndex = (webBufferIndex - count + WEB_BUFFER_SIZE) % WEB_BUFFER_SIZE;
    interrupts();
    
    for (uint32_t i = 0; i < count; i++) {
        uint32_t index = (startIndex + i) % WEB_BUFFER_SIZE;
        JsonObject sample = samplesArray.createNestedObject();
        sample["adcValue"] = webBuffer[index].adcValue;
        sample["bufferIndex"] = webBuffer[index].bufferIndex;
        sample["timestamp"] = webBuffer[index].timestamp;
    }
    
    // Limpa o buffer após enviar
    noInterrupts();
    webBufferCount = 0;
    interrupts();
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

/**
 * Configura as rotas do servidor web
 */
void setupWebServer() {
    server.on("/", handleRoot);
    server.on("/data", handleData);
    
    server.begin();
    Serial.println("Servidor web iniciado");
}

/**
 * Adiciona uma amostra ao buffer web
 */
void addWebSample(uint16_t adcValue, uint32_t bufferIndex, uint32_t timestamp) {
    noInterrupts();
    
    webBuffer[webBufferIndex].adcValue = adcValue;
    webBuffer[webBufferIndex].bufferIndex = bufferIndex;
    webBuffer[webBufferIndex].timestamp = timestamp;
    
    webBufferIndex = (webBufferIndex + 1) % WEB_BUFFER_SIZE;
    
    if (webBufferCount < WEB_BUFFER_SIZE) {
        webBufferCount++;
    }
    
    interrupts();
}

/**
 * Processa requisições do servidor web
 */
void handleWebServer() {
    server.handleClient();
} 