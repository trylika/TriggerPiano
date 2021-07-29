#ifndef main_h
#define main_h

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#include <AppleMIDI.h>

#define SerialMon Serial
#define APPLEMIDI_DEBUG SerialMon

#define SONAR_ROUNDTRIP_CM_US 57
#define SONAR_MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define SONAR_MAX_WAIT (SONAR_MAX_DISTANCE + 1) * SONAR_ROUNDTRIP_CM_US // Maximum uS to wait for sensor response. (SONAR_MAX_DISTANCE + 1) * SONAR_ROUNDTRIP_CM_US
#define SONAR_PING_OVERHEAD 2
#define SONAR_SENSOR_DELAY 5800 // uS delay for sensor to start responding
#define SONAR_NUM 7
#define SONAR_TRIGGER 32

#define MIDI_DISTANCE_START 40 * SONAR_ROUNDTRIP_CM_US // Raw values we care about start from this value. CM * SONAR_ROUNDTRIP_CM_US
#define MIDI_DISTANCE_END 150 * SONAR_ROUNDTRIP_CM_US // Raw values we care about end with this value. CM * SONAR_ROUNDTRIP_CM_US

#define MIDI_VALUE_MIN 0
#define MIDI_VALUE_MAX 127

uint8_t sonar[SONAR_NUM] = {
    35, // 1
    33, // 3
    26, // 2
    34, // 6
    25, // 7
    27, // 8
    23, // 4
    // 5, // 5
};
bool sonarStarted[SONAR_NUM];
uint32_t sonarDistanceNew[SONAR_NUM];
uint32_t sonarDistance[SONAR_NUM];
uint32_t sonarDistanceTotal[SONAR_NUM];
uint32_t sonarDistanceAvg[SONAR_NUM][50];
uint8_t sonarDistanceAvgIndex[SONAR_NUM];

int16_t sonarMidi[SONAR_NUM];
bool sonarMidiOnOff[SONAR_NUM];

char ssid[] = "";
char pass[] = "";
int8_t isConnected = 0;

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
