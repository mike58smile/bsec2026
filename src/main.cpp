#include "TimerEvent.h"
#include <Arduino.h>
#include "power_sensor.h"

CurrentVoltageSensor sensor(0x40);  // I2C address 0x40


// LED pins (Arduino Uno built-in LED and pin 12)
#define LED_PIN 13
#define LED2_PIN 12


void setup() {
    // Initialize serial communication
    Serial.begin(9600);
    Serial.println("Starting INA226 example...");

    Wire.begin();
    if (!sensor.initialize()) {
        Serial.println("could not connect. Fix and Reboot");
    }

    // Print CSV header
    Serial.println("BUS,SHUNT,CURRENT,POWER");
}


void loop()
{
    sensor.update();
    
    Serial.print(sensor.get_voltage(), 3);
    Serial.print(",");
    Serial.print(sensor.get_current(), 3);
    Serial.print(",");
    Serial.print(sensor.get_power_mW(), 3);
    Serial.println();
    delay(100);
}