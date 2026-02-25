#include "TimerEvent.h"
#include <Arduino.h>

// LED pins (Arduino Uno built-in LED and pin 12)
#define LED_PIN 13
#define LED2_PIN 12

// Task functions - your own functions you wanna call periodically
void blinkLed1() {
    static bool state = false;
    state = !state;
    digitalWrite(LED_PIN, state);
}

void blinkLed2() {
    static bool state = false;
    state = !state;
    digitalWrite(LED2_PIN, state);
}

void serialTask() {
    Serial.println("Task running every 2 seconds");
}

// Create timer instances with different periods
TimerEvent timer1(blinkLed1, 500000);    // 500ms - LED1 blink
TimerEvent timer2(blinkLed2, 1000000);   // 1000ms - LED2 blink  
TimerEvent timer3(serialTask, 2000000);  // 2000ms - Serial output

void setup() {
    // Initialize serial communication
    Serial.begin(9600);
    while (!Serial) delay(10);
    
    // Set LED pins as outputs
    pinMode(LED_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    
    // Start all timers
    timer1(RUN);
    timer2(RUN);
    timer3(RUN);
    
    Serial.println("TimerEvent system started");
    Serial.println("LED1 (pin 13) blinks every 500ms");
    Serial.println("LED2 (pin 12) blinks every 1000ms");
    Serial.println("Serial message every 2000ms");
}

void loop() {
    // Everything is handled by interrupts - no processTimers() needed!
    // Your main code can focus on other tasks
    delay(10); // Small delay to prevent overwhelming serial monitor
}