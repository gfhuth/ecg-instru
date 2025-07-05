#include <Arduino.h>
#include "setup.h"
#include "web_server.h"

void setup() {
    // Inicializa a comunicação serial
    Serial.begin(115200);
    Serial.println("\n=== SISTEMA DE AQUISIÇÃO DE DADOS ANALÓGICOS ===");
    Serial.println("ESP32 com DMA, Fila Circular e Interface Web");
    Serial.println("================================================");
    
    // Imprime as configurações do sistema
    Serial.printf("Frequência de amostragem: %d Hz\n", sampleRateHz);
    Serial.printf("Taxa do consumidor: %d ms\n", consumerRateMs);
    Serial.printf("Tamanho do buffer: %d amostras\n", bufferSize);
    Serial.printf("Pino ADC: %d (Canal ADC1_CH7)\n", adcPin);
    Serial.println("================================================");
    
    // Configura o WiFi
    setupWiFi();
    
    // Configura o servidor web
    setupWebServer();
    
    // Configura o ADC
    setupADC();
    
    // Configura o DMA
    setupDMA();
    
    // Inicia a amostragem
    startSampling();
    
    Serial.println("Sistema inicializado com sucesso!");
    Serial.println("Acesse http://[IP_DO_ESP32] para ver os dados em tempo real");
    Serial.println("Aguardando dados...\n");
    
    // Imprime o cabeçalho da tabela de dados
    Serial.println("Timestamp(ms) | Valor ADC | Índice Buffer");
    Serial.println("----------------------------------------");
}