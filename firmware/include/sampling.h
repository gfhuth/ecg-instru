#ifndef SAMPLING_H
#define SAMPLING_H
#include <Arduino.h>
#include <Ticker.h>
#include <esp_attr.h>
#include "setup.h"

extern volatile uint16_t ecgBuffer[bufferSize];
extern volatile uint16_t bufferIndex;
extern Ticker sampler;
extern volatile uint32_t lastPeakMillis;
extern volatile float currentBPM;
extern volatile BeatInfo beatLog[maxBeats];
extern volatile uint16_t beatCount;
extern volatile bool recording;
extern uint32_t startTimeMillis;

extern volatile uint16_t begin_index;


void IRAM_ATTR sampleISR();
void setupSampling();

#endif