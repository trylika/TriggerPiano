#ifndef main_h
#define main_h

#include <Arduino.h>

#define SONAR_ROUNDTRIP_CM_US 57
#define SONAR_MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define SONAR_MAX_WAIT (SONAR_MAX_DISTANCE + 1) * SONAR_ROUNDTRIP_CM_US // Maximum uS to wait for sensor response. (SONAR_MAX_DISTANCE + 1) * SONAR_ROUNDTRIP_CM_US
#define SONAR_PING_OVERHEAD 2
#define SONAR_SENSOR_DELAY 5800 // uS delay for sensor to start responding
#define SONAR_NUM 3
#define SONAR_TRIGGER 32

#define MIDI_DISTANCE_START 10 * SONAR_ROUNDTRIP_CM_US // Raw values we care about start from this value. CM * SONAR_ROUNDTRIP_CM_US
#define MIDI_DISTANCE_END 200 * SONAR_ROUNDTRIP_CM_US // Raw values we care about end with this value. CM * SONAR_ROUNDTRIP_CM_US

#define MIDI_VALUE_MIN 0
#define MIDI_VALUE_MAX 127

uint8_t sonar[SONAR_NUM] = {
    35,
    33,
    26,
};
bool sonarStarted[SONAR_NUM];
uint32_t sonarDistanceNew[SONAR_NUM];
uint32_t sonarDistance[SONAR_NUM];
uint32_t sonarDistanceTotal[SONAR_NUM];
uint32_t sonarDistanceAvg[SONAR_NUM][10];
uint8_t sonarDistanceAvgIndex[SONAR_NUM];

int16_t sonarMidi[SONAR_NUM];
bool sonarMidiOnOff[SONAR_NUM];

void setup();
void loop();
void triggerAll();
void readSonars();
void calculateMovingAverage();
void calculateMiDi();
void printDebug();
void printPlotter();
uint32_t convertToCm(uint32_t us);

#endif
