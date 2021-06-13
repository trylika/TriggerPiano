#ifndef main_h
#define main_h

#include <Arduino.h>
#include <NewPing.h>

#define MAX_DISTANCE 150 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define SONAR_NUM 3

#define MIDI_DISTANCE_START 10
#define MIDI_DISTANCE_END 100

#define MIDI_VALUE_MIN 0
#define MIDI_VALUE_MAX 127

NewPing sonar[SONAR_NUM] = {
    NewPing(34 /* TRIGGER */, 35 /* ECHO */, MAX_DISTANCE),
    NewPing(32 /* TRIGGER */, 33 /* ECHO */, MAX_DISTANCE),
    NewPing(25 /* TRIGGER */, 26 /* ECHO */, MAX_DISTANCE),
};

int16_t sonarDistance[SONAR_NUM];
int16_t sonarDistanceTotal[SONAR_NUM];
int16_t sonarDistanceAvg[SONAR_NUM][10];
int16_t sonarDistanceAvgIndex[SONAR_NUM];

int16_t sonarMidi[SONAR_NUM];
bool sonarMidiOnOff[SONAR_NUM] = {
    false,
    false,
    false,
};

void setup();
void loop();

#endif
