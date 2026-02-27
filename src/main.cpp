#include <Arduino.h>
#include "pinout.h"
#include "fsm.h"
#include "constants.h"
#include "engine_control.h"

#include "CapacitorCharger.h"
#include <avr/sleep.h>
#include "led_controller.h"


RGBLED_Controller statusLED(RGB_R_PIN, RGB_G_PIN, RGB_B_PIN); // RGB LED on pins 3, 5, 6

CapacitorCharger capacitor(CAP_SENSOR_PIN, CAP_PWM_PIN, CUR_SENSOR_I2C_ADDR);


State current_state = State::CHARGING;
MotorController motor(MOTOR_PWM_PIN);


void setup() {
    Serial.begin(9600);
    
    if (!capacitor.begin()) {
        Serial.println("ERROR: Capacitor charger not found!");
        while (1); 
    }
    
    motor.begin();
    statusLED.begin();
    pinMode(LVR_PIN, INPUT);
    
    // CSV header for plotting
    Serial.println("State,Voltage,U_Wake,U_Eco,U_Survival");
}


void loop()
{
    static unsigned long last_print = 0;
    const unsigned long PRINT_INTERVAL = 500; // Print every 500ms
    static unsigned long last_flash = 0;
    const unsigned long FLASH_INTERVAL = 200; // Flash every 200ms
    static unsigned long last_eco_flash = 0;
    const unsigned long ECO_FLASH_INTERVAL = 500; // Eco flash every 500ms
    static unsigned long last_breath = 0;
    const unsigned long BREATH_INTERVAL = 50; // Breath update every 50ms
    static bool flash_state = false;
    static bool eco_flash_state = false;
    static bool was_in_turbo = false;
    static int breath_value = 0;
    static bool breathing_up = true;

    float capVoltage = capacitor.getVoltage();
    int status = capacitor.charge(U_Cap_Max, 0.750, 4.8);

    // Print data for plotting
    if (millis() - last_print >= PRINT_INTERVAL) {
        Serial.print(static_cast<int>(current_state));
        Serial.print(",");
        Serial.print(capVoltage, 2);
        Serial.print(",");
        Serial.print(U_Wake, 2);
        Serial.print(",");
        Serial.print(U_Eco, 2);
        Serial.print(",");
        Serial.println(U_Survival, 2);
        last_print = millis();
    }

    switch (current_state) {
        case State::CHARGING:
            if (capVoltage > U_Wake) {
                current_state = State::WAKEUP;
            }
            motor.stop();
            statusLED.set(255, 0, 0);
            break;
        case State::WAKEUP:
            if (status == 1) {
                current_state = State::READY;
            }
            if (digitalRead(LVR_PIN) == LOW) {
                current_state = State::TURBO;
            }
            statusLED.set(255, 165, 0);
            break;
        case State::READY:
            if (digitalRead(LVR_PIN) == LOW) {
                current_state = State::TURBO;
            }
            statusLED.set(0, 255, 0);
            break;
        case State::TURBO:
            if (capVoltage < U_Turbo) {
                current_state = State::ECO;
            }
            motor.eco_power();
            
            // Police car flashing: red/blue alternating every 200ms
            if (!was_in_turbo) {
                last_flash = millis();
                flash_state = false;
                was_in_turbo = true;
            }
            
            if (millis() - last_flash >= FLASH_INTERVAL) {
                flash_state = !flash_state;
                last_flash = millis();
            }
            
            if (flash_state) {
                statusLED.set(255, 0, 0); // Red
            } else {
                statusLED.set(0, 0, 255); // Blue
            }
            break;
        case State::ECO:
            if (capVoltage < U_Eco) {
                current_state = State::SURVIVAL;
            }
            motor.full_power();
            
            // Reset turbo mode flag when leaving turbo
            was_in_turbo = false;
            
            // Yellow flashing every 500ms
            if (millis() - last_eco_flash >= ECO_FLASH_INTERVAL) {
                eco_flash_state = !eco_flash_state;
                last_eco_flash = millis();
            }
            
            if (eco_flash_state) {
                statusLED.set(255, 255, 0); // Yellow
            } else {
                statusLED.off(); // Off
            }
            break;
        case State::SURVIVAL:
            if (capVoltage < U_Survival) {
                current_state = State::SLEEP;
            }
            motor.full_power();
            
            // Reset turbo mode flag when leaving turbo
            was_in_turbo = false;
            
            // Breathing red LED effect
            if (millis() - last_breath >= BREATH_INTERVAL) {
                last_breath = millis();
                
                if (breathing_up) {
                    breath_value += 20;
                    if (breath_value >= 125) {
                        breath_value = 125;
                        breathing_up = false;
                    }
                } else {
                    breath_value -= 20;
                    if (breath_value <= 10) {
                        breath_value = 10;
                        breathing_up = true;
                    }
                }
                
                statusLED.set(breath_value, 0, 0); // Breathing red
            }
            break;
        case State::SLEEP:
            motor.stop();
            statusLED.off();
            if (capVoltage > U_Wake) {
                current_state = State::WAKEUP;
            }
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            sleep_enable();
            sleep_cpu();
            break;
    }
    delay(100); // Main loop delay to reduce CPU load
}