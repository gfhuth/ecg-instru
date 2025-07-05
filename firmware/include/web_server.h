#ifndef WEB_SERVER_H
#define WEB_SERVER_H

// Funções do servidor web
void setupWiFi();
void setupWebServer();
void handleWebServer();
void addWebSample(uint16_t adcValue, uint32_t bufferIndex, uint32_t timestamp);

#endif 