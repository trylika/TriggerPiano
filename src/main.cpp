#include "main.h"

APPLEMIDI_CREATE_DEFAULTSESSION_INSTANCE();

void setup() {
    memset(sonarStarted, 0, sizeof(sonarStarted));
    memset(sonarDistanceNew, 0, sizeof(sonarDistance));
    memset(sonarDistance, 0, sizeof(sonarDistance));
    memset(sonarDistanceTotal, 0, sizeof(sonarDistanceTotal));
    memset(sonarDistanceAvg, 0, sizeof(sonarDistanceAvg));
    memset(sonarDistanceAvgIndex, 0, sizeof(sonarDistanceAvgIndex));    

    memset(sonarMidi, 0, sizeof(sonarMidi));
    memset(sonarMidiOnOff, 0, sizeof(sonarMidiOnOff));

    pinMode(SONAR_TRIGGER, OUTPUT);
    for (uint8_t i = 0; i < SONAR_NUM; i++) {
        pinMode(sonar[i], INPUT);
    }

    Serial.begin(115200);

    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Establishing connection to WiFi..");
    }

    Serial.println("Connected to network");
    Serial.println(WiFi.localIP());
    DBG(F("OK, now make sure you an rtpMIDI session that is Enabled"));
    DBG(F("Add device named Arduino with Host"), WiFi.localIP(), "Port", AppleMIDI.getPort(), "(Name", AppleMIDI.getName(), ")");
    DBG(F("Select and then press the Connect button"));
    DBG(F("Then open a MIDI listener and monitor incoming notes"));
    DBG(F("Listen to incoming MIDI commands"));

    MIDI.begin();

    AppleMIDI.setHandleConnected([](const APPLEMIDI_NAMESPACE::ssrc_t & ssrc, const char* name) {
        isConnected++;
        DBG(F("Connected to session"), ssrc, name);
    });

    AppleMIDI.setHandleDisconnected([](const APPLEMIDI_NAMESPACE::ssrc_t & ssrc) {
        isConnected--;
        DBG(F("Disconnected"), ssrc);
    });
  
    MIDI.setHandleNoteOn([](byte channel, byte note, byte velocity) {
        DBG(F("NoteOn"), note);
    });

    MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {
        DBG(F("NoteOff"), note);
    });

    DBG(F("Sending NoteOn/Off of note 45, every second"));
}

void loop() {
    // Just delay to slow down while inside
    //delay(50); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.

    MIDI.read();

    readSonars();

    calculateMiDi();

    // Process MIDI here

    //printDebug();
     printPlotter();
}

void calculateMiDi() {
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
            if (sonarMidiOnOff[i] == false) { // New input detected
                sonarMidiOnOff[i] = true;
                // Event On
                MIDI.sendNoteOn(60+i, 100, 1);
            }
        } else {
            sonarMidi[i] = 0;
            if (sonarMidiOnOff[i] == true) { // Input lost
                sonarMidiOnOff[i] = false;
                // Event Off
                MIDI.sendNoteOff(60+i, 100, 1);
            }
        }
    }
}

void triggerAll() {
    digitalWrite(SONAR_TRIGGER, LOW); // Set the trigger pin low, should already be low, but this will make sure it is.
    delayMicroseconds(4); // Wait for pin to go low.
    digitalWrite(SONAR_TRIGGER, HIGH); // Set trigger pin high, this tells the sensor to send out a ping.
    delayMicroseconds(10); // Wait long enough for the sensor to realize the trigger pin is high. Sensor specs say to wait 10uS.
    digitalWrite(SONAR_TRIGGER, LOW); // Set trigger pin back to low.

    // Delay until at least one sensor start doing anything
    uint32_t startMaxDelay = micros() + SONAR_MAX_WAIT + SONAR_SENSOR_DELAY; // Maximum time we'll wait for ping to start (most sensors are <450uS, the SRF06 can take up to 34,300uS!)
    while (micros() < startMaxDelay) {
        for (uint8_t i = 0; i < SONAR_NUM; i++) {
            if (digitalRead(sonar[i]) == 1) {
                sonarStarted[i] = true;
                return;
            }
        }
    }
}

void readSonars() {
    bool finished = false;

    // Reset new values and read out statuses
    for (uint8_t i = 0; i < SONAR_NUM; i++) {
        sonarStarted[i] = false;
        sonarDistanceNew[i] = 0;
    }

    triggerAll();

    uint32_t startMicros = micros();
    uint32_t timeoutMicros = startMicros + SONAR_MAX_WAIT;

    while (micros() < timeoutMicros && finished == false) {
        finished = true;
        uint32_t currentTime = micros();
        for (uint8_t i = 0; i < SONAR_NUM; i++) {
            if (sonarDistanceNew[i] == 0) {
                int currentState = digitalRead(sonar[i]);
                if (currentState == 0 && sonarStarted[i] == true) { // If reading sensor data was started and just now finished
                    sonarDistanceNew[i] = currentTime - startMicros - SONAR_PING_OVERHEAD;
                } else if (currentState == 1 && sonarStarted[i] == false) { // If reading sensor data wasn't started and still ongoing
                    sonarStarted[i] = true;
                    finished = false;
                } else { // Any other case
                    finished = false;
                }
            }
        }
    }

    calculateMovingAverage();
}

void calculateMovingAverage() {
    for (uint8_t i = 0; i < SONAR_NUM; i++) {
        int16_t pingResult = sonarDistanceNew[i];
        sonarDistanceTotal[i] -= sonarDistanceAvg[i][sonarDistanceAvgIndex[i]];
        sonarDistanceAvg[i][sonarDistanceAvgIndex[i]] = pingResult;
        sonarDistanceTotal[i] += pingResult;

        sonarDistanceAvgIndex[i] = (sonarDistanceAvgIndex[i] + 1) % 50;

        sonarDistance[i] = sonarDistanceTotal[i] / 50.0f;
    }
}

void printDebug() {
    Serial.print("Sensors: ");
    for (uint8_t i = 0; i < SONAR_NUM; i++) {
        Serial.print(i); Serial.print(" (");

        Serial.print("RAW: "); Serial.print(sonarDistance[i]); Serial.print(" ");
        Serial.print("Distance: "); Serial.print(convertToCm(sonarDistance[i])); Serial.print("cm ");
        Serial.print("MIDI: "); Serial.print(sonarMidi[i]); Serial.print(" ");
        Serial.print("MIDI on/off: "); Serial.print(sonarMidiOnOff[i]);

        Serial.print(") ");
    }
    Serial.println();
}

void printPlotter() {
    for (uint8_t i = 0; i < SONAR_NUM; i++) {
        Serial.print(i); Serial.print(":");
        Serial.print(convertToCm(sonarDistance[i]));
        if (i < SONAR_NUM - 1) {
            Serial.print(", ");
        }
    }
    Serial.println();
}

uint32_t convertToCm(uint32_t us) {
    return us / SONAR_ROUNDTRIP_CM_US;
}
