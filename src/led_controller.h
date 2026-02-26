#pragma once

#include <Arduino.h>

class RGBLED_Controller {
public:
    RGBLED_Controller(uint8_t rPin, uint8_t gPin, uint8_t bPin) 
        : _rPin(rPin), _gPin(gPin), _bPin(bPin) {}
    
    void begin() {
        pinMode(_rPin, OUTPUT);
        pinMode(_gPin, OUTPUT);
        pinMode(_bPin, OUTPUT);
        off();
    }   

    void setColor(byte r, byte g, byte b) {
        analogWrite(_rPin, r);
        analogWrite(_gPin, g);
        analogWrite(_bPin, b);
    }

    void off() {
        digitalWrite(_rPin, LOW);
        digitalWrite(_gPin, LOW);
        digitalWrite(_bPin, LOW);
    }

    void set(byte r, byte g, byte b) {
        setColor(r, g, b);
    }


private:
    uint8_t _rPin;
    uint8_t _gPin;
    uint8_t _bPin;
};