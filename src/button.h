#pragma once
#include <Arduino.h>

class Button {
public:
    Button(uint8_t pin, uint16_t debounce_ms = 50, bool pull_up = true)
        : pin_(pin)
        , debounce_delay_(debounce_ms)
        , use_pullup_(pull_up)
        , last_stable_state_(HIGH)
        , last_reading_(HIGH)
        , last_debounce_time_(0)
        , press_detected_(false) {}

    void begin() {
        pinMode(pin_, use_pullup_ ? INPUT_PULLUP : INPUT);
        last_stable_state_ = digitalRead(pin_);
        last_reading_ = last_stable_state_;
    }

    void update() {
        uint8_t reading = digitalRead(pin_);
        unsigned long now = millis();

        // Reset debounce timer if reading changed
        if (reading != last_reading_) {
            last_debounce_time_ = now;
        }

        // Check if enough time has passed for stable reading
        if ((now - last_debounce_time_) > debounce_delay_) {
            // Detect falling edge (button press with pull-up)
            if (last_stable_state_ == HIGH && reading == LOW) {
                press_detected_ = true;
            }
            last_stable_state_ = reading;
        }

        last_reading_ = reading;
    }

    bool was_pressed() {
        if (press_detected_) {
            press_detected_ = false;  // Clear flag
            return true;
        }
        return false;
    }

    bool is_pressed() const {
        return last_stable_state_ == LOW;
    }

private:
    uint8_t pin_;
    uint16_t debounce_delay_;
    bool use_pullup_;
    uint8_t last_stable_state_;
    uint8_t last_reading_;
    unsigned long last_debounce_time_;
    bool press_detected_;
};
