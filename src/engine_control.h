#pragma once

#include <Arduino.h>
#include "pinout.h"

class MotorController {
public:

MotorController(uint8_t pwmPin = MOTOR_PWM_PIN) : _pwmPin(pwmPin) {}

void begin() {
    pinMode(_pwmPin, OUTPUT);
    digitalWrite(_pwmPin, LOW); // Ensure motor is off at start
}

void set_power(byte power) {
    analogWrite(_pwmPin, power);
}

void eco_power() {
    set_power(180);
}

void full_power() {
    set_power(255);
}

void stop() {
    digitalWrite(_pwmPin, LOW);

}

private:
    uint8_t _pwmPin;
};