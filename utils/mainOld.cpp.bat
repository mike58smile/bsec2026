#include "TimerEvent.h"
#include <Arduino.h>
#include "power_sensor.h"

CurrentVoltageSensor sensor(0x40);  // I2C address 0x40


void setup() {
    // Initialize serial communication
    Serial.begin(9600);

}


void loop()
{

}