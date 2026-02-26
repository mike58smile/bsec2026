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
        setColor(0, 0, 0);
    }

    void callback() {
        unsigned long now = millis();

        switch (_mode) {
            case Mode::STEADY:
                // Nothing to update
                break;
            case Mode::BLINK:
                if (_blink_interval_ms == 0) {
                    return;
                }
                if (now - _last_toggle_ms >= _blink_interval_ms) {
                    _last_toggle_ms = now;
                    _blink_on = !_blink_on;
                    if (_blink_on) {
                        setColor(_base_r, _base_g, _base_b);
                    } else {
                        off();
                    }
                }
                break;
            case Mode::ALTERNATE:
                if (_blink_interval_ms == 0) {
                    return;
                }
                if (now - _last_toggle_ms >= _blink_interval_ms) {
                    _last_toggle_ms = now;
                    _blink_on = !_blink_on;
                    if (_blink_on) {
                        setColor(_base_r, _base_g, _base_b);
                    } else {
                        setColor(_alt_r, _alt_g, _alt_b);
                    }
                }
                break;
            case Mode::BREATHE:
                if (_breath_period_ms == 0) {
                    off();
                    return;
                }
                updateBreathing(now);
                break;
        }
    }

    void setBlinking(byte r, byte g, byte b, unsigned long interval) {
        _mode = Mode::BLINK;
        _base_r = r;
        _base_g = g;
        _base_b = b;
        _blink_interval_ms = interval;
        _blink_on = true;
        _last_toggle_ms = millis();
        setColor(_base_r, _base_g, _base_b);
    }

    void breathing(byte r, byte g, byte b, unsigned long duration) {
        _mode = Mode::BREATHE;
        _base_r = r;
        _base_g = g;
        _base_b = b;
        _breath_period_ms = duration;
        _breath_start_ms = millis();
        updateBreathing(_breath_start_ms);
    }

    void setSteady(byte r, byte g, byte b) {
        _mode = Mode::STEADY;
        _base_r = r;
        _base_g = g;
        _base_b = b;
        setColor(_base_r, _base_g, _base_b);
    }

    void setAlternating(byte r1, byte g1, byte b1, byte r2, byte g2, byte b2, unsigned long interval) {
        _mode = Mode::ALTERNATE;
        _base_r = r1;
        _base_g = g1;
        _base_b = b1;
        _alt_r = r2;
        _alt_g = g2;
        _alt_b = b2;
        _blink_interval_ms = interval;
        _blink_on = true;
        _last_toggle_ms = millis();
        setColor(_base_r, _base_g, _base_b);
    }


private:
    enum class Mode : uint8_t {
        STEADY,
        BLINK,
        ALTERNATE,
        BREATHE,
    };

    void updateBreathing(unsigned long now) {
        unsigned long elapsed = now - _breath_start_ms;
        unsigned long period = _breath_period_ms;
        unsigned long half = period / 2;

        unsigned long phase = (period == 0) ? 0 : (elapsed % period);
        uint8_t brightness = 0;

        if (half == 0) {
            brightness = 255;
        } else if (phase <= half) {
            brightness = static_cast<uint8_t>((phase * 255UL) / half);
        } else {
            brightness = static_cast<uint8_t>(((period - phase) * 255UL) / half);
        }

        uint16_t r = (static_cast<uint16_t>(_base_r) * brightness) / 255;
        uint16_t g = (static_cast<uint16_t>(_base_g) * brightness) / 255;
        uint16_t b = (static_cast<uint16_t>(_base_b) * brightness) / 255;
        setColor(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
    }

    uint8_t _rPin;
    uint8_t _gPin;
    uint8_t _bPin;

    Mode _mode = Mode::STEADY;
    byte _base_r = 0;
    byte _base_g = 0;
    byte _base_b = 0;
    byte _alt_r = 0;
    byte _alt_g = 0;
    byte _alt_b = 0;

    unsigned long _blink_interval_ms = 0;
    unsigned long _last_toggle_ms = 0;
    bool _blink_on = false;

    unsigned long _breath_period_ms = 0;
    unsigned long _breath_start_ms = 0;
};