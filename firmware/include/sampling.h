#ifndef SAMPLING_H
#define SAMPLING_H

#include <stdint.h>

// Declarações das variáveis globais
extern volatile uint16_t adcBuffer[];
extern volatile uint32_t bufferInicio;
extern volatile uint32_t bufferFim;
extern volatile bool bufferOverflow;

// Declarações das funções
void setupADC();
void setupDMA();
void startSampling();
void stopSampling();
void consumerTask();

// Callbacks (declarados como IRAM_ATTR para melhor performance)
void IRAM_ATTR samplingTimerCallback(void* arg);

#endif 