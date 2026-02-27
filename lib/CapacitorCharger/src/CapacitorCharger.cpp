#include "CapacitorCharger.h"

CapacitorCharger::CapacitorCharger(uint8_t capPin, uint8_t pwmPin, uint8_t i2cAddress)
    : _capPin(capPin), _pwmPin(pwmPin), _i2cAddress(i2cAddress),
      _maxVoltage(5.4), _currentLimit(0.5), _panelTargetVoltage(4.8), _pwmValue(0), _charging(false), _complete(false) {
    _sensor = new CurrentVoltageSensor(_i2cAddress);
}

bool CapacitorCharger::begin() {
    pinMode(_pwmPin, OUTPUT);
    analogWrite(_pwmPin, 0);
    
    if (!_sensor->initialize()) {
        return false;
    }
    
    reset();
    return true;
}

int CapacitorCharger::charge(float maxVoltage, float currentLimit, float panelVoltage) {
    _maxVoltage = maxVoltage;
    _currentLimit = currentLimit;
    _panelTargetVoltage = panelVoltage;  // MPPT target (80% of 6V = 4.8V)
    
    // Read current values
    readSensors();
    
    // Check if charging is complete
    if (getVoltage() >= _maxVoltage) {
        stop();
        _complete = true;
        return 1; // Complete
    }
    
    // Start charging if not already
    if (!_charging && !_complete) {
        _charging = true;
        _pwmValue = 150; // Start with high PWM for FULL BLAST charging
    }
    
    // Control PWM for MPPT + current limiting
    controlPwm();
    
    return 0; // Still charging
}

float CapacitorCharger::getVoltage() {
    return readCapacitorVoltage();
}

float CapacitorCharger::getCurrent() {
    return _sensor->get_current();
}

float CapacitorCharger::getPanelVoltage() {
    return _sensor->get_voltage();
}

int CapacitorCharger::getPwm() {
    return _pwmValue;
}

bool CapacitorCharger::isCharging() {
    return _charging;
}

bool CapacitorCharger::isComplete() {
    return _complete;
}

void CapacitorCharger::stop() {
    _charging = false;
    _pwmValue = 0;
    analogWrite(_pwmPin, 0);
}

void CapacitorCharger::reset() {
    stop();
    _complete = false;
    _maxVoltage = 5.4;
    _currentLimit = 0.5;
    _panelTargetVoltage = 4.8;
}

void CapacitorCharger::readSensors() {
    _sensor->update();
}

void CapacitorCharger::controlPwm() {
    if (!_charging || _complete) {
        return;
    }
    
    // TEMPORARY: FULL BLAST charging - bypass MPPT for testing
    _pwmValue = 255; // Maximum PWM
    
    // Apply PWM
    analogWrite(_pwmPin, _pwmValue);
}

float CapacitorCharger::readCapacitorVoltage() {
//#ifdef MOCK_SENSOR
    //return _sensor->get_mock_capacitor_voltage();
//#else
    // Store current PWM value
    int currentPwm = _pwmValue;
    
    // Turn MOSFET ON briefly to measure capacitor voltage
    analogWrite(_pwmPin, 255);  // Full ON
    //digitalWrite(_pwmPin, HIGH);
    delay(5);  // Let voltage stabilize
    
    // Voltage divider: 750Ω to ground, 3kΩ to capacitor
    // Ratio = 750 / (3000 + 750) = 750 / 3750 = 0.2
    // To get actual voltage: (analogRead / 1023.0) * 5.0 / 0.2
    // Simplified: (analogRead / 1023.0) * 25.0
    
    float raw = analogRead(_capPin) / 1023.0;  // 0-1 range
    float actualVoltage = raw * 25.0;         // Convert to actual voltage
    
    // Restore original PWM value
    //digitalWrite(_pwmPin, LOW);
    delay(5);
    analogWrite(_pwmPin, currentPwm);
    
    return actualVoltage;
//#endif
}
