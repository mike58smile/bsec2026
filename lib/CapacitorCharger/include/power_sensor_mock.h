#pragma once

#include "ring_buffer.h"

class CurrentVoltageSensor {

public:
  CurrentVoltageSensor(uint8_t i2c_address): 
    m_current_buffer(), 
    m_voltage_buffer() {
    mock_time = 0;
  }

  bool initialize(){
    return true; // Always succeed in mock
  }

  void update(){
    mock_time += 0.1; // Simulate time passing
    
    // Mock panel voltage based on MPPT target with some variation
    float target_voltage = 4.8;
    float voltage_variation = sin(mock_time * 0.5) * 0.2; // ±0.2V variation
    float mock_voltage = target_voltage + voltage_variation;
    
    // Mock current based on panel voltage and load conditions
    float mock_current;
    if (mock_voltage > 4.6) {
      // Good sun: 400-750mA
      mock_current = 400 + sin(mock_time * 0.3) * 175 + rand() % 50;
    } else if (mock_voltage > 3.0) {
      // Medium sun: 200-400mA  
      mock_current = 200 + sin(mock_time * 0.4) * 100 + rand() % 50;
    } else {
      // Low sun: 50-200mA
      mock_current = 50 + sin(mock_time * 0.6) * 75 + rand() % 25;
    }
    
    m_current_buffer.add_number(mock_current);
    m_voltage_buffer.add_number(mock_voltage);
  }

  float get_current(){
    return m_current_buffer.get_mean();
  }

  float get_voltage(){
    return m_voltage_buffer.get_mean();
  }

  float get_power_mW(){
    // Power (mW) = Voltage (V) × Current (mA)
    return get_voltage() * get_current();
  }

  float get_mock_capacitor_voltage(){
    // Simulate capacitor charging from 0V to 5.4V
    static float cap_voltage = 0.5; // Start at 0.5V
    float current = get_current();
    
    // Charge rate proportional to current (simplified model)
    float charge_rate = current * 0.001; // Simple charging model
    
    cap_voltage += charge_rate;
    if (cap_voltage > 5.4) cap_voltage = 5.4; // Max voltage
    if (cap_voltage < 0.5) cap_voltage = 0.5; // Min voltage
    
    return cap_voltage;
  }

private:
  RingBuffer m_current_buffer;
  RingBuffer m_voltage_buffer;
  float mock_time;
};
