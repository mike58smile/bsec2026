#include "TimerEvent.h"
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
    Serial.println("Status: State, CapVoltage(V), PanelVoltage(V), Current(mA), Power(mW), U_Wake(V), U_Eco(V), U_Survival(V)");
    
    // Serial command instructions
    Serial.println("=== STATE CONTROL ===");
    Serial.println("Send number 0-6 to change state (enables manual mode):");
    Serial.println("0 = CHARGING (Red)");
    Serial.println("1 = WAKEUP (Orange)");  
    Serial.println("2 = READY (Green)");
    Serial.println("3 = TURBO (Police Flash)");
    Serial.println("4 = ECO (Yellow Flash)");
    Serial.println("5 = SURVIVAL (Breathing Red)");
    Serial.println("6 = SLEEP (Off)");
    Serial.println("9 = Exit manual mode (re-enable automatic changes)");
    Serial.println("===================");
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
    static bool manual_mode = false; // Override automatic state changes

    static float capVoltage = 0;
    // PWM generation for police siren effect - Better sounding frequencies
    static int siren_phase = 0;
    static unsigned long last_siren = 0;
    const unsigned long SIREN_INTERVAL = 350; // Slightly faster for better rhythm
    
    // Generate PWM based on current state
    if (current_state == State::TURBO) {
        // Better sounding siren: Use more distinct PWM values
        if (millis() - last_siren >= SIREN_INTERVAL) {
            last_siren = millis();
            siren_phase = (siren_phase + 1) % 4;
            
            switch(siren_phase) {
                case 0: analogWrite(CAP_PWM_PIN, 255);  break;  // Full high (bright sound)
                case 1: analogWrite(CAP_PWM_PIN, 255);  break;  // Full high (bright sound)
                case 2: analogWrite(CAP_PWM_PIN, 60);   break;  // Low (deep sound)
                case 3: analogWrite(CAP_PWM_PIN, 60);   break;  // Low (deep sound)
            }
        }
    } else {
        // Turn OFF PWM in all other modes
        digitalWrite(CAP_PWM_PIN, LOW);
    }
    
    // Debug: Print status value
    static int last_status = -1;
    if (last_status == -1) {
        Serial.println("DEBUG: Manual mode only - capacitor logic disabled");
        Serial.println("DEBUG: Better PWM siren (255/60) - 350ms intervals");
        last_status = 0;
    }

    // Print data for plotting
    if (millis() - last_print >= PRINT_INTERVAL) {
        Serial.print("State:");
        Serial.print(static_cast<int>(current_state));
        Serial.print(", Manual:");
        Serial.print(manual_mode ? "Y" : "N");
        Serial.println(" - Manual LED testing mode");
        last_print = millis();
    }

    // Check for serial commands to change state
    if (Serial.available()) {
        int command = Serial.parseInt();
        if (command >= 0 && command <= 6) {
            State old_state = current_state;
            current_state = static_cast<State>(command);
            manual_mode = true; // Enable manual mode when manually changing state
            Serial.print("State changed from ");
            Serial.print(static_cast<int>(old_state));
            Serial.print(" to ");
            Serial.print(command);
            Serial.print(" (Manual Mode: ");
            Serial.print(manual_mode ? "ON" : "OFF");
            Serial.println(")");
            
            // Reset timing variables when manually changing state
            was_in_turbo = false;
            last_flash = millis();
            last_eco_flash = millis();
            last_breath = millis();
            flash_state = false;
            eco_flash_state = false;
            breath_value = 10;
            breathing_up = true;
        } else if (command == 9) {
            // Command 9 to exit manual mode
            manual_mode = false;
            Serial.print("Manual Mode: ");
            Serial.print(manual_mode ? "ON" : "OFF");
            Serial.println(" - Automatic state changes enabled");
        } else {
            Serial.println("Invalid command. Use:");
            Serial.println("0-6 = Change state (enables manual mode)");
            Serial.println("9 = Exit manual mode");
            Serial.println("States: 0=CHARGING, 1=WAKEUP, 2=READY, 3=TURBO, 4=ECO, 5=SURVIVAL, 6=SLEEP");
        }
        
        // Clear serial buffer
        while (Serial.available()) {
            Serial.read();
        }
    }
    
    // DEBUG: Check if state changed unexpectedly
    static State last_state_check = static_cast<State>(-1);
    if (current_state != last_state_check) {
        if (last_state_check != static_cast<State>(-1)) {
            Serial.print("!!! UNEXPECTED STATE CHANGE: ");
            Serial.print(static_cast<int>(last_state_check));
            Serial.print(" -> ");
            Serial.println(static_cast<int>(current_state));
        }
        last_state_check = current_state;
    }

    switch (current_state) {
        case State::CHARGING:
            // Manual mode only - no automatic transitions
            motor.stop();
            statusLED.set(255, 0, 0);
            break;
        case State::WAKEUP:
            // Manual mode only - no automatic transitions
            statusLED.set(255, 165, 0);
            break;
        case State::READY:
            // Manual mode only - no automatic transitions
            statusLED.set(0, 255, 0);
            break;
        case State::TURBO:
            // Manual mode only - no automatic transitions
            motor.eco_power();
            
            // Police car flashing: red/blue alternating every 200ms
            if (manual_mode || was_in_turbo) {
                // Reset flash timing when entering turbo mode
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
            } else {
                statusLED.set(255, 0, 0); // Default red when not in manual mode
            }
            break;
        case State::ECO:
            // Manual mode only - no automatic transitions
            motor.full_power();
            
            // Reset turbo mode flag when leaving turbo
            was_in_turbo = false;
            
            // Yellow flashing every 500ms
            if (manual_mode) {
                if (millis() - last_eco_flash >= ECO_FLASH_INTERVAL) {
                    eco_flash_state = !eco_flash_state;
                    last_eco_flash = millis();
                }
                
                if (eco_flash_state) {
                    statusLED.set(255, 255, 0); // Yellow
                } else {
                    statusLED.off(); // Off
                }
            } else {
                statusLED.set(255, 255, 0); // Default yellow when not in manual mode
            }
            break;
        case State::SURVIVAL:
            // Manual mode only - no automatic transitions
            motor.full_power();
            
            // Reset turbo mode flag when leaving turbo
            was_in_turbo = false;
            
            // Breathing red LED effect
            if (manual_mode) {
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
            } else {
                statusLED.set(125, 0, 0); // Default dim red when not in manual mode
            }
            break;
        case State::SLEEP:
            motor.stop();
            statusLED.off();
            // Manual mode only - no automatic transitions or sleep
            break;
    }
    delay(100); // Main loop delay to reduce CPU load
}