#include "sampling.h"

volatile uint16_t ecgBuffer[bufferSize];
volatile uint16_t bufferIndex = 0;
Ticker sampler;
volatile uint32_t lastPeakMillis = 0;
volatile float currentBPM = 0;
volatile BeatInfo beatLog[maxBeats];
volatile uint16_t beatCount = 0;
volatile bool recording = false;
uint32_t startTimeMillis = 0;

void IRAM_ATTR sampleISR() {
  uint16_t value = analogRead(ecgPin);
  uint32_t now = millis();

  ecgBuffer[bufferIndex] = value;
  bufferIndex = (bufferIndex + 1) % bufferSize;

  static bool above = false;
  if (value > peakThreshold && !above) {
    if (now - lastPeakMillis > refractoryMs) {
      if (lastPeakMillis != 0) {
        float period = (now - lastPeakMillis) / 1000.0;
        currentBPM = 60.0 / period;
        if (recording && beatCount < maxBeats) {
          beatLog[beatCount].bpm = currentBPM;
          beatLog[beatCount].timestamp = time(nullptr);
          beatCount++;
        }
      }
      lastPeakMillis = now;
    }
    above = true;
  } else if (value < peakThreshold) {
    above = false;
  }
}

void setupSampling() {
  analogReadResolution(12);
  sampler.attach_ms(1000 / sampleRateHz, sampleISR);
}