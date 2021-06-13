#include "main.h"

void setup() {
    memset(sonarDistance, 0, sizeof(sonarDistance));
    memset(sonarDistanceTotal, 0, sizeof(sonarDistanceTotal));
    memset(sonarDistanceAvg, 0, sizeof(sonarDistanceAvg));
    memset(sonarDistanceAvgIndex, 0, sizeof(sonarDistanceAvgIndex));

    memset(sonarMidi, 0, sizeof(sonarMidi));

    Serial.begin(115200);
}

void loop() {
    // Moving avarage
    for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through each sensor and display results.
        delay(50); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.

        int16_t pingResult = sonar[i].ping_cm();
        sonarDistanceTotal[i] -= sonarDistanceAvg[i][sonarDistanceAvgIndex[i]];
        sonarDistanceAvg[i][sonarDistanceAvgIndex[i]] = pingResult;
        sonarDistanceTotal[i] += pingResult;

        sonarDistanceAvgIndex[i] = (sonarDistanceAvgIndex[i] + 1) % 10;

        sonarDistance[i] = sonarDistanceTotal[i] / 10.0f;
    }

    // Convert to MIDI values
    for (uint8_t i = 0; i < SONAR_NUM; i++) {
        if (sonarDistance[i] >= MIDI_DISTANCE_START && sonarDistance[i] <= MIDI_DISTANCE_END) {
            sonarMidi[i] = map(
                constrain(sonarDistance[i], MIDI_DISTANCE_START, MIDI_DISTANCE_END),
                MIDI_DISTANCE_START,
                MIDI_DISTANCE_END,
                MIDI_VALUE_MIN,
                MIDI_VALUE_MAX
            );
            if (sonarMidiOnOff[i] == false) {
                sonarMidiOnOff[i] = true;
                // New input detected
            }
        } else {
            sonarMidi[i] = 0;
            if (sonarMidiOnOff[i] == true) {
                sonarMidiOnOff[i] = false;
                // Input lost
            }
        }
    }

    // Process MIDI here

    // Debug
    Serial.print("Sensors: ");
    for (uint8_t i = 0; i < SONAR_NUM; i++) {
        Serial.print(i); Serial.print(" (");

        Serial.print("Distance: "); Serial.print(sonarDistance[i]); Serial.print("cm ");
        Serial.print("MIDI: "); Serial.print(sonarMidi[i]); Serial.print(" ");
        Serial.print("MIDI on/off: "); Serial.print(sonarMidiOnOff[i]);

        Serial.print(") ");
    }
    Serial.println();
}
