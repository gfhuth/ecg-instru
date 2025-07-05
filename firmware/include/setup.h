#ifndef SETUP_H
#define SETUP_H

#include <stdint.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "sampling.h"

// Configurações do ADC
const int adcPin = 35;           // Pino de entrada analógica
const int adcChannel = 7;        // Canal ADC1_CH7 (pino 35)
const int adcUnit = 1;           // ADC1

// Configurações de amostragem
const uint32_t sampleRateHz = 200;        // Frequência de amostragem (configurável)
const uint32_t consumerRateMs = 25;       // Taxa de execução do consumidor (configurável)

// Cálculo do tamanho do buffer baseado na taxa de amostragem e taxa do consumidor
const uint32_t bufferSize = (sampleRateHz * consumerRateMs * (5/2)) / 1000;  // Amostras por ciclo do consumidor

// Configurações do DMA
const int dmaChan = 0;           // Canal DMA
const int dmaPriority = 1;       // Prioridade do DMA

// Estrutura para dados da amostra
struct SampleData {
    uint32_t timestamp;          // Timestamp em milissegundos
    uint16_t adcValue;           // Valor do ADC (12 bits)
    uint32_t bufferIndex;        // Índice no buffer
};

// Variáveis globais para controle da fila circular
extern volatile uint32_t bufferInicio;    // Índice de início da fila
extern volatile uint32_t bufferFim;       // Índice de fim da fila
extern volatile bool bufferOverflow;      // Flag de overflow

// Funções
void setup();
void loop();
void setupADC();
void setupDMA();
void startSampling();
void stopSampling();
void consumerTask();

#endif