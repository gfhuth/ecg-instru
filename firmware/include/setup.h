#ifndef SETUP_H
#define SETUP_H
#include <stdint.h>
#include <time.h>

extern const char* ssid;
extern const char* password;

const int ecgPin = 35;
const uint16_t sampleRateHz = 200;
const uint16_t bufferSize = 1000;

const uint16_t peakThreshold = 2200;
const uint32_t refractoryMs = 250;

struct BeatInfo { float bpm; float timestamp; };
const uint16_t maxBeats = 1024;

void setup();

#endif