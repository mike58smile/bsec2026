#include "CapacitorCharger.h"

CapacitorCharger capacitor;

void setup() {
  Serial.begin(9600);
  
  if (!capacitor.begin()) {
    Serial.println("ERROR: Capacitor charger not found!");
    while (1); 
  }
  
  Serial.println("Capacitor Charger Library Demo");
  Serial.println("Usage: capacitor.charge(5.4, 0.5)");
  Serial.println("float voltage = capacitor.getVoltage();");
  Serial.println("Ucap(V) | Current(mA) | PWM | Status");
}

void loop() {
  // Charge to 5.4V at 500mA
  int status = capacitor.charge(5.4, 0.5);
  
  // Get values using library functions
  float voltage = capacitor.getVoltage();
  float current = capacitor.getCurrent();
  int pwm = capacitor.getPwm();
  
  // Print status
  Serial.print(voltage, 2);
  Serial.print(" | ");
  Serial.print(current, 0);
  Serial.print(" | ");
  Serial.print(pwm);
  Serial.print(" | ");
  
  if (status == 1) {
    Serial.println("COMPLETE");
    delay(1000); // Wait when complete
  } else if (status == -1) {
    Serial.println("ERROR");
  } else {
    Serial.println("CHARGING");
  }
  
  delay(50);
}
