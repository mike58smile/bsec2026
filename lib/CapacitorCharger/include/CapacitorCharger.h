#pragma once

#include <Arduino.h>

#ifdef MOCK_SENSOR
#include "power_sensor_mock.h"
#else
#include "power_sensor.h"
#endif

class CapacitorCharger {
public:
    CapacitorCharger(uint8_t capPin = A0, uint8_t pwmPin = 3, uint8_t i2cAddress = 0x40);
    
    bool begin();
    
    // Main charging function - returns status
    // 0 = charging, 1 = complete, -1 = error
    int charge(float maxVoltage = 5.41, float currentLimit = 0.750, float panelVoltage = 4.8);
    
    // Getters
    float getVoltage();        // Returns capacitor voltage
    float getCurrent();        // Returns charging current in mA
    float getPanelVoltage();   // Returns solar panel voltage from INA226
    int getPwm();             // Returns current PWM value
    bool isCharging();        // Returns true if currently charging
    bool isComplete();        // Returns true if charging complete
    
    // Control functions
    void stop();              // Stop charging immediately
    void reset();             // Reset charger state
    
private:
    uint8_t _capPin;
    uint8_t _pwmPin;
    uint8_t _i2cAddress;
    
    CurrentVoltageSensor* _sensor;
    
    float _maxVoltage;
    float _currentLimit;
    float _panelTargetVoltage;  // MPPT target voltage (80% of Voc)
    int _pwmValue;
    bool _charging;
    bool _complete;
    
    // Internal methods
    void readSensors();
    void controlPwm();
    float readCapacitorVoltage();
};
