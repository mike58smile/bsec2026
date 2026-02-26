# Solar MPPT Capacitor Charger Library

A simple Arduino library for charging capacitors from solar panels with Maximum Power Point Tracking (MPPT) and current limiting.

## Features

- **MPPT Control**: Maintains solar panel at optimal voltage (default 80% of Voc)
- **Current Limiting**: Prevents overcurrent (default 500mA)
- **Voltage Cutoff**: Stops charging at target voltage (default 5.4V)
- **Solar Optimized**: Designed for 6V solar panels
- **Simple API**: Easy to use with minimal code

## Hardware Requirements

- Arduino Uno (or compatible)
- INA226 current/voltage sensor (I2C address 0x40)
- MOSFET module on pin D3
- Solar panel (6V recommended)
- Capacitor bank
- Voltage divider (3kΩ + 750Ω) on analog pin A0
- 0.01Ω shunt resistor

## Installation

1. Copy this library to your Arduino `libraries` folder
2. Restart Arduino IDE
3. Include in your sketch: `#include "CapacitorCharger.h"`

## Basic Usage

```cpp
#include "CapacitorCharger.h"

CapacitorCharger capacitor;

void setup() {
  Serial.begin(9600);
  
  if (!capacitor.begin()) {
    Serial.println("ERROR: Capacitor charger not found!");
    while (1); 
  }
  
  Serial.println("Solar MPPT Capacitor Charger");
  Serial.println("Cap(V) | Panel(V) | Current(mA) | PWM | Status");
}

void loop() {
  // Charge to 5.4V at 500mA with MPPT at 4.8V (80% of 6V panel)
  int status = capacitor.charge(5.4, 0.5, 4.8);
  
  // Get values
  float capVoltage = capacitor.getVoltage();
  float panelVoltage = capacitor.getPanelVoltage();
  float current = capacitor.getCurrent();
  int pwm = capacitor.getPwm();
  
  // Print status
  Serial.print(capVoltage, 2);
  Serial.print(" | ");
  Serial.print(panelVoltage, 2);
  Serial.print(" | ");
  Serial.print(current, 0);
  Serial.print(" | ");
  Serial.print(pwm);
  Serial.print(" | ");
  
  if (status == 1) {
    Serial.println("COMPLETE");
    delay(1000); // Wait when complete
  } else {
    Serial.println("CHARGING");
  }
  
  delay(50);
}
```

## API Reference

### Constructor

```cpp
CapacitorCharger(capPin = A0, pwmPin = 3, i2cAddress = 0x40)
```

### Main Functions

```cpp
bool begin()  // Initialize the charger
int charge(maxVoltage, currentLimit, panelVoltage)  // Main charging loop
```

**Parameters:**

- `maxVoltage`: Target capacitor voltage (default 5.4V)
- `currentLimit`: Maximum charging current in amps (default 0.5A)
- `panelVoltage`: MPPT target voltage for solar panel (default 4.8V)

**Returns:**

- `0`: Still charging
- `1`: Charging complete
- `-1`: Error

### Getter Functions

```cpp
float getVoltage()        // Returns capacitor voltage
float getCurrent()        // Returns charging current in mA
float getPanelVoltage()   // Returns solar panel voltage from INA226
int getPwm()             // Returns current PWM value (0-255)
bool isCharging()        // Returns true if currently charging
bool isComplete()        // Returns true if charging complete
```

### Control Functions

```cpp
void stop()    // Stop charging immediately
void reset()   // Reset charger state
```

## Example Output

```
Solar MPPT Capacitor Charger
Cap(V) | Panel(V) | Current(mA) | PWM | Status
1.23 | 4.85 | 450 | 120 | CHARGING
2.45 | 4.78 | 480 | 125 | CHARGING
3.67 | 4.81 | 490 | 128 | CHARGING
4.89 | 4.79 | 495 | 130 | CHARGING
5.40 | 4.80 | 0 | 0 | COMPLETE
```

## MPPT Algorithm

The library maintains the solar panel at the specified MPPT voltage (typically 80% of open-circuit voltage) while respecting current limits. This ensures maximum power extraction from the solar panel.

**Priority Order:**

1. **Safety**: Stop at max capacitor voltage
2. **Current Limit**: Never exceed specified current
3. **MPPT**: Maintain panel at optimal voltage

## Calibration

The library is pre-calibrated for:

- 0.01Ω shunt resistor
- Voltage divider multiplier of 25.0
- 6V solar panels (4.8V MPPT target)

Adjust these values in the library if your hardware differs.

## License

MIT License - Feel free to use in your projects!
