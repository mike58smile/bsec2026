#include "TimerEvent.h"
#include <Arduino.h>
#include <INA226.h>

INA226 INA(0x40);


// LED pins (Arduino Uno built-in LED and pin 12)
#define LED_PIN 13
#define LED2_PIN 12


void setup() {
    // Initialize serial communication
    Serial.begin(9600);

    Wire.begin();
    if (!INA.begin() )
    {
        Serial.println("could not connect. Fix and Reboot");
    }
    INA.setMaxCurrentShunt(1, 0.002);

    // Print CSV header
    Serial.println("BUS,SHUNT,CURRENT,POWER");
}


void loop()
{
    Serial.print(INA.getBusVoltage(), 3);
    Serial.print(",");
    Serial.print(INA.getShuntVoltage_mV(), 3);
    Serial.print(",");
    Serial.print(INA.getCurrent_mA(), 3);
    Serial.print(",");
    Serial.print(INA.getPower_mW(), 3);
    Serial.println();
    delay(100);
}