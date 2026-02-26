#pragma once

#include <INA226.h>
#include "ring_buffer.h"

class CurrentVoltageSensor {

public:
  CurrentVoltageSensor(uint8_t i2c_address): 
    m_ina(i2c_address), 
    m_current_buffer(), 
    m_voltage_buffer() {}

  bool initialize(){
    if (!m_ina.begin()) {
      return false;
    }
    m_ina.setMaxCurrentShunt(1, 0.002);
    return true;
  }

  void update(){
    m_current_buffer.add_number(m_ina.getCurrent_mA());
    m_voltage_buffer.add_number(m_ina.getBusVoltage());
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

private:
    INA226 m_ina;
    RingBuffer m_current_buffer;
    RingBuffer m_voltage_buffer;
};
