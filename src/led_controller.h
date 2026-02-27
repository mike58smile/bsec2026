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

    void policeFlash(unsigned long currentMillis) {
        static unsigned long lastFlash = 0;
        static bool isRed = true;
        
        if (currentMillis - lastFlash >= 200) {
            if (isRed) {
                setColor(255, 0, 0);  // Red
            } else {
                setColor(0, 0, 255);  // Blue
            }
            isRed = !isRed;
            lastFlash = currentMillis;
        }
    }

    void breatheRed(unsigned long currentMillis) {
        static unsigned long lastUpdate = 0;
        static int brightness = 0;
        static int direction = 1;
        
        if (currentMillis - lastUpdate >= 20) {  // Update every 20ms for smooth breathing
            brightness += direction * 2;
            if (brightness >= 125) {
                brightness = 125;
                direction = -1;
            } else if (brightness <= 0) {
                brightness = 0;
                direction = 1;
            }
            setColor(brightness, 0, 0);
            lastUpdate = currentMillis;
        }
    }

private:
    uint8_t _rPin;
    uint8_t _gPin;
    uint8_t _bPin;
};