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
        _pwmValue = 50; // Start with low PWM
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
    
    float current_mA = getCurrent();
    float panelVoltage = getPanelVoltage();
    
    // MPPT: Keep panel voltage at target (80% of Voc)
    float panelError = _panelTargetVoltage - panelVoltage;
    
    // Current limiting logic
    if (current_mA > _currentLimit * 1000) {
        // Current too high - reduce PWM
        _pwmValue = max(0, _pwmValue - 5);
    }
    else if (current_mA < _currentLimit * 800) {
        // Current below limit - adjust for MPPT
        if (panelError > 0.1) {
            // Panel voltage too low - increase PWM
            _pwmValue = min(255, _pwmValue + 3);
        }
        else if (panelError < -0.1) {
            // Panel voltage too high - decrease PWM
            _pwmValue = max(0, _pwmValue - 3);
        }
        else {
            // Panel voltage at target - increase PWM slowly
            _pwmValue = min(255, _pwmValue + 1);
        }
    }
    
    // Apply PWM
    analogWrite(_pwmPin, _pwmValue);
}

float CapacitorCharger::readCapacitorVoltage() {
    // Using your existing voltage divider multiplier
    return (analogRead(_capPin) / 1023.0) * 25.0;
}
