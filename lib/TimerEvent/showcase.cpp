#include <Arduino.h>
#include <TimerEvent.h>

#define LED_PIN 13

void toggleLed() {
    static bool state = false;
    state = !state;
    digitalWrite(LED_PIN, state);
}

TimerEvent ledTimer(toggleLed, 500000);  // 500ms blink

void setup() {
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
    
    // Start timer
    ledTimer(RUN);
    
    Serial.println("LED blinker started!");
}

void loop() {
    // Main loop does other work while timer runs in background
    Serial.println("Main loop working...");
    delay(2000);
    
    // Stop timer after 6 seconds
    static unsigned long startTime = millis();
    if (millis() - startTime > 6000) {
        ledTimer(STOP);
        Serial.println("Timer stopped!");
        while(1); // End demo
    }
}
